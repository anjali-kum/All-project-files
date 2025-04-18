/*
 * rp203ThermalPrinterDriver.c
 *
 *  Created on: Apr 3, 2024
 *      Author: LAVANYA
 */


#include "rp203ThermalPrinterDriver.h"
#include "string.h"
#include <stdlib.h>

uint8_t rp203ThermalPrinterDriver_startHour ;
uint8_t rp203ThermalPrinterDriver_startMin ;
uint8_t rp203ThermalPrinterDriver_startSec ;

uint8_t rp203ThermalPrinterDriver_endHour ;
uint8_t rp203ThermalPrinterDriver_endMin ;
uint8_t rp203ThermalPrinterDriver_endSec ;

uint8_t rp203ThermalPrinterDriver_TestDate[15] ;

void uart_Tarnsfter(uint8_t *data,uint8_t length, uint8_t testResultFlag)
{
	if(testResultFlag != 5)
	{
		HAL_UART_Transmit(&huart3,data,length,HAL_MAX_DELAY);
		HAL_UART_Transmit(&huart2,data,length,HAL_MAX_DELAY);
		HAL_Delay(1) ;
	}

}


void Bold_enable()
{
	uint8_t boldcommand[]={0x1B,0x45,0x01};
	uint8_t length = sizeof(boldcommand);
	HAL_UART_Transmit(&huart3, boldcommand, length,HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart2, boldcommand, length,HAL_MAX_DELAY);
	HAL_Delay(1000);

}
void Bold_disable()
{
	uint8_t boldcommand[]={0x1B,0x45,0x00};
	uint8_t length = sizeof(boldcommand);
	HAL_UART_Transmit(&huart3, boldcommand, length,HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart2, boldcommand, length,HAL_MAX_DELAY);
	HAL_Delay(1000);

}

void Default_font()
{
	uint8_t widthDefsize_command[]={0x1D,0x21,0x00};
	uint8_t fontDefsize_command[]={0x1B,0x21,0x00};
	uint8_t length_widthDefsize = sizeof(widthDefsize_command);
	uint8_t length_fontDefsize = sizeof(fontDefsize_command);
	HAL_UART_Transmit(&huart3, widthDefsize_command, length_widthDefsize, HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart2, widthDefsize_command, length_widthDefsize, HAL_MAX_DELAY);
	HAL_Delay(1) ;
	HAL_UART_Transmit(&huart3, fontDefsize_command, length_fontDefsize, HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart2, fontDefsize_command, length_fontDefsize, HAL_MAX_DELAY);
	HAL_Delay(1) ;
	Bold_disable();


}
void Vaccum_test_result_title(uint8_t *data1,uint8_t *data2, uint8_t *op, uint8_t testResultFlag )
{


	uint8_t smallfont_command[]={0x1B,0x21,0x01};
	uint8_t widthInc_command[] = {0x1D,0x21,0x10};
	uint8_t length_smallfont = sizeof(smallfont_command);
	uint8_t length_widthInc = sizeof(widthInc_command);
	uint8_t main_title[] = "RAISE LAB EQUIPMENT\n\n";

	if(testResultFlag != 5)
	{
		HAL_UART_Transmit(&huart3, smallfont_command, length_smallfont, HAL_MAX_DELAY);
		HAL_UART_Transmit(&huart2, smallfont_command, length_smallfont, HAL_MAX_DELAY);
		HAL_UART_Transmit(&huart3, widthInc_command, length_widthInc, HAL_MAX_DELAY);
		HAL_UART_Transmit(&huart2, widthInc_command, length_widthInc, HAL_MAX_DELAY);
		Bold_enable();
	}

	uart_Tarnsfter( (uint8_t *)main_title,strlen( (const char*)main_title), testResultFlag);
	memcpy((char *)op, main_title, strlen((char *)main_title)) ;

	if(testResultFlag != 5)
	{
		Default_font();
	}

	uint8_t data[] = "Package Integrity Tester\n";

	uart_Tarnsfter(data,strlen( (const char*)data), testResultFlag);
	strcat((char *)op, (char *)data) ;

	uint8_t Model[] = "Model: ";
	uint8_t sr_no[] = "sr.No.: ";
	uart_Tarnsfter(Model,strlen( (const char*)Model), testResultFlag);
	strcat((char *)op, (char *)Model) ;

	uart_Tarnsfter(data1,strlen( (const char*)data1), testResultFlag);
	strcat((char *)op, (char *)data1) ;

	uart_Tarnsfter(sr_no,strlen( (const char*)sr_no), testResultFlag);
	strcat((char *)op, (char *)sr_no) ;

	uart_Tarnsfter(data2,strlen( (const char*)data2), testResultFlag);
	strcat((char *)op, (char *)data2) ;

}

void validation_title(uint8_t *data1,uint8_t *data2)
{
	uint8_t smallfont_command[]={0x1B,0x21,0x01};
	uint8_t widthInc_command[] = {0x1D,0x21,0x10};
	uint8_t length_smallfont = sizeof(smallfont_command);
	uint8_t length_widthInc = sizeof(widthInc_command);
	uint8_t main_title[] = "RAISE LAB EQUIPMENT\n\n";

	HAL_UART_Transmit(&huart3, smallfont_command, length_smallfont, HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart2, smallfont_command, length_smallfont, HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart3, widthInc_command, length_widthInc, HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart2, widthInc_command, length_widthInc, HAL_MAX_DELAY);
	Bold_enable();
	uart_Tarnsfter( (uint8_t *)main_title,strlen( (const char*)main_title), 0);
	Default_font();

	uint8_t data[] = "Package Integrity Tester\n";

	uart_Tarnsfter(data,strlen( (const char*)data), 0);

	uint8_t Model[] = "Model: ";
	uint8_t sr_no[] = "sr.No.: ";
	uart_Tarnsfter(Model,strlen( (const char*)Model),0);

	uart_Tarnsfter(data1,strlen( (const char*)data1), 0);

	uart_Tarnsfter(sr_no,strlen( (const char*)sr_no), 0);

	uart_Tarnsfter(data2,strlen( (const char*)data2), 0);
}

void Vaccum_test_result_subtitle(uint8_t *data, uint8_t testResultFlag)
{
	uint8_t smallfont_command[]={0x1B,0x21,0x01};
	uint8_t widthInc_command[] = {0x1D,0x21,0x10};
	uint8_t length_smallfont = sizeof(smallfont_command);
	uint8_t length_widthInc = sizeof(widthInc_command);
	HAL_UART_Transmit(&huart3, smallfont_command, length_smallfont, HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart2, smallfont_command, length_smallfont, HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart3, widthInc_command, length_widthInc, HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart2, widthInc_command, length_widthInc, HAL_MAX_DELAY);

//	Bold_enable();
	uart_Tarnsfter(data,strlen((const char *)data), testResultFlag );
	HAL_Delay(10);
//	Default_font();
	uint8_t widthDefsize_command[]={0x1D,0x21,0x00};
	uint8_t fontDefsize_command[]={0x1B,0x21,0x00};
	uint8_t length_widthDefsize = sizeof(widthDefsize_command);
	uint8_t length_fontDefsize = sizeof(fontDefsize_command);
	HAL_UART_Transmit(&huart3, widthDefsize_command, length_widthDefsize, HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart2, widthDefsize_command, length_widthDefsize, HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart3, fontDefsize_command, length_fontDefsize, HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart2, fontDefsize_command, length_fontDefsize, HAL_MAX_DELAY);
}

void Vaccum_test_result_report(uint8_t *data1,uint8_t *data2,uint8_t *data3,uint8_t *data4,uint8_t *data5,uint8_t *data6, uint8_t *op,
		uint8_t testResultFlag)
{
	uint8_t subtitle1[] = "\nTest Report:\n\n";
	strcat((char *)op, (char *)subtitle1   );

	if(testResultFlag != 5)
	{
		Vaccum_test_result_subtitle(subtitle1, testResultFlag);
		Default_font();
	}


	uint8_t company_name[] = "Company Name: ";
	strcat((char *)op, (char *)company_name  );

	uart_Tarnsfter(company_name,sizeof(company_name), testResultFlag);

	uart_Tarnsfter(data1,strlen((const char *)data1), testResultFlag);
	strcat((char *)op,(char *) data1 );

	uint8_t location[] = "\nLocation: ";
	uart_Tarnsfter(location,sizeof(location), testResultFlag);
	strcat((char *)op, (char *)location );

	uart_Tarnsfter(data2,strlen((const char *)data2), testResultFlag);
	strcat((char *)op, (char *)data2  );

	uint8_t test_date[] ="\nTest Date        : ";
	uart_Tarnsfter(test_date,sizeof(test_date), testResultFlag);
	strcat((char *)op, (char *)test_date );

	uart_Tarnsfter(data3,strlen((const char *)data3), testResultFlag);
	strcat((char *)op, (char *)data3 );

	uint8_t test_start_time[] ="\nTest Start Time  : ";
	uart_Tarnsfter(test_start_time,sizeof(test_start_time), testResultFlag);
	strcat((char *)op, (char *)test_start_time  );

	uart_Tarnsfter(data4,strlen((const char *)data4), testResultFlag);
	strcat((char *)op, (char *)data4 );

	uint8_t test_end_time[] ="\nTest End Time    : ";
	uart_Tarnsfter(test_end_time,sizeof(test_end_time), testResultFlag);
	strcat((char *)op, (char *)test_end_time  );

	uart_Tarnsfter(data5,strlen((const char *)data5), testResultFlag);
	strcat((char *)op,(char *) data5 );

	uint8_t user_name[] = "\nUser Name: ";
	uart_Tarnsfter(user_name,sizeof(user_name), testResultFlag);
	strcat((char *)op,(char *) user_name  );

	uart_Tarnsfter(data6,strlen((const char *)data6), testResultFlag);
	strcat((char *)op,(char *) data6  );


}
void User_prod_details(uint8_t *data2,uint8_t *data3,uint8_t *data4,uint8_t *data5,uint8_t *data6,uint8_t *data7,
		uint8_t *data8, uint8_t *op, uint8_t testResultFlag)
{
	uint8_t subtitle2[] = "\nProduct details:\n";

	strcat((char *)op, (char *)subtitle2);
	if(testResultFlag != 5)
	{
		Vaccum_test_result_subtitle(subtitle2, testResultFlag);
		Default_font();
	}
		uint8_t prod_name[] = "\nProd. Name: ";
	uart_Tarnsfter(prod_name,sizeof(prod_name), testResultFlag);
	strcat((char *)op, (char *)prod_name);

	uart_Tarnsfter(data2,strlen((const char *)data2), testResultFlag);
	strcat((char *)op,(char *) data2);

	uint8_t prod_no[] ="\nProd. No.: ";
	uart_Tarnsfter(prod_no,sizeof(prod_no), testResultFlag);
	strcat((char *)op, (char *)prod_no);

	uart_Tarnsfter(data3,strlen((const char *)data3), testResultFlag);
	strcat((char *)op, (char *)data3);

	uint8_t batch_no[] ="\nBatch No: ";
	uart_Tarnsfter(batch_no,sizeof(batch_no), testResultFlag);
	strcat((char *)op, (char *)batch_no);

	uart_Tarnsfter(data4,strlen((const char *)data4), testResultFlag);
	strcat((char *)op, (char *)data4);

	uint8_t batch_size[] ="\nBatch Size: ";
	uart_Tarnsfter(batch_size,sizeof(batch_size), testResultFlag);
	strcat((char *)op, (char *)batch_size);

	uart_Tarnsfter(data5,strlen((const char *)data5), testResultFlag);
	strcat((char *)op, (char *)data5);

	uint8_t sample_qty[] ="\nSample Qty: ";
	uart_Tarnsfter(sample_qty,sizeof(sample_qty), testResultFlag);
	strcat((char *)op, (char *)sample_qty);

	uart_Tarnsfter(data6,strlen((const char *)data6), testResultFlag);
	strcat((char *)op, (char *)data6);

//	uint8_t package_type[] ="\nPackage type: ";
//	uart_Tarnsfter(package_type,sizeof(package_type));
//	strcat((char *)op, (char *)package_type);
//
//	uart_Tarnsfter(data7,strlen((const char *)data7));
//	strcat((char *)op, (char *)data7);

//	uint8_t tolerance[] ="\nTolerance: ";
//	uart_Tarnsfter(tolerance,sizeof(tolerance));
//	strcat((char *)op, (char *)tolerance);
//
//	uart_Tarnsfter(data8,strlen((const char *)data8));
//	strcat((char *)op, (char *)data8);



}
void Test_settings(uint8_t *data1,uint8_t *data2, uint8_t *op, uint8_t testResultFlag)
{
	uint8_t subtitle2[] = "\nTest Settings: \n";

	strcat((char *)op, (char *)subtitle2);

	if( testResultFlag != 5)
	{
		Vaccum_test_result_subtitle(subtitle2, testResultFlag);
		Default_font();

	}
	uint8_t set_vaccum[] = "\nSet Vaccum : ";
	uart_Tarnsfter(set_vaccum,sizeof(set_vaccum), testResultFlag);
	strcat((char *)op, (char *)set_vaccum);

	uart_Tarnsfter(data1,strlen((const char *)data1), testResultFlag);
	strcat((char *)op, (char *)data1);

	uint8_t act_vaccum[] = "Act. Vaccum : ";
	uart_Tarnsfter(act_vaccum,sizeof(act_vaccum), testResultFlag);
	strcat((char *)op, (char *)act_vaccum);

	uart_Tarnsfter(data2,strlen((const char *)data2), testResultFlag);
	strcat((char *)op, (char *)data2);



}
void Test_result(uint8_t *data0,uint8_t *data1,uint8_t *data2,uint8_t *data3,uint8_t *data4,uint8_t *data5, uint8_t *op, uint8_t testResultFlag)
{

	uint8_t subtitle3[] = "\nTest Result: \n";
	strcat((char *)op, (char *)subtitle3);
	if(testResultFlag != 5)
	{
		Vaccum_test_result_subtitle(subtitle3, testResultFlag);
		Default_font();
	}
	uint8_t test_duration[] ="\nTest Duration: ";
	uart_Tarnsfter(test_duration,sizeof(test_duration), testResultFlag);
	strcat((char *)op,(char *) test_duration);

//	uart_Tarnsfter(&data0, 1);
	uart_Tarnsfter(data0,strlen((const char *)data0), testResultFlag);
	strcat((char *)op, (char *)data0);

//	uint8_t test_sample_no[] ="\nTest Sample no.: ";
//	uart_Tarnsfter(test_sample_no,sizeof(test_sample_no), testResultFlag);
//	strcat((char *)op, (char *)test_sample_no);

//	uart_Tarnsfter(data1,strlen((const char *)data1), testResultFlag);
//	strcat((char *)op, (char *)data1);

//	uint8_t weight_before_test[] ="Weight Before Test(Mg): ";
//	uart_Tarnsfter(weight_before_test,sizeof(weight_before_test));
//	strcat(rp203ThermalPrinterDriver_sdCard_buffer2, weight_before_test);
//
//	uart_Tarnsfter(data2,strlen((const char *)data2));
//	strcat(rp203ThermalPrinterDriver_sdCard_buffer2, data2);
//
//	uint8_t weight_after_test[] ="Weight After Test(Mg): ";
//	uart_Tarnsfter(weight_after_test,sizeof(weight_after_test));
//	strcat(rp203ThermalPrinterDriver_sdCard_buffer2, weight_after_test);
//
//	uart_Tarnsfter(data3,strlen((const char *)data3));
//	strcat(rp203ThermalPrinterDriver_sdCard_buffer2, data3);
//
//	uint8_t difference[] ="Difference:  ";
//	uart_Tarnsfter(difference,sizeof(difference));
//	strcat(rp203ThermalPrinterDriver_sdCard_buffer2, difference);
//
//	uart_Tarnsfter(data4,strlen((const char *)data4));
//	strcat(rp203ThermalPrinterDriver_sdCard_buffer2, data4);

	uint8_t test_result[] ="\nTest Result(Pass/Fail): ";
	uart_Tarnsfter(test_result,sizeof(test_result), testResultFlag);
	strcat((char *)op, (char *)test_result);

//	uart_Tarnsfter(data5,strlen((const char *)data5));
//	strcat(op, data5);




}
void printTextred_on(uint8_t *data1,uint8_t *data2, uint8_t *op, uint8_t testResultFlag)
{
	uint8_t printed_on[] = "\nPrinted On :";
	uart_Tarnsfter(printed_on,sizeof(printed_on), testResultFlag);
	strcat((char *)op, (char *)printed_on);

	uart_Tarnsfter(data1,strlen( (const char *)data1), testResultFlag);
	strcat((char *)op,(char *) data1);

	uart_Tarnsfter(data2,strlen( (const char *)data2), testResultFlag);
	strcat((char *)op,(char *) data2);

	uint8_t end[] = "\n* * * * * * * * * * * * * * * * \n";
	uart_Tarnsfter(end,sizeof(end), testResultFlag);
	strcat((char *)op,(char *) end);


}
void Test_status(uint8_t *data1,uint8_t *data2,uint8_t *data3,uint8_t *data4, uint8_t *op, uint8_t testResultFlag)
{
	uint8_t test_status[] = "\nTest Status: ";
	uart_Tarnsfter(test_status,sizeof(test_status), testResultFlag);
	strcat((char *)op, (char *)test_status);

	uart_Tarnsfter(data1,strlen( (const char *)data1), testResultFlag);
	strcat((char *)op, (char *)data1);

	uint8_t performed_by[] = "\nPerformed by : ";
	uart_Tarnsfter(performed_by,sizeof(performed_by), testResultFlag);
	strcat((char *)op, (char *)performed_by);

	uart_Tarnsfter(data2,strlen( (const char *)data2), testResultFlag);
	strcat((char *)op, (char *)data2);

	uint8_t verified_by[] = "\nVerified by : ";
	uart_Tarnsfter(verified_by,sizeof(verified_by), testResultFlag);
	strcat((char *)op, (char *)verified_by);

	uart_Tarnsfter(data3,strlen( (const char *)data3), testResultFlag);
	strcat((char *)op, (char *)data3);

	uint8_t sign[] ="\nSign:";
	uart_Tarnsfter(sign,sizeof(sign), testResultFlag);
	strcat((char *)op, (char *)sign);

	uart_Tarnsfter(data4,strlen( (const char *)data4), testResultFlag);
	strcat((char *)op, (char *)data4);


}
//void Vaccum_test_result_fun1(uint8_t *data1,uint8_t *data2)
//{
//	uint8_t data[] = "Package Integrity Tester\n";
//	uart_Tarnsfter(data,strlen(data));
//	uint8_t Model[] = "Model: ";
//	uint8_t sr_no[] = "sr.No.: ";
//	uart_Tarnsfter(Model,strlen(Model));
//	uart_Tarnsfter(data1,strlen(data1));
//	uart_Tarnsfter(sr_no,strlen(sr_no));
//	uart_Tarnsfter(data2,strlen(data2));
//
//}

// Validation Report functions

void Validation_report_start(uint8_t *data1,uint8_t *data2,uint8_t *data3,uint8_t *data4,uint8_t *data5 )
{
	uint8_t subtitle1[] = "\nValidation Report: \n";
	Vaccum_test_result_subtitle(subtitle1, 0);
	uint8_t company_name[] = "\nCompany Name: ";
	uart_Tarnsfter(company_name,sizeof(company_name), 0);
	uart_Tarnsfter(data1,strlen( (const char *)data1), 0);
	uint8_t location[] = "Location: ";
	uart_Tarnsfter(location,sizeof(location), 0);
	uart_Tarnsfter(data2,strlen( (const char *)data2), 0);
	uint8_t test_date[] = "Test Date: ";
	uart_Tarnsfter(test_date,sizeof(test_date), 0);
	uart_Tarnsfter(data3,strlen( (const char *)data3), 0);
	uint8_t test_start_time[] = "Test start Time: ";
	uart_Tarnsfter(test_start_time,sizeof(test_start_time), 0);
	uart_Tarnsfter(data4,strlen( (const char *)data4), 0);
	uint8_t test_end_time[] = "Test end Time: ";
	uart_Tarnsfter(test_end_time,sizeof(test_end_time), 0);
	uart_Tarnsfter(data5,strlen( (const char *)data5), 0);

	uint8_t subtitle3[] = "\nValidation Results: \n";
	Vaccum_test_result_subtitle(subtitle3, 0);
	Default_font();
}

//void Validation_report_product_details(uint8_t *data1,uint8_t *data2)
//{
//	uint8_t subtitle2[] = "\nProduct details: \n";
//	Vaccum_test_result_subtitle(subtitle2);
//	Default_font();
//	uint8_t prod_name[] = "\nProd Name: ";
//	uart_Tarnsfter(prod_name,sizeof(prod_name));
//	uart_Tarnsfter(data1,strlen( (const char *)data1));
//	uint8_t prod_no[] = "Prod No.: ";
//	uart_Tarnsfter(prod_no,sizeof(prod_no));
//	uart_Tarnsfter(data2,strlen( (const char *)data2));
//}

void Validation_report_results(uint8_t *srNo, uint8_t *inputmmHg, uint8_t *inputMin, uint8_t *inputSec,
		uint8_t *c_Mmhg, uint8_t *status, uint8_t *gauge_mmhg)
{

	uint8_t test_number[] = "\n\nTest Number:";
	uart_Tarnsfter(test_number,sizeof(test_number), 0);
	uart_Tarnsfter(srNo,strlen((const char *)srNo), 0);

	uint8_t input[] = "\nSet mm/Hg:";
	uart_Tarnsfter(input,sizeof(input), 0);
	uart_Tarnsfter(inputmmHg,strlen((const char *)inputmmHg), 0);

	uint8_t _inputMin[] ="\nSet Min: ";
	uart_Tarnsfter(_inputMin,sizeof(_inputMin), 0);
	uart_Tarnsfter(inputMin,strlen( (const char *)inputMin), 0);

	uint8_t _inputSec[] ="\nSet Sec: ";
	uart_Tarnsfter(_inputSec,sizeof(_inputSec), 0);
	uart_Tarnsfter(inputSec,strlen( (const char *)inputSec), 0);

//	uint8_t _c_mmhg[] ="\nMaintained mm/Hg: ";
//	uart_Tarnsfter(_c_mmhg,sizeof(_c_mmhg), 0);
//	uart_Tarnsfter(c_Mmhg,strlen( (const char *)c_Mmhg), 0);


	uint8_t _status = (uint8_t)( atoi( (char *)status) ) ;

	if( _status == 1 )
	{
		uint8_t results[] ="\nResults(pass/fail): ";
		uart_Tarnsfter(results,sizeof(results), 0);
		uart_Tarnsfter( (uint8_t *)("Pass"),4, 0);
	}
	else if( _status == 0 )
	{
		uint8_t results[] ="\nResults(pass/fail): ";
		uart_Tarnsfter(results,sizeof(results), 0);
		uart_Tarnsfter((uint8_t *)("Fail"),4, 0);
	}

		uint8_t subtitle4[] = "\nCalibration Results:";
		uart_Tarnsfter(subtitle4,strlen( (const char *)subtitle4), 0);
//		Vaccum_test_result_subtitle(subtitle4, 0);
//		Default_font();

		uint8_t meter[] ="\nGauge mm/Hg: ";
		uart_Tarnsfter(meter,sizeof(meter), 0);
		uart_Tarnsfter(gauge_mmhg,strlen( (const char *)gauge_mmhg), 0);

//		uint8_t difference[] ="\nDiff(+/-): ";
//		uart_Tarnsfter(difference,sizeof(difference), 0);
//		uint8_t setAndGaugeMmhgDiff[5] ;
//		sprintf(setAndGaugeMmhgDiff, "%03d",inputmmHg - gauge_mmhg) ;
//		uart_Tarnsfter(setAndGaugeMmhgDiff,strlen( (const char *)setAndGaugeMmhgDiff), 0);


}

void Validation_report_end(uint8_t *data1)
{
	uint8_t performed_by[] = "\nPerformed by : " ;
	uart_Tarnsfter(performed_by,strlen((const char *)performed_by), 0);
	uart_Tarnsfter(data1,strlen((const char *)data1), 0);

	uint8_t verified_by[] = "\nVerified by : ";
	uart_Tarnsfter(verified_by,strlen((const char *)verified_by), 0);
	uint8_t sign[] ="\nSign:";
	uart_Tarnsfter(sign,strlen((const char *)sign), 0);

}
