/*
 * display.h
 *
 *  Created on: Dec 15, 2023
 *      Author: naveen
 */

#ifndef INC_DISPLAY_H_
#define INC_DISPLAY_H_

//#include <stdint.h>
//#include <string.h>
//#include <stdio.h>


#include "myheader.h"

#include "rp203ThermalPrinterDriver.h"
#include "mem_w25qxxDriver.h"


#define RUN_STATE 1
#define STOP_STATE 2
#define CLEAR_MMHG_STATE 3
#define CLEAR_TIME_STATE 4

extern uint8_t modemRxBuff[MAX_MODEM_RX_BUFF];
extern uint8_t current_page ;

extern uint8_t testResultFlag ;
extern uint8_t validationTestVar ;
extern uint8_t testdoneRnotFlag  ;

extern uint16_t validcalib_gaugeVal ;
extern uint8_t quickTestFlag ;
//extern uint8_t calibrationModifySetMmhg ;


//typedef struct
//{
//	volatile float 		 setpointt 			;
//	volatile uint8_t	 setTimeInMin		;
//	volatile uint8_t	 setTimeInSec		;
//	volatile float 		 runTime 			;
//	volatile uint8_t	 min				;
//	volatile uint8_t	 sec				;
//	volatile uint8_t 	ControlSystemFlag	;
//	uint8_t 			controlTimeFlag		;
//
//}displayParams;


//void display_monitorRunningParametersFromDisplay(displayParams *display, uint8_t *displayUartFlag,	float currenttime);
void display_mmhgValueUpdateToDisplay( uint64_t mmhg,uint8_t ControlSystemFlag);

void display_runTimeUpdateToDisplay(uint8_t runstopFlag, uint8_t uni_sec);

void Page_validation(uint8_t *displayUartFlag, uint8_t *page_invoke);

//methods below are for profile write page
//void profile_write_serialNo_details_update(uint8_t serNo, ProfileWrieParamAddrs *writeProfileParamsAddrs, uint8_t *flag, uint8_t *displayUartFlag);
void profile_write_serialNo_read( uint8_t *displayUartFlag,uint8_t *op, uint8_t *serialNo_read_flag,uint8_t *write_button_flag);
void getTargetAddrfor16bit(uint8_t *_arr, uint8_t _arrSize,uint16_t targetAddr , uint16_t value);

void profile_page_change(uint8_t value) ;
void display_clearModemBuffer() ;

void getTime();

void process_SD_card( void );


#endif /* INC_DISPLAY_H_ */
