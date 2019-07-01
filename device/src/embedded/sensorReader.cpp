#include <DHT.h>


class SensorReader {
 
    private:
        DHT sensor;
        unsigned long previousMillis = 0;
        
        // update interval:
        const long interval = 1000; 

        void updateReadings() {

            // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
            float h = sensor.readHumidity();
            // Read temperature as Celsius (the default)
            float t = sensor.readTemperature();
            // Read temperature as Fahrenheit (isFahrenheit = true)
            float f = sensor.readTemperature(true);

            // Check if any reads failed and exit early (to try again).
            if (isnan(h) || isnan(t) || isnan(f)) {
                Serial.println("Failed to read from DHT sensor!");
                return;
            }

            humidity = h;
            tempC = t;
            tempF = f;

            // Compute heat index in Fahrenheit (the default)
            heatIndexF = sensor.computeHeatIndex(f, h);

            // Compute heat index in Celsius (isFahreheit = false)
            heatIndexC = sensor.computeHeatIndex(t, h, false);
        }

        void printLastReadingOnConsole() {
            
            Serial.print("Humidity: ");
            Serial.print(humidity);
            Serial.print(" %\t");
            Serial.print("Temperature: ");
            Serial.print(tempC);
            Serial.print(" *C ");
            Serial.print(tempF);
            Serial.print(" *F\t");
            Serial.print("Heat index: ");
            Serial.print(heatIndexC);
            Serial.print(" *C ");
            Serial.print(heatIndexF);
            Serial.println(" *F");
        }

    public:
        SensorReader(int gpio, int type) : sensor(gpio, type) { }

        float tempF;
        float tempC;
        float humidity;
        float heatIndexC;
        float heatIndexF;

        void setup() {
            sensor.begin();
        }

        void loop() {

            unsigned long currentMillis = millis();

            if (currentMillis - previousMillis >= interval && previousMillis + interval < 0) {
                // save the last time we read the data the LED
                previousMillis = currentMillis;

                updateReadings();
                printLastReadingOnConsole();
            }
        }
};