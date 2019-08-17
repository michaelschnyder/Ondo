#include "RemoteUpdater.h"
#include <ArduinoOTA.h>

void RemoteUpdater::setup(String deviceId) {
    // Port defaults to 8266
    ArduinoOTA.setPort(8266);

    ArduinoOTA.setHostname(deviceId.c_str());

    // Password can be set with it's md5 value as well
    ArduinoOTA.setPasswordHash("7cdaf4ec9f030cc289c4efc2a9d2d551");

    ArduinoOTA.onStart([this]() {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH) {
            type = "sketch";
        } else { // U_FS
            type = "filesystem";
            SPIFFS.end();
        }

        logger.trace("Start updating " + type);
    });
    ArduinoOTA.onEnd([this]() {
        logger.trace("OTA Completed. Restarting.");
    });
    ArduinoOTA.onProgress([this](unsigned int progress, unsigned int total) {
        logger.verbose("Flashing in progress. Done: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([this](ota_error_t error) {
        if (error == OTA_AUTH_ERROR) {
            logger.error("Error[%u]: %s", (int)error, "Auth Failed");
        } else if (error == OTA_BEGIN_ERROR) {
            logger.error("Error[%u]: %s", (int)error, "Begin Failed");
        } else if (error == OTA_CONNECT_ERROR) {
            logger.error("Error[%u]: %s", (int)error, "Connect Failed");
        } else if (error == OTA_RECEIVE_ERROR) {
            logger.error("Error[%u]: %s", (int)error, "Receive Failed");
        } else if (error == OTA_END_ERROR) {
            logger.error("Error[%u]: %s", (int)error, "End Failed");
        }
    });
    ArduinoOTA.begin(true);
}

void RemoteUpdater::handle() {
    ArduinoOTA.handle();
}