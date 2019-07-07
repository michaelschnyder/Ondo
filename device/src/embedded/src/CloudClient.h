#ifndef CloudClient_h
#define CloudClient_h

#include <stdint.h>
#include <Losant.h>
#include <ESP8266HTTPClient.h>

typedef void (*SetAcCommandCallback)(bool, int16_t, int16_t, bool, bool);

class CloudClient {
private:

    const char* LOSANT_DEVICE_ID = "xxxxxxxxxxxxxxxxxxxxxxx";
    const char* LOSANT_ACCESS_KEY = "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx";
    const char* LOSANT_ACCESS_SECRET = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";

    WiFiClient wifiClient;
    LosantDevice device;
    void validateLosantConnection();
    void connectToLosant();
    void handleCommand(LosantCommand *command);

    SetAcCommandCallback setAcCommandCallback = NULL;
public:
    CloudClient(WiFiClient);   
    void setup();
    void onSetAcCommand(SetAcCommandCallback callback);
	void loop();
    void send(JsonObject& data);
};

#endif
