#include "CloudClient.h"

CloudClient::CloudClient(WiFiClient wificlient) {
  CloudClient::wifiClient = wifiClient;
  
  // TODO: Initialize PubSub
}

void CloudClient::setup() {

  // TODO: Register for topics
  // device.onCommand(&handleCommand);
}

void CloudClient::loop() {
  // TODO: Loop device
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