#ifndef BLE_ALLOWLIST_H
#define BLE_ALLOWLIST_H


#define BLE_ALLOWLIST_DEBUG(str, ...)  Serial.printf(str, __VA_ARGS__)
//#define BLE_WHITELIST_DEBUG(str, ...)


void BleAllowlistStorageAdd(String device_mac);
void BleAllowlistStorageDel(String device_mac);
void BleAllowlistStorageIterate(void (*callback)(char *mac));


#endif