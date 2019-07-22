#ifndef CloudClient_h
#define CloudClient_h

#include <stdint.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

typedef void (*SetAcCommandCallback)(bool, int16_t, int16_t, bool, bool);

class CloudClient {
private:
    WiFiClient wifiClient;

    void connect();
    
public:
    CloudClient(WiFiClient);   
    
    void setup();
    static SetAcCommandCallback setAcCommandCallbackr;
    
    void onSetAcCommand(SetAcCommandCallback callback);
	void loop();
    void send(JsonObject& data);
};

#endif
