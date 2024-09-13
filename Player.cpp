#include "Arduino.h"
#include "Player.h"

Player::Player(bool debug)
{
    curr = 0;
    primary = 0;
    lifetimeCount = 0;
    mult = 1;

    if(debug)
        size = 5;
    else
        size = 3;

    for(int i = 0; i < size; i++)
        mon[i] = Monster((species)i, random(2));
}

Player::Player()
{
    curr = 0;
    primary = 0;
    lifetimeCount = 0;
    mult = 1;

    size = 1;

    for(int i = 0; i < size; i++)
        mon[i] = Monster((species)i, random(2));
}

Monster *Player::getPrimary()
{
    return &mon[primary];
}

Monster *Player::breedNew(uint8_t one, uint8_t two)
{
    uint16_t atk1 = mon[one].atk;
	uint16_t def1 = mon[one].def;
	uint16_t agl1 = mon[one].agl;
	uint16_t intl1 = mon[one].intl;
	uint16_t hp1 = mon[one].hp;
	uint16_t mp1 = mon[one].mp;

    uint16_t atk2 = mon[two].atk;
	uint16_t def2 = mon[two].def;
	uint16_t agl2 = mon[two].agl;
	uint16_t intl2 = mon[two].intl;
	uint16_t hp2 = mon[two].hp;
	uint16_t mp2 = mon[two].mp;

    Monster bred = Monster(mon[two].breed, random(2));

    bred.atk += (atk1 + atk2) / 4;
    bred.def += (def1 + def2) / 4;
    bred.agl += (agl1 + agl2) / 4;
    bred.intl += (intl1 + intl2) / 4;
    bred.hp += (hp1 + hp2) / 4;
    bred.mp += (mp1 + mp2) / 4;

    return &bred;
}

Monster *Player::setPrimary(uint8_t num)
{
    primary = num;
    return &mon[num];
}

Monster *Player::setPrimary()
{
    primary = curr;
    return &mon[curr];
}

Monster *Player::getCurr()
{
    return &mon[curr];
}

Monster *Player::getNext()
{
    ++curr;
    curr %= size;
    return &mon[curr];
}

Monster *Player::getPrev()
{
    --curr;
    if(curr > size) curr = size-1;
    return &mon[curr];
}

void Player::updateEXP(uint16_t num)
{
    getPrimary()->pumpEXP(num * mult);
}

void Player::updateSteps(uint64_t num)
{
    lifetimeCount += num;

    if(lifetimeCount > 128000)
    {
        mon[1] = Monster(jellyfish, random(2));
        size = 5;
    }
    else if(lifetimeCount > 64000)
    {
        mon[1] = Monster(jellyfish, random(2));
        size = 4;
    }
    else if(lifetimeCount > 32000)
    {
        mon[1] = Monster(jellyfish, random(2));
        size = 3;
    }
    else if(lifetimeCount > 16000)
    {
        mon[1] = Monster(jellyfish, random(2));
        size = 2;
    }
    
}