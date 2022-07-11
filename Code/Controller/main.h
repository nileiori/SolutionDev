

#ifndef __MAIN_H
#define __MAIN_H

#include "string.h"
#include "DelayAdapter.h"
#include "TimAdapter.h"
#include "UartAdapter.h"
#include "LedAdapter.h"
#include "FMCAdapter.h"
#include "CanAdapter.h"
#include "ExtiAdapter.h"
#include "UdpAdapter.h"


#define MAIN_FREQUENCY 1000

//
u32 g_counter_main = 0x00;

void Peripheral_Init(void);
//main interrupt function
void ISR_Main_Int(void);

#endif

