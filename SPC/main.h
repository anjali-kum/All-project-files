/*
 * main.h
 *
 * Created: 24-12-2022 13:51:09
 *  Author: abbho
 */ 


#ifndef MAIN_H_
#define MAIN_H_


#include <avr/wdt.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "extern_timer.h"
#include "extern_ISR.h"
#include "defines.h"


extern struct _KeySt KeySt;




void G_PortConfig_Mth(void);

void G_InitVar_Mth(void);



#endif /* MAIN_H_ */