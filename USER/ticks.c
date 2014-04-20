/**
 * @file ticks.c
 * @brief 
 * @author Li Ming <Lee.dhucst@gmail.com>
 * @url http://blog.csdn.net/lim680
 * @version V1.0
 * @date 2014-04-17
 * @Copyright (c) DHU, CST.  All Rights Reserved
 */

#include "mysys.h"
#include "ticks.h"


void systick_init(u32 us)
{
	u32 ticks = (SystemCoreClock / 1000000) * us;
	
	if(SysTick_Config(ticks) == 1) {
		while(1);
	}
}


void delay_ms(u32 ms_lag)
{
	u8 start_timing = 0;
	u32 cur_ticks = 0;
	while(1) {
		if(start_timing == 0) {
			cur_ticks = local_ticks;
			start_timing = 1;
			continue;
		}
		else if(((local_ticks - cur_ticks)*SYS_TICKS/1000) >= ms_lag) {
			start_timing = 0;
			break;
		}
	}
}

