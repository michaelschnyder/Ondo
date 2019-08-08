#include <ESP8266WebServer.h>  
#include <ESP8266WiFi.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Daikin.h>
#include <pins_arduino.h>
#include <time.h>

#include "src/CloudClient.h"
#include "src/SensorReader.h"
#include "src/AppConfig.h"

#define DHTPIN 0 //D3 = 0
#define DHTTYPE DHT22   

#define IRPIN 4 //D2 = 4

AppConfig config;
WiFiClient wifiClient;
WiFiClientSecure wifiClientSecure;

IRDaikinESP dakinir(IRPIN);

SensorReader sensorReader(DHTPIN, DHTTYPE);
CloudClient cloudClient(config);

void setup() {

  Serial.begin(115200);
  Serial.setTimeout(2000);

  // Wait for serial to initialize.
  while(!Serial) { }

  Serial.println("\nDevice Started");
  Serial.println("-------------------------------------");
  
  byte mac[6]; 
  WiFi.macAddress(mac);

  char deviceName[10];
  sprintf(deviceName, "Ondo-%2x%2x%2x%2x%2x%2x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  Serial.print("Device Id: ");
  Serial.println(deviceName);
  Serial.println("-------------------------------------\n");


  if (!SPIFFS.begin()) {
    SPIFFS.format();
    Serial.println("ERROR: Unable to start filesystem.");
  }
  else {
    Serial.println("INFO: Filesystem ready. Loading configuration...");
    config.load();
  }


  cloudClient.onSetAcCommand(&handleSetAcCommand);
  sensorReader.onUpdate(&handleSensorUpdate);

  sensorReader.setup();

  setupAndConnectWifi();

  cloudClient.setup(deviceName);
}

void setupAndConnectWifi() { 
  WiFi.mode(WIFI_STA);
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