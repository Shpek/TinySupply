#include "config.h"
#include "hd4478.h"

/* returns change in encoder state (-1,0,1) */
i8 read_encoder()
{
  static i8 enc_states[] = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0};
  static u8 old_AB = 0;
  
  old_AB <<= 2;
  old_AB |= (PORTAbits.RA0 << 1) | PORTAbits.RA4;
  return enc_states[old_AB & 0x0f];
}

i8 counting;
u16 count;

void interrupt InterruptServiceRoutine()
{
    if (IOCAF2) {
        IOCIF = 0;
        IOCAF2 = 0;

        counting = 1;
        count = 0;

        /*
        i8 state = read_encoder();

        if (state == 1)
            LATA5 = 0;
        if (state == -1)
            LATA5 = 1;
         */
    }
}

void main(void) {
    // Set the internal oscillator to 4 MHz
    OSCCON = 0b01101000;

    // Configure RA5 as output
    TRISA5 = 0;

    // Configure RA2 and RA4 as inputs
    TRISA0 = 1;
    TRISA4 = 1;
    ANSELA = 0;
    
    // Light up the diode
    LATA5 = 1;

/*
    IOCIF = 0;
    IOCIE = 1;
    IOCAP2 = 1;
    GIE = 1;

    while (1) {
        if (counting) {
            ++ count;

            if (count == 20) {
                if (PORTAbits.RA0) {
                    if (PORTAbits.RA4)
                        LATA5 = 0;
                    else
                        LATA5 = 1;
                }
                
                counting = 0;
            }
        }
    }
 */
    u8 I2CDriverAddress = 0x4E;

    /*
    Pcf8574Init(I2CDriverAddress);
    Pcf8574Set(0b11111111);
    
    while (1) {
        Pcf8584ReadUntil(0b10000000, 0b10000000);
        LATA5 = 0;
        Pcf8584ReadUntil(0b10000000, 0b00000000);
        LATA5 = 1;
    }
     */

    
    Hd4478Init(I2CDriverAddress);

    while (1) {
        Hd4478Putc("\fKre Me Na\nKre Me Na");
        __delay_ms(500);
        Hd4478Putc("\f");
        __delay_ms(500);
    }
}

