#ifndef HD4478_H
#define	HD4478_H

#include "pcf8574.h"

#ifdef	__cplusplus
extern "C" {
#endif

#define RS_MASK   0b00000001
#define RW_MASK   0b00000010
#define E_MASK    0b00000100
#define L_MASK    0b00001000
#define DATA_MASK 0b11110000
#define BSY_MASK  0b10000000

static u8 state;
static u8 driverAddr;

u8 Hd4478ReadByte()
{
    // E high, RW high, Data high (to configure as input)
    state = state | E_MASK | RW_MASK | DATA_MASK ;
    Pcf8574Write(state);

    // Get the high nibble
    u8 data = Pcf8574Read();

    // E low
    state = state & ~E_MASK;
    Pcf8574Write(state);

    // E high
    state = state | E_MASK;
    Pcf8574Write(state);

    data = data | (Pcf8574Read() >> 4);

    // E low
    state = state & ~E_MASK;
    Pcf8574Write(state);

    return data;
}

void Hd4478SendNibble(u8 data)
{
    // Set the high nibble
    state = state & ~DATA_MASK | ((data << 4) & DATA_MASK);
    Pcf8574Write(state);

    // E high
    state = state | E_MASK;
    Pcf8574Write(state);

    // E low
    state = state & ~E_MASK;
    Pcf8574Write(state);
}

void Hd4478SendByte(u8 address, u8 data)
{
    // Select address or instruction register
    if (address)
        state = state | RS_MASK;
    else
        state = state & ~RS_MASK;

    // RW low
    state = state & ~RW_MASK;

    // Set the high nibble
    state = state & ~DATA_MASK | (data & DATA_MASK);
    Pcf8574Write(state);

    // E high
    state = state | E_MASK;
    Pcf8574Write(state);

    // E low
    state = state & ~E_MASK;
    Pcf8574Write(state);

    // Set the low nibble
    state = state & ~DATA_MASK | ((data << 4) & DATA_MASK);
    Pcf8574Write(state);

    // E high
    state = state | E_MASK;
    Pcf8574Write(state);

    // E low
    state = state & ~E_MASK;
    Pcf8574Write(state);
}

void WaitForBusy()
{
    // RS low
    if (state & RS_MASK)
        state = state & ~RS_MASK;

    // Wait for busy flag to clear
    while (Hd4478ReadByte() & BSY_MASK);
}

void Hd4478Init(u8 driverAddress)
{
    Pcf8574Init(driverAddress);
    
    // RS and RW low, L high
    state = ~RS_MASK & ~RW_MASK | L_MASK;
    Pcf8574Write(state);

    Hd4478SendNibble(0x03);
    __delay_ms(5);
    Hd4478SendNibble(0x03);
    __delay_ms(5);
    Hd4478SendNibble(0x03);
    __delay_ms(5);
    Hd4478SendNibble(0x02);

    WaitForBusy();
    
    // Set 4 bit transfer mode, 2 lines
    Hd4478SendByte(0, 0b00101000);
    WaitForBusy();
    
    // Display on cursor off blinking off
    Hd4478SendByte(0, 0b00001100);
    WaitForBusy();

    // Go home
    Hd4478SendByte(0, 0b00000010);
    WaitForBusy();

    // Clear screen
    Hd4478SendByte(0, 0b00000001);
    WaitForBusy();

    Pcf8574Stop();
}

void Hd4478Putc(const char *c)
{
    while (*c) {
        switch(*c)
        {
        case '\f':
            Hd4478SendByte(0, 1);
            WaitForBusy();
            break;

        case '\n':
            Hd4478SendByte(0, 0x80 | 0x40);
            break;

        case '\b':
            Hd4478SendByte(0,0x10);
            WaitForBusy();
            break;

        default:
            Hd4478SendByte(1, *c);
            break;
        }

        c = c + 1;
    }

    Pcf8574Stop();
}

#ifdef	__cplusplus
}
#endif

#endif	/* HD4478_H */

