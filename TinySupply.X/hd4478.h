#ifndef HD4478_H
#define	HD4478_H

#include "config.h"
#include "pcf8574.h"

#ifdef	__cplusplus
extern "C" {
#endif

typedef struct
{
    u8 State;
    Pcf8574State DriverState;
} LcdState;

void Hd4478Init(LcdState *lcd, u8 driverAddress);

void Hd4478Putc(LcdState *lcd, const char *c);

#ifdef	__cplusplus
}
#endif

#endif	/* HD4478_H */

