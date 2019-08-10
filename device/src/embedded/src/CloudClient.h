#ifndef CloudClient_h
#define CloudClient_h

#include <stdint.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include "PubSubClient.h"

#include "AppConfig.h"

#define SETACCOMMAND_CALLBACK_SIGNATURE std::function<void(bool, int16_t, int16_t, bool, bool)> onSetAcCommandCallback

class CloudClient {
private:
    BearSSL::WiFiClientSecure wifiClient;
    AppConfig& config;
    PubSubClient client;
    
    SETACCOMMAND_CALLBACK_SIGNATURE;
    void loadCACert();
    void connect(String);
    
public:
    void callback(char*, uint8_t*, unsigned int);
    CloudClient(AppConfig&);   
    
    void setup(String);
    
    void onSetAcCommand(SETACCOMMAND_CALLBACK_SIGNATURE);
	void loop();
    void send(JsonObject& data);
};

#endif
