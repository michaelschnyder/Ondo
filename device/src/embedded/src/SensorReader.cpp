#include "SensorReader.h"

SensorReader::SensorReader(int gpio, int type) : sensor(gpio, type) { }

void SensorReader::setup() {
    sensor.begin();
}

void SensorReader::loop() {

    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= interval) {
        
        // save the last time we read the data from sensor
        previousMillis = currentMillis;

        if (!updateReadings()) {
            return;
        }

        printLastReadingOnConsole();

        if (updateCallback != NULL) {
            updateCallback(humidity, tempC, tempF, heatIndexC, heatIndexF);
        }

    }
}

void SensorReader::onUpdate(MEASUREMENT_CALLBACK_SIGNATURE) {
    this->updateCallback = updateCallback;
}

bool SensorReader::updateReadings() {

    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    float h = sensor.readHumidity();
    // Read temperature as Celsius (the default)
    float t = sensor.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    float f = sensor.readTemperature(true);

    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t) || isnan(f)) {
        logger.warning("Failed to read from DHT sensor!");
        return false;
    }

    humidity = h;
    tempC = t;
    tempF = f;

    // Compute heat index in Fahrenheit (the default)
    heatIndexF = sensor.computeHeatIndex(f, h);

    // Compute heat index in Celsius (isFahreheit = false)
    heatIndexC = sensor.computeHeatIndex(t, h, false);

    return true;
}

void SensorReader::printLastReadingOnConsole() {
    logger.verbose("New Measurement. Humidity: %4.2f, Temperature:  %4.2f°C (%4.2f°F), Heat index: %4.2f°C (%4.2f°F)", humidity, tempC, tempF, heatIndexC, heatIndexF);
}