#ifndef AzureIoTMqttClient_h
#define AzureIoTMqttClient_h

#include <stdint.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <Log4Esp.h>

#include "libs/PubSubClient/PubSubClient.h"

#include "AppConfig.h"

#define ONCOMMAND_CALLBACK_SIGNATURE std::function<void(String, JsonObject&)> onCommandCallback
#define DESIREDPROPERTYCHANGE_CALLBACK_SIGNATURE std::function<void(JsonObject&, int)> onDesiredPropertyChangeCallback

class AzureIoTMqttClient {
private:
    BearSSL::WiFiClientSecure wifiClient;
    AppConfig& config;
    PubSubClient client;
    
    ONCOMMAND_CALLBACK_SIGNATURE;
    DESIREDPROPERTYCHANGE_CALLBACK_SIGNATURE;
    void loadCACert();
    boolean connect();
    
    log4Esp::Logger logger = log4Esp::Logger("AzureIoTMqttClient");
    void report(PubSubClient &client, log4Esp::Logger &logger, String path);
    void reconnectIfNecessary();
    bool handleReportedPropertyUpdateResponse(String topic);
    bool handleDesiredPropertiesUpdate(String topic, char *payload, unsigned int length);
    bool handleCloudToDeviceCommand(String topic, char *payload, unsigned int length);

public:
    AzureIoTMqttClient(AppConfig&);   
    void callback(char*, uint8_t*, unsigned int);
    
    void setup(String);
    
    void onCommand(ONCOMMAND_CALLBACK_SIGNATURE);
    void onDesiredPropertyChange(DESIREDPROPERTYCHANGE_CALLBACK_SIGNATURE);
	void loop();
    void send(JsonObject& data);
    void report(String, int);
    void report(String, float);
    void report(String, String);
    void report(JsonObject&);
};

#endif
