#ifndef SECRETS_H
#define SECRETS_H

#define DEFAULT_WIFI_SSID			"wifi"
#define DEFAULT_WIFI_PASS			"pass"

//
// Topic example for Manufacture Data
// Only filtered MAC address will be forwared to MQTT.
// 	blegateway/.../manufacure_data/<BLE-DEVICE-MAC>
//
// Example to add/remove BLE DEVICE to be processed
// Payload shall be of format "00:00:00:00:00:00"
//   blegateway/.../filter/add
//   blegateway/.../filter/del
//
// If prefix contain %mac% this will be replaced with WiFi MAC address.
//
#define DEFAULT_MQTT_URI			"1.1.1.1"
#define DEFAULT_MQTT_USER			"user"
#define DEFAULT_MQTT_PASS			"pass"
#define DEFAULT_MQTT_PREFIX		    "blegateway/%mac%"

#endif
