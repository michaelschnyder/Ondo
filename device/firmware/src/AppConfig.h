#ifndef AppConfig_h
#define AppConfig_h

#include <stdint.h>
#include <FS.h>
#include <ArduinoJson.h>
#include <Log4Esp.h>

class AppConfig {
private:
    const String filename = "/config.json";

    char wifiSSID[33];
    char wifiKey[33]; 

    // Name max lenght: 50 (according to azure)
    char azIoTHubName[51];

    // Unknown max lenght, taking 257
    char azIoTSASToken[257]; 

    log4Esp::Logger logger = log4Esp::Logger("AppConfig");
public:
    AppConfig();   
    void load();
    bool update(String& content);

    String getWifiSSID();
    String getWifiKey();
    String getAzIoTHubName();
    String getAzIoTSASToken();
};

#endif