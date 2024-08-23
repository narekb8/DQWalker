#line 1 "C:\\Users\\th3fr\\OneDrive\\Desktop\\Personal-Projects\\DQWalker\\Monster.cpp"
#include "Monster.h"
#include "Arduino.h"

uint32_t baseSlime = 0b00101100101101110100010101101101;
uint32_t haloSlime = 0b00010011010001011011010110010110;
uint32_t puddleSlime = 0b00100010011110001010000111001101;
uint32_t jellySlime = 0b00010110111101011010111010010010;
uint32_t clamSlime = 0b00010101000001000101100111001001;

uint16_t expLevels = 0b0111111100011010;

// Array of all bitmaps for convenience. (Total bytes used to store images in PROGMEM = 6240)
const uint16_t* angel_sheet[6] = {
	epd_bitmap_angel_1,
	epd_bitmap_angel_2,
	epd_bitmap_angel_side_1,
	epd_bitmap_angel_side_2,
	epd_bitmap_angel_up_1,
	epd_bitmap_angel_up_2
};
// Array of all bitmaps for convenience. (Total bytes used to store images in PROGMEM = 6240)
const uint16_t* clam_sheet[6] = {
	epd_bitmap_clam_1,
	epd_bitmap_clam_2,
	epd_bitmap_clam_side_1,
	epd_bitmap_clam_side_2,
	epd_bitmap_clam_up_1,
	epd_bitmap_clam_up_2
};
// Array of all bitmaps for convenience. (Total bytes used to store images in PROGMEM = 6240)
const uint16_t* jelly_sheet[6] = {
	epd_bitmap_jellyfish_1,
	epd_bitmap_jellyfish_2,
	epd_bitmap_jellyfish_side_1,
	epd_bitmap_jellyfish_side_2,
	epd_bitmap_jellyfish_up_1,
	epd_bitmap_jellyfish_up_2
};
// Array of all bitmaps for convenience. (Total bytes used to store images in PROGMEM = 6240)
const uint16_t* puddle_sheet[6] = {
	epd_bitmap_puddle_1,
	epd_bitmap_puddle_2,
	epd_bitmap_puddle_side_1,
	epd_bitmap_puddle_side_2,
	epd_bitmap_puddle_up_1,
	epd_bitmap_puddle_up_2
};
// Array of all bitmaps for convenience. (Total bytes used to store images in PROGMEM = 6240)
const uint16_t* slime_sheet[6] = {
	epd_bitmap_slime_1,
	epd_bitmap_slime_2,
	epd_bitmap_slime_side_1,
	epd_bitmap_slime_side_2,
	epd_bitmap_slime_up_1,
	epd_bitmap_slime_up_2
};

Monster::Monster()
{
    breed = basic;
    info = 0;
    baseStats();
}

Monster::Monster(species _breed, bool isFem)
{
    breed = _breed;
    info = (isFem) ? 0x80 : 0;
    baseStats();
}

void Monster::baseStats()
{
    info++;

    uint32_t stat_base;
    switch (breed)
    {
    case basic:
        stat_base = baseSlime;
        break;
    case halo:
        stat_base = haloSlime;
        break;
    case puddle:
        stat_base = puddleSlime;
        break;
    case clam:
        stat_base = clamSlime;
        break;
    case jellyfish:
        stat_base = jellySlime;
        break;
    }

    uint16_t hp_growth = (stat_base << 2) >> 27;
    uint16_t mp_growth = (stat_base << 7) >> 27;
    uint16_t atk_growth = (stat_base << 12) >> 27;
    uint16_t def_growth = (stat_base << 17) >> 27;
    uint16_t agl_growth = (stat_base << 22) >> 27;
    uint16_t int_growth = (stat_base << 27) >> 27;

    hp = std::ceil(((float)hp_growth + (float)hp_growth) / 4);
    mp = std::ceil(((float)mp_growth + (float)mp_growth) / 4);
    atk = std::ceil(((float)atk_growth + (float)atk_growth) / 4);
    def = std::ceil(((float)def_growth + (float)def_growth) / 4);
    agl = std::ceil(((float)agl_growth + (float)agl_growth) / 4);
    intl = std::ceil(((float)int_growth + (float)int_growth) / 4);

    uint8_t growth_rate;
    switch(breed)
    {
        case basic:
            growth_rate = (expLevels << 1) >> 13;
        case halo:
            growth_rate = (expLevels << 4) >> 13;
        case puddle:
            growth_rate = (expLevels << 7) >> 13;
        case clam:
            growth_rate = (expLevels << 10) >> 13;
        case jellyfish:
            growth_rate = (expLevels << 13) >> 13;
    }

    toNext += growth_rate * 50;
}

void Monster::levelUp()
{
    uint16_t level = info & ~(1 << 7);

    if(level >= 64)
    {
        toNext = UINT32_MAX;
        return;
    }

    info++;
    level = info & ~(1 << 7);

    uint32_t stat_base;
    switch (breed)
    {
        case basic:
            stat_base = baseSlime;
            break;
        case halo:
            stat_base = haloSlime;
            break;
        case puddle:
            stat_base = puddleSlime;
            break;
        case clam:
            stat_base = clamSlime;
            break;
        case jellyfish:
            stat_base = jellySlime;
            break;
    }

    uint16_t hp_growth = (stat_base << 2) >> 27;
    uint16_t mp_growth = (stat_base << 7) >> 27;
    uint16_t atk_growth = (stat_base << 12) >> 27;
    uint16_t def_growth = (stat_base << 17) >> 27;
    uint16_t agl_growth = (stat_base << 22) >> 27;
    uint16_t int_growth = (stat_base << 27) >> 27;

    hp += growthRates[level-2][hp_growth-1];
    mp += growthRates[level-2][mp_growth-1];
    atk += growthRates[level-2][atk_growth-1];
    def += growthRates[level-2][def_growth-1];
    agl += growthRates[level-2][agl_growth-1];
    intl += growthRates[level-2][int_growth-1];

    uint8_t growth_rate;
    switch(breed)
    {
        case basic:
            growth_rate = (expLevels << 1) >> 13;
        case halo:
            growth_rate = (expLevels << 4) >> 13;
        case puddle:
            growth_rate = (expLevels << 7) >> 13;
        case clam:
            growth_rate = (expLevels << 10) >> 13;
        case jellyfish:
            growth_rate = (expLevels << 13) >> 13;
    }

    toNext += growth_rate * 5 * level;
}

uint16_t* Monster::getStats()
{
    uint16_t level = info & ~(1 << 7);
    static uint16_t stats[8];

    stats[0] = level;
    stats[1] = info >> 7;
    stats[2] = atk;
    stats[3] = def;
    stats[4] = agl;
    stats[5] = intl;
    stats[6] = hp;
    stats[7] = mp;

    return stats;
}

const uint16_t* Monster::getPixelData(uint8_t type, bool frame)
{
    switch (breed)
    {
        case basic:
            return slime_sheet[type*2 + frame];
        case halo:
            return angel_sheet[type*2 + frame];
        case puddle:
            return puddle_sheet[type*2 + frame];
        case clam:
            return clam_sheet[type*2 + frame];
        case jellyfish:
            return jelly_sheet[type*2 + frame];
    }
}

void Monster::pumpEXP(uint16_t count)
{
    if(exp + count < exp)
        exp = UINT32_MAX;
    else
    {
        exp += count;
        while(exp > toNext)
            levelUp();
    }
}