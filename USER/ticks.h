#ifndef __TICKS_H
#define __TICKS_H

#include "mysys.h"

extern void systick_init(u32 us);
extern void delay_ms(u32 lag);
#endif
