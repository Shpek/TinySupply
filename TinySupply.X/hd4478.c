#include "hd4478.h"

#define RS_MASK   0b00000001
#define RW_MASK   0b00000010
#define E_MASK    0b00000100
#define L_MASK    0b00001000
#define DATA_MASK 0b11110000
#define BSY_MASK  0b10000000

inline void SendToLcd(LcdState *lcd)
{
    Pcf8574Write(&lcd->DriverState, lcd->State);
}

inline u8 ReadFromLcd(LcdState *lcd)
{
    return Pcf8574Read(&lcd->DriverState);
}

inline void EHigh(LcdState *lcd)
{
    lcd->State = lcd->State | E_MASK;
    Pcf8574Write(&lcd->DriverState, lcd->State);
}

inline void ELow(LcdState *lcd)
{
    lcd->State = lcd->State & ~E_MASK;
    Pcf8574Write(&lcd->DriverState, lcd->State);
}

/*
static u8 Hd4478ReadByte(LcdState *lcd)
{
    // E high, RW high, Data high (to configure PCF8574 pins as inputs)
    lcd->State = lcd->State | RW_MASK | DATA_MASK | E_MASK;
    SendToLcd(lcd);

    // Get the high nibble
    EHigh(lcd);
    u8 data = ReadFromLcd(lcd);
    ELow(lcd);

    // Get the low nibble
    EHigh(lcd);
    data = data | (ReadFromLcd(lcd) >> 4);
    ELow(lcd);
    return data;
}
*/

static void Hd4478SendByte(LcdState *lcd, u8 address, u8 data)
{
    // Select address or instruction register
    if (address)
        lcd->State = lcd->State | RS_MASK;
    else
        lcd->State = lcd->State & ~RS_MASK;

    // Set the high nibble
    lcd->State = (lcd->State & ~RW_MASK & ~DATA_MASK) | (data & DATA_MASK);
    SendToLcd(lcd);
    EHigh(lcd);

    // Set the low nibble
    lcd->State = (lcd->State & ~DATA_MASK) | (data << 4);
    ELow(lcd);
    EHigh(lcd);
    ELow(lcd);
}

static void WaitForBusy(LcdState *lcd)
{
    // E high, RW high, Data high (to configure PCF8574 pins as inputs)
    lcd->State = (lcd->State | RW_MASK | DATA_MASK) & ~RS_MASK;
    SendToLcd(lcd);

    while (1) {
        // Get the high nibble
        lcd->State = lcd->State | DATA_MASK;
        EHigh(lcd);
        u8 data = ReadFromLcd(lcd);
        ELow(lcd);

        // Get the low nibble
        EHigh(lcd);
        data = data | (ReadFromLcd(lcd) >> 4);
        lcd->State = lcd->State & ~DATA_MASK;
        ELow(lcd);

        if (!(data & BSY_MASK))
            break;
    }
}

void Hd4478Init(LcdState *lcd, u8 driverAddress)
{
    Pcf8574Init(&lcd->DriverState, driverAddress);
    __delay_ms(25);

    // L high. E, RS, RW - low.
    lcd->State = L_MASK & ~(E_MASK | RS_MASK | RW_MASK);
    lcd->State = (lcd->State & ~DATA_MASK) | (0b0011 << 4);

    SendToLcd(lcd);
    EHigh(lcd);
    ELow(lcd);
    __delay_ms(5);

    EHigh(lcd);
    ELow(lcd);
    __delay_us(101);

    EHigh(lcd);
    ELow(lcd);

    lcd->State = (lcd->State & ~DATA_MASK) | (0b0010 << 4);
    EHigh(lcd);
    ELow(lcd);

    // Set 4 bit transfer mode, 2 lines
    Hd4478SendByte(lcd, 0, 0b00101000);
    WaitForBusy(lcd);

    // Display on cursor off blinking off
    Hd4478SendByte(lcd, 0, 0b00001100);
    WaitForBusy(lcd);

    // Go home
    Hd4478SendByte(lcd, 0, 0b00000010);
    WaitForBusy(lcd);

    // Clear screen
    Hd4478SendByte(lcd, 0, 0b00000001);
    WaitForBusy(lcd);

    Pcf8574Stop(&lcd->DriverState);
}

void Hd4478Putc(LcdState *lcd, const char *c)
{
    while (*c) {
        switch(*c)
        {
        case '\f':
            Hd4478SendByte(lcd, 0, 1);
            WaitForBusy(lcd);
            break;

        case '\n':
            Hd4478SendByte(lcd, 0, 0x80 | 0x40);
            WaitForBusy(lcd);
            break;

        case '\b':
            Hd4478SendByte(lcd, 0,0x10);
            WaitForBusy(lcd);
            break;

        default:
            Hd4478SendByte(lcd, 1, *c);
            break;
        }

        c = c + 1;
    }

    Pcf8574Stop(&lcd->DriverState);
}

