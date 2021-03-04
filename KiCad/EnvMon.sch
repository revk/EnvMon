EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "Environmental Monitor"
Date "2021-03-04"
Rev "1"
Comp "@TheRealRevK"
Comment1 "www.me.uk"
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L RF_Module:ESP32-WROOM-32 U2
U 1 1 6040A9D3
P 5550 3650
F 0 "U2" H 5550 5231 50  0000 C CNN
F 1 "ESP32-WROOM-32" H 5550 5140 50  0000 C CNN
F 2 "RF_Module:ESP32-WROOM-32" H 5550 2150 50  0001 C CNN
F 3 "https://www.espressif.com/sites/default/files/documentation/esp32-wroom-32_datasheet_en.pdf" H 5250 3700 50  0001 C CNN
	1    5550 3650
	1    0    0    -1  
$EndComp
$Comp
L Connector:USB_C_Receptacle_USB2.0 J6
U 1 1 6040ECF7
P 8850 3200
F 0 "J6" H 8957 4067 50  0000 C CNN
F 1 "USB-C" H 8957 3976 50  0000 C CNN
F 2 "" H 9000 3200 50  0001 C CNN
F 3 "https://www.usb.org/sites/default/files/documents/usb_type-c.zip" H 9000 3200 50  0001 C CNN
	1    8850 3200
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Female J5
U 1 1 6041953F
P 2500 3550
F 0 "J5" H 2528 3526 50  0000 L CNN
F 1 "12V power" H 2528 3435 50  0000 L CNN
F 2 "" H 2500 3550 50  0001 C CNN
F 3 "~" H 2500 3550 50  0001 C CNN
	1    2500 3550
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x04_Male J1
U 1 1 6040AA34
P 2350 4500
F 0 "J1" H 2458 4781 50  0000 C CNN
F 1 "CO2 sensor" H 2458 4690 50  0000 C CNN
F 2 "" H 2350 4500 50  0001 C CNN
F 3 "~" H 2350 4500 50  0001 C CNN
	1    2350 4500
	1    0    0    -1  
$EndComp
$Comp
L RevK:D24V5F3 U1
U 1 1 6042C825
P 2600 6300
F 0 "U1" H 2968 6211 50  0000 L CNN
F 1 "D24V5F3" H 2968 6120 50  0000 L CNN
F 2 "RevK:D24V5F3" H 2600 6500 50  0001 C CNN
F 3 "https://www.pololu.com/product/2842/resources" H 2600 6500 50  0001 C CNN
	1    2600 6300
	1    0    0    -1  
$EndComp
$Comp
L RevK:OLED1.5 U?
U 1 1 604359F8
P 2350 5150
F 0 "U?" H 3119 4861 50  0000 L CNN
F 1 "OLED1.5" H 3119 4770 50  0000 L CNN
F 2 "RevK:OLED1.5" H 2350 5250 50  0001 C CNN
F 3 "" H 2350 5250 50  0001 C CNN
	1    2350 5150
	1    0    0    -1  
$EndComp
$Comp
L Sensor_Temperature:DS18B20 U?
U 1 1 60437DCC
P 5250 6900
F 0 "U?" H 5020 6946 50  0000 R CNN
F 1 "DS18B20" H 5020 6855 50  0000 R CNN
F 2 "Package_TO_SOT_THT:TO-92_Inline" H 4250 6650 50  0001 C CNN
F 3 "http://datasheets.maximintegrated.com/en/ds/DS18B20.pdf" H 5100 7150 50  0001 C CNN
	1    5250 6900
	1    0    0    -1  
$EndComp
$Comp
L RevK:Prog J?
U 1 1 6043AD11
P 2400 2250
F 0 "J?" H 2769 2161 50  0000 L CNN
F 1 "Prog" H 2769 2070 50  0000 L CNN
F 2 "Connector_Molex:Molex_SPOX_5268-04A_1x04_P2.50mm_Horizontal" H 2400 2500 50  0001 C CNN
F 3 "" H 2400 2500 50  0001 C CNN
	1    2400 2250
	1    0    0    -1  
$EndComp
$EndSCHEMATC
