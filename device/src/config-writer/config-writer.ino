void setup() {
  // put your setup code here, to run once:

  /*
  
  How to flash files to ESP8266
  =============================


  1. You need arduino studio
  2. Install the ESP8266 SPIFFS Flash Tool for android studio
     Follow this guide: http://arduino.esp8266.com/Arduino/versions/2.0.0/doc/filesystem.html#uploading-files-to-file-system
     (Basically download the jar, place in tools folder, and restart studion)
     --> The option "ESP8266 Skatch Data Upload" should be visible in the "Tools" menu
  3. Add or create the data folder and add any files there
  4. Make sure you have a correct config, like
     - Board: "NodeMCU 1.0 (ESP12E Module)" or "WeMos D1 R1"
     - Flash-Size: "4M (3M SPIFFS)"
  5. Disconnet any Serial Monitor from the COm-Port
  6. Flash via Tools -> ESP8266 Skatch Data Upload

  ATTENTION: Flashing the firmware *must* have the exact same Flash
  config when programming. Otherwise files would be corrupted
   */
}

void loop() {
  // put your main code here, to run repeatedly:

}
