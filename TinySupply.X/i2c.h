#ifndef I2C_H
#define	I2C_H

#include "config.h"

#ifdef	__cplusplus
extern "C" {
#endif

// Init I2C subsystem as master, clock frequency is FOSC/(4 * (clockDivider + 1))
void I2CInitMaster(u8 clockDivider);

// Set start condition
void I2CStart();

// Set stop condition
void I2CStop();

// Set restart condition
void I2CRestart();

// Generate ACK
void I2CAck();

// Generate NACK
void I2CNak();

// Wait for transfer to finish
void I2CWait();

// Sends a byte
void I2CSend(u8 dat);

// Reads a byte
u8 I2CRead(void);

#ifdef	__cplusplus
}
#endif


#endif	/* I2C_H */
