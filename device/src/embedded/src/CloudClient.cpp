#include "CloudClient.h"

String mqtt_server;
int port = 8883;
String mqtt_user;
String inbound_topic; 
String outbound_topic;
String deviceId;

boolean clientReady = false;
long lastReconnectAttempt = 0;
int retryTimoutInMs = 5000;

CloudClient::CloudClient(AppConfig& appConfig) : config(appConfig) { 
  
  CloudClient::wifiClient.setInsecure();
  CloudClient::client.setClient(CloudClient::wifiClient);

  // Required to make signature of member function (that comes with a implict *this) match
  // with expected signature. See: https://stackoverflow.com/a/46489820
  client.setCallback([this](char* a, uint8_t* b, unsigned int c) { this->callback(a, b, c); });
}

void CloudClient::setup(String devId) {
  
  const char* domain = "azure-devices.net";
  deviceId = devId;
  mqtt_server = CloudClient::config.getAzIoTHubName() + '.' + domain;
  
  mqtt_user = mqtt_server + "/" + deviceId;
  inbound_topic = "devices/" + deviceId + "/messages/devicebound/#";
  outbound_topic = "devices/" + deviceId + "/messages/events/";
  int maxAttempts = 5;

  for(int i = 0; i < maxAttempts; i++) {
    if(this->connect()) {
      clientReady = true;
      return;
    }

    if (i+1 < maxAttempts) {
      logger.warning("Unable to connect to MQTT Server in attempt %d/%d. Retry in %ds...", i+1, maxAttempts, retryTimoutInMs/1000);
      delay(retryTimoutInMs);
    }
  }

  logger.error("Initial connection to MQTT not successful. Will continue trying...");
  lastReconnectAttempt = millis();
}

void CloudClient::loadCACert() {

  // Load CA file from SPIFFS
  File ca = SPIFFS.open("/BaltimoreCyberTrustRoot.der", "r"); 
  if (!ca) {
    logger.verbose("Failed to open ca ");
  }
  else
    logger.verbose("Success to open ca");

  delay(1000);

  // Set server CA file
  if(wifiClient.loadCACert(ca, ca.size())) {
    logger.verbose("CA loaded to wifiClientSecure");
  }
  else {
    logger.verbose("CA loading failed");
  }

  File ca2 = SPIFFS.open("/BaltimoreCyberTrustRoot.der", "r"); 
  if(wifiClient.loadCertificate(ca2, ca.size())) {
    logger.verbose("cert loaded");
  }
  else {
    logger.verbose("cert failed");
  }

  wifiClient.setInsecure();
  wifiClient.allowSelfSignedCerts();
}

void CloudClient::callback(char* topic, byte* payload, unsigned int length) {
  String message = String((char*)payload);
  logger.verbose("MQQT Message: '%s'", message.c_str());
}

boolean CloudClient::connect() {

  logger.trace("Attempting to connect to MQTT server...");
  logger.verbose("URL: %s:%d, MQTT_MAX_PACKET_SIZE: %d", mqtt_server.c_str(), port, MQTT_MAX_PACKET_SIZE);  

  CloudClient::client.setServer(mqtt_server.c_str(), port);

  logger.verbose("Credentials: DeviceId: %s, User: %s, Pass: %s", deviceId.c_str(), mqtt_user.c_str(), CloudClient::config.getAzIoTSASToken().c_str());

  if (!client.connect(deviceId.c_str(), mqtt_user.c_str(), CloudClient::config.getAzIoTSASToken().c_str())) {
    char lastErrorText[64];
    int errorNo = CloudClient::wifiClient.getLastSSLError(lastErrorText, 64);
    
    logger.fatal("Connection to MQTT failed!. Client-State: %d, lastSSLError: %d ('%s'). Next try in 5s", client.state(), errorNo, lastErrorText);  

    return false;    
  }

  logger.trace("Connection established to '%s:%d'. Subscribing for inbound topic '%s'", mqtt_server.c_str(), port, inbound_topic.c_str());      
    
  if(!client.subscribe(inbound_topic.c_str())) {
    logger.fatal("Subscribe to topic failed");
    return false;
  }

  logger.verbose("Subscribe to topic successful. Sending welcome message to '%s'", outbound_topic.c_str());  
  
  if (!CloudClient::client.publish(outbound_topic.c_str(), "hello world")) {
    logger.fatal("Unable to send welcome message!");
    return false;
  }

  logger.verbose("Welome message send successful");
  
  return true;
}

void CloudClient::loop() {
  CloudClient::client.loop();

  reconnectIfNecessary();
}

void CloudClient::reconnectIfNecessary() {

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
    logger.warning("MQTT client got disconnected. Trying to reconnect.");
  }

  // Start a new connection attempt
  logger.verbose("Starting new reconnect attempt.");
  lastReconnectAttempt = millis();
   
  if(this->connect()) {
    clientReady = true;
    logger.trace("Successfully re-established connection MQTT Server");
    return;
  }

  logger.error("Re-establishing connection and initializing client failed.");
}

void CloudClient::send(JsonObject& data) {
  char buffer[512];
  data.printTo(buffer);

  CloudClient::client.publish("devices/Ondo-3c71bf3168b1/messages/events/", buffer);
}

void CloudClient::onSetAcCommand(SETACCOMMAND_CALLBACK_SIGNATURE) {
    this->onSetAcCommandCallback = onSetAcCommandCallback;
}

/*
void handleCommand(LosantCommand *command) {

  Serial.println();
  Serial.print("Command received: ");
  Serial.println(command->name);

  if (strcmp(command->name, "setAc") == 0) {

    JsonObject &payload = *command->payload;
    payload.printTo(Serial);
    char json[400];

    StaticJsonBuffer<200> jsonBuffer;

    payload.printTo(json, sizeof(json));

    JsonObject &root = jsonBuffer.parseObject(json);

    if (!root.success()) {
      Serial.println("parseObject() failed");
      return;
    }

    bool statuss = root["status"];
    bool quiet = root["quite"];
    bool powerful = root["powerful"];

    int16_t temperature = root["temperature"];
    int16_t fan = root["fan"];

    if (CloudClient::setAcCommandCallback != NULL) {
        CloudClient::setAcCommandCallback(statuss, fan, temperature, quiet, powerful);
    }
  }
}*/