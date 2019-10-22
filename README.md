# Ondo (温度)
<sup>**Backend**</sup> [![Build status](https://dev.azure.com/ondo-ea/Ondo/_apis/build/status/Cloud)](https://dev.azure.com/ondo-ea/Ondo/_build/latest?definitionId=2)
<sup>**Firmware**</sup> [![Build status](https://dev.azure.com/ondo-ea/Ondo/_apis/build/status/Firmware)](https://dev.azure.com/ondo-ea/Ondo/_build/latest?definitionId=3)

Project that aims to control multiple Daikin aircons by using multiple ESP8266 boards and cloud backing.


# Parts
You might find them here or at other places
* LoLin D1 mini [LoLin Shop on AliExpress](https://www.aliexpress.com/item/D1-mini-Mini-NodeMcu-4M-bytes-Lua-WIFI-Internet-of-Things-development-board-based-ESP8266/32529101036.html) 
* DHT22 Temperature Sensor [Electronicsfan Shop on AliExpress](https://www.aliexpress.com/item/DHT22-AM2302-Digital-Temperature-and-Humidity-Sensor-Replace-SHT11-SHT15/2033122954.html?spm=a2g0s.9042311.0.0.71404c4daJ73kf)
* 5mm 940nm IR infrared LED Lamp (940nm, 1.2-1.4V, 100mA) [Electronicsfan Shop on AliExpress](https://www.aliexpress.com/item/50Pcs-5mm-940nm-IR-infrared-LED-Lamp/32224584146.html?spm=a2g0s.9042311.0.0.71404c4daJ73kf)
* Double Side Prototype PCB Breadboard (40x60mm) [Electronicsfan  Shop on AliExpress](https://www.aliexpress.com/item/10PCS-Double-Side-Prototype-PCB-Bread-board-Tinned-Universal-4x6-cm-40x60-mm-FR4/32230580075.html?spm=a2g0s.9042311.0.0.71404c4daJ73kf)


# How to

## Flash the device in local network
The firmware supports OTA via network interface. It's protected by a password, which has to be provided with the -p switch
`espota.py -f ../build/embedded.ino.bin -a PASSWORD -r -i [IP-Address]` 

### Troubleshooting
Solve **[ERROR]: No response from device** problems
1. Make sure that the firewall allowes phyton.exe to communicate in both private and public networks, as it opens as server to which the ESP8266 will connect to
2. Make sure that you are in the same network as the device, or that the device can at least communicate to your device over TCP/IP

# Resources

## ESP8266
A collection of links related to ESP8266 development boards
* [Boards Overview](https://frightanic.com/iot/comparison-of-esp8266-nodemcu-development-boards/)
* [ESP8266 with Arduino](https://randomnerdtutorials.com/how-to-install-esp8266-board-arduino-ide/)
* [Max Current on ESP8266 Development Board](https://www.esp8266.com/viewtopic.php?f=6&t=9815)
* [LoLin Node MCU v3 Pinout](https://www.instructables.com/id/NodeMCU-ESP8266-Details-and-Pinout/), at Step 7/8
* [SO: LoLin Node MCU v3 BuiltIn LED](https://arduino.stackexchange.com/questions/38477/does-the-node-mcu-v3-lolin-not-have-a-builtin-led) or https://gist.github.com/jhorsman/6a93191ba31a48cf0cea75acd4c20cea

## Azure IoT Hub
* Create a new IoTHub (https://catalog.azureiotsolutions.com/docs?title=Azure/azure-iot-device-ecosystem/setup_iothub)
* IoT Device Explorer, to create SAS tokens (https://catalog.azureiotsolutions.com/docs?title=Azure/azure-iot-device-ecosystem/manage_iot_hub)
* Connecting to Azure IoT Hub via Standard PubSub Arduino Library (http://www.radupascal.com/2016/04/03/esp8266-arduino-iot-hub) 

## Electronics
You might be interested to learn about this
* [SO: Figure out LED's Vf](https://electronics.stackexchange.com/questions/173468/easy-way-to-figure-out-a-leds-vf-in-order-to-pick-an-appropriate-resistor)
* [SO: Hot to drive a 20ma LED from a 4ma max GPIO Pin](https://electronics.stackexchange.com/questions/60865/how-to-drive-a-20ma-led-from-a-4ma-max-gpio-pin)
* [Soldering 101](https://www.instructables.com/id/Soldering-101-for-the-Beginner/)
* Calculate resistor alternatives on [Wolfram Alpha](https://www.instructables.com/id/Soldering-101-for-the-Beginner/)
* Fritzing Design Tool for electronic circuits [Download from fritzing.org](http://fritzing.org/home/)
* Transistor as Switch (German only) on [Elektronik Kompendium](https://www.elektronik-kompendium.de/sites/slt/0208031.htm)
## Arduino
* IRremote ESP8266 Library (https://github.com/markszabo/IRremoteESP8266)
* IR Daikin Server (https://github.com/Andreaqc/IR-Daikin-Server-ESP8266)
* ESP8266 with DHT11/22 Temperature and Humidity Webserver (https://randomnerdtutorials.com/esp8266-dht11dht22-temperature-and-humidity-web-server-with-arduino-ide/)
