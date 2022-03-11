#ifndef PM1006_H
#define PM1006_H

#include <stdint.h>

/* Shared filter Length */
#define PM1006_FILTER_LENGTH    8

//Debug change if debug is needed
//#define PM1006_DEBUG(str, ...)  Serial.printf(str, __VA_ARGS__)
#define PM1006_DEBUG(str, ...)

class PM1006{
public:
    PM1006(){
        Reset();
    }

    void Process(uint8_t data);


    void RegisterCallback(void (*callback)(uint16_t pm25)){
        this->callback = callback;
    };

private:
    void Parse(void);
    void Reset(void);
    void CheckTimeout(void);
    bool FilterCalculate(uint16_t value, uint16_t *pfiltered);


    void (*callback)(uint16_t pm25) = nullptr;
    bool receiving;
    uint8_t header[3];
    uint8_t buffer[17];
    uint8_t received;
    uint8_t checksum;

    uint16_t filter[PM1006_FILTER_LENGTH];
    uint8_t filter_index=0;

    uint32_t timestamp;

};



#endif
