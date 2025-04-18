/*
 * ISR.h
 *
 * Created: 24-12-2022 13:38:57
 *  Author: Tachyon Automations
 */ 


#ifndef ISR_H_
#define ISR_H_


#include <avr/interrupt.h>
#include "defines.h"
uint8_t G_StartPhase1Count_Ui8  = false;
uint8_t G_StartPhase2Count_Ui8  = false;
uint32_t G_Phase1Count_ui16  = 0;
uint32_t G_Phase2Count_ui16  = 0;
uint32_t G_Phase1CountLimit_ui16  = 900000;//900000
uint32_t G_Phase2CountLimit_ui16  = 300000;//300000
uint8_t G_StateLevel_Ui8	= 0;
uint8_t G_InputStatus_Ui8	= 0;
uint8_t G_PhaseStatus_Ui8	= 0;
uint16_t G_LED_Count_Ui8 = 0;
uint16_t G_LED_CountLimit_Ui8 = 500;
uint8_t G_LedStatus_Ui8 = 0;
uint8_t G_DebounceCount_Ui8 = 20;
uint8_t G_DebounceCounter_Ui8 = 0;

#endif /* ISR_H_ */