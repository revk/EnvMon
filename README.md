Environmental monitoring with OLED display working over MQTT

Note that this is the original version, designed for 1.5 mono OLED display.

* Uses ESP32-RevK library for MQTT/settings
* Uses 128x128 1.5" OLED display
* Uses SCD30 CO2 sensor
* Used DS18B20 Temp sensor

The design makes for a small cuboid unit powered by a USB cable with
an OLED display showing CO2, temperature, and humidity.

It can also be set with CO2 level and temperature levels which can trigger
MQTT messages allowing control of fans or heating systems.

The design is intended to work with a leaded external DS18B20 temperature
sensor, allowing more accurate temperature to be measured where needed and
without heat from components impacting the reading.

The CO2 sensor used is an expensive NDIR module providing accurate CO2
measurement which is not fooled by farts, etc.

A PCB design is included based on milling tracks. There is also a PCB
design in the ESP32-OLED project which uses a professionally printed
PCB layout.

(c) 2019-21 Andrews & Arnold Ltd, Adrian Kennard. See LICENSE file (GPL).
