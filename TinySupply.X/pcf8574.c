#include "pcf8574.h"

// Initializes the device
void Pcf8574Init(Pcf8574State *state, u8 address)
{
    I2CInitMaster(0x09);
    state->Address = address;
    state->Status = Idle;
}

// Reads the device as single I2C transaction
u8 Pcf8574Get(Pcf8574State *state)
{
    I2CStart();
    I2CSend(state->Address | 0b00000001);
    u8 data = I2CRead();
    I2CNak();
    I2CStop();
    return data;
}

// Writes to the device as single I2C transaction
void Pcf8574Set(Pcf8574State *state, u8 data)
{
    I2CStart();
    I2CSend(state->Address & 0b11111110);
    I2CSend(data);
    I2CStop();
}

// Initiates or continues writing to the device
void Pcf8574Write(Pcf8574State *state, u8 data)
{
    if (state->Status == Reading) {
        I2CNak();
        I2CRestart();
        I2CSend(state->Address & 0b11111110);
        state->Status = Writing;
    } else if (state->Status != Writing) {
        I2CStart();
        I2CSend(state->Address & 0b11111110);
        state->Status = Writing;
    }

    I2CSend(data);
}

// Initiates or continues reading from the device
u8 Pcf8574Read(Pcf8574State *state)
{
    if (state->Status == Writing) {
        I2CRestart();
        I2CSend(state->Address | 0b00000001);
        state->Status = Reading;
    } else if (state->Status == Reading) {
        I2CAck();
    } else {
        I2CStart();
        I2CSend(state->Address | 0b00000001);
        state->Status = Reading;
    }

    u8 data = I2CRead();
    return data;
}

// Stop the current reading or writing
void Pcf8574Stop(Pcf8574State *state)
{
    if (state->Status == Reading) {
        I2CNak();
        I2CStop();
    } else if (state->Status == Writing) {
        I2CStop();
    }

    state->Status = Idle;
}

// Reads the device until data & mask == cond
u8 Pcf8584ReadUntil(Pcf8574State *state, u8 mask, u8 cond) {
    I2CStart();
    I2CSend(state->Address | 0b00000001);

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
