#include "Application.h"

int const ONE_SECOND_IN_MS = 1000;
int wifiConnectionTimeoutInMs = 10 * ONE_SECOND_IN_MS;
int lastSensorUpdateSent = 0;
int lastNetworkBSSIInfoSent = 0;

Application::Application(): dakinir(IRPIN), sensorReader(DHTPIN, DHTTYPE), azureIoTMqttClient(Application::config) {
  
}

void Application::bootstrap() {
  
  setGeneratedDeviceId();
  startupBanner();
  initializeFileSystem();
  setupWebServer();
  wireEventHandlers();

  config.load();

  setupWifi();

  if (!connectToWifi()) {
    logger.fatal(F("Can't connect to WIFI. Restarting ESP in 2s"));
    delay(2000);
    ESP.reset();
  }
  
  remoteUpdater.setup(deviceId);
  server.begin();

  sensorReader.setup();
  azureIoTMqttClient.setup(deviceId);

  publishCurrentNetworkInfo();
}

void Application::loop() {

  remoteUpdater.handle();

  azureIoTMqttClient.loop();
  sensorReader.loop();

  if(WiFi.status() != WL_CONNECTED) {
    logger.warning(F("Wifi connection was interrupted. Trying to re-astablish connection."));
    
    for(int i = 0; i < 12; i++) {
      if (connectToWifi()) {
        logger.trace(F("Wifi connection successfully re-established."));
        publishCurrentNetworkInfo();
        return;
      }
    }

    logger.fatal(F("Wifi connection failed and unable to reconnect after %d trials during %ds. Resetting."), 12, 12 * wifiConnectionTimeoutInMs / 1000);
    ESP.reset();
  }

  if(millis() - lastSensorUpdateSent >= 60 * ONE_SECOND_IN_MS) {
    publishCurrentSensorReadings();
  }

  if(millis() - lastNetworkBSSIInfoSent >= 3600 * ONE_SECOND_IN_MS) {
    publishCurrentBSSIInfo();
  }
}

void Application::setupWifi() {
  WiFi.mode(WIFI_STA);    // Station Mode, i.e. connect to a WIFI and don't serve as AP
  WiFi.persistent(false); // Do not store WIFI information in EEPROM.
}

bool Application::connectToWifi() { 

  logger.trace(F("Connecting to WLAN with SSID '%s'. This may take some time..."), config.getWifiSSID().c_str());

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
    logger.error(F("Could not connect to Wifi with SSID '%s' after %ds"), config.getWifiSSID().c_str(), wifiConnectionTimeoutInMs / 1000);
    return false;
  }

  logger.trace(F("Connected, IP address: %s"), WiFi.localIP().toString().c_str());
  return true;
}

void Application::setGeneratedDeviceId() {
  byte mac[6]; 
  WiFi.macAddress(mac);

  sprintf(deviceId, "Ondo-%02x%02x%02x%02x%02x%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

void Application::startupBanner() {
  logger.trace(F("Device Started"));
  logger.trace(F("-------------------------------------"));
  logger.trace(F("(Device Id: %s)"), deviceId);
  logger.trace(__FILE__ " " __DATE__ " " __TIME__);
  logger.trace(F("-------------------------------------"));
}

void Application::initializeFileSystem() {
  if (!SPIFFS.begin()) {
      logger.error(F("Unable to start filesystem. Formatting now as remediation action..."));
      if (!SPIFFS.format()) {
        logger.error(F("Formatting failed. Unable to recover."));
      }
      else {
        logger.trace(F("Fomatting succeded. Restarting now."));
        ESP.reset();
      }
  }
  else {
      logger.trace(F("Filesystem ready for usage."));    
  }
}

void Application::setupWebServer() {

  server.on("/status", HTTP_GET, [this](AsyncWebServerRequest *request){
    request->send(200, "text/plain", "Ready");
  });

  server.serveStatic("/api/config", SPIFFS, "/config.json");

  server.on("/api/config", HTTP_POST, [this](AsyncWebServerRequest *request){}, NULL, [this](AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total) {
    
    logger.trace("Received new configuration");
    
    char content[len];

    for (size_t i = 0; i < len; i++) {
        content[i] = data[i];
    }

    content[len] = '\0';
    String c = String(content);
    if (Application::config.update(c)) {
      request->send(200, "text/plain", content);
    }
    else {
      request->send(500, "text/plain", "Update of configuration failed.");
    }
  });
}

void Application::wireEventHandlers() {
  azureIoTMqttClient.onCommand([this](String a, JsonObject &b) { handleSetAcCommand(a, b); });
  azureIoTMqttClient.onDesiredPropertyChange([this](JsonObject &a, int b) { handleDesiredPropertiesUpdate(a, b); });

  sensorReader.onUpdate([this](float a, float b, float c, float d, float e) { handleSensorUpdate(a, b, c, d, e); });
}

void Application::handleSensorUpdate(float humidity, float tempC, float tempF, float heatIndexC, float heatIndexF) {
  
  bool hasChanged = (humidity != lastSensorReading.humidity || tempC != lastSensorReading.tempC);
  lastSensorReading.lastUpdate = millis();
  
  if (hasChanged) {   
    lastSensorReading.humidity = humidity;
    lastSensorReading.tempC = tempC;

    publishCurrentSensorReadings();
  }
}

void Application::handleDesiredPropertiesUpdate(JsonObject &properties, int version) {
  updateCurrentAcState(properties);
  sendAcStateToAircon();
}

void Application::handleSetAcCommand(String commandName, JsonObject &root) {
  updateCurrentAcState(root);
  sendAcStateToAircon();
}

void Application::updateCurrentAcState(JsonObject &settings) {
  currentAcState.devicePower = settings.containsKey("devicePower") ? settings["devicePower"] : true;
  currentAcState.targetTempC = settings.containsKey("targetTempC") ? settings["targetTempC"] : 25;
  currentAcState.fanMode = settings.containsKey("fanMode") ? settings["fanMode"] : 1;

  currentAcState.quietOn =    settings.containsKey("quietOn") ? settings["quietOn"] : true;
  currentAcState.powerfulOn = settings.containsKey("powerfulOn") ? settings["powerfulOn"] : false ;
  currentAcState.swingHOn =   settings.containsKey("swingHOn") ? settings["swingHOn"] : false;
  currentAcState.swingVOn =   settings.containsKey("swingVOn") ? settings["swingVOn"] : false;
}

void Application::sendAcStateToAircon() {
    
    dakinir.begin();

    if(currentAcState.devicePower){
      dakinir.on();
    }else{
      dakinir.off();
    }

    dakinir.setFan(currentAcState.fanMode);
    dakinir.setMode(kDaikinCool);
    dakinir.setTemp(currentAcState.targetTempC);
    dakinir.setQuiet(currentAcState.quietOn);
    dakinir.setPowerful(currentAcState.powerfulOn);
    
    dakinir.setSwingHorizontal(currentAcState.swingHOn);
    dakinir.setSwingVertical(currentAcState.swingVOn);

    dakinir.send();

    logger.trace("AC IR Update send. DevicePower: %i, TargetTemp: %i, FanMode: %i, QuietOn: %i, PowerfulOn: %i, SwingHOn: %i, SwingVOn: %i", 
          currentAcState.devicePower, currentAcState.targetTempC, currentAcState.fanMode, currentAcState.quietOn, currentAcState.powerfulOn, currentAcState.swingHOn, currentAcState.swingVOn);
}

void Application::publishCurrentSensorReadings() {
 StaticJsonBuffer<500> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();

  JsonObject& sensor = root.createNestedObject("sensor");

  sensor["humidity"] = lastSensorReading.humidity;
  sensor["tempC"] = lastSensorReading.tempC;
  
  // Send as message to default event bus
  azureIoTMqttClient.send(root);

  // update reported property
  azureIoTMqttClient.report(root);

  lastSensorUpdateSent = millis();
}

void Application::publishCurrentNetworkInfo() {
  StaticJsonBuffer<500> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();

  JsonObject& network = root.createNestedObject("network");

  network["mode"] = "wifi";
  network["ssid"] = WiFi.SSID();
  network["rssi"] = WiFi.RSSI();
  network["mac"] = WiFi.macAddress();
  network["ip"] = WiFi.localIP().toString();
  network["subnet"] = WiFi.subnetMask().toString();
  network["gateway"] = WiFi.gatewayIP().toString();

  // Send as message to default event bus
  azureIoTMqttClient.send(root);

  // update reported property
  azureIoTMqttClient.report(root);

  lastNetworkBSSIInfoSent = millis();
}

void Application::publishCurrentBSSIInfo() {
  StaticJsonBuffer<500> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();

  JsonObject& network = root.createNestedObject("network");

  network["mode"] = "wifi";
  network["rssi"] = WiFi.RSSI();

  // Send as message to default event bus
  azureIoTMqttClient.send(root);

  // update reported property
  azureIoTMqttClient.report(root);
  lastNetworkBSSIInfoSent = millis();
}