#include <DHT.h>

#ifndef SensorReader_h
#define SensorReader_h

typedef void (*MeasurementCallback)(float, float, float, float, float);

class SensorReader {
 
 private:
        DHT sensor;
        unsigned long previousMillis = 0;
        
        // update interval:
        const long interval = 2000; 

        MeasurementCallback updateCallback = NULL;

        void updateReadings();
        void printLastReadingOnConsole();

    public:
        SensorReader(int gpio, int type);

        float tempC;
        float tempF;
        float humidity;
        float heatIndexC;
        float heatIndexF;

        void setup();
        void loop();
        void onUpdate(MeasurementCallback callback);
};

#endif