#include "src/Application.h"

Application app;

void setup() {

  Serial.begin(115200);
  Serial.setTimeout(2000);

  // Wait for serial to initialize.
  while(!Serial) { }

  Serial.println("\nDevice Started");
  Serial.println("-------------------------------------");
  
  app.boostrap();
}

void loop() {

  app.loop();
}