#ifndef BLE_SCANNER_H
#define BLE_SCANNER_H

#include <NimBLEDevice.h>

class BleAdvestingScanner 
{
public:
    void Loop(void);
    void RegisterManufactureDataCallback(void (*callback)(char *mac, char *payload));
    void WhitelistAdd(String mac);
    void WhitelistDel(String mac);
    
    //Singleton pattern
    static BleAdvestingScanner* GetInstance() {
        static BleAdvestingScanner instance;
        return &instance;
    }
private:
    BleAdvestingScanner();
    void OnManufactureData(char *mac, char *payload);

    NimBLEScan* pBLEScan = nullptr;
    void (*manufactureDataCallback)(char *mac, char *payload);



};

#endif

