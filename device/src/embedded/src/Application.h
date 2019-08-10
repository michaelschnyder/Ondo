#ifndef Application_h
#define Application_h

//#include <pins_arduino.h>

#include <ESP8266WiFi.h>

#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Daikin.h>

#include "CloudClient.h"
#include "SensorReader.h"
#include "AppConfig.h"

#define DHTPIN 0 //D3 = 0
#define DHTTYPE DHT22   

#define IRPIN 4 //D2 = 4

class Application {

private:

    AppConfig config;
    WiFiClient wifiClient;
    WiFiClientSecure wifiClientSecure;

    IRDaikinESP dakinir;

    SensorReader sensorReader;
    CloudClient cloudClient;

    char deviceId[10];
    void setupAndConnectWifi();
    void wireEventHandlers();
    void setGeneratedDeviceId();
    void startupBanner();
    void initializeFileSystem();

    void handleSensorUpdate(float humidity, float tempC, float tempF, float heatIndexC, float heatIndexF);
    void handleSetAcCommand(bool status, int16_t fanLevel, int16_t tempC, bool quiet, bool powerful);
public:
    Application();   
    void boostrap();
    void loop();    
};

#endif
