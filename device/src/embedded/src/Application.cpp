#include "Application.h"

Application::Application(): dakinir(IRPIN), sensorReader(DHTPIN, DHTTYPE), cloudClient(Application::config) {
}

void Application::boostrap() {
    
  setGeneratedDeviceId();
  startupBanner();
  initializeFileSystem();

  wireEventHandlers();

  config.load();

  setupAndConnectWifi();
  
  sensorReader.setup();
  cloudClient.setup(deviceId);
}

void Application::loop() {
  cloudClient.loop();
  sensorReader.loop();
}

void Application::setupAndConnectWifi() { 
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

void Application::setGeneratedDeviceId() {
  byte mac[6]; 
  WiFi.macAddress(mac);

  sprintf(deviceId, "Ondo-%2x%2x%2x%2x%2x%2x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

void Application::startupBanner() {
  Serial.println("\nDevice Started");
  Serial.println("-------------------------------------");
  
  Serial.print("Device Id: ");
  Serial.println(deviceId);
  Serial.println("-------------------------------------\n");
}

void Application::initializeFileSystem() {
  if (!SPIFFS.begin()) {
      SPIFFS.format();
      Serial.println("ERROR: Unable to start filesystem.");
  }
  else {
      Serial.println("INFO: Filesystem ready. Loading configuration...");    
  }
}

void Application::wireEventHandlers() {
  cloudClient.onSetAcCommand([this](bool a, int16_t b, int16_t c, bool d, bool e) { handleSetAcCommand(a, b, c, d, e); });
  sensorReader.onUpdate([this](float a, float b, float c, float d, float e) { handleSensorUpdate(a, b, c, d, e); });
}

void Application::handleSensorUpdate(float humidity, float tempC, float tempF, float heatIndexC, float heatIndexF) {
  
  StaticJsonBuffer<500> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["roomHumidity"] = humidity;
  root["roomTemp"] = tempC;
  // root["acPower"] = acPower;
  
  cloudClient.send(root);
  // Serial.println("Reported!");
}

void Application::handleSetAcCommand(bool status, int16_t fanLevel, int16_t tempC, bool quiet, bool powerful) {
    
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