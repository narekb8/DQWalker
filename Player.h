#include "Arduino.h"
#include "Monster.h"

#ifndef Player_h
#define Player_h

class Player
{
    public:
        Player(bool);
        uint64_t lifetimeCount;
        uint8_t mult;
        uint8_t size;
        Monster *getPrimary();
        Monster *getCurr();
        Monster *setPrimary(uint8_t);
        Monster *setPrimary();
        Monster *getNext();
        Monster *getPrev();
        Monster *breedNew(uint8_t, uint8_t);
        void updateSteps(uint64_t);
        void updateEXP(uint16_t);
    private:
        uint8_t primary;
        uint8_t curr;
        Monster mon[5];
};

#endif Player_h