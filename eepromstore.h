#ifndef BB_EEPROMSTORE
#define BB_EEPROMSTORE

#define BB_EEPROM_BASE_ADDR 0

#include "Arduino.h"
#include <EEPROM.h>

struct eepromStore {
    int selectedAlgorithm;
    
    int threshold;
    int normalDistance;
    int thresholdPercent;
    int thresholdDistanceCm;
    
    int alarmDuration;
    int alarmFrequency;
    bool alarmEnabeld;

};


void restoreStorage(struct eepromStore &r_storage)
{
    EEPROM.get(BB_EEPROM_BASE_ADDR, r_storage);    
}

void saveStorage(struct eepromStore &r_storage)
{
    EEPROM.put(BB_EEPROM_BASE_ADDR, r_storage);
}

void clearEEPROM()
{
    unsigned int i;
    for (i = 0; i < EEPROM.length(); i++) {
        EEPROM.write(i, 0);
    }
}

#endif
