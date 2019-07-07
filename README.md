# esp8266-IR-Daikin
Esp8266 IR Daikin AC Control

## Intro
Control Daikin Ar-Conditioning with an ESP8266 and infra-red emitter.
Also we have this options:
  * Receive infra-red from remote control of AC
  * Receive Information from PIR sensor
  * Receive Temperature e Humidity of DHT22
  * Receive Lighting nivel from LDR
Use MQTT to receive and send information to ESP8266 and MQTT Server

## Hardware
* NodeMCU board ESP8266 WiFi module ESP-12E Lua WiFi
* PIR Motion Sensor Module
* Infrared Receiver 38KHz 
* Infrared Emitter 940nm
* LDR - Light Controlled Resistor 1Mohm
* DHT22 temperature-humidity sensor
* Transistor 2N222
* Resistor 10Kohm

![devices](https://github.com/samtd/esp8266-IR-Daikin/blob/master/hardware/Schematic_rev2_tumb.png)

## Software
- Arduino Ide https://www.arduino.cc
- Fritzing https://fritzing.org/
- Trought HTML:
   * Connect to wifi network ??????
   * Open Webpage with the adress 192.168.0.1 and configure your Wifi Network
   * After save and connect to your wifi network, For open web brower select http://hostname or http://hvac_teste
   
  ![html](https://github.com/samtd/esp8266-IR-Daikin/blob/master/hardware/html_rev105.png)

## Libraries
Nome | Link | Versão 
:---: | :---: | ---:
IRremoteESP8266 | [GIT](https://github.com/crankyoldgit/IRremoteESP8266) | `last`

##
Thanks for [David Conran] (https://github.com/crankyoldgit) for contribution of library IRremoteESP8266 and great support
