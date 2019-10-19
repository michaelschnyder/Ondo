#ifndef Application_h
#define Application_h

//#include <pins_arduino.h>

#include <ESP8266WiFi.h>

#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Daikin.h>
#include <Log4Esp.h>

#include "AzureIoTMqttClient.h"
#include "SensorReader.h"
#include "RemoteUpdater.h"
#include "AppConfig.h"

#define DHTPIN 0 //D3 = 0
#define DHTTYPE DHT22   

#define IRPIN 4 //D2 = 4
struct AcState { bool devicePower; int targetTempC; int fanMode = 1; bool quietOn = true; bool powerfulOn = false; bool swingHOn = false; bool swingVOn = false; };
struct SensorReading { unsigned long lastUpdate; float tempC; float humidity; };
class Application {

private:

    AppConfig config;
    WiFiClient wifiClient;
    WiFiClientSecure wifiClientSecure;

    IRDaikinESP dakinir;

    SensorReader sensorReader;
    AzureIoTMqttClient azureIoTMqttClient;
    RemoteUpdater remoteUpdater;

    AcState currentAcState;
    SensorReading lastSensorReading;
    log4Esp::Logger logger = log4Esp::Logger("Application");

    char deviceId[10];
    void setupWifi();
    boolean connectToWifi();
    void wireEventHandlers();
    void setGeneratedDeviceId();
    void startupBanner();
    void initializeFileSystem();
    void initializeOTAUpdater();

    void handleSensorUpdate(float humidity, float tempC, float tempF, float heatIndexC, float heatIndexF);
    void handleSetAcCommand(String, JsonObject&);
    void handleDesiredPropertiesUpdate(JsonObject&, int);
    void updateCurrentAcState(JsonObject&);
    void sendAcStateToAircon();
    void publishCurrentSensorReadings();
public:
    Application();   
    void bootstrap();
    void loop();    
};

#endif
