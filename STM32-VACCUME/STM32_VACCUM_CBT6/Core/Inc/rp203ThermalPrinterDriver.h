/*
 * rp203ThermalPrinterDriver.h
 *
 *  Created on: Apr 3, 2024
 *      Author: LAVANYA
 */

#ifndef INC_RP203THERMALPRINTERDRIVER_H_
#define INC_RP203THERMALPRINTERDRIVER_H_

#include "main.h"
#include "myheader.h"


typedef struct validationReport
{
	uint16_t setMmhg ;
	uint8_t setMin ;
	uint8_t setSec ;
	uint16_t currentMmhg ;
	uint16_t gaugeMmhg ;
	uint8_t passvar[5] ;

}validationReport_t;

extern uint8_t rp203ThermalPrinterDriver_startHour ;
extern uint8_t rp203ThermalPrinterDriver_startMin ;
extern uint8_t rp203ThermalPrinterDriver_startSec ;

extern uint8_t rp203ThermalPrinterDriver_endHour ;
extern uint8_t rp203ThermalPrinterDriver_endMin ;
extern uint8_t rp203ThermalPrinterDriver_endSec ;

extern uint8_t rp203ThermalPrinterDriver_TestDate[15] ;

extern UART_HandleTypeDef huart1;

//Vaccum Test result report functions

void UART_transfer(uint8_t *data,uint8_t length);
void Bold_enable();
void Bold_disable();
void Vaccum_test_result_title(uint8_t *data1,uint8_t *data2, uint8_t *op, uint8_t testResultFlag );
void Vaccum_test_result_subtitle(uint8_t *data, uint8_t testResultFlag);
void Default_font();
//void Vaccum_test_result_fun1(uint8_t *data1,uint8_t *data2);
void Vaccum_test_result_report(uint8_t *data1,uint8_t *data2,uint8_t *data3,uint8_t *data4,uint8_t *data5,uint8_t *data6, uint8_t *op, uint8_t testResultFlag );
void User_prod_details(uint8_t *data2,uint8_t *data3,uint8_t *data4,uint8_t *data5,uint8_t *data6,uint8_t *data7,uint8_t *data8, uint8_t *op, uint8_t testResultFlag );
void Test_result(uint8_t *data0,uint8_t *data1,uint8_t *data2,uint8_t *data3,uint8_t *data4,uint8_t *data, uint8_t *op, uint8_t testResultFlag);
void printTextred_on(uint8_t *data1,uint8_t *data2, uint8_t *op, uint8_t testResultFlag);
void Test_status(uint8_t *data1,uint8_t *data2,uint8_t *data3,uint8_t *data4, uint8_t *op, uint8_t testResultFlag);
void Test_settings(uint8_t *data1,uint8_t *data2, uint8_t *op, uint8_t testResultFlag);

//Validation Report Functions

void Validation_report_start(uint8_t *data1,uint8_t *data2,uint8_t *data3,uint8_t *data4,uint8_t *data5 );
void Validation_report_product_details(uint8_t *data1,uint8_t *data2);
void Validation_report_results(uint8_t *srNo, uint8_t *inputmmHg, uint8_t *inputMin, uint8_t *inputSec,
		uint8_t *c_Mmhg, uint8_t *status, uint8_t *gauge_mmhg) ;

void Validation_report_end(uint8_t *data1) ;
void validation_title(uint8_t *data1,uint8_t *data2) ;

#endif /* INC_RP203THERMALPRINTERDRIVER_H_ */
