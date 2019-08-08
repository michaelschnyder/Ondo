#include "AppConfig.h"

AppConfig::AppConfig() {
}

void AppConfig::load() {
    if (!SPIFFS.exists(AppConfig::filename)) {
        
        Serial.println("Cannot find configuration at '" + AppConfig::filename + "'");
        return;
    }

    File jsonFile;
    jsonFile = SPIFFS.open(AppConfig::filename, "r");

    if (!jsonFile) {
        Serial.println("Cannot open configuration file '" + AppConfig::filename + "'");
        return;
    }

    // Allocate a buffer to store contents of the file.
    StaticJsonBuffer<512> jsonBuffer;
    JsonObject &root = jsonBuffer.parseObject(jsonFile);

    if (root.success()) {
        strcpy(AppConfig::wifiSSID, root["wifiSSID"]);
        strcpy(AppConfig::wifiKey, root["wifiKey"]);
        strcpy(AppConfig::azIoTHubName, root["azIoTHubName"]);
        strcpy(AppConfig::azIoTSASToken, root["azIoTSASToken"]);
        Serial.println("Configuration loaded.");

    } else {
        Serial.println("failed to load json config");
    }

    jsonFile.close();

}

String AppConfig::getWifiSSID() {
    return AppConfig::wifiSSID;
}

String AppConfig::getWifiKey() {
    return AppConfig::wifiKey;
}

String AppConfig::getAzIoTHubName() {
    return AppConfig::azIoTHubName;
}

String AppConfig::getAzIoTSASToken() {
    return AppConfig::azIoTSASToken;
}