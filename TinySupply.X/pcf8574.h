#ifndef PCF8574_H
#define	PCF8574_H

#include "config.h"
#include "i2c.h"

#ifdef	__cplusplus
extern "C" {
#endif

// What the master is doing at the moment
typedef enum  { 
    Writing = 0,
    Reading,
    Idle,
} Pcf8574MasterStatus;

// Current state of the device
typedef struct
{
    u8 Address; // I2C address of the device
    Pcf8574MasterStatus Status;
} Pcf8574State;

// Initializes the device
void Pcf8574Init(Pcf8574State *state, u8 address);

// Reads the device as single I2C transaction
u8 Pcf8574Get(Pcf8574State *state);

// Writes to the device as single I2C transaction
void Pcf8574Set(Pcf8574State *state, u8 data);

// Initiates or continues reading from the device
u8 Pcf8574Read(Pcf8574State *state);

// Initiates or continues writing to the device
void Pcf8574Write(Pcf8574State *state, u8 data);

// Stop the current I2C transaction
void Pcf8574Stop(Pcf8574State *state);

// Reads the device until data & mask == cond
u8 Pcf8584ReadUntil(Pcf8574State *state, u8 mask, u8 cond);

#ifdef	__cplusplus
}
#endif

#endif	/* PCF8574_H */

