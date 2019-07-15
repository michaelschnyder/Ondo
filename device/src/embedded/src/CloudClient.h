#ifndef CloudClient_h
#define CloudClient_h

#include <stdint.h>
#include <Losant.h>
#include <ESP8266HTTPClient.h>

typedef void (*SetAcCommandCallback)(bool, int16_t, int16_t, bool, bool);

class CloudClient {
private:

    const char* MQTT_DEVICE_ID = "xxxxxxxxxxxxxxxxxxxxxxx";
    const char* MQTT_ACCESS_KEY = "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx";
    const char* MQTT_ACCESS_SECRET = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";

    WiFiClient wifiClient;

    void validateLosantConnection();
    void connectToLosant();
    
public:
    CloudClient(WiFiClient);   
       
    void setup();
    static SetAcCommandCallback setAcCommandCallback = NULL;
    
    void onSetAcCommand(SetAcCommandCallback callback);
	void loop();
    void send(JsonObject& data);
};

#endif
