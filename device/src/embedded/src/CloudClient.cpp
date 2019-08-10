#include "CloudClient.h"

CloudClient::CloudClient(AppConfig& appConfig) : config(appConfig) { 
  CloudClient::wifiClient.setInsecure();
  
  CloudClient::client.setClient(CloudClient::wifiClient);

  // Required to make signature of member function (that comes with a implict *this) match
  // with expected signature. See: https://stackoverflow.com/a/46489820
  client.setCallback([this](char* a, uint8_t* b, unsigned int c) { this->callback(a, b, c); });
}


void CloudClient::setup(String deviceId) {
  this->connect(deviceId);
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

void CloudClient::connect(String deviceId) {
  int port = 8883;

  const char* domain = "azure-devices.net";

  String mqtt_server = CloudClient::config.getAzIoTHubName() + '.' + domain;
  String mqtt_user = mqtt_server + "/" + deviceId;
  String inbound_topic = "devices/" + deviceId + "/messages/devicebound/#";
  String outbound_topic = "devices/" + deviceId + "/messages/events/";

  for(int i = 0; i < 5; i++) {

    logger.trace("Attempting to connect to MQTT server...");
    logger.verbose("URL: %s:%d, MQTT_MAX_PACKET_SIZE: %d", mqtt_server.c_str(), port, MQTT_MAX_PACKET_SIZE);  

    CloudClient::client.setServer(mqtt_server.c_str(), port);

    logger.verbose("Credentials: DeviceId: %s, User: %s, Pass: %s", deviceId.c_str(), mqtt_user.c_str(), CloudClient::config.getAzIoTSASToken().c_str());

    if (client.connect(deviceId.c_str(), mqtt_user.c_str(), CloudClient::config.getAzIoTSASToken().c_str())) {
      logger.trace("Connection established to '%s:%d'. Subscibing for inbound topic '%s'", mqtt_server.c_str(), port, inbound_topic.c_str());      
      
      if(client.subscribe(inbound_topic.c_str())) {
        logger.verbose("Subscribe to topic successful");
      }
      else {
        logger.fatal("Subscribe to topic failed");
      }

      logger.verbose("Sending welcome message to '%s'", outbound_topic.c_str());  
      if (CloudClient::client.publish(outbound_topic.c_str(), "hello world")) {
        logger.verbose("Welome message send successful");
      }
      else {
        logger.fatal("Unable to send welcome message!");
      }
      
      return;
    }
    else {
      char lastErrorText[64];
      int errorNo = CloudClient::wifiClient.getLastSSLError(lastErrorText, 64);
      
      logger.fatal("Connection to MQTT failed!. Client-State: %d, lastSSLError: %d ('%s'). Next try in 5s", client.state(), errorNo, lastErrorText);      

      delay(5000);
    }
  }
}

void CloudClient::loop() {
  CloudClient::client.loop();
}

void CloudClient::send(JsonObject& data) {
  char buffer[512];
  data.printTo(buffer);

  CloudClient::client.publish("devices/Ondo-3c71bf3168b1/messages/events/", buffer);
}

void CloudClient::onSetAcCommand(SETACCOMMAND_CALLBACK_SIGNATURE) {
    this->onSetAcCommandCallback = onSetAcCommandCallback;
}

/*/
void CloudClient::connectToLosant() {

  // Connect to Losant.
  Serial.println();
  Serial.print("Connecting to Losant...");

  // device.connect(wifiClient, LOSANT_ACCESS_KEY, LOSANT_ACCESS_SECRET);

  int attemptToConnect = 1;

  while(!device.connected() && attemptToConnect <= 5) {
    delay(500);
    Serial.print(".");
    attemptToConnect++;
  } 

  if (device.connected()) {
    Serial.println("Connected!");
    Serial.println();
    Serial.println("This device is now ready for use!");
  }
  else {
    Serial.println("ERR: Device is not connected to cloud backend");
  }


}
*/

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