#include "Application.h"


int wifiConnectionTimeoutInMs = 10000;

Application::Application(): dakinir(IRPIN), sensorReader(DHTPIN, DHTTYPE), azureIoTMqttClient(Application::config) {
  
}

void Application::boostrap() {
  
  setGeneratedDeviceId();
  startupBanner();
  initializeFileSystem();

  wireEventHandlers();

  config.load();

  setupWifi();

  if (!connectToWifi()) {
    logger.fatal("Can't connect to WIFI. Restarting ESP in 2s");
    delay(2000);
    ESP.reset();
  }
  
  sensorReader.setup();
  azureIoTMqttClient.setup(deviceId);
}

void Application::loop() {
  azureIoTMqttClient.loop();
  sensorReader.loop();

  if(WiFi.status() != WL_CONNECTED) {

    for(int i = 0; i < 12; i++) {
      if (connectToWifi()) {
        return;
      }
    }

    logger.fatal("Wifi connection failed and unable to reconnect after %d trials during %ds. Resetting.", 12, 12 * wifiConnectionTimeoutInMs / 1000);
    ESP.reset();
  }
}

void Application::setupWifi() {
  WiFi.mode(WIFI_STA);    // Station Mode, i.e. connect to a WIFI and don't serve as AP
  WiFi.persistent(false); // Do not store WIFI information in EEPROM.
}

bool Application::connectToWifi() { 

  logger.trace("Connecting to WLAN with SSID '%s'. This may take some time...", config.getWifiSSID().c_str());

  WiFi.begin(config.getWifiSSID(), config.getWifiKey());
  
  long lastAttemptt = millis();
  bool isTimeout = false;

  while (WiFi.status() != WL_CONNECTED && !isTimeout)
  {
    delay(50);
    
    unsigned long currentMillis = millis();
    isTimeout = (currentMillis - lastAttemptt) >= wifiConnectionTimeoutInMs;
  }

  if(isTimeout) {
    logger.error("Could not connect to Wifi with SSID '%s' after %ds", config.getWifiSSID().c_str(), wifiConnectionTimeoutInMs / 1000);
    return false;
  }

  logger.trace("Connected, IP address: %s", WiFi.localIP().toString().c_str());
  return true;
}

void Application::setGeneratedDeviceId() {
  byte mac[6]; 
  WiFi.macAddress(mac);

  sprintf(deviceId, "Ondo-%2x%2x%2x%2x%2x%2x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

void Application::startupBanner() {
  logger.trace("Device Started");
  logger.trace("-------------------------------------");
  logger.trace("Device Id: %s", deviceId);
  logger.trace("-------------------------------------");
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
  azureIoTMqttClient.onCommand([this](String a, JsonObject &b) { handleSetAcCommand(a, b); });
  sensorReader.onUpdate([this](float a, float b, float c, float d, float e) { handleSensorUpdate(a, b, c, d, e); });
}

void Application::handleSensorUpdate(float humidity, float tempC, float tempF, float heatIndexC, float heatIndexF) {
  
  StaticJsonBuffer<500> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["currentHumidity"] = humidity;
  root["currentTempC"] = tempC;
  // root["acPower"] = acPower;
  
  // Send as message to default event bus
  azureIoTMqttClient.send(root);

  // Send individual reported properties
  // azureIoTMqttClient.report("humidity", humidity);
  // azureIoTMqttClient.report("tempC", tempC);

  azureIoTMqttClient.report(root);

}

void Application::handleSetAcCommand(String commandName, JsonObject &root) {
    
    bool status = root["status"];
    bool quiet = root["quiet"];
    bool powerful = root["powerful"];

    int16_t tempC = root["temperature"];
    int16_t fanLevel = root["fan"];

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

    logger.trace("Send command to AC done.");
}