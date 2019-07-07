
#include <ESP8266WebServer.h>  
#include <ESP8266WiFi.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Daikin.h>
#include <pins_arduino.h>

#include <AutoConnect.h>

#include "src/CloudClient.h"
#include "src/SensorReader.h"

#define DHTPIN 0 //D3 = 0
#define DHTTYPE DHT22   

#define IRPIN 4 //D2 = 4

WiFiClient wifiClient;
IRDaikinESP dakinir(IRPIN);

ESP8266WebServer Server;          
AutoConnect      Portal(Server);
AutoConnectConfig  config;

SensorReader sensorReader(DHTPIN, DHTTYPE);
CloudClient cloudClient(wifiClient);

void setup() {

  Serial.println("Device Started");
  Serial.println("-------------------------------------");
  
  Serial.begin(115200);
  Serial.setTimeout(2000);

  // Wait for serial to initialize.
  while(!Serial) { }

  Server.on("/", rootPage);

  cloudClient.onSetAcCommand(&handleSetAcCommand);
  sensorReader.onUpdate(&handleSensorUpdate);

  sensorReader.setup();
  cloudClient.setup();

  setupAndConnectWifi();
}

void setupAndConnectWifi() {
  byte mac[6]; 
  WiFi.macAddress(mac);

  char setupSSID[10];
  sprintf(setupSSID, "Ondo-%2x%2x%2x%2x", mac[2], mac[3], mac[4], mac[5]);

  config.title = "Configuration";
  config.apid = setupSSID;
  config.psk = "";

  Portal.config(config);

  if (Portal.begin()) {
    Serial.println("WiFi connected: " + WiFi.localIP().toString());
  }
}

void rootPage() {
  char content[] = "Hello, world";
  Server.send(200, "text/plain", content);
}

void loop() {

  sensorReader.loop();
  cloudClient.loop();
  
  Portal.handleClient();
}

void handleSensorUpdate(float humidity, float tempC, float tempF, float heatIndexC, float heatIndexF) {
  
  StaticJsonBuffer<500> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["roomHumidity"] = humidity;
  root["roomTemp"] = tempC;
  // root["acPower"] = acPower;
  
  cloudClient.send(root);
  Serial.println("Reported!");
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