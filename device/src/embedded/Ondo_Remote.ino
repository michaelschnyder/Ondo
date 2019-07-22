#include <ESP8266WebServer.h>  
#include <ESP8266WiFi.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Daikin.h>
#include <pins_arduino.h>

#include "src/CloudClient.h"
#include "src/SensorReader.h"
#include "src/AppConfig.h"

#define DHTPIN 0 //D3 = 0
#define DHTTYPE DHT22   

#define IRPIN 4 //D2 = 4

WiFiClient wifiClient;
IRDaikinESP dakinir(IRPIN);

SensorReader sensorReader(DHTPIN, DHTTYPE);
CloudClient cloudClient(wifiClient);
AppConfig config;

void setup() {

  Serial.println("Device Started");
  Serial.println("-------------------------------------");
  
  Serial.begin(115200);
  Serial.setTimeout(2000);

  // Wait for serial to initialize.
  while(!Serial) { }

  if (!SPIFFS.begin()) {
    SPIFFS.format();
    Serial.println("ERROR: Unable to start filesystem.");
  }
  else {
    config.load();
  }

  cloudClient.onSetAcCommand(&handleSetAcCommand);
  sensorReader.onUpdate(&handleSensorUpdate);

  sensorReader.setup();
  cloudClient.setup();

  setupAndConnectWifi();
}

void setupAndConnectWifi() {
  Serial.print("Connecting to '" + config.getWifiSSID() + "'");

  WiFi.begin(config.getWifiSSID(), config.getWifiKey());

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());

  byte mac[6]; 
  WiFi.macAddress(mac);

  char deviceName[10];
  sprintf(deviceName, "Ondo-%2x%2x%2x%2x", mac[2], mac[3], mac[4], mac[5]);

  Serial.print("Device name: ");
  Serial.println(deviceName);
}

void loop() {
  sensorReader.loop();
  cloudClient.loop();
}

void handleSensorUpdate(float humidity, float tempC, float tempF, float heatIndexC, float heatIndexF) {
  
  StaticJsonBuffer<500> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["roomHumidity"] = humidity;
  root["roomTemp"] = tempC;
  // root["acPower"] = acPower;
  
  cloudClient.send(root);
  // Serial.println("Reported!");
}

void handleSetAcCommand(bool status, int16_t fanLevel, int16_t tempC, bool quiet, bool powerful) {
    
    dakinir.begin();

    if(status){
      dakinir.on();
    }else{
      dakinir.off();
    }

    dakinir.setFan(fanLevel);
    dakinir.setMode(kDaikinCool);
    dakinir.setTemp(tempC);
    dakinir.setQuiet(quiet);
    dakinir.setPowerful(powerful);
    
    dakinir.setSwingVertical(false);
    dakinir.setSwingHorizontal(false);

    dakinir.send();
}