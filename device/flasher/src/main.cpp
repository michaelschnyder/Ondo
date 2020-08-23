#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>

#include <ESPAsyncTCP.h>
#include "ESPAsyncWebServer.h"

AsyncWebServer server = AsyncWebServer(80);  

void initializeFileSystem() {
  if (!SPIFFS.begin()) {
      Serial.println(F("Unable to start filesystem. Formatting now as remediation action..."));
      if (!SPIFFS.format()) {
        Serial.println(F("Formatting failed. Unable to recover."));
      }
      else {
        Serial.println(F("Fomatting succeded. Restarting now."));
        ESP.reset();
      }
  }
  else {
      Serial.println(F("Filesystem ready for usage."));    
  }
}

void setupWifi() {
  Serial.begin(115200);
  Serial.println();

  Serial.print("Setting soft-AP ... ");
  boolean result = WiFi.softAP("OndoSetup");
  if(result == true)
  {
    Serial.println("Ready");
  }
  else
  {
    Serial.println("Failed!");
  }
}

void setupWebServer() {
  server.on("/status", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", "Ready");
  });

  server.serveStatic("/api/config", SPIFFS, "/config.json");

  server.on("/api/config", HTTP_POST, [](AsyncWebServerRequest *request){}, NULL, [](AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total) {
    
    Serial.println("Received new configuration");
    
    File jsonFile = SPIFFS.open("/config.json", "w");

    for (size_t i = 0; i < len; i++) {
        
        jsonFile.write(data[i]);
    }

    jsonFile.close();

    request->send(200, "text/plain", "Saved");
  });


  server.begin();

}

void setupOTA() {
    ArduinoOTA.setHostname("FlashMe");
    ArduinoOTA.setPort(8266);
    
    ArduinoOTA.onStart([]() {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH) {
            type = "sketch";
        } else { // U_FS
            type = "filesystem";
        }

        Serial.println("Start updating" + type);
    });
    ArduinoOTA.onEnd([]() {
        Serial.printf("OTA Completed. Restarting.");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Flashing in progress. Done: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
        if (error == OTA_AUTH_ERROR) {
            Serial.printf("Error[%u]: %s\n", (int)error, "Auth Failed");
        } else if (error == OTA_BEGIN_ERROR) {
            Serial.printf("Error[%u]: %s. Free space %i\n", (int)error, "Begin Failed", (int) ESP.getFreeSketchSpace());
        } else if (error == OTA_CONNECT_ERROR) {
            Serial.printf("Error[%u]: %s\n", (int)error, "Connect Failed");
        } else if (error == OTA_RECEIVE_ERROR) {
            Serial.printf("Error[%u]: %s\n", (int)error, "Receive Failed");
        } else if (error == OTA_END_ERROR) {
            Serial.printf("Error[%u]: %s\n", (int)error, "End Failed");
        }
    });    
    
    ArduinoOTA.begin(true);
    Serial.println("OTA ready");
}


void setup() {

  setupWifi();
  initializeFileSystem();
  setupWebServer();
  setupOTA();
}

void loop()
{
      ArduinoOTA.handle();
}