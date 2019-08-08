#include "CloudClient.h"

CloudClient::CloudClient(AppConfig& appConfig) : config(appConfig) { 
  CloudClient::wifiClient.setInsecure();
  CloudClient::client.setClient(CloudClient::wifiClient);
}

void CloudClient::setup(String deviceId) {
  this->connect(deviceId);
}

void CloudClient::loadCACert() {

  // Load CA file from SPIFFS
  File ca = SPIFFS.open("/BaltimoreCyberTrustRoot.der", "r"); 
  if (!ca) {
    Serial.println("Failed to open ca ");
  }
  else
    Serial.println("Success to open ca");

  delay(1000);

  // Set server CA file
  if(wifiClient.loadCACert(ca)) {
    Serial.println("CA loaded to wifiClientSecure");
  }
  else {
    Serial.println("CA loading failed");
  }

  File ca2 = SPIFFS.open("/BaltimoreCyberTrustRoot.der", "r"); 
  if(wifiClient.loadCertificate(ca2)) {
    Serial.println("cert loaded");
  }
  else {
    Serial.println("cert failed");
  }

  wifiClient.setInsecure();
  wifiClient.allowSelfSignedCerts();
}

void CloudClient::connect(String deviceId) {
  int port = 8883;

  const char* domain = "azure-devices.net";

  String mqtt_server = CloudClient::config.getAzIoTHubName() + '.' + domain;
  String mqtt_user = mqtt_server + "/" + deviceId;

  for(int i = 0; i < 5; i++) {

    Serial.print("Attempting to connect to MQTT server. ");
    Serial.print("URL: " + mqtt_server + ":" + port + ", ");  
    Serial.print("MQTT_MAX_PACKET_SIZE: ");
    Serial.println(MQTT_MAX_PACKET_SIZE);

    CloudClient::client.setServer(mqtt_server.c_str(), port);

    Serial.print("Using credentials. ");
    Serial.print("DeviceId: " + deviceId);
    Serial.print(", User: ");
    Serial.print(mqtt_user);
    Serial.println(", Pass: " + CloudClient::config.getAzIoTSASToken());

    if (client.connect(deviceId.c_str(), mqtt_user.c_str(), CloudClient::config.getAzIoTSASToken().c_str())) {
      Serial.print("Connection established to ");
      Serial.println(mqtt_server);
      CloudClient::client.publish(("devices/" + deviceId + "/messages/events/").c_str(), "hello world");
      Serial.println("Welcome message sent.");
      break;
    }
    else {
      Serial.print("ERROR: Connection to MQTT failed!");
      Serial.print("RC-State=");
      Serial.print(client.state());
      char lastErrorText[64];
      int errorNo = CloudClient::wifiClient.getLastSSLError(lastErrorText, 64);
      Serial.print(", LastSSLError=");
      Serial.print(errorNo);
      Serial.print(", '");
      Serial.print(lastErrorText);
      Serial.println("'");
      
      
      Serial.println("try again in 5 seconds....");
      // Wait 5 seconds before retrying
      delay(5000);
    }
    
  }
}

void CloudClient::loop() {
  CloudClient::client.loop();
}

void CloudClient::send(JsonObject& data) {
  // TODO: Use Pubsub to send
  // CloudClient::device.sendState(data);

  //IoTHubClient_LL_SendEventAsync
}

void CloudClient::onSetAcCommand(SetAcCommandCallback callback) {
    //CloudClient::setAcCommandCallback = callback;
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