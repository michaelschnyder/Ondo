#include <ESP8266WiFi.h>
#include <DHT.h>
#include <SD.h>

// Pins for self Test
#define DHTPIN 0              // D3 = GPIO0
#define DHTTYPE DHT22   

#define LED_IR 4               // D2 = GPIO4
#define LED_BUILTIN 2         // D4 = GPIO2


#define LED_RGB_RED_PIN   16  // D0 = GPIO16
#define LED_RGB_GREEN_PIN 5   // D1 = GPIO5
#define LED_RGB_BLUE_PIN  15   // D8 = GPIO15

#define CS_PIN  2

DHT sensor(DHTPIN, DHTTYPE);


void setup() {

  Serial.begin(115200);
  Serial.setTimeout(2000);

  // Wait for serial to initialize.
  while(!Serial) { }
  Serial.println();

  Serial.println("Self Test has booted.");


  Serial.println("Initializing...");
  Serial.println(" * Sensor");
  sensor.begin();

  Serial.println(" * IR LED Pin");
  pinMode(LED_IR, OUTPUT);

  Serial.println(" * Built-In LED Pin");
  pinMode(LED_BUILTIN, OUTPUT);


  Serial.println(" * Red RGB LED Pin");
  pinMode(LED_RGB_RED_PIN, OUTPUT);
  digitalWrite(LED_RGB_RED_PIN, HIGH); // Off


  Serial.println(" * Green RGB LED Pin");
  pinMode(LED_RGB_GREEN_PIN, OUTPUT);
  digitalWrite(LED_RGB_GREEN_PIN, HIGH); // Off


  Serial.println(" * Blue RGB LED Pin");
  pinMode(LED_RGB_BLUE_PIN, OUTPUT);
  digitalWrite(LED_RGB_BLUE_PIN, HIGH); // OFF

  Serial.println();
  Serial.println();

}

void loop() {
  selfTest();
  delay(2000);
}

void ledTest(int pin, String name, int duration, bool inverse) {
  
  Serial.println(name + " LED");
  Serial.printf(" * Pin %i\n", pin);
  Serial.printf(" * Turning LED on for %ims\n", duration);
  Serial.flush();
  digitalWrite(pin, (inverse ? LOW : HIGH));
  delay(duration);
  Serial.println(" * Turning LED OFF");
  digitalWrite(pin, (inverse ? HIGH : LOW));
  Serial.println();
}

void selfTest() {

  Serial.printf("Started Selftest@%i\n", millis());
  Serial.println("================");

  Serial.println("DHT Sensor");
  Serial.printf(" * PIN %i\n", DHTPIN);
  Serial.printf(" * Type %i\n", DHTTYPE);
  Serial.printf(" * DHT22 Sensor.read(): %i\n", sensor.read());
  Serial.printf(" * Last measurement: Humidity=%4.2f, Temperature: %4.2f°C\n", sensor.readHumidity(), sensor.readTemperature());
  Serial.println();

  ledTest(LED_BUILTIN, "Built-In", 1000, true);
  
  ledTest(LED_IR, "Infrared", 1000, false);
  
  ledTest(LED_RGB_RED_PIN, "RGB: Red", 1000, true);
  ledTest(LED_RGB_BLUE_PIN, "RGB: Blue", 1000, true);
  ledTest(LED_RGB_GREEN_PIN, "RGB: Green", 1000, true);


  Serial.println("SD Card");


  Serial.printf(" * CS_PIN %i\n", CS_PIN);
  bool isCardPresent = SD.begin(CS_PIN);
  Serial.printf(" * Card is present? %i\n", isCardPresent);

  if (isCardPresent) {
    Serial.printf(" * Card Type: %i\n", SD.type());
    Serial.printf(" * Size: %i\n", SD.size64());
    Serial.printf(" * Fat Type: FAT%i\n", SD.fatType());

    if (SD.exists("testdata.txt")) {
      Serial.println(" * Previous testfile.txt found");
      if (SD.remove("testdata.txt")) Serial.println(" * Previous File successfully deleted"); else Serial.println(" * ERROR - existing file not deleted");
    }

    File f = SD.open("testdata.txt", FILE_WRITE);

    if (f) {
      Serial.println(" * File created.");
      f.println("Content of testfile");
      f.print("Current Millis: ");
      f.print(millis());
      for (int i = 0; i < 20; i++) {
        f.println(i);
      }

      f.flush();
      f.close();
    }

    SD.end();
  }
}