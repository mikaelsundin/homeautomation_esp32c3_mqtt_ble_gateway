#include <nvs.h>
#include <nvs_flash.h>
#include <Arduino.h>
#include <stdint.h>

#include "ble_scanner.h"
#include "ble_whitelist.h"


/**
 * @brief Used for Callback from Nimble BLE.
 */
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks 
{
private:
    void (*callback)(char* mac, char* payload);

public:
    MyAdvertisedDeviceCallbacks(void (*callback)(char* mac, char* payload))
    {
        this->callback = callback;
    }

    void onResult(BLEAdvertisedDevice* advertisedDevice) 
    {
        //Xiaomi ServiceData 
        if(advertisedDevice->haveServiceData()){
            uint8_t buffer[200];
            auto adr =advertisedDevice->getAddress().toString();

            auto serviceData = advertisedDevice->getServiceData(NimBLEUUID((uint16_t)0xfe95));
            if(serviceData.length() != 0)
            { 
                char* dataptr = NimBLEUtils::buildHexData(&buffer[0], 
                                    (uint8_t*)serviceData.data(), 
                                    serviceData.length());

                this->callback((char*)adr.c_str(), (char*)dataptr);
            }


        }

        //Other BLE with ManufacturerData
        if(advertisedDevice->haveManufacturerData()){
            uint8_t buffer[200];
            auto adr =advertisedDevice->getAddress().toString();

            char* dataptr = NimBLEUtils::buildHexData(&buffer[0], 
                                (uint8_t*)advertisedDevice->getManufacturerData().data(), 
                                advertisedDevice->getManufacturerData().length());

            this->callback((char*)adr.c_str(), (char*)dataptr);
        }
    }
};

/**
 * @brief Register callback for Manufacture Data
 */
void BleAdvestingScanner::RegisterManufactureDataCallback(void (*callback)(char* mac, char* payload)){
    manufactureDataCallback = callback;
}


/**
 * @brief Constructor
 */
BleAdvestingScanner::BleAdvestingScanner(){

}

void BleAdvestingScanner::WhitelistAdd(String mac){
    auto adr = NimBLEAddress(mac.c_str());
                
    Serial.printf("WhitelistAdd %s\n", mac);

    /* Update Nimble direcly if initlized */
    if(pBLEScan != nullptr){
        NimBLEDevice::whiteListAdd(adr);
    }

    BleWhitelistStorageAdd(mac);
}


void BleAdvestingScanner::WhitelistDel(String mac){
    auto adr = NimBLEAddress(mac.c_str());
    
    Serial.printf("WhitelistDel %s\n", mac);

    /* Update Nimble direcly if initlized */
    if(pBLEScan != nullptr){
        NimBLEDevice::whiteListRemove(adr);
    }

    BleWhitelistStorageDel(mac);
}

void WhitelistCallback(char *mac){
    auto adr = NimBLEAddress(mac);

    Serial.printf("WhitelistCallback %s\n", mac);

    NimBLEDevice::whiteListAdd(adr);
}


void ScanComplete(NimBLEScanResults result){
    

    
}
/**
 * @brief Call this from application loop
 */
void BleAdvestingScanner::Loop(){
    if(pBLEScan == nullptr){
        Serial.println("start create BLE scan");

        auto cb = new MyAdvertisedDeviceCallbacks(this->manufactureDataCallback);

        //Duplicate filter based on MAC+PAYLOAD
        //This must be called before init..
        NimBLEDevice::setScanFilterMode(CONFIG_BTDM_SCAN_DUPL_TYPE_DATA_DEVICE);
        NimBLEDevice::setScanDuplicateCacheSize(200);

        //Init Nible.
        NimBLEDevice::init("NimBLE");

        //configure Scan
        //TODO: make it active every 5 minutes and passive other 15 minutes.
        pBLEScan = NimBLEDevice::getScan();
        pBLEScan->setAdvertisedDeviceCallbacks(cb, false); 
        pBLEScan->setActiveScan(false);
        pBLEScan->setInterval(300);
        pBLEScan->setFilterPolicy(BLE_HCI_SCAN_FILT_USE_WL);
        pBLEScan->setWindow(300);

        //Refresh whitelist
        BleWhitelistStorageIterate(WhitelistCallback);

        //scan forever
        //ScanComplete
        pBLEScan->start(0, nullptr, false);

        Serial.println("done create BLE scan");
    }
}

