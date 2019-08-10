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
  WiFi.mode(WIFI_STA);    // Station Mode, i.e. connect to a WIFI and don't serve as AP
  WiFi.persistent(false); // Do not store WIFI information in EEPROM.

  logger.trace("Connecting to WLAN with SSID '%s'. This may take some time...", config.getWifiSSID().c_str());

  WiFi.begin(config.getWifiSSID(), config.getWifiKey());

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(100);
  }

  logger.trace("Connected, IP address: %s", WiFi.localIP().toString().c_str());
}

void Application::setGeneratedDeviceId() {
  byte mac[6]; 
  WiFi.macAddress(mac);

  sprintf(deviceId, "Ondo-%2x%2x%2x%2x%2x%2x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

void Application::startupBanner() {
  logger.trace("\nDevice Started");
  logger.trace("-------------------------------------");
  logger.trace("Device Id: %s", deviceId);
  logger.trace("-------------------------------------\n");
}

void Application::initializeFileSystem() {
  if (!SPIFFS.begin()) {
      logger.error("Unable to start filesystem. Formatting now as remediation action...");
      if (!SPIFFS.format()) {
        logger.error("Formatting failed. Unable to recover.");
      }
      else {
        logger.trace("Fomatting succeded. Restarting now.");
        ESP.reset();
      }
  }
  else {
      logger.trace("Filesystem ready. Loading configuration...");    
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