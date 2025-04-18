/*
 * timer.c
 *
 * Created: 24-12-2022 13:38:05
 *  Author: abbho
 */ 
#include "timer.h"


void G_TimerConfig_Mth(void)
{
	TCCR0A = 0;
	TCCR0B = 0;
	TCCR0B = (1 << CS01);
	TCNT0 = 130;
	TIFR0 = (1 << TOV0);
	TIMSK0 = (1 << TOIE0);
	
}