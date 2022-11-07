# This project is intended to be installed in IKEA VINDRIKTNING.

## Description
It will listen to PM1006 traffic and forward to MQTT over WIFI.

Another function for this code is to forward BLE Advertising Manufacturer Data to MQTT.

It has also support for forward Xiaomi ServiceData, for example MiFlora. 

## BLE features
To listen for BLE Advertising Manufacturer a MAC filter is implemented.

For add/remote from this filter list the MQTT is used. 
The filter is saved in non volatile storage (ESP32 NVS).

Filter add: TOPIC: 'blegateway/GATEWAY-WIFI-MAC/filter/add' payload: '11:22:33:44:55:66'

Filter remove: TOPIC: 'blegateway/GATEWAY-WIFI-MAC/filter/del'  payload: '11:22:33:44:55:66'


## Settings
For setup wifi password and MQTT copy exampel secrets.example.h to secrets.h and change the file.
