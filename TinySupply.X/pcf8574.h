#ifndef PCF8574_H
#define	PCF8574_H

#include "config.h"
#include "i2c.h"

#ifdef	__cplusplus
extern "C" {
#endif

struct Pcf8574State
{
    u8 Address;
    u8 Direction;
};

static u8 g_address;
static u8 g_direction = 2; // 0 - write, 1 - read, other - stopped

void Pcf8574Set(u8 data)
{
    I2CStart();
    I2CSend(g_address & 0b11111110);
    I2CSend(data);
    I2CStop();
}

u8 Pcf8574Get()
{
    I2CStart();
    I2CSend(g_address | 0b00000001);
    u8 data = I2CRead();
    I2CNak();
    I2CStop();
    return data;
}

void Pcf8574Write(u8 data)
{
    if (g_direction == 1) { // read
        I2CRestart();
        I2CSend(g_address & 0b11111110);
        g_direction = 0;
    } else if (g_direction != 0) { // not started
        I2CStart();
        I2CSend(g_address & 0b11111110);
        g_direction = 0;
    }

    I2CSend(data);
}

u8 Pcf8574Read()
{
    if (g_direction == 0) { // write
        I2CRestart();
        I2CSend(g_address | 0b00000001);
        g_direction = 1;
    } else if (g_direction == 1) { // read
        I2CAck();
    } else { // not started
        I2CStart();
        I2CSend(g_address | 0b00000001);
        g_direction = 1;
    }
    
    u8 data = I2CRead();
    return data;
}

void Pcf8574Stop() {
    if (g_direction == 1) {
        I2CNak();
        I2CStop();
    } else if (g_direction == 0) {
        I2CStop();
    }

    g_direction = 2;
}

u8 Pcf8584ReadUntil(u8 mask, u8 cond) {
    I2CStart();
    I2CSend(g_address | 0b00000001);
    
    while (1) {
        u8 data = I2CRead();    

        if ((data & mask) == cond) {
            I2CNak();
            I2CStop();
            return data;
        }

        I2CAck();
    }

    // To make the compiler happy
    return 0;
}

void Pcf8574Init(u8 address) {
    I2CInitMaster(0x09);
    g_address = address;
}

#ifdef	__cplusplus
}
#endif

#endif	/* PCF8574_H */

