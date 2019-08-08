#ifndef CloudClient_h
#define CloudClient_h

#include <stdint.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include "PubSubClient.h"

#include "AppConfig.h"

typedef void (*SetAcCommandCallback)(bool, int16_t, int16_t, bool, bool);

class CloudClient {
private:
    BearSSL::WiFiClientSecure wifiClient;
    AppConfig& config;
    PubSubClient client;
    
    void loadCACert();
    void connect(String);
    
public:
    CloudClient(AppConfig&);   
    
    void setup(String);
    static SetAcCommandCallback setAcCommandCallbackr;
    
    void onSetAcCommand(SetAcCommandCallback callback);
	void loop();
    void send(JsonObject& data);
};

#endif
