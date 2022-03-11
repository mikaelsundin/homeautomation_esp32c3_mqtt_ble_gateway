#include <string.h>
#include <stdint.h>
#include "pm1006.h"
#include <Arduino.h>

/**
 * @brief Ensure that we received all chars within 500ms.
 */
void PM1006::CheckTimeout(void){
    if(millis() - timestamp > 100){
        Reset();
    }
    timestamp = millis();
}

/**
 * @brief Process incomming chars from PM1006.
 */
void PM1006::Process(uint8_t data)
{
    CheckTimeout();


    if(receiving == false){
        //Queue, drop oldest value.
        header[0] = header[1];
        header[1] = header[2];
        header[2] = data;

        PM1006_DEBUG("header %u,%u,%u\n", header[0], header[1], header[2]);

        //Check for HEADER 0x16 0x11 0x0B
        receiving = (header[0] == 0x16) && (header[1] == 0x11) && (header[2] == 0x0B);
    }else{
        buffer[received++] = data;
        checksum += data;

        PM1006_DEBUG("received %u (index: %u)\n", data, received);
        
        if(received >= 17){
            Parse();
            Reset();
        }
    }
}

/**
 * @brief Reset Receiv state
 */
void PM1006::Reset(){
    checksum = 0;
    received = 0;
    receiving = false;
    memset(header, 0x00, 3);
}

/**
 * @brief Parse Received data and call callback.
 */
void PM1006::Parse(){
    uint16_t pm25;

    /* Add header to checksum */
    checksum += (0x16 + 0x11 + 0x0B);

    if(checksum == 0){
        uint16_t pm25_filtered;

        //PM1006 has only 16bit PM2.5 counter
        pm25 = (buffer[2] << 8) | buffer[3];

        //Debug output
        PM1006_DEBUG("PM1006 new value %u\n", pm25);

        //feed filter and call callback if needed
        if(FilterCalculate(pm25, &pm25_filtered)){
            PM1006_DEBUG("PM1006 filtered value %u\n", pm25_filtered);

            if(callback != nullptr){
                callback(pm25_filtered);
            }
        }
    }else{
        PM1006_DEBUG("PM1006 checksum error %u\n", checksum);
    }
}

/**
 * @brief Perform Mean filter.
 * @return true if new filter value is available.
 */
bool PM1006::FilterCalculate(uint16_t pm25, uint16_t *pfiltered){
    uint32_t filtered=0;

    filter[filter_index++] = pm25;

    if(filter_index >= PM1006_FILTER_LENGTH){
        filter_index = 0;

        /* Calculate Mean */
        for(int i=0;i<PM1006_FILTER_LENGTH;i++){
            filtered += filter[i];
        }
        filtered /= PM1006_FILTER_LENGTH;

        *pfiltered = (uint16_t)filtered;
        
        return true;
    }else{
        return false;
    }

}