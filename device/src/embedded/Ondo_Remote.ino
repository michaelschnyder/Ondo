#include <DHT.h>
#include <ESP8266WiFi.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ESP8266HTTPClient.h>
#include <Losant.h>
#include <ir_Daikin.h>
#include <pins_arduino.h>

#include "my_credentials.h"
#include "cloudClient.cpp"

#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic
#include <DoubleResetDetector.h>

#define DHTPIN D3
#define DHTTYPE DHT22   
#define IRPIN D2
#define DRD_TIMEOUT 1
#define DRD_ADDRESS 0

DHT dht(DHTPIN, DHTTYPE);

WiFiClient wifiClient;
IRDaikinESP dakinir(IRPIN);
LosantDevice device(LOSANT_DEVICE_ID);
WiFiManager wifiManager;
DoubleResetDetector drd(DRD_TIMEOUT, DRD_ADDRESS);

void setup() {  

  Serial.println("Device Started");
  Serial.println("-------------------------------------");
  
  Serial.begin(115200);
  Serial.setTimeout(2000);

  CloudClient client;
  client.call();

  dht.begin();

  // Wait for serial to initialize.
  while(!Serial) { }

  device.onCommand(&handleCommand);
  

  if (drd.detectDoubleReset()) {
    Serial.println("Resetting WiFi-Information");
    wifiManager.resetSettings();
  } 

  setupAndConnectWifi();
  validateLosantConnection();
  connectToLosant();
}

void setupAndConnectWifi() {
  byte mac[6]; 
  WiFi.macAddress(mac);

  char setupSSID[10];
  sprintf(setupSSID, "Ondo-%2x%2x%2x%2x", mac[2], mac[3], mac[4], mac[5]);

  if (!wifiManager.autoConnect(setupSSID)) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(5000);
  }
}

void connectToWiFi() {
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.print("Authenticating Device...");
  
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

  while(!device.connected()) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("Connected!");
  Serial.println();
  Serial.println("This device is now ready for use!");
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

int timeSinceLastRead = 0;

void loop() {

  drd.loop();

   bool toReconnect = false;

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Disconnected from WiFi");
    toReconnect = true;
  }

  if (!device.connected()) {
    Serial.println("Disconnected from MQTT");
    Serial.println(device.mqttClient.state());
    toReconnect = true;
  }

  if (toReconnect) {
    connectToWiFi();
  }

  device.loop();

  // Report every 2 seconds.
  if(timeSinceLastRead > 2000) {
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    float f = dht.readTemperature(true);

    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t) || isnan(f)) {
      Serial.println("Failed to read from DHT sensor!");
      timeSinceLastRead = 0;
      return;
    }

    // Compute heat index in Fahrenheit (the default)
    float hif = dht.computeHeatIndex(f, h);
    // Compute heat index in Celsius (isFahreheit = false)
    float hic = dht.computeHeatIndex(t, h, false);

    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.print(" *C ");
    Serial.print(f);
    Serial.print(" *F\t");
    Serial.print("Heat index: ");
    Serial.print(hic);
    Serial.print(" *C ");
    Serial.print(hif);
    Serial.println(" *F");

    dakinir.begin();
    bool power = dakinir.getPowerful();
    report(h, t, f, hic, hif, power);

    timeSinceLastRead = 0;
  }
  delay(100);
  timeSinceLastRead += 100;
}

void report(double humidity, double tempC, double tempF, double heatIndexC, double heatIndexF, bool acPower) {
  StaticJsonBuffer<500> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["roomHumidity"] = humidity;
  root["roomTemp"] = tempC;
  root["acPower"] = acPower;
  device.sendState(root);
  Serial.println("Reported!");
}
