#include "AzureIoTMqttClient.h"

String mqtt_server;
int port = 8883;
String mqtt_user;
String inbound_topic; 
String outbound_topic;
char* reportingProp_topic = "$iothub/twin/res/#";
char* desiredProp_topic = "$iothub/twin/PATCH/properties/desired/#";

String deviceId;

boolean clientReady = false;
long lastReconnectAttempt = 0;
int retryTimoutInMs = 5000;

AzureIoTMqttClient::AzureIoTMqttClient(AppConfig& appConfig) : config(appConfig) { 
  
  AzureIoTMqttClient::wifiClient.setInsecure();
  AzureIoTMqttClient::client.setClient(AzureIoTMqttClient::wifiClient);

  // Required to make signature of member function (that comes with a implict *this) match
  // with expected signature. See: https://stackoverflow.com/a/46489820
  client.setCallback([this](char* a, uint8_t* b, unsigned int c) { this->callback(a, b, c); });
}

void AzureIoTMqttClient::setup(String devId) {
  
  const char* domain = "azure-devices.net";
  deviceId = devId;
  mqtt_server = AzureIoTMqttClient::config.getAzIoTHubName() + '.' + domain;
  
  mqtt_user = mqtt_server + "/" + deviceId + "/?api-version=2018-06-30";
  inbound_topic = "devices/" + deviceId + "/messages/devicebound/#";
  outbound_topic = "devices/" + deviceId + "/messages/events/";


  int maxAttempts = 5;

  for(int i = 0; i < maxAttempts; i++) {
    if(this->connect()) {
      clientReady = true;
      return;
    }

    if (i+1 < maxAttempts) {
      logger.warning(F("Unable to connect to MQTT Server in attempt %d/%d. Retry in %ds..."), i+1, maxAttempts, retryTimoutInMs/1000);
      delay(retryTimoutInMs);
    }
  }

  logger.error(F("Initial connection to MQTT not successful. Will continue trying..."));
  lastReconnectAttempt = millis();
}

void AzureIoTMqttClient::loadCACert() {

  // Load CA file from SPIFFS
  File ca = SPIFFS.open("/BaltimoreCyberTrustRoot.der", "r"); 
  if (!ca) {
    logger.verbose(F("Failed to open ca "));
  }
  else
    logger.verbose(F("Success to open ca"));

  delay(1000);

  // Set server CA file
  if(wifiClient.loadCACert(ca, ca.size())) {
    logger.verbose(F("CA loaded to wifiClientSecure"));
  }
  else {
    logger.verbose(F("CA loading failed"));
  }

  File ca2 = SPIFFS.open("/BaltimoreCyberTrustRoot.der", "r"); 
  if(wifiClient.loadCertificate(ca2, ca.size())) {
    logger.verbose(F("cert loaded"));
  }
  else {
    logger.verbose(F("cert failed"));
  }

  wifiClient.setInsecure();
  wifiClient.allowSelfSignedCerts();
}

void AzureIoTMqttClient::callback(char* topic, byte* payload, unsigned int length) {
  
  char* buffer = (char*)payload;
  payload[length] = '\0'; // Manually add null-termination at given lenght since PubSub is reusing the buffer;

  String topicString = String(topic);

  if (handleReportedPropertyUpdateResponse(topicString)) {
    return;
  }

  if (handleDesiredPropertiesUpdate(topicString, buffer, length)) {
    return;
  }

  if (handleCloudToDeviceCommand(topicString, buffer, length)) {
    return;
  }

  logger.warning(F("Recieved message not handled: '%s'"), buffer);
}

boolean AzureIoTMqttClient::connect() {

  logger.trace(F("Attempting to connect to MQTT server..."));
  logger.verbose(F("URL: %s:%d, MQTT_MAX_PACKET_SIZE: %d"), mqtt_server.c_str(), port, MQTT_MAX_PACKET_SIZE);  

  AzureIoTMqttClient::client.setServer(mqtt_server.c_str(), port);

  logger.verbose(F("Credentials: DeviceId: %s, User: %s, Pass: %s"), deviceId.c_str(), mqtt_user.c_str(), AzureIoTMqttClient::config.getAzIoTSASToken().c_str());

  if (!client.connect(deviceId.c_str(), mqtt_user.c_str(), AzureIoTMqttClient::config.getAzIoTSASToken().c_str())) {
    char lastErrorText[64];
    int errorNo = AzureIoTMqttClient::wifiClient.getLastSSLError(lastErrorText, 64);
    
    logger.fatal(F("Connection to MQTT failed!. Client-State: %d, lastSSLError: %d ('%s'). Next try in 5s"), client.state(), errorNo, lastErrorText);  

    return false;    
  }

  logger.trace(F("Connection established to '%s:%d'. Subscribing to topics: '%s', '%s', '%s'"), mqtt_server.c_str(), port, inbound_topic.c_str(), reportingProp_topic, desiredProp_topic);      
    
  if(!client.subscribe(inbound_topic.c_str())) {
    logger.fatal(F("Subscribe to event topic failed"));
    return false;
  }

  if(!client.subscribe(reportingProp_topic)) {
    logger.error(F("Unable to subscribe to Reported Properties topic on '%s'"), reportingProp_topic);
    return false;
  }

  if(!client.subscribe(desiredProp_topic)) {
    logger.error(F("Unable to subscribe to Desired Properties topic on '%s'"), desiredProp_topic);
    return false;
  }

  logger.verbose(F("Subscribe to topic successful. Sending welcome message to '%s'"), outbound_topic.c_str());  
  
  if (!AzureIoTMqttClient::client.publish(outbound_topic.c_str(), "hello world")) {
    logger.fatal(F("Unable to send welcome message!"));
    return false;
  }

  logger.verbose(F("Welcome message send successful"));
  
  return true;
}

void AzureIoTMqttClient::loop() {
  AzureIoTMqttClient::client.loop();

  reconnectIfNecessary();
}

bool AzureIoTMqttClient::handleReportedPropertyUpdateResponse(String topic) {
  
  String topicPrefix = "$iothub/twin/res";

  if (!topic.startsWith(topicPrefix)) {
    return false;
  }

  String responseCode = topic.substring(topicPrefix.length() + 1, topicPrefix.length() + 4);
  int ridPosStart = topic.indexOf("$rid") + 5;
  int ridPosEnd = topic.indexOf("&", ridPosStart);
  int versionPosStart = topic.indexOf("$version") + 9;
  
  String ridString = topic.substring(ridPosStart, ridPosEnd);
  String versionString = topic.substring(versionPosStart);

  int requestId = ridString.toInt();
  int version = versionString.toInt();

  if (responseCode == "204") {
    logger.verbose(F("Reported property update (Request: %i) accepted by broker. Response Code: %s. New Version: %i"), requestId, responseCode.c_str(), version);
  }
  else {
    logger.warning(F("Reported Property (Request: %i) got rejected by broker. Response Code: %s"), requestId, responseCode.c_str());
  }
  
  return true;
}

bool AzureIoTMqttClient::handleDesiredPropertiesUpdate(String topic, char* payload, unsigned int length)
{
  if (topic.length() == 0) {
    return false;
  }

  if (!topic.startsWith("$iothub/twin/PATCH/properties/desired")) {
    return false;
  }

  logger.trace(F("Desired property change update"));

  DynamicJsonBuffer jsonBuffer;
  JsonObject& jsonMessage = jsonBuffer.parseObject((char*)payload);

  if (onDesiredPropertyChangeCallback != NULL) {
    onDesiredPropertyChangeCallback(jsonMessage, 0);
  }

  return true;
}

bool AzureIoTMqttClient::handleCloudToDeviceCommand(String topic, char* payload, unsigned int length) {
  
  if (topic.length() == 0) {
    return false;
  }

  String message = String(payload);
  logger.verbose(F("New Message from Broker. Topic: '%s', Lenght: %d, Content: '%s'"), topic.c_str(), length, message.c_str());

  DynamicJsonBuffer jsonBuffer;
  JsonObject& jsonMessage = jsonBuffer.parseObject((char*)payload);

  if(!jsonMessage.success()) {
    return false;
  }

  if (!jsonMessage.containsKey("cmd")) {
    return false;
  }

  String commandName = jsonMessage["cmd"];

  if (onCommandCallback != NULL) {
    logger.trace(F("Dispatching execution of command '%s'"), commandName.c_str());
    onCommandCallback(commandName, jsonMessage);
  }
  else {
    logger.trace(F("No callback registered for command '%s'"), commandName.c_str());
  }
}

void AzureIoTMqttClient::reconnectIfNecessary() {

  if (clientReady && client.connected()) {
    return;
  }

  unsigned long currentMillis = millis();
  if (!(currentMillis - lastReconnectAttempt >= retryTimoutInMs * 2)) {
    return;
  }

  if(clientReady && !client.connected()) {
    // The initial state was valid, means the connection did reset
    clientReady = false;
    logger.warning(F("MQTT client got disconnected. Trying to reconnect."));
  }

  // Start a new connection attempt
  logger.verbose(F("Starting new reconnect attempt."));
  lastReconnectAttempt = millis();
   
  if(this->connect()) {
    clientReady = true;
    logger.trace(F("Successfully re-established connection MQTT Server"));
    return;
  }

  logger.error(F("Re-establishing connection and initializing client failed."));
}

void AzureIoTMqttClient::send(JsonObject& data) {
  char buffer[512];
  data.printTo(buffer);

  if(!AzureIoTMqttClient::client.publish(outbound_topic.c_str(), buffer)) {
    logger.error(F("Unable to publish message '%s'"), buffer);
    }
}

void AzureIoTMqttClient::report(String propertyName, int value) {
  String patch = "{\"" + propertyName + "\": " + value + "}";
  AzureIoTMqttClient::report(AzureIoTMqttClient::client, logger, patch);  
}
void AzureIoTMqttClient::report(String propertyName, String value) {
  String patch = "{\"" + propertyName + "\": \"" + value + "\"}";
  AzureIoTMqttClient::report(AzureIoTMqttClient::client, logger, patch);  
}
void AzureIoTMqttClient::report(JsonObject& value) {
  char patch[512];
  value.printTo(patch);
  AzureIoTMqttClient::report(AzureIoTMqttClient::client, logger, patch);  
}
void AzureIoTMqttClient::report(String propertyName, float value) {
  String patch = "{\"" + propertyName + "\": " + value + "}";
  AzureIoTMqttClient::report(AzureIoTMqttClient::client, logger, patch);  
}

void AzureIoTMqttClient::report(PubSubClient &client, log4Esp::Logger &logger, String patch) {
  int rid = millis();
    
  char topic[100] = "";
  sprintf(topic, "$iothub/twin/PATCH/properties/reported/?$rid=%d", rid);

  if (!AzureIoTMqttClient::client.publish(topic, patch.c_str())) {
    logger.error(F("Unable to publish Reported Property update to '%s'. Update was: '%s'"), topic, patch.c_str());
  }
}

void AzureIoTMqttClient::onCommand(ONCOMMAND_CALLBACK_SIGNATURE) {
    this->onCommandCallback = onCommandCallback;
}

void AzureIoTMqttClient::onDesiredPropertyChange(DESIREDPROPERTYCHANGE_CALLBACK_SIGNATURE) {
    this->onDesiredPropertyChangeCallback = onDesiredPropertyChangeCallback;
}