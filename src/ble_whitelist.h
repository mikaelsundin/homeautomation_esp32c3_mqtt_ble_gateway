#ifndef BLE_WHITELIST_H
#define BLE_WHITELIST_H


#define BLE_WHITELIST_DEBUG(str, ...)  Serial.printf(str, __VA_ARGS__)
//#define BLE_WHITELIST_DEBUG(str, ...)


void BleWhitelistStorageAdd(String device_mac);
void BleWhitelistStorageDel(String device_mac);
void BleWhitelistStorageIterate(void (*callback)(char *mac));


#endif