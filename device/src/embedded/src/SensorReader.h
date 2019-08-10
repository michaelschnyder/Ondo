
#ifndef SensorReader_h
#define SensorReader_h

#include <DHT.h>
#include <functional>
#include <Log4Esp.h>

#define MEASUREMENT_CALLBACK_SIGNATURE std::function<void(float, float, float, float, float)> updateCallback

//typedef void (*MeasurementCallback)(float, float, float, float, float);

class SensorReader {
 
 private:
        DHT sensor;
        unsigned long previousMillis = 0;
        
        // update interval:
        const long interval = 2000; 

        MEASUREMENT_CALLBACK_SIGNATURE;

        void updateReadings();
        void printLastReadingOnConsole();

        log4Esp::Logger logger = log4Esp::Logger("SensorReader");
    public:
        SensorReader(int gpio, int type);

        float tempC;
        float tempF;
        float humidity;
        float heatIndexC;
        float heatIndexF;

        void setup();
        void loop();
        void onUpdate(MEASUREMENT_CALLBACK_SIGNATURE);
};

#endif