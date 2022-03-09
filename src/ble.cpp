#include <Arduino.h>
#include <NimBLEDevice.h>

#include "ble.h"
int scanTime = 5; //In seconds
NimBLEScan* pBLEScan;

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks 
{
  void onResult(BLEAdvertisedDevice* advertisedDevice) 
  {

    Serial.printf("Advertised Device: %s \n", advertisedDevice->toString().c_str());

    if(advertisedDevice->haveManufacturerData()){

        auto manufactureData = advertisedDevice->getManufacturerData();

        //haveRSSI

        //Payload will be in bytes
        for(int i=0;i<manufactureData.length();i++){
            Serial.printf("%02X", manufactureData.data()[i]);
        }
        Serial.printf("\n");
    }

  }
};


void ScanDoneHandler(NimBLEScanResults result){
  Serial.print("Devices found: ");
  Serial.println(result.getCount());
  Serial.println("Scan done!");
  
  pBLEScan->clearResults();

  /* Restart SCAN */
  pBLEScan->start(scanTime, ScanDoneHandler, false);
}

void BleScannerInit() {
  NimBLEDevice::init("NimBLE");
  pBLEScan = NimBLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(false);
  pBLEScan->setInterval(100);
  pBLEScan->setFilterPolicy(BLE_HCI_SCAN_FILT_USE_WL);
  pBLEScan->setWindow(99);

  //Is it possible to add to whitelist everywhere?
}

void BleScannerTask(){
    if(pBLEScan->isScanning() == false) {
        pBLEScan->start(0, nullptr, false);
    }
}

void BleScannerWhitelistAdd(const std::string &stringAddress){
  NimBLEDevice::whiteListAdd(NimBLEAddress(stringAddress));
}
