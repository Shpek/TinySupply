#include "i2c.h"

// Init I2C subsystem as master, clock frequency is FOSC/(4 * (clockDivider + 1))
void I2CInitMaster(u8 clockDivider)
{
    // Set the i2c pins as inputs
    TRISA1 = 1;
    TRISA2 = 1;
    SSPSTAT = 0x80; // Disable slew rate
    SSPCON1 = 0x28; // SSPEN = 1, I2C Master mode, clock = FOSC/(4 * (SSPADD + 1))
    SSPADD = clockDivider;
}

// Set start condition
void I2CStart()
{
    SEN = 1;
    while (SEN);
}

// Set stop condition
void I2CStop()
{
    PEN = 1;
    while (PEN);
}

// Set restart condition
void I2CRestart()
{
    RSEN = 1;
    while (RSEN);
}

// Generate ACK
void I2CAck()
{
    ACKDT = 0; // Acknowledge data bit, 0 = ACK
    ACKEN = 1; // Ack data enabled
    while(ACKEN); // wait for ack data to send on bus
}

// Generate NACK
void I2CNak()
{
    ACKDT = 1; // Acknowledge data bit, 1 = NAK
    ACKEN = 1; // Ack data enabled
    while(ACKEN); // wait for ack data to send on bus
}

// Wait for transfer to finish
void I2CWait()
{
    while ((SSPCON2 & 0x1F) || (SSPSTAT & 0x04));
}

// Sends a byte
void I2CSend(u8 dat)
{
    SSPBUF = dat; // Move data to SSPBUF
    while(BF); // wait till complete data is sent from buffer
    I2CWait(); // wait for any pending transfer
}

// Reads a byte
u8 I2CRead(void)
{
    u8 temp;
    RCEN = 1; // Enable data reception
    while(!BF); // wait for buffer full
    temp = SSPBUF; // Read serial buffer and store in temp register
    I2CWait(); // wait to check any pending transfer
    return temp; // Return the read data from bus
}
