
#include <ESP8266WebServer.h>  
#include <ESP8266WiFi.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ESP8266HTTPClient.h>
#include <Losant.h>
#include <ir_Daikin.h>
#include <pins_arduino.h>

#include <AutoConnect.h>

#include "my_credentials.h"
#include "cloudClient.cpp"
#include "sensorReader.cpp"

#define DHTPIN 0 //D3 = 0
#define DHTTYPE DHT22   

#define IRPIN 4 //D2 = 4

WiFiClient wifiClient;
IRDaikinESP dakinir(IRPIN);
LosantDevice device(LOSANT_DEVICE_ID);

ESP8266WebServer Server;          
AutoConnect      Portal(Server);
AutoConnectConfig  config;

SensorReader sensorReader(DHTPIN, DHTTYPE);

void setup() {  

  Serial.println("Device Started");
  Serial.println("-------------------------------------");
  
  Serial.begin(115200);
  Serial.setTimeout(2000);

  sensorReader.setup();

  // Wait for serial to initialize.
  while(!Serial) { }

  CloudClient client;
  client.call();

  Server.on("/", rootPage);

  device.onCommand(&handleCommand);  
  sensorReader.onUpdate(&handleSensorUpdate);

  setupAndConnectWifi();
  // validateLosantConnection();
  // connectToLosant();
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



void validateLosantConnection() {
  HTTPClient http;
  http.begin("http://api.losant.com/auth/device");
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Accept", "application/json");

  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["deviceId"] = LOSANT_DEVICE_ID;
  root["key"] = LOSANT_ACCESS_KEY;
  root["secret"] = LOSANT_ACCESS_SECRET;
  String buffer;
  root.printTo(buffer);

  int httpCode = http.POST(buffer);

  if(httpCode > 0) {
      if(httpCode == HTTP_CODE_OK) {
        Serial.println("This device is authorized!");
      } else {
        Serial.println("Failed to authorize device to Losant.");
        if(httpCode == 400) {
          Serial.println("Validation error: The device ID, access key, or access secret is not in the proper format.");
        } else if(httpCode == 401) {
          Serial.println("Invalid credentials to Losant: Please double-check the device ID, access key, and access secret.");
        } else {
           Serial.println("Unknown response from API");
        }
      }
    } else {
        Serial.println("Failed to connect to Losant API.");

   }

  http.end();
}

void connectToLosant() {

  // Connect to Losant.
  Serial.println();
  Serial.print("Connecting to Losant...");

  device.connect(wifiClient, LOSANT_ACCESS_KEY, LOSANT_ACCESS_SECRET);

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

    dakinir.begin();

    if(statuss){
      dakinir.on();
    }else{
      dakinir.off();
    }
  

    dakinir.setFan(fan);
    dakinir.setMode(kDaikinCool);
    dakinir.setTemp(temperature);
    dakinir.setQuiet(quiet);
    dakinir.setPowerful(powerful);
    
    dakinir.setSwingVertical(false);
    dakinir.setSwingHorizontal(false);

    dakinir.send();
  }
}

void loop() {

  // device.loop();
  sensorReader.loop();

  Portal.handleClient();
}

void handleSensorUpdate(float humidity, float tempC, float tempF, float heatIndexC, float heatIndexF) {
  
  StaticJsonBuffer<500> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["roomHumidity"] = humidity;
  root["roomTemp"] = tempC;
  // root["acPower"] = acPower;
  
  device.sendState(root);
  Serial.println("Reported!");
}
