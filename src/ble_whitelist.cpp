#include <nvs.h>
#include <nvs_flash.h>
#include <esp_err.h>
#include <stdint.h>
#include <NimBLEDevice.h>
#include <esp_bt_defs.h>
#include "ble_whitelist.h"


#define ESP_BD_ADDR_STR_COMPRESSED "%02X%02X%02X%02X%02X%02X"

const char ble_whitelist_partition[] = "nvs";
const char ble_whitelist_namespace[] = "ble_whitelist";


String ble_mac_to_nvs_key(String mac){
    mac.replace(":","");
    return mac;
}

/**
 * @brief Add MAC address whitelist
 */
void BleWhitelistStorageAdd(String device_mac){
    nvs_handle_t handle;
    esp_err_t err;

    auto name = ble_mac_to_nvs_key(device_mac);

    err = nvs_open(ble_whitelist_namespace, NVS_READWRITE, &handle);
    if (err != ESP_OK){
        BLE_WHITELIST_DEBUG("BLE_Whitelist_Add nvs open failed: %s", esp_err_to_name(err));
    }

    err = nvs_set_u64(handle, name.c_str(), 0);
    if (err != ESP_OK){
        BLE_WHITELIST_DEBUG("BLE_Whitelist_Add extension_nvs_set_bd_adr: %s", esp_err_to_name(err));
    }

    err = nvs_commit(handle);
    if (err != ESP_OK){
        BLE_WHITELIST_DEBUG("BLE_Whitelist_Add nvs_commit: %s", esp_err_to_name(err));
    }

    BLE_WHITELIST_DEBUG("BLE_Whitelist_Add done %s\n", name.c_str());

    nvs_close(handle);
}

/**
 * @brief Delete MAC address whitelist
 */
void BleWhitelistStorageDel(String device_mac){
    nvs_handle_t handle;
    esp_err_t err;

    auto name = ble_mac_to_nvs_key(device_mac);

    err = nvs_open(ble_whitelist_namespace, NVS_READWRITE, &handle);
    if (err != ESP_OK){
        BLE_WHITELIST_DEBUG("BLE_Whitelist_Add nvs open failed: %s", esp_err_to_name(err));
    }

    err = nvs_erase_key(handle, name.c_str());
    if (err != ESP_OK){
        BLE_WHITELIST_DEBUG("BLE_Whitelist_Add extension_nvs_set_bd_adr: %s", esp_err_to_name(err));
    }

    err = nvs_commit(handle);
    if (err != ESP_OK){
        BLE_WHITELIST_DEBUG("BLE_Whitelist_Add nvs_commit: %s", esp_err_to_name(err));
    }


    nvs_close(handle);
}

/**
 * @brief Convert nvs compact name to XX:XX:XX:XX:XX:XX format
 */
void BleWhitelistNameToMac(char packed_mac[12], char output[18]){
    char *ptr = &output[0]; 

    //Update this and check if packed_mac is only hexadecimal before contructring the new list.

    *ptr++ = packed_mac[0];
    *ptr++ = packed_mac[1];
    *ptr++ = ':';
    *ptr++ = packed_mac[2];
    *ptr++ = packed_mac[3];
    *ptr++ = ':';
    *ptr++ = packed_mac[4];
    *ptr++ = packed_mac[5];
    *ptr++ = ':';
    *ptr++ = packed_mac[6];
    *ptr++ = packed_mac[7];
    *ptr++ = ':';
    *ptr++ = packed_mac[8];
    *ptr++ = packed_mac[9];
    *ptr++ = ':';
    *ptr++ = packed_mac[10];
    *ptr++ = packed_mac[11];
    *ptr++ = '\0';
}


void BleWhitelistStorageIterate(void (*callback)(char *mac)){
    esp_err_t err;
    nvs_handle_t handle;
    nvs_iterator_t it;
    nvs_type_t type = NVS_TYPE_U64;
    
    it = nvs_entry_find(ble_whitelist_partition, ble_whitelist_namespace, type);
    err = nvs_open_from_partition(ble_whitelist_partition, ble_whitelist_namespace, NVS_READONLY, &handle);

    /* ensure correct iterator and nvs handle */
    if(it == NULL || err != ESP_OK){
        return;
    }

    /* Go over list and call callback on success */
    while(it != NULL){
        char mac[18];

        nvs_entry_info_t info;
        nvs_entry_info(it, &info);

        //Convert compact format to XX:XX:XX:XX:XX:XX
        BleWhitelistNameToMac(info.key, mac);

        if(callback != nullptr){
            callback(mac);
        }

        it = nvs_entry_next(it);
    }

    /* close resources */
    nvs_close(handle);
    nvs_release_iterator(it);

}