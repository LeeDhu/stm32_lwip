#ifndef __SPI_H
#define __SPI_H

#include "mysys.h"

void spi1_init(void);
void spi1_set_speed(u8 speed_set);
u8 spi1_rw_byte(u8 tx_data);

#endif
