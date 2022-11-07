# This project is intended to be installed in IKEA VINDRIKTNING.

## Description
It will listen to PM1006 traffic and forward to MQTT over WIFI.

Another function for this code is to forward BLE Advertising Manufacturer Data to MQTT.

It has also support for forward Xiaomi ServiceData, for example MiFlora. 

BLE data will be coded as ascii-HEX values.

## Build the code
* Install VS code
* Install Platformio
* Run Platformio:Build

## BLE features
To listen for BLE Advertising Manufacturer a MAC filter is implemented.

For add/remote from this filter list the MQTT is used. 
The filter is saved in non volatile storage (ESP32 NVS).

Filter add: TOPIC: 'blegateway/GATEWAY-WIFI-MAC/filter/add' payload: '11:22:33:44:55:66'

Filter remove: TOPIC: 'blegateway/GATEWAY-WIFI-MAC/filter/del'  payload: '11:22:33:44:55:66'

# Hardware 
ESP32-C3 based board with pin 5 connected to PM1006 tx.
+5V can be taken from IKEA VINDRIKTNING and supply ESP32-C3 via voltage regulator.

The board must be small enough to fit IKEA VINDRIKTNING. 

Suggested board is Beetle ESP32-C3



## Settings
For setup wifi password and MQTT copy exampel secrets.example.h to secrets.h and change the file.
