# Heat control system

## Overview

The main goal of the System is to control the motor that spreads hot water in a house. There is a number of temperature sensors that send data to Managing System and then it controls the motor.

## Structure

The System consists of sensors based on ESP8266 and DHT22, motor controller based on ESP8266 and Managing System build on [Homeassistant](https://home-assistant.io/)
Data are sent by MQTT over WIFI channel.

## Setup

### Sketches

Sketches are prepared in Arduino IDE. So it should be helpful to compile sketches to your blocks.

#### Steps

* [Prepare Arduino IDE for ESP8266](https://github.com/esp8266/Arduino#installing-with-boards-manager)
* Setup required libraries (ESP8266MQTTClient and ESP8266WiFi are available via libraries manager. DHT should be got somewhere)
* Open sketch (/sketches directory)
* Update `settings.h`. Specify WIFI settings, MQTT broker url and MQTT queue name
* Select Board (Generic ESP8266 Module)
* Select Port
* Connect your ESP8266 module
* Upload the sketch
