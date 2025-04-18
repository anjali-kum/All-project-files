/*
 * display.c
 *
 *  Created on: Dec 15, 2023
 *      Author: Naveen
 */
#include "authentication.h"
#include "display.h"

#include <stdlib.h>
#include "fatfs.h"

#include "ds1307_i2c.h"
#include "fatfs_sd_card.h"


#ifdef SD_CARD

FATFS       FatFs;                //Fatfs handle
FIL         fil;                  //File handle
FRESULT     fres;                 //Result after operations

#endif

uint8_t device_model[11] ;
uint8_t device_version[11] ;
uint8_t deviceUserCompanyName[17] ;
uint8_t deviceUserCompanyLocation[17] ;
uint8_t deviceinstalledDate[12] ;
uint8_t deviceFixedmmHG[5] ;
uint8_t factoryTrueFlag = 0;

uint8_t test_prod_name[26];
uint8_t test_prod_no[5];
uint8_t test_batch_no[8];
uint8_t test_batch_size[5];
uint8_t test_sample_qty[5];

uint8_t test_noOfSamples_fail[4] ;
uint8_t test_noOfSamples_pass[4] ;



uint8_t test_package_type[5];
uint8_t test_tolerance[5];

uint8_t noOfSamples ;
uint8_t verifyIDUserName[26] ;
uint8_t testResultFlag  = 0;

uint8_t modemRxBuff[MAX_MODEM_RX_BUFF];
uint8_t current_page  = 199;

uint8_t changeTimeIndex = 0;
uint8_t testprofileIndex = 0;

uint64_t _c_mmhg = 0 ;

uint8_t quickTestFlag = 3 ;
uint8_t validationTestVar = 0 ;
uint16_t validcalib_gaugeVal = 0 ;

//uint8_t calibrationModifySetMmhg = 0 ;

uint8_t testdoneRnotFlag = 0 ;
uint8_t resultsPageidx  = 0 ;

validationReport_t *validReport_t[4] ;

void endUserDeleteDetailsUpdate(uint8_t *index, uint8_t *delete1, uint8_t *delete2, uint8_t *delete3 , uint8_t *validx, uint8_t user) ;
void endUserDeleteRead(uint8_t *tempbuff, uint8_t *delete1, uint8_t *delete2, uint8_t *delete3, uint8_t *index);
static void clearDisplayFields() ;

void display_clearModemBuffer()
{
	memset(modemRxBuff,0,sizeof(modemRxBuff));
	rxBufferCount=0;
}

 void profile_page_change(uint8_t value)
{
	uint8_t nextPage_buff[] = { 0x5A, 0xA5, 0x07, 0x82, 0x00, 0x84, 0x5A, 0x01, 00, value } ;
	HAL_UART_Transmit(&huart1, nextPage_buff, sizeof(nextPage_buff), HAL_MAX_DELAY);

}


static void getTargetAddr(uint8_t *_arr, uint8_t _arrSize,uint16_t targetAddr , uint8_t value)
{
	_arr[0]		= 	0x5A 	;
	_arr[1]		= 	0xA5	;
	_arr[2]		=	0x05	;
	_arr[3]		=	0x82	;
	_arr[5]		=	targetAddr>>0	;
	_arr[4]		=	targetAddr>>8 	;

	uint8_t _srNo		=	value	;
	uint8_t arr[10] 			;
	uint8_t iter		=	0	;
	uint8_t j			=	0	;


	memset(arr,0,sizeof(arr));


	while(_srNo > 0)
	{
		_srNo 		=	_srNo/10		;
		iter++;
	}

	sprintf((char*)&arr,"%02d",value);

	for(uint8_t i= 6 ; i<8 ;i++)
	{
		_arr[i] = arr[j];
		j++;
	}
}


static void getTargetProfileAddr(uint8_t *_arr, uint8_t _arrSize, uint16_t targetAddr, uint8_t *value)
{
	uint8_t i=0 , j=0 ;
	_arr[0]		= 	0x5A 	;
	_arr[1]		= 	0xA5	;
	_arr[2]		=	3+23	;
	_arr[3]		=	0x82	;
	_arr[5]		=	targetAddr>>0	;
	_arr[4]		=	targetAddr>>8 	;

	for( i=0, j= 6;i <strlen((char*)value) ;i++,j++)
	{
		_arr[j] = value[i] ;
	}
	for( uint8_t k= j ; k<24;k++)
	{
		_arr[k] = '\0' ;
	}
}





void getTargetAddrfor16bit(uint8_t *_arr, uint8_t _arrSize,uint16_t targetAddr , uint16_t value)
{
	_arr[0]		= 	0x5A 	;
	_arr[1]		= 	0xA5	;
	_arr[2]		=	0x06	;
	_arr[3]		=	0x82	;
	_arr[5]		=	targetAddr>>0	;
	_arr[4]		=	targetAddr>>8 	;

	uint16_t _srNo		=	value	;
	uint8_t arr[10] 			;
	uint8_t iter		=	0	;
	uint8_t j			=	0	;


	memset(arr,0,sizeof(arr));


	while(_srNo > 0)
	{
		_srNo 		=	_srNo/10		;
		iter++	;
	}
	sprintf((char*)&arr,"%03d",value);


	for(uint8_t i= 6 ; i<10 ;i++)
	{
		_arr[i] = arr[j];

		j++;
	}

}


static void profile_load_update_values_uartFormate_8(uint8_t *eepromRead,uint8_t eepromReadSize,uint8_t addrFlag)
{
	/*** ADDRESS ***/
	uint16_t srNoAddr 			=	0	;
	uint16_t profilenameAddr	=	0	;
	uint16_t mmhgAddr			=	0	;
	uint16_t minAddr			=	0	;
	uint16_t secAddr			=	0	;

	if(addrFlag == 1)
	{
		 srNoAddr 			=	LOADPAGE_FST_R_SRNO_ADDR			;
		 profilenameAddr	=	LOADPAGE_FST_R_PROFILENAME_ADDR		;
		 mmhgAddr			=	LOADPAGE_FST_R_MMHG_ADDR			;
		 minAddr			=	LOADPAGE_FST_R_MIN_ADDR				;
		 secAddr			=	LOADPAGE_FST_R_SEC_ADDR				;
	}
//	HAL_UART_Transmit(&huart1, &srNoAddr, sizeof(), HAL_MAX_DELAY);
	else if(addrFlag == 2)
	{
		 srNoAddr 			=	LOADPAGE_SND_R_SRNO_ADDR			;
		 profilenameAddr	=	LOADPAGE_SND_R_PROFILENAME_ADDR		;
		 mmhgAddr			=	LOADPAGE_SND_R_MMHG_ADDR			;
		 minAddr			=	LOADPAGE_SND_R_MIN_ADDR				;
		 secAddr			=	LOADPAGE_SND_R_SEC_ADDR				;
	}
	else if(addrFlag == 3)
	{
		 srNoAddr 			=	LOADPAGE_TRD_R_SRNO_ADDR			;
		 profilenameAddr	=	LOADPAGE_TRD_R_PROFILENAME_ADDR		;
		 mmhgAddr			=	LOADPAGE_TRD_R_MMHG_ADDR			;
		 minAddr			=	LOADPAGE_TRD_R_MIN_ADDR				;
		 secAddr			=	LOADPAGE_TRD_R_SEC_ADDR				;
	}

	uint8_t _arr[31] ;
	memset(_arr,0,sizeof(_arr));
	if(eepromRead[26] == 255)
	{
		getTargetAddr(_arr, sizeof(_arr), srNoAddr, 00 ) ;
		HAL_UART_Transmit(&huart1, _arr, 8, 1000);
		HAL_UART_Transmit(&huart1, _arr, 8, 1000);


		uint16_t __mmhg = 0000 ;

		memset(_arr , 0 , sizeof(_arr));
		getTargetAddrfor16bit(_arr, sizeof(_arr),mmhgAddr, __mmhg) ;
		HAL_UART_Transmit(&huart1, _arr, 9, HAL_MAX_DELAY);


		//belowcode is for profile write min to display from eeprom with respect to Serial number
		memset(_arr , 0 , sizeof(_arr));
		getTargetAddr(_arr, sizeof(_arr),minAddr, 00) ;
		HAL_UART_Transmit(&huart1, _arr,8, HAL_MAX_DELAY);


		//below code is for profile write sec to display from eeprom with respect to Serial number
		memset(_arr , 0 , sizeof(_arr));
		getTargetAddr(_arr, sizeof(_arr),secAddr, 00) ;
		HAL_UART_Transmit(&huart1, _arr, 8, HAL_MAX_DELAY);


		memset(_arr , 0 , sizeof(_arr));
		getTargetProfileAddr(_arr, sizeof(_arr),profilenameAddr, "NULL") ;
		HAL_UART_Transmit(&huart1, _arr, 26, HAL_MAX_DELAY);
	}
	else
	{
		getTargetAddr(_arr, sizeof(_arr), srNoAddr, eepromRead[26] ) ;
		HAL_UART_Transmit(&huart1, _arr, 8, 1000);
		HAL_UART_Transmit(&huart1, _arr, 8, 1000);


		uint16_t __mmhg = eepromRead[27] << 0 | eepromRead[28] << 8 ;

		memset(_arr , 0 , sizeof(_arr));
		getTargetAddrfor16bit(_arr, sizeof(_arr),mmhgAddr, __mmhg) ;
		HAL_UART_Transmit(&huart1, _arr, 9, HAL_MAX_DELAY);


		//belowcode is for profile write min to display from eeprom with respect to Serial number
		memset(_arr , 0 , sizeof(_arr));
		getTargetAddr(_arr, sizeof(_arr),minAddr, eepromRead[29]) ;
		HAL_UART_Transmit(&huart1, _arr,8, HAL_MAX_DELAY);


		//below code is for profile write sec to display from eeprom with respect to Serial number
		memset(_arr , 0 , sizeof(_arr));
		getTargetAddr(_arr, sizeof(_arr),secAddr, eepromRead[30]) ;
		HAL_UART_Transmit(&huart1, _arr, 8, HAL_MAX_DELAY);


		memset(_arr , 0 , sizeof(_arr));
		getTargetProfileAddr(_arr, sizeof(_arr),profilenameAddr, eepromRead) ;
		HAL_UART_Transmit(&huart1, _arr, 26, HAL_MAX_DELAY);
	}




}


static void push_load_value_homePage(uint16_t mmhg, uint8_t min, uint8_t sec, uint8_t *profileName)
{
	uint8_t _arr[50] ;
	memset(_arr,0,sizeof(_arr));

	memset(_arr , 0 , sizeof(_arr));
	getTargetAddrfor16bit(_arr, sizeof(_arr),MMHG_TO_HOME_PAGE_ADDR ,mmhg ) ;
	HAL_UART_Transmit(&huart1, _arr, 9, HAL_MAX_DELAY);


	memset(_arr , 0 , sizeof(_arr));
	getTargetAddr(_arr, sizeof(_arr),MIN_TO_HOME_PAGE_ADDR, min ) ;
	HAL_UART_Transmit(&huart1, _arr, 8, HAL_MAX_DELAY);


	memset(_arr , 0 , sizeof(_arr));
	getTargetAddr(_arr, sizeof(_arr),SEC_TO_HOME_PAGE_ADDR, sec ) ;
	HAL_UART_Transmit(&huart1, _arr, 8, HAL_MAX_DELAY);

	memset(_arr , 0 , sizeof(_arr));
	getTargetProfileAddr(_arr, sizeof(_arr),PEOFILENAME_TO_HOME_PAGE_ADDR, profileName) ;
	HAL_UART_Transmit(&huart1, _arr, (26+6), HAL_MAX_DELAY);


}

static void universal_loadPage_read(uint8_t *tempbuff, uint8_t *nextFlag, uint8_t *backFlag, uint8_t *applyFlag, uint8_t *deleteFlag)
{

	char 	*c 				= NULL ;

/*** @LOAD PAGE ADRESS ***/
	uint8_t loadPage_next_buttonBuff[]  = { 0x5A, 0xA5, 0x06, 0x83, LOADPAGE_NEXTBUTTON_ADDR1, LOADPAGE_NEXTBUTTON_ADDR2, 0x01, 0x00 };
	uint8_t loadPage_prev_buttonBuff[]  = { 0x5A, 0xA5, 0x06, 0x83, LOADPAGE_BACKBUTTON_ADDR1, LOADPAGE_BACKBUTTON_ADDR2, 0x01, 0x00 };

	uint8_t loadPage_first_ApplyBuff[]  = { 0x5A, 0xA5, 0x06, 0x83, LOADPAGE_FST_R_APPLY_ADDR1, LOADPAGE_FST_R_APPLY_ADDR2, 0x01, 0x00 };
	uint8_t loadPage_second_ApplyBuff[] = { 0x5A, 0xA5, 0x06, 0x83, LOADPAGE_SND_R_APPLY_ADDR1, LOADPAGE_SND_R_APPLY_ADDR2, 0x01, 0x00 };
	uint8_t loadPage_third_ApplyBuff[]  = { 0x5A, 0xA5, 0x06, 0x83, LOADPAGE_TRD_R_APPLY_ADDR1, LOADPAGE_TRD_R_APPLY_ADDR2, 0x01, 0x00 };

	uint8_t loadPage_first_deletebuff[]  = { 0x5A, 0xA5, 0x06, 0x83, LOADPAGE_FST_DELTEBUTTON_ADDR1, LOADPAGE_FST_DELTEBUTTON_ADDR2, 0x01, 0x00, 0x01, 0x00 };
	uint8_t loadPage_second_deletebuff[]  = {  0x5A, 0xA5, 0x06, 0x83, LOADPAGE_SND_DELTEBUTTON_ADDR1, LOADPAGE_SND_DELTEBUTTON_ADDR2, 0x01, 0x00, 0x01, 0x00 };
	uint8_t loadPage_third_deletebuff[]  = { 0x5A, 0xA5, 0x06, 0x83, LOADPAGE_TRD_DELTEBUTTON_ADDR1, LOADPAGE_TRD_DELTEBUTTON_ADDR2, 0x01, 0x00, 0x01, 0x00 };


	c= NULL ;
	c = strstr((char *)tempbuff, (char *)loadPage_first_deletebuff);//FIRSTAPPLY BUTTON
	if( c!= NULL)
	{
		if(tempbuff[4] == loadPage_first_deletebuff[4] && tempbuff[5] == loadPage_first_deletebuff[5] )
			*deleteFlag = 1;

	}
	c= NULL ;
	c = strstr((char *)tempbuff, (char *)loadPage_second_deletebuff);//FIRSTAPPLY BUTTON
	if( c!= NULL)
	{
		if(tempbuff[4] == loadPage_second_deletebuff[4] && tempbuff[5] == loadPage_second_deletebuff[5] )
			*deleteFlag = 2;

	}
	c= NULL ;
	c = strstr((char *)tempbuff, (char *)loadPage_third_deletebuff);//FIRSTAPPLY BUTTON
	if( c!= NULL)
	{
		if(tempbuff[4] == loadPage_third_deletebuff[4] && tempbuff[5] == loadPage_third_deletebuff[5] )
			*deleteFlag = 3;

	}

/*** @next button ***/
	c = strstr((char *)tempbuff, (char *)loadPage_next_buttonBuff);
	if( c!= NULL)
	{

		if(tempbuff[4] == loadPage_next_buttonBuff[4] && tempbuff[5] == loadPage_next_buttonBuff[5] )
		{
			*nextFlag = 1;
		}
	}

/*** @back button ***/
	c = 	NULL ;
	c = strstr((char *)tempbuff, (char *)loadPage_prev_buttonBuff ) ;
	if( c != NULL )
	{

		if(tempbuff[4] == loadPage_prev_buttonBuff[4] && tempbuff[5] == loadPage_prev_buttonBuff[5] )
		{
			*backFlag = 1;

		}

	}

/*** @first apply button ***/
	c= NULL ;
	c = strstr((char *)tempbuff, (char *)loadPage_first_ApplyBuff);//FIRSTAPPLY BUTTON
	if( c!= NULL)
	{
		if(tempbuff[4] == loadPage_first_ApplyBuff[4] && tempbuff[5] == loadPage_first_ApplyBuff[5] )
			*applyFlag = 1;

	}


/*** @second apply button ***/
	c= NULL ;
	c = strstr((char *)tempbuff, (char *)loadPage_second_ApplyBuff);//FIRSTAPPLY BUTTON
	if( c!= NULL)
	{
		if(tempbuff[4] == loadPage_second_ApplyBuff[4] && tempbuff[5] == loadPage_second_ApplyBuff[5] )
			*applyFlag = 2;

	}

/*** @third apply button ***/
	c= NULL ;
	c = strstr((char *)tempbuff, (char *)loadPage_third_ApplyBuff);//FIRSTAPPLY BUTTON
	if( c!= NULL)
	{
		if(tempbuff[4] == loadPage_third_ApplyBuff[4] && tempbuff[5] == loadPage_third_ApplyBuff[5] )
			*applyFlag = 3;

	}

}

void arrangedeletetestprofile(uint8_t *username, uint8_t val)
{
	  uint8_t _dbg9[30] ;
	  memset(_dbg9, 0, sizeof(_dbg9));
	  POPUP_NOTOFICATION_WITHSTRING( UNIVERSAL_NOTIFICATION_ADDR2, 0x24 , _dbg9) ;
	  sprintf((char *)&_dbg9, "please wait") ;

	  deleteTestProfile(TESTPROFILE_ADDR, username, val) ;


	  uint16_t idx = mem_w25qxxDriver_getTestProfile_idx();
	  idx-- ;
	  mem_w25qxxDriver_setTestProfile_idx(idx) ;

	  memset(_dbg9, 0, sizeof(_dbg9));
	  POPUP_NOTOFICATION_WITHSTRING( (uint16_t )UNIVERSAL_NOTIFICATION_ADDR, NOTIFICATION_PAGE_NUMBER, _dbg9) ;
	  sprintf((char *)&_dbg9, "#deleted %s",username) ;
	  POPUP_NOTOFICATION_WITHSTRING( (uint16_t )UNIVERSAL_NOTIFICATION_ADDR, NOTIFICATION_PAGE_NUMBER, _dbg9) ;
}


static void universal_loadPage_update_values(uint8_t *nextFlag , uint8_t *backFlag, uint8_t *applyFlag,
		uint8_t *tp_Delete1, uint8_t page_number)
{
	uint8_t e24_index = 3 ;
	uint8_t eepromRead[31] = {0};



	if(testProfileDeleteinitVal == 9 )
	{
		memset(eepromRead,0,sizeof(eepromRead));
		mem_w25qxxDriver_Read_TestProfiles(eepromRead,sizeof(eepromRead), 0);
		profile_load_update_values_uartFormate_8(eepromRead, sizeof(eepromRead), 1);


		memset(eepromRead,0,sizeof(eepromRead));
		mem_w25qxxDriver_Read_TestProfiles(eepromRead, sizeof(eepromRead), 1);
		profile_load_update_values_uartFormate_8(eepromRead, sizeof(eepromRead), 2);

		memset(eepromRead,0,sizeof(eepromRead));
		mem_w25qxxDriver_Read_TestProfiles(eepromRead,sizeof(eepromRead),2);
		profile_load_update_values_uartFormate_8(eepromRead, sizeof(eepromRead), 3);

		testProfileDeleteinitVal = 0;
	}


	if(*nextFlag == 1)
	{

		testProfileDeleteTrackIndex++ ;

		memset(eepromRead,0,sizeof(eepromRead));
		mem_w25qxxDriver_Read_TestProfiles(eepromRead,sizeof(eepromRead), ( e24_index * testProfileDeleteTrackIndex ) );
		profile_load_update_values_uartFormate_8(eepromRead, sizeof(eepromRead), 1);

		memset(eepromRead,0,sizeof(eepromRead));
		mem_w25qxxDriver_Read_TestProfiles(eepromRead,sizeof(eepromRead),( e24_index * testProfileDeleteTrackIndex )+1 );
		profile_load_update_values_uartFormate_8(eepromRead, sizeof(eepromRead), 2);

		memset(eepromRead,0,sizeof(eepromRead));
		mem_w25qxxDriver_Read_TestProfiles(eepromRead,sizeof(eepromRead),( e24_index * testProfileDeleteTrackIndex )+2 );
		profile_load_update_values_uartFormate_8(eepromRead, sizeof(eepromRead), 3);

		*nextFlag = 0;
	}

	if(*backFlag == 1)
	{

		testProfileDeleteTrackIndex-- ;

		memset(eepromRead,0,sizeof(eepromRead));
		mem_w25qxxDriver_Read_TestProfiles(eepromRead,sizeof(eepromRead), ( e24_index * testProfileDeleteTrackIndex ) );
		profile_load_update_values_uartFormate_8(eepromRead, sizeof(eepromRead), 1);

		memset(eepromRead,0,sizeof(eepromRead));
		mem_w25qxxDriver_Read_TestProfiles(eepromRead,sizeof(eepromRead),( e24_index * testProfileDeleteTrackIndex )+1 );
		profile_load_update_values_uartFormate_8(eepromRead, sizeof(eepromRead), 2);

		memset(eepromRead,0,sizeof(eepromRead));
		mem_w25qxxDriver_Read_TestProfiles(eepromRead,sizeof(eepromRead),( e24_index * testProfileDeleteTrackIndex )+2 );
		profile_load_update_values_uartFormate_8(eepromRead, sizeof(eepromRead), 3);

		*backFlag = 0 ;
	}
	if(*applyFlag == 1)
	{
		memset(eepromRead,0,sizeof(eepromRead));
		mem_w25qxxDriver_Read_TestProfiles(eepromRead,sizeof(eepromRead), ( e24_index * testProfileDeleteTrackIndex ) );

		for( uint8_t i=0; i<26; i++)
		{
			homepage_setprofile_name[i] = eepromRead[i] ;
		}
		uint16_t __mmhg = eepromRead[27] << 0 | eepromRead[28] << 8 ;

//		uint8_t *gaugeRef[12] ;
//		memset(gaugeRef, 0, sizeof(gaugeRef)) ;
//		mem_w25qxxDriver_Read_mmhg_GAUGEval( (uint8_t *)gaugeRef, sizeof(gaugeRef)) ;
//		int dummymmhg = atoi((const char *)gaugeRef) ;

		homePage_setmmhg 		    = 	__mmhg	;
		homePage_setMin 		=	eepromRead[29]	;
		homePage_setSec 		=	eepromRead[30]	;

		profile_page_change(page_number);
		push_load_value_homePage( __mmhg , homePage_setMin, homePage_setSec, homepage_setprofile_name );

//		uint8_t data[30] ;
//		sprintf(data, "#@mm: %d\n", homePage_setmmhg) ;
//		HAL_UART_Transmit(&huart3, data, strlen(data), HAL_MAX_DELAY);

		*applyFlag = 0 ;

	}

	if(*applyFlag == 2)
	{

		memset(eepromRead,0,sizeof(eepromRead));
		mem_w25qxxDriver_Read_TestProfiles(eepromRead,sizeof(eepromRead), ( e24_index * testProfileDeleteTrackIndex )+1 );


		for( uint8_t i=0; i<26; i++)
		{
			homepage_setprofile_name[i] = eepromRead[i] ;
		}
		uint16_t __mmhg = eepromRead[27] << 0 | eepromRead[28] << 8 ;

//		uint8_t *gaugeRef[12] ;
//		memset(gaugeRef, 0, sizeof(gaugeRef)) ;
//		mem_w25qxxDriver_Read_mmhg_GAUGEval( (uint8_t *)gaugeRef, sizeof(gaugeRef)) ;
//		int dummymmhg = atoi((const char *)gaugeRef) ;

		homePage_setmmhg 		    = 	__mmhg	;
		homePage_setMin 		=	eepromRead[29]	;
		homePage_setSec 		=	eepromRead[30]	;

		profile_page_change(page_number);
		push_load_value_homePage( __mmhg , homePage_setMin, homePage_setSec, homepage_setprofile_name );

		*applyFlag = 0 ;

	}
	if(*applyFlag == 3)
	{

		memset(eepromRead,0,sizeof(eepromRead));
		mem_w25qxxDriver_Read_TestProfiles(eepromRead,sizeof(eepromRead), ( e24_index * testProfileDeleteTrackIndex )+ 2 );

		for( uint8_t i=0; i<26; i++)
		{
			homepage_setprofile_name[i] = eepromRead[i] ;
		}
		uint16_t __mmhg = eepromRead[27] << 0 | eepromRead[28] << 8 ;

//		uint8_t *gaugeRef[12] ;
//		memset(gaugeRef, 0, sizeof(gaugeRef)) ;
//		mem_w25qxxDriver_Read_mmhg_GAUGEval( (uint8_t *)gaugeRef, sizeof(gaugeRef)) ;
//		int dummymmhg = atoi((const char *)gaugeRef) ;

		homePage_setmmhg 		    = 	__mmhg 	;
		homePage_setMin 		=	eepromRead[29]	;
		homePage_setSec 		=	eepromRead[30]	;

		profile_page_change(page_number);
		push_load_value_homePage( __mmhg , homePage_setMin, homePage_setSec, homepage_setprofile_name );

		*applyFlag = 0 ;

	}

	if(*tp_Delete1 == 1)
	{
		memset(eepromRead,0,sizeof(eepromRead));
		memset(homepage_setprofile_name,0,sizeof(homepage_setprofile_name));

		mem_w25qxxDriver_Read_TestProfiles(eepromRead,sizeof(eepromRead), ( e24_index * testProfileDeleteTrackIndex ) );


		for( uint8_t i=0; i<26; i++)
		{
			homepage_setprofile_name[i] = eepromRead[i] ;
		}
		arrangedeletetestprofile(homepage_setprofile_name, ( e24_index * testProfileDeleteTrackIndex )) ;
		*tp_Delete1 = 0 ;
	}

	else if(*tp_Delete1 == 2)
	{
		memset(eepromRead,0,sizeof(eepromRead));
		memset(homepage_setprofile_name,0,sizeof(homepage_setprofile_name));

		mem_w25qxxDriver_Read_TestProfiles(eepromRead,sizeof(eepromRead), ( e24_index * testProfileDeleteTrackIndex )+1 );


		for( uint8_t i=0; i<26; i++)
		{
			homepage_setprofile_name[i] = eepromRead[i] ;
		}
		arrangedeletetestprofile(homepage_setprofile_name, ( e24_index * testProfileDeleteTrackIndex )+1) ;
		*tp_Delete1 = 0 ;
	}
	else if(*tp_Delete1 == 3)
	{
		memset(eepromRead,0,sizeof(eepromRead));
		memset(homepage_setprofile_name,0,sizeof(homepage_setprofile_name));

		mem_w25qxxDriver_Read_TestProfiles(eepromRead,sizeof(eepromRead), ( e24_index * testProfileDeleteTrackIndex )+2 );


		for( uint8_t i=0; i<26; i++)
		{
			homepage_setprofile_name[i] = eepromRead[i] ;
		}
		arrangedeletetestprofile(homepage_setprofile_name, ( e24_index * testProfileDeleteTrackIndex )+2) ;
		*tp_Delete1 = 0 ;
	}



}

uint8_t stoi(uint8_t *arr,  uint8_t size)
{
	uint8_t val = 0 ;
	for(uint8_t i=0;i<size; i++)
	{
		val = val * 10 ;
		val = val + arr[i]-'0' ;

	}
	return val ;
}
uint8_t s_s(uint8_t *arr1, uint8_t *arr2)
{
	uint8_t temp1 = stoi(arr1,2) ;
	uint8_t temp2 = stoi(arr2,2) ;
	return temp1 - temp2 ;
}

void getTestDurationTime(uint8_t *s_arr, uint8_t *e_arr,
		  uint8_t *hr, uint8_t *min, uint8_t *sec)
{
	uint8_t arr1[2] ;
	uint8_t arr2[2] ;
	for(uint8_t j=0;j<9;j += 2)
	{
		uint8_t k=0;
		for(uint8_t i=j ;i <j+2; i++,k++)
		{
			arr1[k] = s_arr[i] ;
			arr2[k] = e_arr[i] ;
		}
		if( j< 2)
			*hr = s_s(arr1,arr2) ;
		if( j<4)
			*min = s_s(arr1,arr2) ;
		if(j>4)
			*sec= s_s(arr1,arr2) ;

	}


}

uint64_t convert_to_seconds(uint8_t  hours, uint8_t minutes, uint8_t seconds) {
    return hours * 3600 + minutes * 60 + seconds;
}

void get_hms_from_seconds(uint64_t total_seconds, uint8_t *hours, uint8_t *minutes, uint8_t *seconds) {
    *hours = total_seconds / 3600;
    *minutes = (total_seconds % 3600) / 60;
    *seconds = total_seconds % 60;
}


static void universal_homePage(uint8_t *tempbuff, uint8_t *pageDirectionFlag,  uint8_t user_known)
{

	char 	*c 				= NULL ;
	uint8_t index = 0;
/*** @@UNIVERSAL HOME CALLS BELOW ***/

	uint8_t universal_homePage_home_buttonBuff[] 			= 	{ 0x5A, 0xA5, 0x06, 0x83, UNIQUE_HOMEBUTTON_ADDR1, UNIQUE_HOMEBUTTON_ADDR2, 0x01, 0x00	} 	;
	uint8_t universal_homePage_back_buttonBuff[] 			= 	{ 0X5A, 0XA5, 0X06, 0X83, UNIQUE_BACKBUTTON_ADDR1, UNIQUE_BACKBUTTON_ADDR2, 0X01, 0X00 	} 	;
	uint8_t universal_beforeStart_taking_bathNumber_buff[]  =   { BEFORESTART_BATCH_NUMBER_BUFF_ADDR1, BEFORESTART_BATCH_NUMBER_BUFF_ADDR2, 0X00 } ;
	uint8_t universal_beforeStart_taking_NoOfSamples_buff[]  =   { BEFORESTART_NO_OF_SAMPLES_BUFF_ADDR1, BEFORESTART_NO_OF_SAMPLES_BUFF_ADDR2, 0X00 } ;
	uint8_t universal_beforeStart_samples_bacthNo_applyButton_buff[]
														= { 0x5A, 0xA5, 0x06, 0x83, BEFORESTART_SAMPLES_BATCHNO_APPY_BUTTON_ADDR1
															, BEFORESTART_SAMPLES_BATCHNO_APPY_BUTTON_ADDR2
															, 0x01, 0x00, 0x01 } ;

	uint8_t universal_resultsButton_buff[] 				=	{ 0x5A, 0xA5, 0x06, 0x83, HOMEPAGE_RESULTS_BUTTON_ADDR1, HOMEPAGE_RESULTS_BUTTON_ADDR2, 0x01, 0x00, 0x01, 0X00  } ;

	uint8_t universal_homePage_menu_buttonBuff[]		= 	{ 0x5A, 0xA5, 0x06, 0x83, HOMEPAGE_MENU_BUTTON_ADDR1, HOMEPAGE_MENU_BUTTON_ADDR2, 0x01, 0x00};
	uint8_t universal_homePage_load_buttonBuff[]		= 	{ 0x5A, 0xA5, 0x06, 0x83, HOMEPAGE_LOAD_BUTTON_ADDR1, HOMEPAGE_LOAD_BUTTON_ADDR2, 0x01, 0x00 } ;
	uint8_t universal_homePage_start_buttonBuff[]		=	{ 0x5A, 0xA5, 0x06, 0x83, HOMEPAGE_HOMEPAGE_STARTBUTTON_ADDR1, HOMEPAGE_HOMEPAGE_STARTBUTTON_ADDR2, 0x01, 0x00  } ;
	uint8_t universal_homePage_stop_buttonBuff[]		=	{ 0x5A, 0xA5, 0x06, 0x83, HOMEPAGE_HOMEPAGE_STOPBUTTON_ADDR1 , HOMEPAGE_HOMEPAGE_STOPBUTTON_ADDR2 , 0x01, 0x00} ;

	uint8_t universal_notification_buttonBuff[] 		=	{ 0x5A, 0xA5, 0x06, 0x83, NOTIFICATION_OK_B1, NOTIFICATION_OK_B2, 0x01, 0x00} ;

	uint8_t universal_logOut_button[]					=	{ 0x5A, 0xA5, 0x06, 0x83, 0x24, 0xD7, 0x01, 0x00, 0x01, 0x00};


	uint8_t setmmHG_addr[]								=	{ 0X10, 0X3C, 0x00 } ;
	uint8_t setMin_addr[]								=	{ 0X10, 0X4D, 0x00 } ;
	uint8_t setSec_addr[]								=	{ 0x10, 0x52, 0x00 } ;

	uint8_t afterTest_print_yes_button[]                = { 0x5A, 0xA5, 0x06, 0x83, 0x25, 0x77, 0x01, 0x00, 0x00} ;
	uint8_t afterTest_print_no_button[]                 = { 0x5A, 0xA5, 0x06, 0x83, 0x25, 0x7B, 0x01, 0x00, 0x00} ;


/*** @@UNIVERSAL HOME CALLS BUTTONS BELOW ***/

	if( *pageDirectionFlag == PRINT_RESULTS)
	{
		c = 	NULL ;
		c = strstr((char *)tempbuff, (char *)afterTest_print_no_button ) ;
		if( (c != NULL) && (*pageDirectionFlag != SUPERVISOR_MENU_VALIDATION_ADDR) )
		{
			 index = 0 ;
			 index = c - (char *)tempbuff ;
			 if( tempbuff[index+4] == afterTest_print_no_button[4] && tempbuff[index+5] == afterTest_print_no_button[5] )
			 {
				 testResultFlag = 5;
			 }
		}

		c = 	NULL ;
		c = strstr((char *)tempbuff, (char *)afterTest_print_yes_button ) ;
		if( (c != NULL) && (*pageDirectionFlag != SUPERVISOR_MENU_VALIDATION_ADDR) )
		{
			 index = 0 ;
			 index = c - (char *)tempbuff ;
			 if( tempbuff[index+4] == afterTest_print_yes_button[4] && tempbuff[index+5] == afterTest_print_yes_button[5] )
			 {
				 testResultFlag = 6 ;
			 }
		}
	}


	c = NULL ;
	c = strstr((char *)tempbuff, (char *)setmmHG_addr ) ;
	if( c != NULL )
	{
		 index = 0 ;
		 index = c - (char *)tempbuff ;
		 if( tempbuff[index-1] == 0x83 )
		 {
			 if(tempbuff[index+1] == setmmHG_addr[1])
			 {
				 uint8_t size = tempbuff[index+4] ;

				 homePage_setmmhg = 0;
				 quickTestFlag = 3 ;
				 for(uint8_t i=0 ;i< size;i++)
				 {
					 homePage_setmmhg = homePage_setmmhg * 10 ;
					 homePage_setmmhg = homePage_setmmhg + ( tempbuff[index+5+i] - '0' ) ;
				 }
//				uint8_t *gaugeRef[12] ;
//				memset(gaugeRef, 0, sizeof(gaugeRef)) ;
//				mem_w25qxxDriver_Read_mmhg_GAUGEval( (uint8_t *)gaugeRef, sizeof(gaugeRef)) ;
//				int dummymmhg = atoi((const char *)gaugeRef) ;
//				uint8_t dt[30] ;
//				sprintf(dt, "#@dt: %d\n", dummymmhg );
//				HAL_UART_Transmit(&huart3, dt, strlen(dt), HAL_MAX_DELAY) ;

//				sprintf(dt, "#@mm: %d\n", homePage_setmmhg );
//				HAL_UART_Transmit(&huart3, dt, strlen(dt), HAL_MAX_DELAY) ;

				 quickTestFlag++ ;
			 }
		 }
	}

	c = NULL ;
	c = strstr((char *)tempbuff, (char *)setMin_addr ) ;
	if( c != NULL )
	{
		 index = 0 ;
		 index = c - (char *)tempbuff ;
		 if( tempbuff[index-1] == 0x83 )
		 {
			 if(tempbuff[index+1] == setMin_addr[1])
			 {
				 uint8_t size = tempbuff[index+4] ;

				 homePage_setMin = 0;
				 for(uint8_t i=0 ;i< size;i++)
				 {
					 homePage_setMin = homePage_setMin * 10 ;
					 homePage_setMin = homePage_setMin + ( tempbuff[index+5+i] - '0' ) ;
				 }
				 quickTestFlag++ ;
			 }
		 }
	}

	c = NULL ;
	c = strstr((char *)tempbuff, (char *)setSec_addr ) ;
	if( c != NULL )
	{
		 index = 0 ;
		 index = c - (char *)tempbuff ;
		 if( tempbuff[index-1] == 0x83 )
		 {
			 if(tempbuff[index+1] == setSec_addr[1])
			 {
				 uint8_t size = tempbuff[index+4] ;

				 homePage_setSec = 0;
				 for(uint8_t i=0 ;i< size;i++)
				 {
					 homePage_setSec = homePage_setSec * 10 ;
					 homePage_setSec = homePage_setSec + ( tempbuff[index+5+i] - '0' ) ;
				 }
				 quickTestFlag++;
			 }
		 }
	}


	c = 	NULL ;
	c = strstr((char *)tempbuff, (char *)universal_logOut_button ) ;
	if( c != NULL )
	{
		 index = 0 ;
		 index = c - (char *)tempbuff ;
		 if( tempbuff[index+4] == universal_logOut_button[4] && tempbuff[index+5] == universal_logOut_button[5] )
		 {
			 authFlag = 5 ;
			 current_page = AUTHENTICATION_PAGE ;
			 DWIN_PAGE_CHANGE(0X01) ;
			 DWIN_PAGE_CHANGE(0X01) ;

			 uint8_t dt[30] ;
			 dt[0] = 0x5A ;
			 dt[1] = 0xA5 ;
			 dt[2] = 0x05 ;
			 dt[3] = 0x82 ;
			 dt[4] = 0x10 ;
			 dt[5] = 0x00 ;
			 dt[6] = 0x00 ;
			 dt[7] = 0x00 ;

			 HAL_UART_Transmit(&huart1, dt, 8, HAL_MAX_DELAY) ;
			 dt[4] = 0x10 ;
			 dt[5] = 0x0C ;

			 HAL_UART_Transmit(&huart1, dt, 8, HAL_MAX_DELAY) ;

			 memset(auth_UserName, 0, sizeof(auth_UserName)) ;
			 memset(auth_PassWord, 0, sizeof(auth_PassWord)) ;


		 }

	}


	c = 	NULL ;
	c = strstr((char *)tempbuff, (char *)universal_notification_buttonBuff ) ;
	if( c != NULL)
	{
//		 if(testResultFlag == 1)
//			 testResultFlag = 2;
//
//		 else if(testResultFlag == 4)
//			 testResultFlag = 3;

		 if( current_page == USER_PRFILE )
		 {
			 *pageDirectionFlag = SUPERVISOR_MAIN_PAGE ;
			 profile_page_change( ENDUSER_START_PAGE );

		 }
		 else if(user_known == ADMIN_PROFILE )
		 {
			 *pageDirectionFlag = ADMIN_MAIN_PAGE ;
			 profile_page_change( ADMIN_MENU_HOME_START_PAGE_NUMBER );

		 }
		 else if( user_known == SUPERVISOR_PROFILE )
		 {
			 *pageDirectionFlag = SUPERVISOR_MAIN_PAGE ;
			 profile_page_change( SUPERVISOR_HOME_START_PAGE_NUMBER );

		 }

//		 if(testResultFlag == 9)
//		 {
//			 testResultFlag = 0 ;
//		 }

	}

	c = 	NULL ;
	c = strstr((char *)tempbuff, (char *)universal_homePage_back_buttonBuff ) ;
	if( c != NULL )
	{
		 index = 0 ;
		 index = c - (char *)tempbuff ;
		 if( tempbuff[index+4] == universal_homePage_back_buttonBuff[4] && tempbuff[index+5] == universal_homePage_back_buttonBuff[5] )
		 {

			 if( current_page == USER_PRFILE )
			 {
				 *pageDirectionFlag = SUPERVISOR_MAIN_PAGE ;
				 profile_page_change( ENDUSER_START_PAGE );

			 }
			 else if(user_known == ADMIN_PROFILE)
			 {
				 if(*pageDirectionFlag == ADMIN_LOAD_PAGE )
				 {

					 *pageDirectionFlag = ADMIN_MAIN_PAGE ;
					 profile_page_change( ADMIN_MENU_HOME_START_PAGE_NUMBER );
				 }
				 else
				 {
					 if(tempbuff[index+8] == 0x07)
					 {
						 profile_page_change(0x0B);
						 *pageDirectionFlag = 0x0B ;
					 }
					 else
					 {
						 profile_page_change(tempbuff[index+8]);
						 *pageDirectionFlag = tempbuff[index+8] ;
					 }

				 }
			 }

			 else if(user_known == SUPERVISOR_PROFILE)
			 {
				 if((*pageDirectionFlag == SUPERVISOR_LOAD_PAGE) && (tempbuff[index+8] != 0x04) )
				 {
					 *pageDirectionFlag = SUPERVISOR_MAIN_PAGE ;
					 profile_page_change( SUPERVISOR_HOME_START_PAGE_NUMBER );
				 }
				 else
				 {
					 profile_page_change(tempbuff[index+8]);
					 *pageDirectionFlag = tempbuff[index+8] ;
				 }
			 }

			 if( tempbuff[index+8] == 0x04)
			 {
				 if( current_page == USER_PRFILE)
				 {
					 *pageDirectionFlag = SUPERVISOR_MAIN_PAGE ;
					 profile_page_change( ENDUSER_START_PAGE );
				 }
				 else
				 {
					 if( user_known == SUPERVISOR_PROFILE)
					 {
						 *pageDirectionFlag = SUPERVISOR_MENU_PAGE ;
						 profile_page_change( SUPERVISOR_MENU_PAGE );
					 }
					 else if( user_known == ADMIN_PROFILE)
					 {
						 *pageDirectionFlag = ADMIN_MENU_PAGE ;
						 profile_page_change( ADMIN_MENU_PAGE );
					 }
				 }


			 }

		 }
	}
	c = 	NULL ;
	c = strstr((char *)tempbuff, (char *)universal_homePage_home_buttonBuff ) ;
	if( c != NULL )
	{
		 index = 0 ;
		 index = c - (char *)tempbuff ;
		 if( tempbuff[index+4] == universal_homePage_home_buttonBuff[4] && tempbuff[index+5] == universal_homePage_home_buttonBuff[5] )
		 {
			 if( current_page == USER_PRFILE )
			 {
				 *pageDirectionFlag = SUPERVISOR_MAIN_PAGE ;
				 profile_page_change( ENDUSER_START_PAGE );

			 }
			 else if(user_known == ADMIN_PROFILE)
			 {
				 *pageDirectionFlag = ADMIN_MAIN_PAGE ;//admin and supervisor homepages are same
				 profile_page_change(ADMIN_MENU_HOME_START_PAGE_NUMBER); //same here

			 }
			 else if(user_known == SUPERVISOR_PROFILE)
			 {
				 *pageDirectionFlag = SUPERVISOR_MAIN_PAGE ;//admin and supervisor homepages are same
				 profile_page_change(SUPERVISOR_HOME_START_PAGE_NUMBER); //same here

			 }

			 validationTestVar = 0;
		 }
	}





	c = 	NULL ;
	c = strstr((char *)tempbuff, (char *)universal_homePage_menu_buttonBuff ) ;
	if( c != NULL )
	{
		 index = 0 ;
		 index = c - (char *)tempbuff ;
		 if( tempbuff[index+4] == universal_homePage_menu_buttonBuff[4] && tempbuff[index+5] == universal_homePage_menu_buttonBuff[5] )
		 {
			 if(user_known == ADMIN_PROFILE)
			 {
				 profile_page_change(ADMIN_MENU_PAGE_NUMBER);
				 *pageDirectionFlag = ADMIN_MENU_PAGE ;

			 }
			 else if(user_known == SUPERVISOR_PROFILE)
			 {
				 profile_page_change(SUPERVISOR_MENU_PAGE_NUMBER);
				 *pageDirectionFlag = SUPERVISOR_MENU_PAGE ;
			 }
		 }
	}

	c = 	NULL ;
	c = strstr((char *)tempbuff, (char *)universal_homePage_load_buttonBuff ) ;
	if( c != NULL )
	{

		 index = 0 ;
		 index = c - (char *)tempbuff ;
		 if( tempbuff[index+4] == universal_homePage_load_buttonBuff[4] && tempbuff[index+5] == universal_homePage_load_buttonBuff[5] )
		 {
			 if(user_known == ADMIN_PROFILE)
			 {
				 profile_page_change(ADMIN_LOAD_PAGE_NUMBER);
				 *pageDirectionFlag = ADMIN_LOAD_PAGE ;
			 }
			 else if(user_known == SUPERVISOR_PROFILE)
			 {
				 profile_page_change(SUPERVISOR_LOAD_PAGE_NUMBER);
				 *pageDirectionFlag = SUPERVISOR_LOAD_PAGE ;
			 }
			 testProfileDeleteinitVal = 9 ;
			 testProfileDeleteTrackIndex= 0;

			 quickTestFlag = 2 ;
		 }
	}

		c = 	NULL ;
		c = strstr((char *)tempbuff, (char *)universal_homePage_start_buttonBuff ) ;
		if( c != NULL )
		{
			 index = 0 ;
			 index = c - (char *)tempbuff ;
			 if( (tempbuff[index+4] == universal_homePage_start_buttonBuff[4]) && (tempbuff[index+5] == universal_homePage_start_buttonBuff[5]) )
			 {
				if( ((quickTestFlag >= 6) || (quickTestFlag == 2)) && (homePage_setmmhg >0) && ( (homePage_setSec >0) || (homePage_setMin >0) )  )
				{

					profile_page_change(NO_OF_SAMPLES_TAKE_PAGE_NUMBER);
					*pageDirectionFlag = NO_OF_SAMPLES_TAKE_PAGE_NUMBER ;
					display_mmhgValueUpdateToDisplay(0, 1) ;

					if((quickTestFlag >= 6))
						sprintf((char *)homepage_setprofile_name, "QuickTest") ;

					 uint8_t dt[30] ;
					 dt[0] = 0x5A ;
					 dt[1] = 0xA5 ;
					 dt[2] = 0x05 ;
					 dt[3] = 0x82 ;
					 dt[4] = 0x23 ;
					 dt[5] = 0xC7 ;
					 dt[6] = 0x00 ;
					 dt[7] = 0x00 ;

					 HAL_UART_Transmit(&huart1, dt, 8, HAL_MAX_DELAY) ;
					 dt[4] = 0x23 ;
					 dt[5] = 0xD1 ;
					 HAL_UART_Transmit(&huart1, dt, 8, HAL_MAX_DELAY) ;


					quickTestFlag = 3 ;
				}

			 }
		}

		if(*pageDirectionFlag ==  NO_OF_SAMPLES_TAKE_PAGE_NUMBER )
		{
			c = NULL ;
			c = strstr((char *)tempbuff, (char *)universal_beforeStart_taking_bathNumber_buff ) ;
			if( c != NULL )
			{
				 index = 0 ;
				 index = c - (char *)tempbuff ;
				 if( tempbuff[index-1] == 0x83 )
				 {
					 uint8_t size = tempbuff[index+4] ;

					 for(uint8_t i=0; i< 8 ; i++)
					 {
						 if( i <= (size-1) )
						 {
							 test_batch_no[i] = tempbuff[(index+5)+i] ;
						 }
						 else
						 {
							 test_batch_no[i] = '\0' ;
						 }
					 }
					 runstopFlag++ ;
					 HAL_UART_Transmit(&huart1, test_batch_no, strlen(( char *)test_batch_no), HAL_MAX_DELAY);


				 }
			}

			c = NULL ;
			c = strstr((char *)tempbuff, (char *)universal_beforeStart_taking_NoOfSamples_buff ) ;
			if( c != NULL )
			{
				 index = 0 ;
				 index = c - (char *)tempbuff ;
				 if( tempbuff[index-1] == 0x83 )
				 {
					 uint8_t size = tempbuff[index+4] ;
					 for(uint8_t i=0; i< 5 ; i++)
					 {
						 if( i <= (size-1) )
							 test_sample_qty[i] = tempbuff[index+5+i] ;
						 else
							 test_sample_qty[i] = '\0' ;
					 }
					 runstopFlag++ ;
					 HAL_UART_Transmit(&huart1, test_sample_qty, strlen( ( char*)test_batch_no), HAL_MAX_DELAY);


				 }
			}

			c = 	NULL ;
			c = strstr((char *)tempbuff, (char *)universal_beforeStart_samples_bacthNo_applyButton_buff ) ;
			if( c != NULL )
			{
				 index = 0 ;
				 index = c - (char *)tempbuff ;
				 if( tempbuff[index+4] == universal_beforeStart_samples_bacthNo_applyButton_buff[4] &&
						 tempbuff[index+5] == universal_beforeStart_samples_bacthNo_applyButton_buff[5] )
				 {
					 if( (runstopFlag == 4) )
					 {
//						 //CHANGE: 20-05
//							uint8_t fixedmmhg[5] ;
//							mem_w25qxxDriver_read_fixed_mmhg(fixedmmhg) ;
//
//							 uint8_t data2[30] ;
//							  data2[0] = 0x5A ;
//							  data2[1] = 0xA5 ;
//							  data2[2] = 0x08 ;
//							  data2[3] = 0X82 ;
//							  data2[4] = 0x25 ;
//							  data2[5] = 0x62 ;
//							  for(uint8_t i=0, j=6; i<4;i++,j++)
//							  {
//								data2[j] = fixedmmhg[i] ;
//							  }
//							  HAL_UART_Transmit(&huart1, data2, 8, HAL_MAX_DELAY);

							if(current_page == USER_PRFILE)
							{

								profile_page_change(ENDUSER_STOP_PAGE);
								*pageDirectionFlag =	ADMIN_STOP_PAGE ;

							}
							else if(user_known == ADMIN_PROFILE)
							{
								profile_page_change(ADMIN_STOP_PAGE_NUMBER);
								*pageDirectionFlag =	ADMIN_STOP_PAGE ;
							}
							else if(user_known == SUPERVISOR_PROFILE)
							{
								 profile_page_change(SUPERVISOR_HOME_STOP_PAGE_NUMBER);
								*pageDirectionFlag =	ADMIN_STOP_PAGE ;
							}

							currenttime = 0;
							runstopFlag = 1;
							c_runstopButton = 1 ;
							c_min	=	0;
							c_sec	=	0;
							c_runtime	= 0	;
//							uint8_t _arr[20];
//							getTargetAddr(_arr, sizeof(_arr),0x1037,00 ) ;
//							HAL_UART_Transmit(&huart1, _arr, 8, HAL_MAX_DELAY);

							rp203ThermalPrinterDriver_startHour = ds1307_hour ;
							rp203ThermalPrinterDriver_startMin 	= ds1307_minute ;
							rp203ThermalPrinterDriver_startSec 	= ds1307_second ;

							memset(rp203ThermalPrinterDriver_TestDate, 0, sizeof(rp203ThermalPrinterDriver_TestDate)) ;
							sprintf((char *)rp203ThermalPrinterDriver_TestDate, "%02d:%02d:%04d",ds1307_date,ds1307_month, ds1307_year) ;

							uni_sec = 0;

					 }
				 }
			}
		}


		c = 	NULL ;
		c = strstr((char *)tempbuff, (char *)universal_resultsButton_buff ) ;
		if( c != NULL )
		{
			 index = 0 ;
			 index = c - (char *)tempbuff ;
			 if( tempbuff[index+4] == universal_resultsButton_buff[4] && tempbuff[index+5] == universal_resultsButton_buff[5] )
			 {
				 profile_page_change(RESULTS_PAGE);
				 *pageDirectionFlag = RESULTS_PAGE ;

				 resultsPageidx = 0 ;

				 mem_w25qxxDriver_ResultsPage_Read_nd_show_Results(resultsPageidx) ;
			 }
		}

		if( *pageDirectionFlag ==  RESULTS_PAGE )
		{
			uint8_t resultsNextButton_addr[] = { 0x5A, 0xA5, 0x06, 0x83, 0x25, 0x7F, 0x01, 0x00, 0x01, 0x00} ;
			uint8_t resultsBackButton_addr[] = { 0x5A, 0xA5, 0x06, 0x83, 0x25, 0x83, 0x01, 0x00, 0x01, 0x00} ;

			c = 	NULL ;
			c = strstr((char *)tempbuff, (char *)resultsNextButton_addr ) ;
			if( c != NULL )
			{
				 index = 0 ;
				 index = c - (char *)tempbuff ;
				 if( tempbuff[index+4] == resultsNextButton_addr[4] && tempbuff[index+5] == resultsNextButton_addr[5] )
				 {
					 if(resultsPageidx <16)
						 resultsPageidx++ ;
					 mem_w25qxxDriver_ResultsPage_Read_nd_show_Results(resultsPageidx) ;
				 }
			}

			c = 	NULL ;
			c = strstr((char *)tempbuff, (char *)resultsBackButton_addr ) ;
			if( c != NULL )
			{
				 index = 0 ;
				 index = c - (char *)tempbuff ;
				 if( tempbuff[index+4] == resultsBackButton_addr[4] && tempbuff[index+5] == resultsBackButton_addr[5] )
				 {
					 if(resultsPageidx >0)
						 resultsPageidx-- ;

					 mem_w25qxxDriver_ResultsPage_Read_nd_show_Results(resultsPageidx) ;
				 }
			}

		}






		c = 	NULL ;
		c = strstr((char *)tempbuff, (char *)universal_homePage_stop_buttonBuff ) ;
		if( c != NULL )
		{
			 index = 0 ;
			 index = c - (char *)tempbuff ;
			 if( tempbuff[index+4] == universal_homePage_stop_buttonBuff[4] && tempbuff[index+5] == universal_homePage_stop_buttonBuff[5] )
			 {

				 _c_mmhg = c_mmhg ;
				  rp203ThermalPrinterDriver_endHour = ds1307_hour ;
				  rp203ThermalPrinterDriver_endMin = ds1307_minute ;
				  rp203ThermalPrinterDriver_endSec = ds1307_second ;

					 c_runstopButton = 2 ;
					 runstopFlag = 2 ;

//					 testResultFlag = 4 ;

					 testdoneRnotFlag = 1 ;
					 quickTestFlag = 3 ;

					 uint8_t dbg[30] ;

					  memset(dbg, 0, sizeof(dbg));
					  uint8_t data1[] = "Print results?" ;
					  DWIN_PAGE_CHANGE1(dbg, NOTIFICATION_W_YES_R_NO_PAGE_NUMBER) ;
					  DWIN_PAGE_CHANGE1(dbg, NOTIFICATION_W_YES_R_NO_PAGE_NUMBER) ;

					  memset(dbg, 0, sizeof(dbg)) ;
					  POPUP_NOTIFICATION(dbg, UNIVERSAL_NOTIFICATION_ADDR, data1, strlen( (char *) data1) ) ;
					  POPUP_NOTIFICATION(dbg, UNIVERSAL_NOTIFICATION_ADDR, data1, strlen( (char *) data1) ) ;

					  *pageDirectionFlag = PRINT_RESULTS ;
			 }
		}



		//below code is for results print
		if((testResultFlag == 6) || (testResultFlag == 5))
		{
			uint8_t debugBuff[50] ;
			uint8_t op[300] ;
			memset(debugBuff, 0, sizeof(debugBuff)) ;

			uint8_t _dbg9[30] ;
			memset(_dbg9, 0, sizeof(_dbg9));

			memset(_dbg9, 0, sizeof(_dbg9));
			POPUP_NOTOFICATION_WITHSTRING(UNIVERSAL_NOTIFICATION_ADDR2,0x24 , _dbg9) ;
			sprintf((char *)&_dbg9, "Please wait") ;
			POPUP_NOTOFICATION_WITHSTRING(UNIVERSAL_NOTIFICATION_ADDR2, 0x24, _dbg9) ;

		  FATFS       FatFs;                //Fatfs handle
		  FIL         fil;                  //File handle
		  FRESULT     fres;                 //Result after operations

		    //Mount the SD Card
		    fres = f_mount(&FatFs, "", 1);    //1=mount now
		    HAL_Delay(2) ;
		    if (fres != FR_OK)
		    {

//			  HAL_Delay(1000) ;
		    }
		    else
		    {
//			    memset(_dbg9, 0, sizeof(_dbg9));
//			    POPUP_NOTOFICATION_WITHSTRING(UNIVERSAL_NOTIFICATION_ADDR2, 0x24 ,  _dbg9) ;
//			    sprintf((char *)&_dbg9, "vaccum released") ;
//			    POPUP_NOTOFICATION_WITHSTRING(UNIVERSAL_NOTIFICATION_ADDR2, 0x24, _dbg9) ;
//			    HAL_Delay(1000) ;
		    }




		    //Read the SD Card Total size and Free Size
		    FATFS *pfs;
		    DWORD fre_clust;
		    uint32_t totalSpace, freeSpace;

		    f_getfree("", &fre_clust, &pfs);
		    totalSpace = (uint32_t)((pfs->n_fatent - 2) * pfs->csize * 0.5);
		    freeSpace = (uint32_t)(fre_clust * pfs->csize * 0.5);

		    sprintf( (char *)debugBuff,"TotalSpace : %lu bytes, FreeSpace = %lu bytes\n", totalSpace, freeSpace);
//		    HAL_UART_Transmit(&huart3, debugBuff, strlen(debugBuff), HAL_MAX_DELAY) ;

			//Open the file
			uint8_t timestamp[50] ;
			memset(timestamp, 0, sizeof(timestamp)) ;
			sprintf( (char *)timestamp, "%d_%d_%d_%d_%d_%d.txt",
					ds1307_year, ds1307_month, ds1307_date, ds1307_hour, ds1307_minute, ds1307_second) ;

		    fres = f_open(&fil, (char *)timestamp , FA_WRITE | FA_READ | FA_CREATE_ALWAYS);
		    if(fres != FR_OK)
		    {
//		      sprintf(debugBuff,"File creation/open Error : (%i)\r\n", fres);
//		      HAL_UART_Transmit(&huart3, debugBuff, strlen(debugBuff), HAL_MAX_DELAY) ;
		    }


			uint8_t arr1[26] ;
			uint8_t arr2[26] ;
			uint8_t arr3[26] ;
			uint8_t arr4[26] ;
			uint8_t arr5[26] ;
			uint8_t arr6[26] ;

			mem_w25qxxDriver_read_device_details(device_model, device_version, deviceUserCompanyName, deviceUserCompanyLocation );
			strcat((char *)&device_model,"\n") ;
			strcat((char *)&device_version,"\n");
			memset(op, 0, sizeof(op)) ;
			Vaccum_test_result_title( (uint8_t *)device_model, (uint8_t *)device_version, op, testResultFlag);
			f_puts( (char *)op,  &fil) ;


			strcat((char *)&deviceUserCompanyName,"\n");
			strcat((char *)&deviceUserCompanyLocation,"\n");
			memset(arr1, 0, sizeof(arr1));

				memcpy(arr1, auth_UserName, strlen((const char *)auth_UserName));
				strcat((char *)arr1, "\n");

			memset(arr2, 0, sizeof(arr2));
			sprintf((char *)&arr2, "%02d:%02d:%02d sec \n",
					 rp203ThermalPrinterDriver_startHour,
					 rp203ThermalPrinterDriver_startMin,
					 rp203ThermalPrinterDriver_startSec) ;

			memset(arr3, 0, sizeof(arr3));
			sprintf((char *)&arr3, "%02d:%02d:%02d sec \n",
					rp203ThermalPrinterDriver_endHour,
					rp203ThermalPrinterDriver_endMin,
					rp203ThermalPrinterDriver_endSec) ;

			strcat((char *)rp203ThermalPrinterDriver_TestDate, "\n") ;

			memset(op, 0, sizeof(op)) ;
			Vaccum_test_result_report( deviceUserCompanyName, deviceUserCompanyLocation, rp203ThermalPrinterDriver_TestDate,
				 arr2, arr3, arr1, op, testResultFlag);
			f_puts( (char *)op,  &fil) ;

			memset(op, 0, sizeof(op)) ;
			User_prod_details(test_prod_name, test_prod_no, test_batch_no, test_batch_size,
				  test_sample_qty, test_package_type, test_tolerance, op, testResultFlag);
			f_puts( (char *)op,  &fil) ;


			memset(arr1, 0, sizeof(arr1));
			memset(arr2, 0, sizeof(arr2));
			sprintf((char *)arr1, "%04d mm/Hg \n",homePage_setmmhg);

		    uint8_t *gaugeRef[12] ;
			memset(gaugeRef, 0, sizeof(gaugeRef)) ;
			mem_w25qxxDriver_Read_mmhg_GAUGEval( (uint8_t *)gaugeRef, sizeof(gaugeRef)) ;
			int dummymmhg = atoi((const char *)&gaugeRef) ;

			if(testdoneRnotFlag == 2)
				sprintf((char *)&arr2, "%04d mm/Hg \n",(uint64_t)homePage_setmmhg); // CHANGE: 20-05
			if(testdoneRnotFlag == 1)
				sprintf((char *)&arr2, "%04d mm/Hg \n",(uint64_t)_c_mmhg - dummymmhg); // CHANGE: 20-05




			memset(op, 0, sizeof(op)) ;
			Test_settings(arr1, arr2, op, testResultFlag);
			f_puts((char *)op,  &fil) ;


			memset(arr1, 0, sizeof(arr1));
			memset(arr2, 0, sizeof(arr2));
			memset(arr3, 0, sizeof(arr3));
			memset(arr4, 0, sizeof(arr4));
			memset(arr5, 0, sizeof(arr5));
			memset(arr6, 0, sizeof(arr6));

			uint64_t start_seconds_total = convert_to_seconds(rp203ThermalPrinterDriver_startHour,
														rp203ThermalPrinterDriver_startMin,
														rp203ThermalPrinterDriver_startSec);

			uint64_t end_seconds_total = convert_to_seconds(rp203ThermalPrinterDriver_endHour,
														rp203ThermalPrinterDriver_endMin,
														rp203ThermalPrinterDriver_endSec);

			uint64_t diff_seconds = end_seconds_total - start_seconds_total;

		    uint8_t  diff_hours, diff_minutes, diff_seconds_rem;
		    get_hms_from_seconds(diff_seconds, &diff_hours, &diff_minutes, &diff_seconds_rem);

		    sprintf((char *)&arr1, "%02d:%02d:%02d sec \n",diff_hours, diff_minutes, diff_seconds_rem) ;
		    uint8_t *duretionTimeForResults[6] ;
		    sprintf((char *)&duretionTimeForResults, "%02d:%02d", diff_minutes, diff_seconds_rem) ;

			sprintf((char *)arr2, "%d \n", noOfSamples) ;
			sprintf((char *)arr3, "%d \n", weight_before_test_val) ;
			sprintf((char *)arr4, "%d \n", weight_after_test_val) ;
			sprintf((char *)arr5, "%d \n", weight_after_test_val-weight_before_test_val) ;
			if(testResultFlag == 1)
			  sprintf((char *)arr6, "pass\n" ) ;
			if(testResultFlag == 0)
			  sprintf((char *)arr6, "fail\n" ) ;

			memset(op, 0, sizeof(op)) ;
			Test_result(arr1, arr2, arr3, arr4, arr5, arr6, op, testResultFlag);
			f_puts((char *)op,  &fil) ;



			memset(arr1, 0, sizeof(arr1));
			memset(arr2, 0, sizeof(arr2));
			memset(arr3, 0, sizeof(arr3));
			memset(arr4, 0, sizeof(arr4));

			memcpy(arr1, auth_UserName, strlen((const char *)auth_UserName));
			strcat((char *)arr1, "\n");

			if(testdoneRnotFlag == 2)
			  sprintf((char *)arr2, "Completed\n" ) ;
			if(testdoneRnotFlag == 1)
			  sprintf((char *)arr2, "Aborted\n" ) ;

			memcpy(arr3, verifyIDUserName, strlen((const char *)verifyIDUserName)) ;
			strcat((char *)arr3, "\n");

			memset(op, 0, sizeof(op)) ;
			Test_status(arr2,arr1,arr3,arr3, op, testResultFlag);
			f_puts((char *)op,  &fil) ;



			getTime() ;
			memset(arr1, 0, sizeof(arr1));
			memset(arr2, 0, sizeof(arr2));
			sprintf((char *)arr1, "%02d:%02d:%02d \n",ds1307_hour, ds1307_minute, ds1307_second) ;
			sprintf((char *)arr2, "%02d/%02d/%04d ",ds1307_date, ds1307_month, ds1307_year) ;

			memset(op, 0, sizeof(op)) ;
			printTextred_on(arr2,arr1, op, testResultFlag);
			f_puts((char *)op,  &fil) ;

		    //close your file
		    f_close(&fil);

		    //close your file
		    f_close(&fil);

		  //We're done, so de-mount the drive
		  f_mount(NULL, "", 0);

		  if(testResultFlag == 5)
		  {
			  HAL_Delay(22000) ;
		  }
		  else
		  {
			  HAL_Delay(19000) ;
		  }

			if(testdoneRnotFlag == 2)
			  sprintf((char *)arr6, "pass" ) ;
			if(testdoneRnotFlag == 1)
			  sprintf((char *)arr6, "fail" ) ;

			uint8_t fixedmmhg[6] ;
			sprintf((char *)&fixedmmhg, "%03ld",(uint64_t)_c_mmhg - dummymmhg) ;

			uint8_t storestartTime[13] ;
			sprintf((char *)storestartTime, "%02d:%02d:%02d",rp203ThermalPrinterDriver_startHour, rp203ThermalPrinterDriver_startMin, rp203ThermalPrinterDriver_startSec ) ;
			mem_w25qxxDriver_ResultsPage_write(homepage_setprofile_name, (uint8_t *)duretionTimeForResults ,auth_UserName, arr2, storestartTime, fixedmmhg, test_sample_qty, arr6 ) ;


			testResultFlag = 0 ;
			testdoneRnotFlag = 0;
			homePage_setSec = 0 ;
			homePage_setMin = 0 ;
			homePage_setmmhg = 0 ;

			 if( current_page == USER_PRFILE )
			 {
				 *pageDirectionFlag = SUPERVISOR_MAIN_PAGE ;
				 profile_page_change( ENDUSER_START_PAGE );

			 }
			 else if(user_known == ADMIN_PROFILE )
			 {
				 *pageDirectionFlag = ADMIN_MAIN_PAGE ;
				 profile_page_change( ADMIN_MENU_HOME_START_PAGE_NUMBER );

			 }
			 else if( user_known == SUPERVISOR_PROFILE )
			 {
				 *pageDirectionFlag = SUPERVISOR_MAIN_PAGE ;
				 profile_page_change( SUPERVISOR_HOME_START_PAGE_NUMBER );

			 }
			 clearDisplayFields() ;

			 HAL_GPIO_WritePin(VACCUM_PRESSURE_GPIO_Port,VACCUM_PRESSURE_Pin,1);
		}






}

/* ########################################################################################################################################################################*/
/************************************ADMIN PAGE DETAILS BELOW************************************/
/* ########################################################################################################################################################################*/


void userDeleteRead(uint8_t *tempbuff, uint8_t *delete1, uint8_t *delete2, uint8_t *delete3, uint8_t *index)
{
	char *c 	= 		NULL ;
	uint8_t idx = 0 ;

	uint8_t adminMenuSupervisorDlete_1_ButtonBuff[] 	= { 0x5A, 0xA5, 0x06, 0x83, ADMIN_MENU_SUPERVISOR_DELETE_1ST_DELETE_BUTTON_ADDR1, ADMIN_MENU_SUPERVISOR_DELETE_1ST_DELETE_BUTTON_ADDR2, 0x01, 0x00, 0x01, 0x00 } ;
	uint8_t adminMenuSupervisorDlete_2_ButtonBuff[] 	= { 0x5A, 0xA5, 0x06, 0x83, ADMIN_MENU_SUPERVISOR_DELETE_2ND_DELETE_BUTTON_ADDR1, ADMIN_MENU_SUPERVISOR_DELETE_2ND_DELETE_BUTTON_ADDR2, 0x01, 0x00, 0x01, 0x00 } ;
	uint8_t adminMenuSupervisorDlete_3_ButtonBuff[] 	= { 0x5A, 0xA5, 0x06, 0x83, ADMIN_MENU_SUPERVISOR_DELETE_3RD_DELETE_BUTTON_ADDR1, ADMIN_MENU_SUPERVISOR_DELETE_3RD_DELETE_BUTTON_ADDR2, 0x01, 0X00, 0x01, 0x00 } ;
	uint8_t adminMenuSupervisorDleteBackButtonBuff[]	= { 0x5A, 0xA5, 0x06, 0x83, ADMIN_MENU_SUPERVISOR_DELETE_BACK_BUTTON_ADDR1, ADMIN_MENU_SUPERVISOR_DELETE_BACK_BUTTON_ADDR2, 0x01, 0x00, 0x01, 0X00 } ;
	uint8_t adminMenuSupervisorDleteNextButtonBuff[] 	= { 0x5A, 0xA5, 0x06, 0x83, ADMIN_MENU_SUPERVISOR_DELETE_NEXT_BUTTON_ADDR1, ADMIN_MENU_SUPERVISOR_DELETE_NEXT_BUTTON_ADDR2, 0x01, 0x00, 0x01, 0X00 } ;

	c = strstr((char *)tempbuff, (char *)adminMenuSupervisorDlete_1_ButtonBuff);
	if( c!= NULL)
	{
		idx = 0 ;
		idx = c - (char *)tempbuff ;
		 if( tempbuff[idx+3] == 0x83 )
		 {
				*delete1 = 1 ;
//				HAL_UART_Transmit(&huart3, &"delete 1 clk\n", 15, HAL_MAX_DELAY) ;

		 }

	}
	c = strstr((char *)tempbuff, (char *)adminMenuSupervisorDlete_2_ButtonBuff);
	if( c!= NULL)
	{
		idx = 0 ;
		idx = c - (char *)tempbuff ;
		 if( tempbuff[idx+3] == 0x83 )
		 {
				*delete2 = 1 ;
//				HAL_UART_Transmit(&huart3, &"delete 2 clk\n", 15, HAL_MAX_DELAY) ;
		 }

	}
	c = strstr((char *)tempbuff, (char *)adminMenuSupervisorDlete_3_ButtonBuff);
	if( c!= NULL)
	{
		idx = 0 ;
		idx = c - (char *)tempbuff ;
		 if( tempbuff[idx+3] == 0x83 )
		 {
				*delete3 = 1 ;
//				HAL_UART_Transmit(&huart3, &"delete 3 clk\n", 15, HAL_MAX_DELAY) ;
		 }

	}
	c = strstr((char *)tempbuff, (char *)adminMenuSupervisorDleteNextButtonBuff);
	if( c!= NULL)
	{
		idx = 0 ;
		idx = c - (char *)tempbuff ;
		 if( tempbuff[idx+3] == 0x83 )
		 {
				*index = 2 ;
//				HAL_UART_Transmit(&huart3, &"next clk\n", 12, HAL_MAX_DELAY) ;
		 }

	}
	c = strstr((char *)tempbuff, (char *)adminMenuSupervisorDleteBackButtonBuff);
	if( c!= NULL)
	{
		idx = 0 ;
		idx = c - (char *)tempbuff ;
		 if( tempbuff[idx+3] == 0x83 )
		 {
				*index = 3;
//				HAL_UART_Transmit(&huart3, &"back clk\n", 12, HAL_MAX_DELAY) ;
		 }

	}
}

void readdwingetint16(uint8_t *tempbuff, uint8_t *comparebuff, uint16_t *intval, uint8_t *idx)
{
	 char *c= NULL ;
	c = strstr((char *)tempbuff, (char *)comparebuff) ;
	if( c != NULL)
	{
		 uint8_t index = 0 ;
		 index = c - (char *)tempbuff ;
		 if( tempbuff[index-1] == 0x83 )
		 {
			 ++*idx ;

			 uint8_t size = tempbuff[index+4] ;
			 uint16_t val = 0;
			 *intval = 0;

			 for(uint8_t i=0 ;i< size;i++)
			 {
				 val = (val) * 10 ;
				 val = val + ( tempbuff[index+5+i] - '0' ) ;


			 }
			 *intval = val ;

		 }
	}

}

void readdwingetint8(uint8_t *tempbuff, uint8_t *comparebuff, uint8_t *intval, uint8_t *idx)
{
	 char *c= NULL ;
	c = strstr((char *)tempbuff, (char *)comparebuff) ;
	if( c != NULL)
	{
		 uint8_t index = 0 ;
		 index = c - (char *)tempbuff ;
		 if( tempbuff[index-1] == 0x83 )
		 {
			 ++*idx ;

			 uint8_t size = tempbuff[index+4] ;
			 uint16_t val = 0;
			 *intval = 0;

			 for(uint8_t i=0 ;i< size;i++)
			 {
				 val = (val) * 10 ;
				 val = val + ( tempbuff[index+5+i] - '0' ) ;


			 }
			 *intval = val ;


		 }
	}

}

void authentication_AdminPage_Read(uint8_t *displayUartFlag, uint8_t *adminPageDirectionFlag,  uint8_t *nextFlag, uint8_t *backFlag, uint8_t *applyFlag, uint8_t *c_runstopButton, uint8_t *loadpage_deleteFlag )
{
	uint8_t tempbuff[MAX_MODEM_RX_BUFF] = {0};
	char *c = NULL ;
	uint8_t index = 0;

/*** @@ADMIN MENU PAGE ***/
	uint8_t admin_add_supervisor_buttonBuff[]	=	{ 0x5A, 0xA5, 0x06, 0x83, ADMIN_MENU_ADD_SUPERV_BUTTON_ADDR1, ADMIN_MENU_ADD_SUPERV_BUTTON_ADDR2, 0x01	, 0x00, 0x01}	;
	uint8_t admin_changetime_buttonBuff[]		=	{ 0x5A, 0xA5, 0x06, 0x83, ADMIN_MENU_CHANGE_TIME_BUTTON_ADDR1, ADMIN_MENU_CHANGE_TIME_BUTTON_ADDR2, 0x01  , 0x00} 	;
	uint8_t admin_changePassword_buttonBuff[]	=	{ 0x5A, 0xA5, 0x06, 0x83, ADMIN_MENU_CHANGE_PASSWORD_BUTTON_ADDR1, ADMIN_MENU_CHANGE_PASSWORD_BUTTON_ADDR2, 0x01, 0x00 } 	;

	uint8_t admin_deletesupervsiorUsers_buttonBuff[]	= 	{ 0x5A, 0xA5, 0x06, 0x83, ADMIN_MENU_DELETESUPERVISOR_BUTTON_ADDR1, ADMIN_MENU_DELETESUPERVISOR_BUTTON_ADDR2, 0x01, 0x00, 0x00  } ;
	uint8_t admin_menu_passwordChange_present_username[]	=	{ ADMIN_MENU_CHANGE_PASSWORD_PRESENT_ADDR1, ADMIN_MENU_CHANGE_PASSWORD_PRESENT_ADDR2 , 0X00} 	;
	uint8_t admin_menu_passwordChange_New_username[]	=	{ ADMIN_MENU_CHANGE_PASSWORD_NEW_ADDR1, ADMIN_MENU_CHANGE_PASSWORD_NEW_ADDR2, 0X00 } 	;
	uint8_t admin_menu_passwordChange_addButton[]	=	{ 0x5A, 0xA5, 0x06, 0x83, ADMIN_MENU_CHANGE_PASSWORD_ADD_BUTTON_ADDR1, ADMIN_MENU_CHANGE_PASSWORD_ADD_BUTTON_ADDR2, 0x01, 0x00 } 	;



	uint8_t admin_add_supervisor_password_addr[]  = 	{ ADMIN_MENU_ADD_SUPERV_PASSWORD_BUTTON_ADDR1, ADMIN_MENU_ADD_SUPERV_PASSWORD_BUTTON_ADDR2 , 0x00} ;
	uint8_t admin_add_supervisor_username_addr[]  = 	{ ADMIN_MENU_ADD_SUPERV_USERNAME_BUTTON_ADDR1, ADMIN_MENU_ADD_SUPERV_USERNAME_BUTTON_ADDR2 , 0x00} ; //5A A5 06 83 12 50 01
	uint8_t admin_add_supervisor_addButton_addr[]  = 	{ 0x5A, 0xA5, 0x06, 0x83, ADMIN_MENU_ADD_SUPERV_ADD_BUTTON_ADDR1, ADMIN_MENU_ADD_SUPERV_ADD_BUTTON_ADDR2, 0x01, 0x00 } ;

	uint8_t admin_add_multi_admin_addr[] 			= 	{ 0x5A, 0xA5, 0x06, 0x83, ADMIN_MENU_ADD_MUITLIADMIN_BUTTON_ADDR1, ADMIN_MENU_ADD_MUITLIADMIN_BUTTON_ADDR2, 0x01, 0x00, 0x00 } ;
	uint8_t admin_delete_multi_admin_addr[] 			= 	{ 0x5A, 0xA5, 0x06, 0x83, ADMIN_MENU_DELETE_MUITLIADMIN_BUTTON_ADDR1, ADMIN_MENU_DELETE_MUITLIADMIN_BUTTON_ADDR2, 0x01, 0x00, 0x00 } ;
	uint8_t admin_factory_settings_addr[] 			= 	{ 0x5A, 0xA5, 0x06, 0x83, ADMIN_MENU_FACTORYSETTINGS_BUTTON_ADDR1, ADMIN_MENU_FACTORYSETTINGS_BUTTON_ADDR2, 0x01, 0x00, 0x00 } ;

	uint8_t admin_add_multi_adminpassword_addr[] 			= { SUPERVISOR_MENU_ADD_USER_USERPASSWORD_BUTTON_ADDR1, SUPERVISOR_MENU_ADD_USER_USERPASSWORD_BUTTON_ADDR2 , 0x00} ;
	uint8_t admin_add_multi_adminUsername_addr[] 			= {SUPERVISOR_MENU_ADD_USER_USERNAMEE_BUTTON_ADDR1, SUPERVISOR_MENU_ADD_USER_USERNAMEE_BUTTON_ADDR2, 0x00} ;

	uint8_t admin_add_multi_adminaddButton_addr[] 			= { 0x5A, 0xA5, 0x06, 0x83, SUPERVISOR_MENU_ADD_USER_ADD_BUTTON_ADDR1,SUPERVISOR_MENU_ADD_USER_ADD_BUTTON_ADDR2, 0x01, 0x00 } ;


	if( *displayUartFlag == 1 )
	{
		HAL_Delay(15);
		memset(tempbuff, 0, sizeof(tempbuff));
		for(int i=0; i< rxBufferCount ; i++)
		{
			tempbuff[i] = modemRxBuff[i];
		}
//		HAL_UART_Transmit(&huart3, tempbuff, rxBufferCount, HAL_MAX_DELAY);
		display_clearModemBuffer();




/*** @@ADMIN UNIVERSAL BOTTON ***/
		universal_homePage(tempbuff, adminPageDirectionFlag, ADMIN_PROFILE);

/*** @@ADMIN LOAD PAGE READ INPUTS ***/
		universal_loadPage_read(tempbuff, nextFlag, backFlag, applyFlag, loadpage_deleteFlag );

		if( *adminPageDirectionFlag == ADMIN_LOAD_PAGE)
		{
			 universal_loadPage_update_values(nextFlag, backFlag, applyFlag, loadpage_deleteFlag, ADMIN_MAIN_PAGE);
		}
/*** @@ADMIN MENU PAGE ***/
//#if(0)
		c = 	NULL ;
		c = strstr( ( char *)tempbuff, ( char *)admin_add_supervisor_buttonBuff ) ;
		if( c != NULL )
		{
			 index = 0 ;
			 index = c - (char *)tempbuff ;

			 if( tempbuff[index+5] == ADMIN_MENU_ADD_SUPERV_BUTTON_ADDR2 )
			 {
				 profile_page_change(ADMIN_ADD_SUPERVISOR_NUMBER);
				 *adminPageDirectionFlag = ADMIN_MENU_ADDSUPERVISOR_PAGE ;

				 uint8_t dt[30] ;
				 dt[0] = 0x5A ;
				 dt[1] = 0xA5 ;
				 dt[2] = 0x05 ;
				 dt[3] = 0x82 ;
				 dt[4] = 0x11 ;
				 dt[5] = 0xA8 ;
				 dt[6] = 0x00 ;
				 dt[7] = 0x00 ;

				 HAL_UART_Transmit(&huart1, dt, 8, HAL_MAX_DELAY) ;
				 dt[4] = 0x11 ;
				 dt[5] = 0xB5 ;

				 HAL_UART_Transmit(&huart1, dt, 8, HAL_MAX_DELAY) ;
			 }
		}

		/*.......................................*** @@ADMIN-MENU-ADD SUPERVISOR-PAGE ***............................................................................................................*/
		c = NULL ;
		c = strstr((char *)tempbuff, (char *)admin_add_supervisor_password_addr ) ;
		if( c != NULL )
		{
			 index = 0 ;
			 index = c - (char *)tempbuff ;
			 if( tempbuff[index-1] == 0x83 )
			 {
				 uint8_t size = tempbuff[index+4] ;
				 for(uint8_t i=0; i< 11 ; i++)
				 {
					 if( i <= (size-1) )
						 admin_supervisor_password[i] = tempbuff[index+5+i] ;
					 else
						 admin_supervisor_password[i] = '\0' ;
				 }
//				 HAL_UART_Transmit(&huart1, admin_supervisor_password, strlen(admin_supervisor_password), HAL_MAX_DELAY);

			 }
		}

		c = NULL ;
		c = strstr((char *)tempbuff, (char *)admin_add_supervisor_username_addr ) ;
		if( c != NULL )
		{
			 index = 0 ;
			 index = c - (char *)tempbuff ;
			 if( tempbuff[index-1] == 0x83 )
			 {
				 uint8_t size = tempbuff[index+4] ;
				 for(uint8_t i=0; i< 11 ; i++)
				 {
					 if( i <= (size-1) )
						 admin_supervisor_username[i] = tempbuff[index+5+i] ;
					 else
						 admin_supervisor_username[i] = '\0' ;
				 }
//				 HAL_UART_Transmit(&huart1, admin_supervisor_username, strlen(admin_supervisor_username), HAL_MAX_DELAY);


			 }
		}

		c = NULL ;
		c = strstr((char *)tempbuff, (char *)admin_add_supervisor_addButton_addr ) ;
		if( c != NULL )
		{
			 index = 0 ;
			 index = c - (char *)tempbuff ;
			 if( tempbuff[index+4] == admin_add_supervisor_addButton_addr[index +4] &&
					 tempbuff[index+5] == admin_add_supervisor_addButton_addr[index +5])
			 {
				 mem_w25qxxDriver_supervisor_write(admin_supervisor_username, admin_supervisor_password);
			 }



		}


		/*............................................*** admin change time *** .............................................................................................*/
		c = 	NULL ;
		c = strstr((char *)tempbuff, (char *)admin_changetime_buttonBuff ) ;
		if( c != NULL )
		{
			 index = 0 ;
			 index = c - (char *)tempbuff ;
			 if( tempbuff[index+5] == ADMIN_MENU_CHANGE_TIME_BUTTON_ADDR2 )
			 {
				 profile_page_change(ADMIN_MENU_CHANGE_TIME_PAGE_NUMBER);
				 *adminPageDirectionFlag = ADMIN_MENU_CHANGE_TIME_PAGE_NUMBER ;
				 changeTimeIndex++ ;
			 }

		}


		if(*adminPageDirectionFlag == ADMIN_MENU_CHANGE_TIME_PAGE_NUMBER )
		{
			uint8_t adminMenuChangeTime_year_buff[] 	=	{ ADMIN_MENU_CHANGE_TIME_YEAR_ADDR1, ADMIN_MENU_CHANGE_TIME_YEAR_ADDR2, 0x00 } ;
			uint8_t adminMenuChangeTime_mounth_buff[] 	=	{ ADMIN_MENU_CHANGE_TIME_MONTH_ADDR1, ADMIN_MENU_CHANGE_TIME_MONTH_ADDR2, 0x00 } ;
			uint8_t adminMenuChangeTime_date_buff[] 	=	{ ADMIN_MENU_CHANGE_TIME_DATE_ADDR1, ADMIN_MENU_CHANGE_TIME_DATE_ADDR2, 0x00 } ;
			uint8_t adminMenuChangeTime_hour_buff[] 	=	{ ADMIN_MENU_CHANGE_TIME_HOUR_ADDR1, ADMIN_MENU_CHANGE_TIME_HOUR_ADDR2, 0x00 } ;
			uint8_t adminMenuChangeTime_min_buff[]		=	{ ADMIN_MENU_CHANGE_TIME_MINUTE_ADDR1, ADMIN_MENU_CHANGE_TIME_MINUTE_ADDR2, 0x00 } ;
			uint8_t adminMenuChangeTime_sec_buff[] 		=	{ ADMIN_MENU_CHANGE_TIME_SEC_ADDR1, ADMIN_MENU_CHANGE_TIME_SEC_ADDR2, 0x00 } ;
			uint8_t adminMenuChangeTime_applyButton_buff[]	= {  0x5A, 0xA5, 0x06, 0x83, ADMIN_MENU_CHANGE_TIME_APPLYBUTTON_ADDR1,
																ADMIN_MENU_CHANGE_TIME_APPLYBUTTON_ADDR2, 0x01, 0x00, 0x00 } ;

			readdwingetint16(tempbuff, adminMenuChangeTime_year_buff, &ds1307_year, &changeTimeIndex) ;
			readdwingetint8(tempbuff, adminMenuChangeTime_mounth_buff, &ds1307_month, &changeTimeIndex) ;
			readdwingetint8(tempbuff, adminMenuChangeTime_date_buff, &ds1307_date, &changeTimeIndex) ;
			readdwingetint8(tempbuff, adminMenuChangeTime_hour_buff, &ds1307_hour, &changeTimeIndex) ;
			readdwingetint8(tempbuff, adminMenuChangeTime_min_buff, &ds1307_minute, &changeTimeIndex) ;
			readdwingetint8(tempbuff, adminMenuChangeTime_sec_buff, &ds1307_second, &changeTimeIndex) ;


			c = 	NULL ;
			c = strstr((char *)tempbuff, (char *)adminMenuChangeTime_applyButton_buff ) ;
			if( c != NULL )
			{
				 index = 0 ;
				 index = c - (char *)tempbuff ;
				 if( tempbuff[index+5] == ADMIN_MENU_CHANGE_TIME_APPLYBUTTON_ADDR2 )
				 {
						uint8_t da[20] ;
//						sprintf( (char *)da, "\n year %d", changeTimeIndex) ;
//						HAL_UART_Transmit(&huart3, da, strlen( (char *)da), 1000) ;
					 if(changeTimeIndex >= 7)
					 {
						 uint8_t data3[] = "Please wait" ;
						 uint8_t db[30] ={ 0 } ;
						 DWIN_PAGE_CHANGE(0x24) ;
						 DWIN_PAGE_CHANGE(0x24) ;
						 POPUP_NOTIFICATION(db, UNIVERSAL_NOTIFICATION_ADDR2, data3, sizeof(data3)) ;
						 POPUP_NOTIFICATION(db, UNIVERSAL_NOTIFICATION_ADDR2, data3, sizeof(data3)) ;
						 HAL_Delay(2000) ;

						mem_W25qxx_Driver_sectorErase(RTC_ADDR) ;
						HAL_Delay(1000) ;
						mem_W25qxx_Driver_writeByte(RTC_ADDR, ds1307_second) ;
						HAL_Delay(100) ;
						mem_W25qxx_Driver_writeByte(RTC_ADDR+1, ds1307_minute) ;
						HAL_Delay(100) ;
						mem_W25qxx_Driver_writeByte(RTC_ADDR+2, ds1307_hour) ;
						HAL_Delay(100) ;
						mem_W25qxx_Driver_writeByte(RTC_ADDR+3, ds1307_date) ;
						HAL_Delay(100) ;
						mem_W25qxx_Driver_writeByte(RTC_ADDR+4, ds1307_month) ;
						HAL_Delay(100) ;
						mem_W25qxx_Driver_writeByte(RTC_ADDR+5,  (ds1307_year>>8) ) ;
						HAL_Delay(100) ;
						mem_W25qxx_Driver_writeByte(RTC_ADDR+6, (ds1307_year & 0xff) ) ;
						HAL_Delay(100) ;


					 	DS1307_Init(&hi2c1);
					 	DS1307_SetTimeZone(+8, 00);
						DS1307_SetDate(ds1307_date);
						DS1307_SetMonth(ds1307_month);

						DS1307_SetYear(ds1307_year);
						DS1307_SetDayOfWeek(4);
						DS1307_SetHour(ds1307_hour);
						DS1307_SetMinute(ds1307_minute);
						DS1307_SetSecond(ds1307_second);

						 uint8_t data4[] = "Updated     " ;
						 POPUP_NOTIFICATION(db, UNIVERSAL_NOTIFICATION_ADDR2, data4, sizeof(data4)) ;
						 POPUP_NOTIFICATION(db, UNIVERSAL_NOTIFICATION_ADDR2, data4, sizeof(data4)) ;
						 HAL_Delay(2000) ;

						HAL_Delay(1000) ;
						changeTimeIndex = 	0	;

						DWIN_PAGE_CHANGE(ADMIN_MENU_HOME_START_PAGE_NUMBER) ;
						DWIN_PAGE_CHANGE(ADMIN_MENU_HOME_START_PAGE_NUMBER) ;
						 *adminPageDirectionFlag = ADMIN_MAIN_PAGE ;
					 }
				 }

			}


		}


		/*............................................*** admin change password *** .............................................................................................*/

		/*............................................*** admin delete supervisor *** .............................................................................................*/


		c = 	NULL ;
		c = strstr((char *)tempbuff, (char *)admin_changePassword_buttonBuff ) ;
		if( c != NULL )
		{
			 index = 0 ;
			 index = c - (char *)tempbuff ;
			 if( tempbuff[index+5] == ADMIN_MENU_CHANGE_PASSWORD_BUTTON_ADDR2 )
			 {

				 if( current_page == 4)
				 {
					 uint8_t data2[30] ;
					  data2[0] = 0x5A ;
					  data2[1] = 0xA5 ;
					  data2[2] = 0x0C ;
					  data2[3] = 0X82 ;
					  data2[4] = 0x23 ;
					  data2[5] = 0xA4 ;
					  for(uint8_t i=6; i<13;i++)
					  {
						data2[i] = '0' ;
					  }
					  HAL_UART_Transmit(&huart1, data2, 15, HAL_MAX_DELAY);
					  data2[4] = 0x23 ;
					  data2[5] = 0xB1 ;
					  HAL_UART_Transmit(&huart1, data2, 15, HAL_MAX_DELAY);

					 DWIN_PAGE_CHANGE(ADMIN_MENU_HOME_ADMIN_PASSWORD_CHANGE_PAGE_NUMBER);
					 *adminPageDirectionFlag =  ADMIN_MENU_HOME_ADMIN_PASSWORD_CHANGE_PAGE_NUMBER ;

				}
				 else if(current_page == 1)
				 {
					 uint8_t data[] = "not Authorized." ;
					 uint8_t dbg[30] ;
					 memset(dbg, 0, sizeof(dbg)) ;
					 DWIN_PAGE_CHANGE(NOTIFICATION_PAGE_NUMBER) ;

					 POPUP_NOTIFICATION(dbg, UNIVERSAL_NOTIFICATION_ADDR, data, sizeof(data)) ;
//					 POPUP_NOTIFICATION(dbg, UNIVERSAL_NOTIFICATION_ADDR2, data, sizeof(data)) ;
				 }

			 }
		}

		c = 	NULL ;
		c = strstr((char *)tempbuff, (char *)admin_deletesupervsiorUsers_buttonBuff ) ;
		if( c != NULL )
		{
			 index = 0 ;
			 index = c - (char *)tempbuff ;
			 if( tempbuff[index+5] == ADMIN_MENU_DELETESUPERVISOR_BUTTON_ADDR2 )
			 {

				 DWIN_PAGE_CHANGE(ADMIN_MENU_DELETE_SUERVISOR_PAGE_NUMBER);
				 *adminPageDirectionFlag =  ADMIN_MENU_DELETE_SUERVISOR_PAGE_NUMBER ;

				 supervisorDeleteinitVal = 9 ;

			 }
		}

		if(*adminPageDirectionFlag ==  ADMIN_MENU_DELETE_SUERVISOR_PAGE_NUMBER)
		{

			userDeleteRead(tempbuff, &supervisorDelete1stButton, &supervisorDelete2stButton, &supervisorDelete3stButton, &supervisorDeleteIndex);
			endUserDeleteDetailsUpdate( &supervisorDeleteIndex, &supervisorDelete1stButton, &supervisorDelete2stButton, &supervisorDelete3stButton, &supervisorDeleteinitVal, 1) ;
		}

		c = 	NULL ;
		c = strstr((char *)tempbuff, (char *)admin_menu_passwordChange_present_username ) ;
		if( c != NULL )
		{
			 index = 0 ;
			 index = c - (char *)tempbuff ;
			 if( tempbuff[index+1] == ADMIN_MENU_CHANGE_PASSWORD_PRESENT_ADDR2 )
			 {
				 uint8_t size = tempbuff[index+4] ;
				 for(uint8_t i=0; i< 11 ; i++)
				 {
					 if( i <= (size-1) )
						 admin_changePassword_PresentPassword[i] = tempbuff[index+5+i] ;
					 else
						 admin_changePassword_PresentPassword[i] = '\0' ;
				 }
//				 HAL_UART_Transmit(&huart3, admin_changePassword_PresentPassword, strlen(admin_changePassword_PresentPassword), HAL_MAX_DELAY);

			 }

		}

		c = 	NULL ;
		c = strstr((char *)tempbuff, (char *)admin_menu_passwordChange_New_username ) ;
		if( c != NULL )
		{
			 index = 0 ;
			 index = c - (char *)tempbuff ;
			 if( tempbuff[index+1] == ADMIN_MENU_CHANGE_PASSWORD_NEW_ADDR2 )
			 {
				 uint8_t size = tempbuff[index+4] ;
				 for(uint8_t i=0; i< 11 ; i++)
				 {
					 if( i <= (size-1) )
						 admin_changePassword_NewPassword[i] = tempbuff[index+5+i] ;
					 else
						 admin_changePassword_NewPassword[i] = '\0' ;
				 }
			 }

		}

		c = 	NULL ;
		c = strstr((char *)tempbuff, (char *)admin_menu_passwordChange_addButton ) ;
		if( c != NULL )
		{
			 index = 0 ;
			 index = c - (char *)tempbuff ;
			 if( tempbuff[index+5] == ADMIN_MENU_CHANGE_PASSWORD_ADD_BUTTON_ADDR2 )
			 {
//				 HAL_UART_Transmit(&huart3, auth_UserName, strlen(auth_UserName), 1000) ;
//				 HAL_UART_Transmit(&huart3, admin_changePassword_PresentPassword, strlen(admin_changePassword_PresentPassword), 1000) ;
//				 HAL_UART_Transmit(&huart3, admin_changePassword_NewPassword, strlen(admin_changePassword_NewPassword), 1000) ;
				 mem_w25qxxDriver_password_change(auth_UserName, admin_changePassword_PresentPassword, admin_changePassword_NewPassword) ;

			 }


		}

		c = 	NULL ;
		c = strstr((char *)tempbuff, (char *)admin_add_multi_admin_addr ) ;
		if( c != NULL )
		{
			 index = 0 ;
			 index = c - (char *)tempbuff ;
			 if( tempbuff[index+5] == ADMIN_MENU_ADD_MUITLIADMIN_BUTTON_ADDR2 )
			 {
//				 if(current_page == 1)
//				 {
//					 profile_page_change(SUPERVISOR_MENU_ADDUSERS_PAGE_NUMBER);
//					 *adminPageDirectionFlag = SUPERVISOR_MENU_ADDUSERS_PAGE_NUMBER ;
//				 }
//				 else if(current_page == 4)
//				 {
//					 uint8_t data[] = "not Authorized." ;
//					 uint8_t dbg[30] ;
//					 memset(dbg, 0, sizeof(dbg)) ;
//					 DWIN_PAGE_CHANGE(NOTIFICATION_PAGE_NUMBER) ;
//
//					 POPUP_NOTIFICATION(dbg, UNIVERSAL_NOTIFICATION_ADDR, data, sizeof(data)) ;
//
//				 }
				 profile_page_change(SUPERVISOR_MENU_ADDUSERS_PAGE_NUMBER);
				 *adminPageDirectionFlag = SUPERVISOR_MENU_ADDUSERS_PAGE_NUMBER ;

				 uint8_t dt[30] ;
				 dt[0] = 0x5A ;
				 dt[1] = 0xA5 ;
				 dt[2] = 0x05 ;
				 dt[3] = 0x82 ;
				 dt[4] = 0x11 ;
				 dt[5] = 0x77 ;
				 dt[6] = 0x00 ;
				 dt[7] = 0x00 ;

				 HAL_UART_Transmit(&huart1, dt, 8, HAL_MAX_DELAY) ;
				 dt[4] = 0x11 ;
				 dt[5] = 0x6A ;

				 HAL_UART_Transmit(&huart1, dt, 8, HAL_MAX_DELAY) ;
			 }


		}

		c = 	NULL ;
		c = strstr((char *)tempbuff, (char *)admin_delete_multi_admin_addr ) ;
		if( c != NULL )
		{
			 index = 0 ;
			 index = c - (char *)tempbuff ;
			 if( tempbuff[index+5] == ADMIN_MENU_DELETE_MUITLIADMIN_BUTTON_ADDR2 )
			 {
//				 if(current_page == 1)
//				 {
//					 DWIN_PAGE_CHANGE(SUPERVISOR_MENU_ENDUSER_DELETE_PAGE_NUMBER);
//					 *adminPageDirectionFlag =  SUPERVISOR_MENU_ENDUSER_DELETE_PAGE_NUMBER ;
//
//					 supervisorDeleteTrackIndex = 0 ;
//					 supervisorDeleteinitVal = 9 ;
//				 }
//				 else if(current_page == 4)
//				 {
//					 uint8_t data[] = "not Authorized." ;
//					 uint8_t dbg[30] ;
//					 memset(dbg, 0, sizeof(dbg)) ;
//					 DWIN_PAGE_CHANGE(NOTIFICATION_PAGE_NUMBER) ;
//
//					 POPUP_NOTIFICATION(dbg, UNIVERSAL_NOTIFICATION_ADDR, data, sizeof(data)) ;
//
//					 endUserDeleteTrackIndex = 0 ;
//					 endUserDeleteinitVal = 9 ;
//
//				 }
				 DWIN_PAGE_CHANGE(SUPERVISOR_MENU_ENDUSER_DELETE_PAGE_NUMBER);
				 *adminPageDirectionFlag =  SUPERVISOR_MENU_ENDUSER_DELETE_PAGE_NUMBER ;




				 endUserDeleteinitVal = 9 ;
			 }


		}

		c = 	NULL ;
		c = strstr((char *)tempbuff, (char *)admin_factory_settings_addr ) ;
		if( c != NULL )
		{
			 index = 0 ;
			 index = c - (char *)tempbuff ;
			 if( tempbuff[index+5] == ADMIN_MENU_FACTORYSETTINGS_BUTTON_ADDR2 )
			 {
				 if(current_page == 1)
				 {
					 DWIN_PAGE_CHANGE(ADMIN_MENU_FACTORYSETTINGS_PAGE_NUMBER);
					 *adminPageDirectionFlag =  ADMIN_MENU_FACTORYSETTINGS_PAGE_NUMBER ;

					 uint8_t dt[30] ;
					 dt[0] = 0x5A ;
					 dt[1] = 0xA5 ;
					 dt[2] = 0x05 ;
					 dt[3] = 0x82 ;
					 dt[4] = 0x24 ;
					 dt[5] = 0xF0 ;
					 dt[6] = 0x00 ;
					 dt[7] = 0x00 ;

					 HAL_UART_Transmit(&huart1, dt, 8, HAL_MAX_DELAY) ;
					 dt[4] = 0x24 ;
					 dt[5] = 0xFE ;
					 HAL_UART_Transmit(&huart1, dt, 8, HAL_MAX_DELAY) ;

					 dt[4] = 0x25 ;
					 dt[5] = 0x0C ;
					 HAL_UART_Transmit(&huart1, dt, 8, HAL_MAX_DELAY) ;

					 dt[4] = 0x25 ;
					 dt[5] = 0x20 ;
					 HAL_UART_Transmit(&huart1, dt, 8, HAL_MAX_DELAY) ;

					 dt[4] = 0x25 ;
					 dt[5] = 0x34 ;
					 HAL_UART_Transmit(&huart1, dt, 8, HAL_MAX_DELAY) ;

					 dt[4] = 0x25 ;
					 dt[5] = 0x42 ;
					 HAL_UART_Transmit(&huart1, dt, 8, HAL_MAX_DELAY) ;

				 }
				 else if(current_page == 4)
				 {
					 uint8_t data[] = "not Authorized." ;
					 uint8_t dbg[30] ;
					 memset(dbg, 0, sizeof(dbg)) ;
					 DWIN_PAGE_CHANGE(NOTIFICATION_PAGE_NUMBER) ;

					 POPUP_NOTIFICATION(dbg, UNIVERSAL_NOTIFICATION_ADDR, data, sizeof(data)) ;

				 }
			 }


		}

		if( *adminPageDirectionFlag == SUPERVISOR_MENU_ADDUSERS_PAGE_NUMBER )
		{
			c = NULL ;
			c = strstr((char *)tempbuff, (char *)admin_add_multi_adminpassword_addr) ;
			if( c != NULL )
			{
				 index = 0 ;
				 index = c - (char *)tempbuff ;
				 if( tempbuff[index-1] == 0x83 )
				 {
					 uint8_t size = tempbuff[index+4] ;
					 for(uint8_t i=0; i< 11 ; i++)
					 {
						 if( i <= (size-1) )
							 supervisor_password[i] = tempbuff[index+5+i] ;
						 else
							 supervisor_password[i] = '\0' ;
					 }
	//				 HAL_UART_Transmit(&huart3, supervisor_password, strlen(supervisor_password), HAL_MAX_DELAY);

				 }
			}

			c = NULL ;
			c =  strstr((char *)tempbuff, (char *)admin_add_multi_adminUsername_addr) ;
			if(c != NULL)
			{
				 index = 0 ;
				 index = c - (char *)tempbuff ;
				 if( tempbuff[index-1] == 0x83 )
				 {
					 uint8_t size = tempbuff[index+4] ;
					 for(uint8_t i=0; i< 11 ; i++)
					 {
						 if( i <= (size-1) )
						 {
							 supervisor_username[i] = tempbuff[index+5+i] ;
						 }

						 else
						 {
							 supervisor_username[i] = '\0' ;
						 }
					 }
				 }
			}
			c = NULL ;
			c = strstr((char *)tempbuff, (char *)admin_add_multi_adminaddButton_addr ) ;
			if( c != NULL )
			{
				 index = 0 ;
				 index = c - (char *)tempbuff ;
				 if( tempbuff[index+4] == admin_add_multi_adminaddButton_addr[4] &&
						 tempbuff[index+5] == admin_add_multi_adminaddButton_addr[5])
				 {
					 mem_w25qxxDriver_multi_Admin_write(supervisor_username, supervisor_password) ;
				 }

			}
		}


		if(*adminPageDirectionFlag ==  SUPERVISOR_MENU_ENDUSER_DELETE_PAGE_NUMBER)
		{
			endUserDeleteRead(tempbuff, &endUserDelete1stButton, &endUserDelete2stButton, &endUserDelete3stButton, &endUserDeleteIndex);
			endUserDeleteDetailsUpdate( &endUserDeleteIndex, &endUserDelete1stButton, &endUserDelete2stButton, &endUserDelete3stButton, &endUserDeleteinitVal, 3) ;
		}

		//FACTORY SETTINGS
		if(  *adminPageDirectionFlag ==  ADMIN_MENU_FACTORYSETTINGS_PAGE_NUMBER  )
		{


			uint8_t admin_factorySetting_SRNO_addr[] 			=	{ADMIN_FACTORYSETTINGS_SRNO_ADDR1, ADMIN_FACTORYSETTINGS_SRNO_ADDR2, 0X00 } ;
			uint8_t admin_factorySetting_ModelNo_addr[] 		=	{ADMIN_FACTORYSETTINGS_MODELNO_ADDR1, ADMIN_FACTORYSETTINGS_MODELNO_ADDR2, 0X00 } ;
			uint8_t admin_factorySetting_CompanyName_addr[] 	=	{ADMIN_FACTORYSETTINGS_COMAPANYNAME_ADDR1, ADMIN_FACTORYSETTINGS_COMAPANYNAME_ADDR2, 0X00 } ;
			uint8_t admin_factorySetting_CompanyLoc_addr[] 		=	{ADMIN_FACTORYSETTINGS_COMAPANYLOC_ADDR1, ADMIN_FACTORYSETTINGS_COMAPANYLOC_ADDR2, 0X00 } ;
			uint8_t admin_factorySetting_InstalledDate_addr[] 	=	{ADMIN_FACTORYSETTINGS_INSTALLEDDATE_ADDR1, ADMIN_FACTORYSETTINGS_INSTALLEDDATE_ADDR2, 0X00 } ;
			uint8_t admin_factorySetting_FixedmmHG_addr[] 		=	{ADMIN_FACTORYSETTINGS_MMHGLIMIT_ADDR1, ADMIN_FACTORYSETTINGS_MMHGLIMIT_ADDR2, 0X00 } ;
			uint8_t admin_factorySetting_ApplyButton_addr[] 	=	{0x5A, 0xA5, 0x06, 0x83, ADMIN_FACTORYSETTINGS_APPLYBUTTON_ADDR1,
																	ADMIN_FACTORYSETTINGS_APPLYBUTTON_ADDR2, 0x01, 0x00, 0x00 } ;

			c = NULL ;
			c = strstr((char *)tempbuff, (char *)admin_factorySetting_ApplyButton_addr ) ;
			if( c != NULL )
			{
				 index = 0 ;
				 index = c - (char *)tempbuff ;
				 if( tempbuff[index+4] == admin_factorySetting_ApplyButton_addr[4] &&
						 tempbuff[index+5] == admin_factorySetting_ApplyButton_addr[5])
				 {
					 if(factoryTrueFlag >= 6)
					 {

						 mem_w25qxxDriver_write_device_details(device_model, device_version, deviceUserCompanyName, deviceUserCompanyLocation,
								 deviceinstalledDate, deviceFixedmmHG) ;
//						 HAL_UART_Transmit(&huart3, device_version, strlen(device_version), HAL_MAX_DELAY);
//						 HAL_UART_Transmit(&huart3, device_model, strlen(device_model), HAL_MAX_DELAY);
//						 HAL_UART_Transmit(&huart3, deviceUserCompanyName, strlen(deviceUserCompanyName), HAL_MAX_DELAY);
//						 HAL_UART_Transmit(&huart3, deviceUserCompanyLocation, strlen(deviceUserCompanyLocation), HAL_MAX_DELAY);
//						 HAL_UART_Transmit(&huart3, deviceinstalledDate, strlen(deviceinstalledDate), HAL_MAX_DELAY);
//						 HAL_UART_Transmit(&huart3, deviceFixedmmHG, strlen(deviceFixedmmHG), HAL_MAX_DELAY);
					 }


				 }

			}

			c = NULL ;
			c = strstr((char *)tempbuff, (char *)admin_factorySetting_FixedmmHG_addr) ;
			if( c != NULL )
			{
				 index = 0 ;
				 index = c - (char *)tempbuff ;
				 if( (tempbuff[index-1] == 0x83) && (tempbuff[index+1] == ADMIN_FACTORYSETTINGS_MMHGLIMIT_ADDR2)  )
				 {
					 uint8_t size = tempbuff[index+4] ;
					 for(uint8_t i=0; i< 4 ; i++)
					 {
						 if( i <= (size-1) )
							 deviceFixedmmHG[i] = tempbuff[index+5+i] ;
						 else
							 deviceFixedmmHG[i] = '\0' ;
					 }
					 factoryTrueFlag++ ;
//					 HAL_UART_Transmit(&huart3, deviceFixedmmHG, strlen(deviceFixedmmHG), HAL_MAX_DELAY);

				 }
			}

			c = NULL ;
			c = strstr((char *)tempbuff, (char *)admin_factorySetting_InstalledDate_addr) ;
			if( c != NULL )
			{
				 index = 0 ;
				 index = c - (char *)tempbuff ;
				 if( (tempbuff[index-1] == 0x83) && (tempbuff[index+1] == ADMIN_FACTORYSETTINGS_INSTALLEDDATE_ADDR2)  )
				 {
					 uint8_t size = tempbuff[index+4] ;
					 for(uint8_t i=0; i< 10 ; i++)
					 {
						 if( i <= (size-1) )
							 deviceinstalledDate[i] = tempbuff[index+5+i] ;
						 else
							 deviceinstalledDate[i] = '\0' ;
					 }
					 factoryTrueFlag++ ;
//					 HAL_UART_Transmit(&huart3, deviceinstalledDate, strlen(deviceinstalledDate), HAL_MAX_DELAY);

				 }
			}

			c = NULL ;
			c = strstr((char *)tempbuff, (char *)admin_factorySetting_CompanyLoc_addr) ;
			if( c != NULL )
			{
				 index = 0 ;
				 index = c - (char *)tempbuff ;
				 if( (tempbuff[index-1] == 0x83) && (tempbuff[index+1] == ADMIN_FACTORYSETTINGS_COMAPANYLOC_ADDR2)  )
				 {
					 uint8_t size = tempbuff[index+4] ;
					 for(uint8_t i=0; i< 16 ; i++)
					 {
						 if( i <= (size-1) )
							 deviceUserCompanyLocation[i] = tempbuff[index+5+i] ;
						 else
							 deviceUserCompanyLocation[i] = '\0' ;
					 }
					 factoryTrueFlag++ ;
//					 HAL_UART_Transmit(&huart3, deviceUserCompanyLocation, strlen(deviceUserCompanyLocation), HAL_MAX_DELAY);

				 }
			}

			c = NULL ;
			c = strstr((char *)tempbuff, (char *)admin_factorySetting_ModelNo_addr) ;
			if( c != NULL )
			{
				 index = 0 ;
				 index = c - (char *)tempbuff ;
				 if( (tempbuff[index-1] == 0x83) && (tempbuff[index+1] == ADMIN_FACTORYSETTINGS_MODELNO_ADDR2)  )
				 {
					 uint8_t size = tempbuff[index+4] ;
					 for(uint8_t i=0; i< 10 ; i++)
					 {
						 if( i <= (size-1) )
							 device_model[i] = tempbuff[index+5+i] ;
						 else
							 device_model[i] = '\0' ;
					 }
					 factoryTrueFlag++ ;
//					 HAL_UART_Transmit(&huart3, device_model, strlen(device_model), HAL_MAX_DELAY);


				 }
			}

			c = NULL ;
			c = strstr((char *)tempbuff, (char *)admin_factorySetting_CompanyName_addr) ;
			if( c != NULL )
			{
				 index = 0 ;
				 index = c - (char *)tempbuff ;
				 if( (tempbuff[index-1] == 0x83) && (tempbuff[index+1] == ADMIN_FACTORYSETTINGS_COMAPANYNAME_ADDR2)  )
				 {
					 uint8_t size = tempbuff[index+4] ;
					 for(uint8_t i=0; i< 16 ; i++)
					 {
						 if( i <= (size-1) )
							 deviceUserCompanyName[i] = tempbuff[index+5+i] ;
						 else
							 deviceUserCompanyName[i] = '\0' ;
					 }
					 factoryTrueFlag++ ;
//					 HAL_UART_Transmit(&huart3, deviceUserCompanyName, strlen(deviceUserCompanyName), HAL_MAX_DELAY);

				 }
			}

			c = NULL ;
			c = strstr((char *)tempbuff, (char *)admin_factorySetting_SRNO_addr) ;
			if( c != NULL )
			{
				 index = 0 ;
				 index = c - (char *)tempbuff ;
				 if( (tempbuff[index-1] == 0x83) && (tempbuff[index+1] == ADMIN_FACTORYSETTINGS_SRNO_ADDR2)  )
				 {
					 uint8_t size = tempbuff[index+4] ;
					 for(uint8_t i=0; i< 10 ; i++)
					 {
						 if( i <= (size-1) )
							 device_version[i] = tempbuff[index+5+i] ;
						 else
							 device_version[i] = '\0' ;
					 }
					 factoryTrueFlag++ ;
//					 HAL_UART_Transmit(&huart3, device_version, strlen(device_version), HAL_MAX_DELAY);

				 }
			}

		}



 *displayUartFlag = 0 ;

	}
}

/* ########################################################################################################################################################################*/
/************************************SUPERVISOR PAGE DETAILS BELOW************************************/
/* ########################################################################################################################################################################*/

void endUserDeleteRead(uint8_t *tempbuff, uint8_t *delete1, uint8_t *delete2, uint8_t *delete3, uint8_t *index)
{


	char *c 	= 		NULL ;
	uint8_t idx = 0 ;

	uint8_t SupervisorMenuEnduserDlete_1_ButtonBuff[] 	= { 0x5A, 0xA5, 0x06, 0x83, 0x12, 0x7A, 0x01, 0x00, 0x01, 0x00} ;
	uint8_t SupervisorMenuEnduserDlete_2_ButtonBuff[] 	= { 0x5A, 0xA5, 0x06, 0x83, 0x12, 0x90, 0x01, 0x00, 0x01, 0x00} ;
	uint8_t SupervisorMenuEnduserDlete_3_ButtonBuff[] 	= { 0x5A, 0xA5, 0x06, 0x83, 0x12, 0xA6, 0x01, 0x00, 0x01, 0x00} ;
	uint8_t SupervisorMenuEnduserDleteBackButtonBuff[]	= { 0x5A, 0xA5, 0x06, 0x83, 0x12, 0xAE, 0x01, 0x00, 0x01, 0x00} ;
	uint8_t SupervisorMenuEnduserDleteNextButtonBuff[] 	= { 0x5A, 0xA5, 0x06, 0x83, 0x12, 0xAA, 0x01, 0x00, 0x01, 0x00} ;

	c = strstr((char *)tempbuff, (char *)SupervisorMenuEnduserDlete_1_ButtonBuff);
	if( c!= NULL)
	{
		idx = 0 ;
		idx = c - (char *)tempbuff ;
		 if( tempbuff[idx+3] == 0x83 )
		 {
				*delete1 = 1 ;
				uint8_t x[20];
				sprintf( (char *)x,"ist del\n");
//				HAL_UART_Transmit(&huart3, x,strlen(x), HAL_MAX_DELAY);
		 }

	}
	c = strstr((char *)tempbuff, (char *)SupervisorMenuEnduserDlete_2_ButtonBuff);
	if( c!= NULL)
	{
		idx = 0 ;
		idx = c - (char *)tempbuff ;
		 if( tempbuff[idx+3] == 0x83 )
		 {
				*delete2 = 1 ;
				uint8_t x[20];
				sprintf((char *)x,"ist delete 2\n");
//				HAL_UART_Transmit(&huart3, x,strlen(x), HAL_MAX_DELAY);
		 }

	}
	c = strstr((char *)tempbuff, (char *)SupervisorMenuEnduserDlete_3_ButtonBuff);
	if( c!= NULL)
	{
		idx = 0 ;
		idx = c - (char *)tempbuff ;
		 if( tempbuff[idx+3] == 0x83 )
		 {
				*delete3 = 1 ;
				uint8_t x[20];
				sprintf((char *)x,"ist delete 3\n");
//				HAL_UART_Transmit(&huart3, x,strlen(x), HAL_MAX_DELAY);
		 }

	}
	c = strstr((char *)tempbuff, (char *)SupervisorMenuEnduserDleteNextButtonBuff);
	if( c!= NULL)
	{
		idx = 0 ;
		idx = c - (char *)tempbuff ;
		 if( tempbuff[idx+3] == 0x83 )
		 {
				*index = 2 ;
				uint8_t x[20];
				sprintf((char *)x,"ist next %d\n", *index);
//				HAL_UART_Transmit(&huart3, x,strlen(x), HAL_MAX_DELAY);
		 }

	}
	c = strstr((char *)tempbuff, (char *)SupervisorMenuEnduserDleteBackButtonBuff);
	if( c!= NULL)
	{
		idx = 0 ;
		idx = c - (char *)tempbuff ;
		 if( tempbuff[idx+3] == 0x83 )
		 {
				*index = 3;
				uint8_t x[20];
				sprintf( (char *)x,"ist back %d\n",*index);
//				HAL_UART_Transmit(&huart3, x,strlen(x), HAL_MAX_DELAY);
		 }

	}
}

void arrangedata1(uint8_t *arr, uint8_t val, uint16_t add1, uint16_t add2, uint8_t user)
{
	uint8_t username[12] ;
	memset(username, 0, sizeof(username)) ;
	memset(arr, 0, 22) ;

	itoa(val, (char *)arr, 10) ;
	ALIGN_SEND_STRING_TO_ADDR_FORMAT_FOR_DWIN( arr, add1) ;
	ALIGN_SEND_STRING_TO_ADDR_FORMAT_FOR_DWIN( arr, add1) ;

	if(user == 2)
		mem_w25qxxDriver_read_endUser( val, arr) ;
	else if(user == 1)
		mem_w25qxxDriver_read_supervisor( val, arr) ;
	else if(user == 3)
		mem_w25qxxDriver_read_multiadmin( val, arr) ;

	memcpy(username, &arr[1], 10) ;
	ALIGN_SEND_STRING_TO_ADDR_FORMAT_FOR_DWIN(username, add2) ;
	ALIGN_SEND_STRING_TO_ADDR_FORMAT_FOR_DWIN(username, add2) ;

}

void arrangedeletedata(uint8_t *arr, uint8_t val, uint8_t *delete, uint8_t user)
{
	uint8_t dbg[40] ;
	memset(dbg, 0, sizeof(dbg)) ;
	uint8_t data1[] = "Please wait." ;

	DWIN_PAGE_CHANGE1(dbg, 0X24) ;
	DWIN_PAGE_CHANGE1(dbg, 0X24) ;

	memset(dbg, 0, sizeof(dbg)) ;
	POPUP_NOTIFICATION(dbg, UNIVERSAL_NOTIFICATION_ADDR2, data1, sizeof(data1)) ;
	POPUP_NOTIFICATION(dbg, UNIVERSAL_NOTIFICATION_ADDR2, data1, sizeof(data1)) ;

	 uint8_t username[12] ;

	uint8_t x[30];
	sprintf((char *)x,"ist delete %d macth\n", *delete);
//	HAL_UART_Transmit(&huart3, x,strlen(x), HAL_MAX_DELAY);
	memset(username, 0, sizeof(username)) ;
	memset(arr, 0, 22) ;
	if(user == 2)
	{
		mem_w25qxxDriver_read_endUser(val , arr) ;
		memcpy(username, &arr[1], 10) ;
		deleteAnyUser(USER_ADDR, username, val) ;

		*delete = 0;
		uint16_t idx = mem_w25qxxDriver_getEndUser_idx();
		idx-- ;
		mem_w25qxxDriver_setEndUser_idx(idx) ;

	}
	else if( user == 1)
	{
		mem_w25qxxDriver_read_supervisor(val , arr) ;
		memcpy(username, &arr[1], 10) ;
		deleteAnyUser(SUPERUSER_ADDR, username, val) ;

		*delete = 0;
		uint16_t idx = mem_w25qxxDriver_getSupervisor_idx();
		idx-- ;
		mem_w25qxxDriver_setSupervisor_idx(idx) ;
	}
	else if(user == 3)
	{
		mem_w25qxxDriver_read_multiadmin(val , arr) ;
		memcpy(username, &arr[1], 10) ;
		deleteAnyUser(MULTIPLE_ADMIN_ADDR, username, val) ;

		*delete = 0;
		uint16_t idx = mem_w25qxxDriver_getMutiliAdmin_idx();
		idx-- ;
		mem_w25qxxDriver_MutliAdmin_idx(idx) ;

	}

	uint8_t debugBuff[30] ;
	memset(dbg, 0, sizeof(dbg)) ;
	sprintf((char *)&debugBuff, "#deleted %s", username) ;
	DWIN_PAGE_CHANGE1(dbg, NOTIFICATION_PAGE_NUMBER) ;
	DWIN_PAGE_CHANGE1(dbg, NOTIFICATION_PAGE_NUMBER) ;

	memset(dbg, 0, sizeof(dbg)) ;
	POPUP_NOTIFICATION(dbg, UNIVERSAL_NOTIFICATION_ADDR, debugBuff, strlen( (char *) debugBuff) ) ;
	POPUP_NOTIFICATION(dbg, UNIVERSAL_NOTIFICATION_ADDR, debugBuff, strlen( (char *) debugBuff) ) ;

}

void endUserDeleteDetailsUpdate(uint8_t *index, uint8_t *delete1, uint8_t *delete2, uint8_t *delete3 , uint8_t *validx, uint8_t user)
{
	uint8_t arr[22] ;
	uint8_t username[12] ;
	memset(username, 0, sizeof(username)) ;

	uint16_t addr1 =0  ;
	uint16_t addr2 =0  ;
	uint16_t addr3 =0  ;
	uint16_t addr4 =0  ;
	uint16_t addr5 =0  ;
	uint16_t addr6 =0  ;

	if((user == 2) || (user == 3))
	{
		 addr1 = SUPERVISOR_MENU_DELETEENDUSER_SRNO1_ADDR ;
		 addr2 = SUPERVISOR_MENU_DELETEENDUSER_USERNAME1_ADDR ;
		 addr3 = SUPERVISOR_MENU_DELETEENDUSER_SRNO2_ADDR ;
		 addr4 = SUPERVISOR_MENU_DELETEENDUSER_USERNAME2_ADDR ;
		 addr5 = SUPERVISOR_MENU_DELETEENDUSER_SRNO3_ADDR ;
		 addr6 = SUPERVISOR_MENU_DELETEENDUSER_USERNAME3_ADDR ;
	}
	else if (user == 1)
	{
		 addr1 = ADMIN_MENU_SUPERVISOR_DELETE_SRNO1_ADDR ;
		 addr2 = ADMIN_MENU_SUPERVISOR_DELETE_USERNAME1_ADDR ;
		 addr3 = ADMIN_MENU_SUPERVISOR_DELETE_SRNO2_ADDR ;
		 addr4 = ADMIN_MENU_SUPERVISOR_DELETE_USERNAME2_ADDR ;
		 addr5 = ADMIN_MENU_SUPERVISOR_DELETE_SRNO3_ADDR ;
		 addr6 = ADMIN_MENU_SUPERVISOR_DELETE_USERNAME3_ADDR ;
	}



	 uint8_t markval = 3;

	if(*validx == 9 )
	{

		arrangedata1(arr, 0, addr1, addr2, user) ;

		arrangedata1(arr, 1, addr3, addr4, user) ;

		arrangedata1(arr, 2, addr5, addr6, user) ;

		*validx = 0 ;
		endUserDeleteTrackIndex = 0;


	}
	if(*index == 2)//next
	{
		endUserDeleteTrackIndex++ ;

		uint8_t val = ( markval*endUserDeleteTrackIndex ) ;

		arrangedata1(arr, val+0, addr1, addr2, user) ;

		arrangedata1(arr, val+1, addr3, addr4, user) ;

		arrangedata1(arr, val+2, addr5, addr6, user) ;

		*index = 0 ;


	}
	else if(*index == 3)//back
	{
		endUserDeleteTrackIndex-- ;

		uint8_t val = ( markval*endUserDeleteTrackIndex ) ;

		arrangedata1(arr, val+0, addr1, addr2, user) ;

		arrangedata1(arr, val+1, addr3, addr4, user) ;

		arrangedata1(arr, val+2, addr5, addr6, user) ;

		*index = 0 ;


	}


	if( *delete1 == 1)
	{

		uint8_t val = ( markval*endUserDeleteTrackIndex ) ;

		arrangedeletedata(arr, val, delete1, user) ;
	}

	else if( *delete2 == 1)
	{
		uint8_t val = ( markval*endUserDeleteTrackIndex )+1 ;
		arrangedeletedata(arr, val, delete2, user) ;
	}

	else if( *delete3 == 1)
	{
		uint8_t val = ( markval*endUserDeleteTrackIndex )+2 ;
		arrangedeletedata(arr, val, delete3, user) ;
	}

}

void validation_print_callback(uint8_t *auth_UserName)
{
	 uint8_t data3[] = "Reports Printing.." ;
	 uint8_t dbg1[30] ;
	 memset(dbg1, 0, sizeof(dbg1)) ;
	 DWIN_PAGE_CHANGE(0x24) ;
	 POPUP_NOTIFICATION(dbg1, UNIVERSAL_NOTIFICATION_ADDR2, data3, sizeof(data3)) ;
	 POPUP_NOTIFICATION(dbg1, UNIVERSAL_NOTIFICATION_ADDR2, data3, sizeof(data3)) ;
	 HAL_Delay(1000) ;

	uint8_t arr1[26] ;
	uint8_t arr2[26] ;
	uint8_t arr3[26] ;
	uint8_t arr4[26] ;
	uint8_t op[26] ;

	mem_w25qxxDriver_read_device_details(device_model, device_version, deviceUserCompanyName, deviceUserCompanyLocation );
	strcat((char *)&device_model,"\n") ;
	strcat((char *)&device_version,"\n");
	memset(op, 0, sizeof(op)) ;

	validation_title( device_model, device_version) ;

	strcat((char *)&deviceUserCompanyName,"\n");
	strcat((char *)&deviceUserCompanyLocation,"\n");
	memset(arr1, 0, sizeof(arr1));

		memcpy(arr1, auth_UserName, strlen((const char *)auth_UserName));
		strcat((char *)arr1, "\n");

	memset(arr2, 0, sizeof(arr2));
	sprintf((char *)&arr2, "%02d:%02d:%02d sec \n",
			 rp203ThermalPrinterDriver_startHour,
			 rp203ThermalPrinterDriver_startMin,
			 rp203ThermalPrinterDriver_startSec) ;

	memset(arr3, 0, sizeof(arr3));
	sprintf((char *)&arr3, "%02d:%02d:%02d sec \n",
			rp203ThermalPrinterDriver_endHour,
			rp203ThermalPrinterDriver_endMin,
			rp203ThermalPrinterDriver_endSec) ;

	strcat((char *)rp203ThermalPrinterDriver_TestDate, "\n") ;

	Validation_report_start( deviceUserCompanyName, deviceUserCompanyLocation, rp203ThermalPrinterDriver_TestDate, arr2, arr3 ) ;

	for( uint8_t i=1 ;i<4; i++)
	{

		mem_W25qxx_Driver_ReadBytes(VALID_AND_CALIB_RESULTS +(PAGE_SIZE * i), arr4, 15) ;
		HAL_Delay(500) ;

		uint8_t input_mmhg[4] ;
		memcpy(&input_mmhg, &arr4, 3) ;

		uint8_t input_min[3] ;
		memcpy(input_min, &arr4[3], 2) ;

		uint8_t input_sec[3] ;
		memcpy(input_sec, &arr4[5], 2) ;

		uint8_t current_mmhg[4] ;
		memcpy(current_mmhg, &arr4[7], 3) ;

		uint8_t gauge_value[4] ;
		memcpy(gauge_value, &arr4[10], 3) ;

		uint8_t passOrFails[3] ;
		memcpy(passOrFails, &arr4[13], 2) ;

		uint8_t srno[4] ;
		sprintf((char *)&srno, "%02d", i) ;
		Validation_report_results(srno, input_mmhg, input_min, input_sec, current_mmhg, passOrFails, gauge_value) ;

	}

	Validation_report_end(auth_UserName) ;

	getTime() ;
	memset(arr1, 0, sizeof(arr1));
	memset(arr2, 0, sizeof(arr2));
	sprintf((char *)arr1, "%02d:%02d:%02d \n",ds1307_hour, ds1307_minute, ds1307_second) ;
	sprintf((char *)arr2, "%02d:%02d:%04d ",ds1307_date, ds1307_month, ds1307_year) ;

	uint8_t printed_on[] = "\nPrinted On :";
	HAL_UART_Transmit(&huart3,printed_on, strlen((char *)&printed_on), HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart3,arr2, strlen((char *)&arr2), HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart3,arr1, strlen((char *)&arr1), HAL_MAX_DELAY);
	uint8_t end[] = "\n* * * * * * * * * * * * * * * * \n";
	HAL_UART_Transmit(&huart3,end, strlen((char *)&end), HAL_MAX_DELAY);

}

static void clearDisplayFields()
{
	 uint8_t dt[30] ;
	 dt[0] = 0x5A ;
	 dt[1] = 0xA5 ;
	 dt[2] = 0x05 ;
	 dt[3] = 0x82 ;
	 dt[4] = ( uint8_t )(HOME_PAGE_PROFILENAME_ADDR>>8) ;
	 dt[5] = ( uint8_t )(HOME_PAGE_PROFILENAME_ADDR>>0) ;
	 dt[6] = 0x00 ;
	 dt[7] = 0x00 ;
	 HAL_UART_Transmit(&huart1, dt, 8, HAL_MAX_DELAY) ;

	 dt[4] = ( uint8_t )(HOME_PAGE_SET_VACCUM_ADDR>>8) ;
	 dt[5] = ( uint8_t ) (HOME_PAGE_SET_VACCUM_ADDR>>0) ;
	 HAL_UART_Transmit(&huart1, dt, 8, HAL_MAX_DELAY) ;

	 dt[4] = ( uint8_t )(HOME_PAGE_SET_TIME_SEC_ADDR>>8) ;
	 dt[5] = ( uint8_t ) (HOME_PAGE_SET_TIME_SEC_ADDR>>0) ;
	 HAL_UART_Transmit(&huart1, dt, 8, HAL_MAX_DELAY) ;

	 dt[4] = ( uint8_t )(HOME_PAGE_SET_TIME_MIN_ADDR>>8) ;
	 dt[5] = ( uint8_t )(HOME_PAGE_SET_TIME_MIN_ADDR>>0) ;
	 HAL_UART_Transmit(&huart1, dt, 8, HAL_MAX_DELAY) ;

	 dt[4] = HOMEPAGE_CURRENT_VACCUM_ADDR1 ;
	 dt[5] = HOMEPAGE_CURRENT_VACCUM_ADDR2 ;
	 HAL_UART_Transmit(&huart1, dt, 8, HAL_MAX_DELAY) ;

	 dt[4] = HOMEPAGE_CURRENT_TIME_MIN_ADDR1 ;
	 dt[5] = HOMEPAGE_CURRENT_TIME_MIN_ADDR2 ;
	 HAL_UART_Transmit(&huart1, dt, 8, HAL_MAX_DELAY) ;

	 dt[4] = HOMEPAGE_CURRENT_TIME_SEC_ADDR1 ;
	 dt[5] = HOMEPAGE_CURRENT_TIME_SEC_ADDR2 ;
	 HAL_UART_Transmit(&huart1, dt, 8, HAL_MAX_DELAY) ;
}

static void setCalibration( uint8_t *tempbuff, uint8_t *supervisorPageDirectionFlag)
{
	char *c = NULL ;
	uint8_t index = 0;

	uint8_t supervisorPage_menu_calibrate_buff[]					= { 0x5A, 0xA5, 0x06, 0x83, SUPERVISOR_MENU_CALIBRAE_BUTTON_ADDR1, SUPERVISOR_MENU_CALIBRAE_BUTTON_ADDR2, 0x01, 0x00, 0x00 } ;
	uint8_t calibration_gaugeinput_addr[] 							= { CALIBRATIONGAUGE_INPUTVAL_ADDR1, CALIBRATIONGAUGE_INPUTVAL_ADDR2 , 0x00} ;
	uint8_t calibration_gaugeinput_Apply_addr[]						= { 0x5A, 0xA5, 0x06, 0x83, CALIBRATIONGAUGE_INPUTVAL_APPLYBUTTON_ADDR1,
																		CALIBRATIONGAUGE_INPUTVAL_APPLYBUTTON_ADDR2, 0x01, 0x00, 0x00 } ;

	c= NULL ;
	c = strstr((char *)tempbuff, (char *)supervisorPage_menu_calibrate_buff) ;
	if( c != NULL)
	{
		 index = 0 ;
		 index = c - (char *)tempbuff ;
		 if( tempbuff[index+5] == SUPERVISOR_MENU_CALIBRAE_BUTTON_ADDR2 )
		 {
			profile_page_change(SUPERVISOR_MENU_CALIPOPUP_ADDR);
			*supervisorPageDirectionFlag = SUPERVISOR_MENU_CALIPOPUP_ADDR ;
		 }
	}

	if( *supervisorPageDirectionFlag == SUPERVISOR_MENU_VALID_CALID_ADDR )
	{


			c = NULL ;
			c = strstr((char *)tempbuff, (char *)calibration_gaugeinput_addr ) ;
			if( c != NULL )
			{
				index = 0 ;
				index = c - (char *)tempbuff ;
				if( tempbuff[index-1] == 0x83 )
				{
					 if(tempbuff[index+1] == calibration_gaugeinput_addr[1])
					 {
						 uint8_t size = tempbuff[index+4] ;

						 validcalib_gaugeVal = 0;
						 for(uint8_t i=0 ;i< size;i++)
						 {
							 validcalib_gaugeVal = validcalib_gaugeVal * 10 ;
							 validcalib_gaugeVal = validcalib_gaugeVal + ( tempbuff[index+5+i] - '0' ) ;
						 }
					 }
				}
			}

			c= NULL ;
			c = strstr((char *)tempbuff, (char *)calibration_gaugeinput_Apply_addr) ;
			if( c != NULL)
			{
				 index = 0 ;
				 index = c - (char *)tempbuff ;
				 if( tempbuff[index+5] == CALIBRATIONGAUGE_INPUTVAL_APPLYBUTTON_ADDR2 )
				 {
					 uint8_t data2[] = "Please Wait" ;
					 uint8_t dbg[30] ;
					 memset(dbg, 0, sizeof(dbg)) ;
					 DWIN_PAGE_CHANGE(0x24) ;
					 POPUP_NOTIFICATION(dbg, UNIVERSAL_NOTIFICATION_ADDR2, data2, sizeof(data2)) ;
					 POPUP_NOTIFICATION(dbg, UNIVERSAL_NOTIFICATION_ADDR2, data2, sizeof(data2)) ;

					 uint8_t temp[24] ;
					 uint8_t dummy[6] ;
					 memset(dummy, 0, sizeof(dummy)) ;
					 sprintf((char *)&dummy, "%03d", homePage_setmmhg) ;
					 memcpy(&temp, dummy, 3) ;		//input mmhg

					 memset(dummy, 0, sizeof(dummy)) ;
					 sprintf((char *)&dummy, "%02d", homePage_setMin) ;
					 memcpy(&temp[3], dummy, 2) ;		//input min

					 memset(dummy, 0, sizeof(dummy)) ;
					 sprintf((char *)&dummy, "%02d", homePage_setSec) ;
					 memcpy(&temp[5], dummy,  2) ;	//input sec

					 uint8_t *gaugeRef[12] ;
					 memset(gaugeRef, 0, sizeof(gaugeRef)) ;
					 mem_w25qxxDriver_Read_mmhg_GAUGEval( (uint8_t *)gaugeRef, sizeof(gaugeRef)) ;
					 int dummymmhg = atoi((const char *)gaugeRef) ;

					 memset(dummy, 0, sizeof(dummy)) ;
					 sprintf((char *)&dummy, "%03d", (uint16_t)(_c_mmhg - dummymmhg) ) ;//current mmhg
					 memcpy(&temp[7], dummy, 3) ;

					 memset(dummy, 0, sizeof(dummy)) ;
					 sprintf((char *)&dummy, "%03d", validcalib_gaugeVal) ;
					 memcpy(&temp[10], dummy, 3) ;  // gauge_value

					 memset(dummy, 0, sizeof(dummy)) ;
					 sprintf((char *)&dummy, "00") ;
					 memcpy(&temp[13], dummy, 2) ; //pass or fail

//					HAL_UART_Transmit( &huart3, temp, 13, HAL_MAX_DELAY) ;
					mem_W25qxx_Driver_writeBytes(VALID_AND_CALIB_RESULTS +(PAGE_SIZE * validationTestVar), temp, 15) ;//input MmHg
					HAL_Delay(1000) ;

					uint8_t *gaugeRef1[12] ;
					memset(gaugeRef1, 0, sizeof(gaugeRef1)) ;
					mem_w25qxxDriver_Read_mmhg_GAUGEval( (uint8_t*)gaugeRef1, sizeof(gaugeRef1)) ;

					 int diff = 0 ;

					 diff = ( atoi((const char *)gaugeRef1) )+ ( homePage_setmmhg - ( validcalib_gaugeVal ) ) ;
					 uint8_t data[13] ;
					 sprintf( (char *)data, "%d", diff) ;
//							 HAL_UART_Transmit(&huart3, data, strlen((char *)data), HAL_MAX_DELAY);
					 mem_w25qxxDriver_Write_mmhg_GAUGEval(data, strlen( (char *)data)) ;

					 uint8_t data3[] = "Updated" ;
					 uint8_t db[30] ={ 0 } ;
					 DWIN_PAGE_CHANGE(0x24) ;
					 POPUP_NOTIFICATION(db, UNIVERSAL_NOTIFICATION_ADDR2, data3, sizeof(data3)) ;
					 POPUP_NOTIFICATION(db, UNIVERSAL_NOTIFICATION_ADDR2, data3, sizeof(data3)) ;
					 HAL_Delay(2000) ;

					validationTestVar++  ;
					validcalib_gaugeVal = 0;
					homePage_setmmhg = 0 ;
					homePage_setMin = 0 ;
					homePage_setSec = 0 ;

					if(validationTestVar >= 4)
					{
//						validation_print_callback(auth_UserName) ;

						 if( current_page == USER_PRFILE )
						 {
							 *supervisorPageDirectionFlag = SUPERVISOR_MAIN_PAGE ;
							 DWIN_PAGE_CHANGE(ENDUSER_START_PAGE) ;


						 }
						 else if(current_page == ADMIN_PROFILE)
						 {
							 *supervisorPageDirectionFlag = ADMIN_MAIN_PAGE ;//admin and supervisor homepages are same
							 DWIN_PAGE_CHANGE(ADMIN_MENU_HOME_START_PAGE_NUMBER) ;
						 }
						 else if(current_page == SUPERVISOR_PROFILE)
						 {
							 *supervisorPageDirectionFlag = SUPERVISOR_MAIN_PAGE ;//admin and supervisor homepages are same
							 DWIN_PAGE_CHANGE(SUPERVISOR_HOME_START_PAGE_NUMBER) ;


						 }
					}
					else
					{
						profile_page_change(SUPERVISOR_MENU_VALIDATION_ADDR);
						*supervisorPageDirectionFlag = SUPERVISOR_MENU_VALIDATION_ADDR ;
					}
					clearDisplayFields() ;

				 }
			}
	}
}

static void resetCalibration( uint8_t *tempbuff, uint8_t *supervisorPageDirectionFlag)
{
	char *c = NULL ;
	uint8_t index = 0;
	c= NULL ;

	uint8_t calibration_reset_button[]								= { 0x5A, 0xA5, 0x06, 0x83, 0x25, 0x94, 0x01, 0x00, 0x00 } ;
	uint8_t afterTest_print_yes_button[]                			= { 0x5A, 0xA5, 0x06, 0x83, 0x25, 0x77, 0x01, 0x00, 0x00} ;
	uint8_t afterTest_print_no_button[]                 			= { 0x5A, 0xA5, 0x06, 0x83, 0x25, 0x7B, 0x01, 0x00, 0x00} ;

	c = strstr((char *)tempbuff, (char *)calibration_reset_button) ;
	if( c != NULL)
	{
		 index = 0 ;
		 index = c - (char *)tempbuff ;
		 if( tempbuff[index+5] == calibration_reset_button[5] )
		 {

			*supervisorPageDirectionFlag = NOTIFICATION_W_YES_R_NO_PAGE_NUMBER ;
			 uint8_t data[] = "Reset Calibration?" ;
			 uint8_t dbg[30] ;
			 memset(dbg, 0, sizeof(dbg)) ;
			 DWIN_PAGE_CHANGE(NOTIFICATION_W_YES_R_NO_PAGE_NUMBER) ;
			 *supervisorPageDirectionFlag == NOTIFICATION_W_YES_R_NO_PAGE_NUMBER ;
			 POPUP_NOTIFICATION(dbg, UNIVERSAL_NOTIFICATION_ADDR, data, sizeof(data)) ;
			 POPUP_NOTIFICATION(dbg, UNIVERSAL_NOTIFICATION_ADDR, data, sizeof(data)) ;
		 }

	}
	if(*supervisorPageDirectionFlag == NOTIFICATION_W_YES_R_NO_PAGE_NUMBER)
	{
		c = 	NULL ;
		c = strstr((char *)tempbuff, (char *)afterTest_print_no_button ) ;
		if( (c != NULL) )
		{
			 index = 0 ;
			 index = c - (char *)tempbuff ;
			 if( tempbuff[index+4] == afterTest_print_no_button[4] && tempbuff[index+5] == afterTest_print_no_button[5] )
			 {
					if(validationTestVar == 1)
					{
						uint8_t _arr[20];
						getTargetAddr(_arr, sizeof(_arr), 0x2551, 0 ) ;
						HAL_UART_Transmit(&huart1, _arr, 8, HAL_MAX_DELAY);
					}
					if(validationTestVar >=4)
					{
						 if( current_page == USER_PRFILE )
						 {
							 *supervisorPageDirectionFlag = SUPERVISOR_MAIN_PAGE ;
							 profile_page_change( ENDUSER_START_PAGE );

						 }
						 else if(current_page == ADMIN_PROFILE)
						 {
							 *supervisorPageDirectionFlag = ADMIN_MAIN_PAGE ;//admin and supervisor homepages are same
							 profile_page_change(ADMIN_MENU_HOME_START_PAGE_NUMBER); //same here

						 }
						 else if(current_page == SUPERVISOR_PROFILE)
						 {
							 *supervisorPageDirectionFlag = SUPERVISOR_MAIN_PAGE ;//admin and supervisor homepages are same
							 profile_page_change(SUPERVISOR_HOME_START_PAGE_NUMBER); //same here

						 }
						 validationTestVar = 0;
					}
					else
					{
						profile_page_change(SUPERVISOR_MENU_VALIDATION_ADDR);
						 *supervisorPageDirectionFlag = SUPERVISOR_MENU_VALIDATION_ADDR ;
						clearDisplayFields() ;
						homePage_setmmhg = 0 ;
					}
			 }
		}

		c = 	NULL ;
		c = strstr((char *)tempbuff, (char *)afterTest_print_yes_button ) ;
		if( (c != NULL) )
		{
			 index = 0 ;
			 index = c - (char *)tempbuff ;
			 if( tempbuff[index+4] == afterTest_print_yes_button[4] && tempbuff[index+5] == afterTest_print_yes_button[5] )
			 {

				 int diff = 0 ;

				 diff = 0 ;
				 uint8_t data[13] ;
				 sprintf( (char *)data, "%d", diff) ;
//				HAL_UART_Transmit(&huart3, data, strlen((char *)data), HAL_MAX_DELAY);
				 mem_w25qxxDriver_Write_mmhg_GAUGEval(data, strlen( (char *)data)) ;

				 uint8_t data2[] = "Updated" ;
				 uint8_t dbg[30] ;
				 memset(dbg, 0, sizeof(dbg)) ;
				 DWIN_PAGE_CHANGE(0x24) ;
				 POPUP_NOTIFICATION(dbg, UNIVERSAL_NOTIFICATION_ADDR2, data2, sizeof(data2)) ;
				 POPUP_NOTIFICATION(dbg, UNIVERSAL_NOTIFICATION_ADDR2, data2, sizeof(data2)) ;
				 HAL_Delay(2000) ;

				if(validationTestVar == 0)
				{
					uint8_t _arr[20];
					getTargetAddr(_arr, sizeof(_arr), 0x2551, 0 ) ;
					HAL_UART_Transmit(&huart1, _arr, 8, HAL_MAX_DELAY);
				}
				if(validationTestVar >=4)
				{
					 if( current_page == USER_PRFILE )
					 {
						 *supervisorPageDirectionFlag = SUPERVISOR_MAIN_PAGE ;
						 profile_page_change( ENDUSER_START_PAGE );

					 }
					 else if(current_page == ADMIN_PROFILE)
					 {
						 *supervisorPageDirectionFlag = ADMIN_MAIN_PAGE ;//admin and supervisor homepages are same
						 profile_page_change(ADMIN_MENU_HOME_START_PAGE_NUMBER); //same here

					 }
					 else if(current_page == SUPERVISOR_PROFILE)
					 {
						 *supervisorPageDirectionFlag = SUPERVISOR_MAIN_PAGE ;//admin and supervisor homepages are same
						 profile_page_change(SUPERVISOR_HOME_START_PAGE_NUMBER); //same here

					 }
					 validationTestVar = 0;
				}
				else
				{
					profile_page_change(SUPERVISOR_MENU_VALIDATION_ADDR);
					 *supervisorPageDirectionFlag = ADMIN_STOP_PAGE ;
					clearDisplayFields() ;
					homePage_setmmhg = 0 ;
				}
//				HAL_UART_Transmit( &huart3, "its calling\n", 14, HAL_MAX_DELAY) ;

			 }
		}

	}
}

void supervisor_calibrationAndValidation( uint8_t *tempbuff, uint8_t *supervisorPageDirectionFlag )
{
	char *c = NULL ;
	uint8_t index = 0;


/*............................................................*/
	uint8_t supervisorPage_menu_validation_buff[]					= { 0x5A, 0xA5, 0x06, 0x83, SUPERVISOR_MENU_VALIDATION_BUTTON_ADDR1, SUPERVISOR_MENU_VALIDATION_BUTTON_ADDR2, 0x01, 0x00 } ;
	uint8_t supervisorPage_menu_validationStop_buff[]				= { 0x5A, 0xA5, 0x06, 0x83, SUPERVISOR_MENU_VALIDATIONSTOP_BUTTON_ADDR1,
																		SUPERVISOR_MENU_VALIDATIONSTOP_BUTTON_ADDR2, 0x01, 0x00, 0x00 } ;
	uint8_t validation_start_buttonBuff[]							= { 0x5A, 0xA5, 0x06, 0x83, SUPERVISOR_MENU_VALIDATIONSTART_BUTTON_ADDR1
																		, SUPERVISOR_MENU_VALIDATIONSTART_BUTTON_ADDR2, 0x01, 0x00, 0X00 } ;

	uint8_t supervisorPage_menu_validFailed_buff[]					= { 0x5A, 0xA5, 0x06, 0x83, SUPERVISOR_MENU_VALIDAFAIL_BUTTON_ADDR1,
																		SUPERVISOR_MENU_VALIDAFAIL_BUTTON_ADDR2, 0x01, 0x00 } ;
	uint8_t validation_pop_yes_click_button[] 						= { 0x5A, 0xA5, 0x06, 0x83, 0x25, 0x77, 0x01, 0x00, 0x00} ;
	uint8_t validation_pop_No_click_button[] 						= { 0x5A, 0xA5, 0x06, 0x83, 0x25, 0x7B, 0x01, 0x00, 0x00} ;


//	calling calibration
	setCalibration( tempbuff, supervisorPageDirectionFlag) ;

//	reset calibration
	resetCalibration( tempbuff, supervisorPageDirectionFlag) ;




	c= NULL ;
	c = strstr((char *)tempbuff, (char *)supervisorPage_menu_validFailed_buff) ;
	if( c != NULL)
	{
		 index = 0 ;
		 index = c - (char *)tempbuff ;
		 if( tempbuff[index+5] == SUPERVISOR_MENU_VALIDAFAIL_BUTTON_ADDR2 )
		 {
			profile_page_change(SUPERVISOR_MENU_VALID_CALID_ADDR);
			*supervisorPageDirectionFlag = SUPERVISOR_MENU_VALID_CALID_ADDR ;
			HAL_Delay(200) ;

			 uint8_t dt[30] ;
			 dt[0] = 0x5A ;
			 dt[1] = 0xA5 ;
			 dt[2] = 0x06 ;
			 dt[3] = 0x82 ;
			 dt[4] = 0x11 ;
			 dt[5] = 0x88 ;

			  uint64_t 	_mmhg		 = (uint64_t)(homePage_setmmhg) ;
			  uint8_t 	iteration 	 =	0 	;
			  uint8_t 	arr[20] ;
			  uint8_t	j			 =	0	;
			  memset(arr, 0 ,sizeof(arr));

			  while(_mmhg>0)
			  {
				  _mmhg 			= _mmhg/10		;
				  iteration++;
			  }
			  //CHANGE: 20-05
			  sprintf((char *)&arr,"%03d",(homePage_setmmhg));
			  for(uint8_t i= 6 ;i <=6+iteration ; i++)
			  {
				  dt[i] =  arr[j];
				  j++;
			  }
			  HAL_UART_Transmit(&huart1, dt, 9, HAL_MAX_DELAY);
				//END
		 }

	}

	c= NULL ;
	c = strstr((char *)tempbuff, (char *)supervisorPage_menu_validation_buff) ;
	if( c != NULL)
	{
		 index = 0 ;
		 index = c - (char *)tempbuff ;
		 if( tempbuff[index+5] == SUPERVISOR_MENU_VALIDATION_BUTTON_ADDR2 )
		 {
			 uint8_t data[] = "Want to validate?" ;
			 uint8_t dbg[30] ;
			 memset(dbg, 0, sizeof(dbg)) ;
			 DWIN_PAGE_CHANGE(NOTIFICATION_W_YES_R_NO_PAGE_NUMBER) ;

			 POPUP_NOTIFICATION(dbg, UNIVERSAL_NOTIFICATION_ADDR, data, sizeof(data)) ;
			 POPUP_NOTIFICATION(dbg, UNIVERSAL_NOTIFICATION_ADDR, data, sizeof(data)) ;
			 *supervisorPageDirectionFlag = SUPERVISOR_MENU_VALIDATION_ADDR ;


		 }

	}

	if(validationTestVar >= 4)
	{
		  rp203ThermalPrinterDriver_endHour = ds1307_hour ;
		  rp203ThermalPrinterDriver_endMin = ds1307_minute ;
		  rp203ThermalPrinterDriver_endSec = ds1307_second ;

		validation_print_callback(auth_UserName) ;

		 if( current_page == USER_PRFILE )
		 {
			 *supervisorPageDirectionFlag = SUPERVISOR_MAIN_PAGE ;
			 DWIN_PAGE_CHANGE(ENDUSER_START_PAGE) ;
//			 HAL_UART_Transmit( &huart3, "\nuserHome\n", 13, HAL_MAX_DELAY) ;
		 }
		 else if(current_page == ADMIN_PROFILE)
		 {
			 *supervisorPageDirectionFlag = ADMIN_MAIN_PAGE ;//admin and supervisor homepages are same
			 DWIN_PAGE_CHANGE(ADMIN_MENU_HOME_START_PAGE_NUMBER) ;
//			 HAL_UART_Transmit( &huart3, "\nadmiHome\n", 13, HAL_MAX_DELAY) ;

		 }
		 else if(current_page == SUPERVISOR_PROFILE)
		 {
			 *supervisorPageDirectionFlag = SUPERVISOR_MAIN_PAGE ;//admin and supervisor homepages are same
			 DWIN_PAGE_CHANGE(SUPERVISOR_HOME_START_PAGE_NUMBER) ;
//			 HAL_UART_Transmit( &huart3, "\nsuseHome\n", 13, HAL_MAX_DELAY) ;
		 }
		 validationTestVar = 0;
	}

	if( (*supervisorPageDirectionFlag == SUPERVISOR_MENU_VALIDATION_ADDR) && (*supervisorPageDirectionFlag != ADMIN_STOP_PAGE))
	{
		c = 	NULL ;
		c = strstr((char *)tempbuff, (char *)validation_pop_No_click_button ) ;
		if( c != NULL )
		{
			 index = 0 ;
			 index = c - (char *)tempbuff ;
			 if( tempbuff[index+4] == validation_pop_No_click_button[4] &&
					 tempbuff[index+5] == validation_pop_No_click_button[5] )
			 {
				 if( current_page == USER_PRFILE )
				 {
					 *supervisorPageDirectionFlag = SUPERVISOR_MAIN_PAGE ;
					 profile_page_change( ENDUSER_START_PAGE );

				 }
				 else if(current_page == ADMIN_PROFILE)
				 {
					 *supervisorPageDirectionFlag = ADMIN_MAIN_PAGE ;//admin and supervisor homepages are same
					 profile_page_change(ADMIN_MENU_HOME_START_PAGE_NUMBER); //same here

				 }
				 else if(current_page == SUPERVISOR_PROFILE)
				 {
					 *supervisorPageDirectionFlag = SUPERVISOR_MAIN_PAGE ;//admin and supervisor homepages are same
					 profile_page_change(SUPERVISOR_HOME_START_PAGE_NUMBER); //same here

				 }
				 homePage_setMin = 0 ;
				 homePage_setSec = 0 ;
				 homePage_setmmhg = 0 ;

				 validationTestVar = 0 ;
			 }
		}

		c = 	NULL ;
		c = strstr((char *)tempbuff, (char *)validation_pop_yes_click_button ) ;
		if( c != NULL )
		{
			 index = 0 ;
			 index = c - (char *)tempbuff ;
			 if( tempbuff[index+4] == validation_pop_yes_click_button[4] &&
					 tempbuff[index+5] == validation_pop_yes_click_button[5] )
			 {

//				 HAL_UART_Transmit(&huart3, &"its here\n", 11, HAL_MAX_DELAY) ;
				 uint8_t data2[] = "Please Wait" ;
				 uint8_t dbg[30] ;
				 memset(dbg, 0, sizeof(dbg)) ;
				 DWIN_PAGE_CHANGE(0x24) ;
				 POPUP_NOTIFICATION(dbg, UNIVERSAL_NOTIFICATION_ADDR2, data2, sizeof(data2)) ;
				 POPUP_NOTIFICATION(dbg, UNIVERSAL_NOTIFICATION_ADDR2, data2, sizeof(data2)) ;

				if(validationTestVar == 0)
				{
					uint8_t _arr[20];
					getTargetAddr(_arr, sizeof(_arr), 0x2551, 0 ) ;
					HAL_UART_Transmit(&huart1, _arr, 8, HAL_MAX_DELAY);

					mem_W25qxx_Driver_sectorErase(VALID_AND_CALIB_RESULTS);
					HAL_Delay(2000) ;

					rp203ThermalPrinterDriver_startHour = ds1307_hour ;
					rp203ThermalPrinterDriver_startMin 	= ds1307_minute ;
					rp203ThermalPrinterDriver_startSec 	= ds1307_second ;

					memset(rp203ThermalPrinterDriver_TestDate, 0, sizeof(rp203ThermalPrinterDriver_TestDate)) ;
					sprintf((char *)rp203ThermalPrinterDriver_TestDate, "%02d:%02d:%04d",ds1307_date,ds1307_month, ds1307_year) ;

				}
				if(validationTestVar < 4)
				{
					if( (validationTestVar) > 0 )
					{
						 uint8_t temp[24] ;
						 uint8_t dummy[6] ;
						 memset(dummy, 0, sizeof(dummy)) ;
						 sprintf((char *)&dummy, "%03d", homePage_setmmhg) ;
						 memcpy(&temp, dummy, 3) ;		//input mmhg

						 memset(dummy, 0, sizeof(dummy)) ;
						 sprintf((char *)&dummy, "%02d", homePage_setMin) ;
						 memcpy(&temp[3], dummy, 2) ;		//input min

						 memset(dummy, 0, sizeof(dummy)) ;
						 sprintf((char *)&dummy, "%02d", homePage_setSec) ;
						 memcpy(&temp[5], dummy,  2) ;	//input sec

						 uint8_t *gaugeRef[12] ;
						 memset(gaugeRef, 0, sizeof(gaugeRef)) ;
						 mem_w25qxxDriver_Read_mmhg_GAUGEval( (uint8_t *)gaugeRef, sizeof(gaugeRef)) ;
						 int dummymmhg = atoi((const char *)gaugeRef) ;

						 memset(dummy, 0, sizeof(dummy)) ;
						 sprintf((char *)&dummy, "%03d", (uint16_t)(_c_mmhg - dummymmhg) ) ;//current mmhg
						 memcpy(&temp[7], dummy, 3) ;

						 memset(dummy, 0, sizeof(dummy)) ;
						 sprintf((char *)&dummy, "%03d", homePage_setmmhg) ;
						 memcpy(&temp[10], dummy, 3) ;  // gauge_value

						 memset(dummy, 0, sizeof(dummy)) ;
						 sprintf((char *)&dummy, "01") ;
						 memcpy(&temp[13], dummy, 2) ; //pass or fail

//						HAL_UART_Transmit( &huart3, temp, 13, HAL_MAX_DELAY) ;

						mem_W25qxx_Driver_writeBytes(VALID_AND_CALIB_RESULTS +(PAGE_SIZE * validationTestVar), temp, 15) ;//input MmHg
						HAL_Delay(1000) ;
//						memset(temp, 0, sizeof(temp)) ;
//						mem_W25qxx_Driver_ReadBytes(VALID_AND_CALIB_RESULTS +(PAGE_SIZE * validationTestVar), temp, 15) ;
//						HAL_Delay(2000) ;
//						uint8_t addr1[20] ;
//						sprintf(addr1, "\naddr: %x, val:%d ,", VALID_AND_CALIB_RESULTS +(PAGE_SIZE * validationTestVar), validationTestVar) ;
//						HAL_UART_Transmit( &huart3, addr1, strlen(addr1), HAL_MAX_DELAY) ;
//						HAL_UART_Transmit( &huart3, temp, 13, HAL_MAX_DELAY) ;
					}

					profile_page_change(SUPERVISOR_MENU_VALIDATION_ADDR);
					clearDisplayFields() ;
					 homePage_setmmhg = 0 ;

				}
				validationTestVar++  ;
			 }
		}
	}

		c = 	NULL ;
		c = strstr((char *)tempbuff, (char *)validation_start_buttonBuff ) ;
		if( c != NULL )
		{
			 index = 0 ;
			 index = c - (char *)tempbuff ;
			 if( tempbuff[index+4] == validation_start_buttonBuff[4] &&
					 tempbuff[index+5] == validation_start_buttonBuff[5] )
			 {
				if( (homePage_setmmhg != 0) && ( (homePage_setMin != 0) || (homePage_setSec != 0) ) )
				{
					profile_page_change(SUPERVISOR_MENU_VALIDATION_ADDRSTOP);
//		  			*supervisorPageDirectionFlag =	SUPERVISOR_MENU_VALIDATION_ADDRSTOP ;

					currenttime = 0;
					runstopFlag = 1;
					c_runstopButton = 1 ;
					c_min	=	0;
					c_sec	=	0;
					c_runtime	= 0	;

					uni_sec = 0;

					uint8_t _arr[20];
					getTargetAddr(_arr, sizeof(_arr), 0x2551, validationTestVar ) ;
					HAL_UART_Transmit(&huart1, _arr, 8, HAL_MAX_DELAY);
				}


			 }
		}


		c = 	NULL ;
		c = strstr((char *)tempbuff, (char *)supervisorPage_menu_validationStop_buff ) ;
		if( c != NULL )
		{
			 index = 0 ;
			 index = c - (char *)tempbuff ;
			 if( tempbuff[index+4] == supervisorPage_menu_validationStop_buff[4] && tempbuff[index+5] == supervisorPage_menu_validationStop_buff[5] )
			 {
				 _c_mmhg = c_mmhg ;

				 profile_page_change(SUPERVISOR_MENU_VALIDATION_STOPPOPUP_ADDR);
				 *supervisorPageDirectionFlag =	SUPERVISOR_MENU_VALIDATION_STOPPOPUP_ADDR ;

				 homePage_setMin = 0 ;
				 homePage_setSec = 0 ;

				 c_runstopButton = 2 ;
				 runstopFlag = 2 ;

				 testResultFlag = 4 ;

			 }
		}

}

void authentication_SupervisorPage_Read(uint8_t *displayUartFlag, uint8_t *supervisorPageDirectionFlag,  uint8_t *loadpage_nextFlag, uint8_t *loadpage_backFlag, uint8_t *loadpage_applyFlag, uint8_t *c_runstopButton1, uint8_t *loadpage_deleteFlag)
{

	uint8_t tempbuff[MAX_MODEM_RX_BUFF] = {0};
	char *c = NULL ;
	uint8_t index = 0;


	uint8_t supervisorPage_menu_add_users_buff[] 					= { 0x5A, 0xA5, 0x06, 0x83, SUPERVISOR_MENU_ADD_USER_BUTTON_ADDR1, SUPERVISOR_MENU_ADD_USER_BUTTON_ADDR2, 0x01, 0x00} ;
	uint8_t supervisorPage_menu_add_users_password_addr[] 			= { SUPERVISOR_MENU_ADD_USER_USERPASSWORD_BUTTON_ADDR1, SUPERVISOR_MENU_ADD_USER_USERPASSWORD_BUTTON_ADDR2 , 0x00} ;
	uint8_t supervisorPage_menu_add_users_Username_addr[] 			= {SUPERVISOR_MENU_ADD_USER_USERNAMEE_BUTTON_ADDR1, SUPERVISOR_MENU_ADD_USER_USERNAMEE_BUTTON_ADDR2, 0x00} ;

	uint8_t supervisorPage_menu_add_users_addButton_addr[] 			= { 0x5A, 0xA5, 0x06, 0x83, SUPERVISOR_MENU_ADD_USER_ADD_BUTTON_ADDR1,SUPERVISOR_MENU_ADD_USER_ADD_BUTTON_ADDR2, 0x01, 0x00 } ;

	uint8_t supervisorPage_menu_add_testProfile_buff[]				= { 0x5A, 0xA5, 0x06, 0x83, SUPERVISOR_MENU_ADD_TESTPROFILE_BUTTON_ADDR1, SUPERVISOR_MENU_ADD_TESTPROFILE_BUTTON_ADDR2, 0x01, 0x00 } ;
	uint8_t supervisorPage_menu_add_testProfile_search_addr[]		= { SUPERVISOR_MENU_ADD_TESTPROFILE_SEARCH_BUTTON_ADDR1, SUPERVISOR_MENU_ADD_TESTPROFILE_SEARCH_BUTTON_ADDR2 , 0x00} ;

	uint8_t supervisorPage_menu_add_testProfile_Pname_addr[]		= { SUPERVISOR_MENU_ADD_TESTPROFILE_PNAME_BUTTON_ADDR1, SUPERVISOR_MENU_ADD_TESTPROFILE_PNAME_BUTTON_ADDR2 , 0x00} ;
	uint8_t supervisorPage_menu_add_testProfile_MMHG_addr[]			= { SUPERVISOR_MENU_ADD_TESTPROFILE_MMHG_BUTTON_ADDR1, SUPERVISOR_MENU_ADD_TESTPROFILE_MMHG_BUTTON_ADDR2, 0x00 } ;
	uint8_t supervisorPage_menu_add_testProfile_MIN_addr[]			= { SUPERVISOR_MENU_ADD_TESTPROFILE_MIN_BUTTON_ADDR1, SUPERVISOR_MENU_ADD_TESTPROFILE_MIN_BUTTON_ADDR2, 0x00 } ;
	uint8_t supervisorPage_menu_add_testProfile_SEC_addr[]			= { SUPERVISOR_MENU_ADD_TESTPROFILE_SEC_BUTTON_ADDR1, SUPERVISOR_MENU_ADD_TESTPROFILE_SEC_BUTTON_ADDR2, 0x00 } ;
	uint8_t supervisorPage_menu_add_testProfile_ADDButton_buff[]	= { 0x5A, 0xA5, 0x06, 0x83, SUPERVISOR_MENU_ADD_TESTPROFILE_ADD_BUTTON_ADDR1, SUPERVISOR_MENU_ADD_TESTPROFILE_ADD_BUTTON_ADDR2, 0x01, 0x00 } ;

	uint8_t supervisor_deleteEndUsers_buttonBuff[]	= 	{ 0x5A, 0xA5, 0x06, 0x83, SUPERVISOR_MENU_DELETEUSERS_BUTTON_ADDR1, SUPERVISOR_MENU_DELETEUSERS_BUTTON_ADDR2, 0x01, 0x00, 0x00  } ;
	uint8_t supervisor_deleteTestProfile_buttonBuff[]	= 	{ 0x5A, 0xA5, 0x06, 0x83, SUPERVISOR_MENU_DELETETESTPROFILE_BUTTON_ADDR1, SUPERVISOR_MENU_DELETETESTPROFILE_BUTTON_ADDR2, 0x01, 0x00, 0x00  } ;

	uint8_t user_passwordChange_present_username[]		=	{ ADMIN_MENU_CHANGE_PASSWORD_PRESENT_ADDR1, ADMIN_MENU_CHANGE_PASSWORD_PRESENT_ADDR2 , 0X00} 	;
	uint8_t user_passwordChange_New_username[]			=	{ ADMIN_MENU_CHANGE_PASSWORD_NEW_ADDR1, ADMIN_MENU_CHANGE_PASSWORD_NEW_ADDR2, 0X00 } 	;
	uint8_t user_passwordChange_addButton[]				=	{ 0x5A, 0xA5, 0x06, 0x83, ADMIN_MENU_CHANGE_PASSWORD_ADD_BUTTON_ADDR1, ADMIN_MENU_CHANGE_PASSWORD_ADD_BUTTON_ADDR2, 0x01, 0x00 } 	;
	uint8_t userpasswordchange_gothrough_button[] 						= { 0x5A, 0xA5, 0x06, 0x83, 0x24, 0xDB, 0x01, 0x00, 0x01, 0x00 } ;

	if( *displayUartFlag == 1 )
	{
		HAL_Delay(15);

		for(int i=0; i< rxBufferCount ; i++)
		{
			tempbuff[i] = modemRxBuff[i];
		}
//		HAL_UART_Transmit(&huart3, tempbuff, rxBufferCount, HAL_MAX_DELAY);
		display_clearModemBuffer();



/*** @@SUPERVISOR HOME PAGE ***/


		universal_homePage(tempbuff, supervisorPageDirectionFlag, SUPERVISOR_PROFILE );

/*** @@SUPERVISOR LOAD PAGE READ INPUTS ***/
		universal_loadPage_read(tempbuff, loadpage_nextFlag, loadpage_backFlag, loadpage_applyFlag, loadpage_deleteFlag);
		if(  current_page == USER_PRFILE )
		{
			universal_loadPage_update_values(loadpage_nextFlag, loadpage_backFlag, loadpage_applyFlag, loadpage_deleteFlag,  ENDUSER_START_PAGE);

		}
		else if( *supervisorPageDirectionFlag == SUPERVISOR_LOAD_PAGE)
		{
			universal_loadPage_update_values(loadpage_nextFlag, loadpage_backFlag, loadpage_applyFlag, loadpage_deleteFlag,  SUPERVISOR_MAIN_PAGE);

		}

/*** @@SUPERVISOR MENU PAGE READ INPUTS ***/

		supervisor_calibrationAndValidation(tempbuff, supervisorPageDirectionFlag ) ;

		/*.......................................*** @@SUPERVISOR-MENU-ADD USERS-PAGE ***............................................................................................................*/
		c= NULL ;
		c = strstr((char *)tempbuff, (char *)supervisorPage_menu_add_users_buff) ;
		if( c != NULL)
		{
			 profile_page_change(SUPERVISOR_MENU_ADDUSERS_PAGE_NUMBER);
			 *supervisorPageDirectionFlag = SUPERVISOR_MENU_ADDUSERS_PAGE_NUMBER ;

			 uint8_t dt[30] ;
			 dt[0] = 0x5A ;
			 dt[1] = 0xA5 ;
			 dt[2] = 0x05 ;
			 dt[3] = 0x82 ;
			 dt[4] = 0x11 ;
			 dt[5] = 0x77 ;
			 dt[6] = 0x00 ;
			 dt[7] = 0x00 ;

			 HAL_UART_Transmit(&huart1, dt, 8, HAL_MAX_DELAY) ;
			 dt[4] = 0x11 ;
			 dt[5] = 0x6A ;

			 HAL_UART_Transmit(&huart1, dt, 8, HAL_MAX_DELAY) ;
		}


		c = NULL ;
		c = strstr((char *)tempbuff, (char *)supervisorPage_menu_add_users_password_addr) ;
		if( c != NULL )
		{
			 index = 0 ;
			 index = c - (char *)tempbuff ;
			 if( tempbuff[index-1] == 0x83 )
			 {
				 uint8_t size = tempbuff[index+4] ;
				 for(uint8_t i=0; i< 11 ; i++)
				 {
					 if( i <= (size-1) )
						 supervisor_password[i] = tempbuff[index+5+i] ;
					 else
						 supervisor_password[i] = '\0' ;
				 }
//				 HAL_UART_Transmit(&huart3, supervisor_password, strlen(supervisor_password), HAL_MAX_DELAY);

			 }
		}

		c = NULL ;
		c =  strstr((char *)tempbuff, (char *)supervisorPage_menu_add_users_Username_addr) ;
		if(c != NULL)
		{
			 index = 0 ;
			 index = c - (char *)tempbuff ;
			 if( tempbuff[index-1] == 0x83 )
			 {
				 uint8_t size = tempbuff[index+4] ;
				 for(uint8_t i=0; i< 11 ; i++)
				 {
					 if( i <= (size-1) )
					 {
						 supervisor_username[i] = tempbuff[index+5+i] ;
					 }

					 else
					 {
						 supervisor_username[i] = '\0' ;
					 }
				 }
			 }
		}
		c = NULL ;
		c = strstr((char *)tempbuff, (char *)supervisorPage_menu_add_users_addButton_addr ) ;
		if( c != NULL )
		{
			 index = 0 ;
			 index = c - (char *)tempbuff ;
			 if( tempbuff[index+4] == supervisorPage_menu_add_users_addButton_addr[index +4] &&
					 tempbuff[index+5] == supervisorPage_menu_add_users_addButton_addr[index +5])
			 {
				 mem_w25qxxDriver_user_write(supervisor_username, supervisor_password, &supervisor_usercounter);
				 supervisor_usercounter++ ;
			 }

		}


		/*.......................................*** @@SUPERVISOR-MENU-ADD TESTPROFILES-PAGE ***............................................................................................................*/


		c= NULL ;
		c = strstr((char *)tempbuff, (char *)supervisorPage_menu_add_testProfile_buff) ;
		if( c != NULL)
		{
			 profile_page_change(SUPERVISOR_MENU_ADDTESTPROFILES_PAGE_NUMBER);
			 *supervisorPageDirectionFlag = SUPERVISOR_MENU_ADDTESTPROFILES_PAGE_NUMBER ;

			 uint8_t dt[30] ;
			 dt[0] = 0x5A ;
			 dt[1] = 0xA5 ;
			 dt[2] = 0x05 ;
			 dt[3] = 0x82 ;
			 dt[4] = 0x11 ;
			 dt[5] = 0x33 ;
			 dt[6] = 0x00 ;
			 dt[7] = 0x00 ;

			 HAL_UART_Transmit(&huart1, dt, 8, HAL_MAX_DELAY) ;
			 dt[4] = 0x11 ;
			 dt[5] = 0x50 ;
			 HAL_UART_Transmit(&huart1, dt, 8, HAL_MAX_DELAY) ;

			 dt[4] = 0x11 ;
			 dt[5] = 0x57 ;
			 HAL_UART_Transmit(&huart1, dt, 8, HAL_MAX_DELAY) ;

			 dt[4] = 0x11 ;
			 dt[5] = 0x5C ;
			 HAL_UART_Transmit(&huart1, dt, 8, HAL_MAX_DELAY) ;

//				//CHANGE: 20-05
//				uint8_t fixedmmhg[5] ;
//				mem_w25qxxDriver_read_fixed_mmhg(fixedmmhg) ;
//
//				uint8_t data2[30] ;
//				data2[0] = 0x5A ;
//				data2[1] = 0xA5 ;
//				data2[2] = 0x08 ;
//				data2[3] = 0X82 ;
//				data2[4] = 0x25 ;
//				data2[5] = 0x62 ;
//				for(uint8_t i=0, j=6; i<4;i++,j++)
//				{
//					data2[j] = fixedmmhg[i] ;
//				}
//				HAL_UART_Transmit(&huart1, data2, 8, HAL_MAX_DELAY);

		}

		c= NULL ;
		c = strstr((char *)tempbuff, (char *)supervisorPage_menu_add_testProfile_search_addr) ;
		if( c != NULL)
		{
			 uint8_t index = 0 ;
			 index = c - (char *)tempbuff ;
			 if( tempbuff[index-1] == 0x83 )
			 {
				 uint8_t size = tempbuff[index+4] ;
				 sVP_M_ADD_TSTP_Srch_var = 0;

				 for(uint8_t i=0 ;i< size;i++)
				 {

					 sVP_M_ADD_TSTP_Srch_var = sVP_M_ADD_TSTP_Srch_var * 10 ;
					 sVP_M_ADD_TSTP_Srch_var = sVP_M_ADD_TSTP_Srch_var + ( tempbuff[index+5+i] - '0' ) ;


				 }

			 }
		}

		c= NULL ;
		c = strstr((char *)tempbuff, (char *)supervisorPage_menu_add_testProfile_Pname_addr) ;
		if( c != NULL)
		{
			 uint8_t index = 0 ;
			 index = c - (char *)tempbuff ;
			 if( tempbuff[index-1] == 0x83 )
			 {
				 uint8_t size = tempbuff[index+4] ;

				 for(uint8_t i=0 ;i<size; i++)
				 {
					 sVP_M_ADD_TSTP_Pname_var[i] = tempbuff[index+5+i] ;
				 }
			 }
		}


		c= NULL ;
		c = strstr((char *)tempbuff, (char *)supervisorPage_menu_add_testProfile_MMHG_addr) ;
		if( c != NULL)
		{
			 uint8_t index = 0 ;
			 index = c - (char *)tempbuff ;

			 if( tempbuff[index-1] == 0x83 )
			 {
				 uint8_t size = tempbuff[index+4] ;
				 sVP_M_ADD_TSTP_MMHG_var = 0 ;

				 for(uint8_t i=0 ;i< size;i++)
				 {
					 sVP_M_ADD_TSTP_MMHG_var = sVP_M_ADD_TSTP_MMHG_var * 10 ;
					 sVP_M_ADD_TSTP_MMHG_var = sVP_M_ADD_TSTP_MMHG_var + ( tempbuff[index+5+i] - '0' ) ;


				 }
			 }
		}

		c= NULL ;
		c = strstr((char *)tempbuff, (char *)supervisorPage_menu_add_testProfile_MIN_addr) ;
		if( c != NULL)
		{
			 uint8_t index = 0 ;
			 index = c - (char *)tempbuff ;
			 if( tempbuff[index-1] == 0x83 )
			 {
				 uint8_t size = tempbuff[index+4] ;
				 sVP_M_ADD_TSTP_MIN_var = 0;
				 for(uint8_t i=0 ;i< size;i++)
				 {
					 sVP_M_ADD_TSTP_MIN_var = sVP_M_ADD_TSTP_MIN_var * 10 ;
					 sVP_M_ADD_TSTP_MIN_var = sVP_M_ADD_TSTP_MIN_var + ( tempbuff[index+5+i] - '0' ) ;


				 }
			 }
		}

		c= NULL ;
		c = strstr((char *)tempbuff, (char *)supervisorPage_menu_add_testProfile_SEC_addr) ;
		if( c != NULL)
		{
			 uint8_t index = 0 ;
			 index = c - (char *)tempbuff ;
			 if( tempbuff[index-1] == 0x83 )
			 {
				 uint8_t size = tempbuff[index+4] ;
				 sVP_M_ADD_TSTP_SEC_var = 0;

				 for(uint8_t i=0 ;i< size;i++)
				 {
					 sVP_M_ADD_TSTP_SEC_var = sVP_M_ADD_TSTP_SEC_var * 10 ;
					 sVP_M_ADD_TSTP_SEC_var = sVP_M_ADD_TSTP_SEC_var + ( tempbuff[index+5+i] - '0' ) ;


				 }

			 }
		}

		c= NULL ;
		c = strstr((char *)tempbuff, (char *)supervisorPage_menu_add_testProfile_ADDButton_buff) ;
		if( c != NULL)
		{
			 mem_w25qxxDriver_Write_TestProfiles(sVP_M_ADD_TSTP_Srch_var, sVP_M_ADD_TSTP_MMHG_var, sVP_M_ADD_TSTP_MIN_var, sVP_M_ADD_TSTP_SEC_var, sVP_M_ADD_TSTP_Pname_var, sVP_TestProfileCounter);
			 sVP_TestProfileCounter++ ;
		}

		c= NULL ;
		c = strstr((char *)tempbuff, (char *)supervisor_deleteEndUsers_buttonBuff) ;
		if( c != NULL)
		{
			 index = 0 ;
			 index = c - (char *)tempbuff ;
			 if( tempbuff[index+5] == SUPERVISOR_MENU_DELETEUSERS_BUTTON_ADDR2 &&  tempbuff[index+4] == SUPERVISOR_MENU_DELETEUSERS_BUTTON_ADDR1)
			 {

				 DWIN_PAGE_CHANGE(SUPERVISOR_MENU_ENDUSER_DELETE_PAGE_NUMBER);
				 *supervisorPageDirectionFlag =  SUPERVISOR_MENU_ENDUSER_DELETE_PAGE_NUMBER ;
				 endUserDeleteinitVal = 9 ;

			 }


		}

		c= NULL ;
		c = strstr((char *)tempbuff, (char *)supervisor_deleteTestProfile_buttonBuff) ;
		if( c != NULL)
		{
			 index = 0 ;
			 index = c - (char *)tempbuff ;
			 if( tempbuff[index+5] == SUPERVISOR_MENU_DELETETESTPROFILE_BUTTON_ADDR2 &&  tempbuff[index+4] == SUPERVISOR_MENU_DELETETESTPROFILE_BUTTON_ADDR1)
			 {

				 DWIN_PAGE_CHANGE(SUPERVISOR_MENU_TESTPROFILE_DELETE_PAGE_NUMBER);
				 *supervisorPageDirectionFlag =  SUPERVISOR_LOAD_PAGE ;

				  testProfileDeleteTrackIndex = 0 ;
				  testProfileDeleteinitVal = 9 ;

			 }


		}

		if( *supervisorPageDirectionFlag ==  SUPERVISOR_MENU_ENDUSER_DELETE_PAGE_NUMBER)
		{
			//suervisor
			endUserDeleteRead(tempbuff, &endUserDelete1stButton, &endUserDelete2stButton, &endUserDelete3stButton, &endUserDeleteIndex);
			endUserDeleteDetailsUpdate( &endUserDeleteIndex, &endUserDelete1stButton, &endUserDelete2stButton, &endUserDelete3stButton, &endUserDeleteinitVal, 2) ;
		}

/*** user password change ***/



		c = NULL ;
		c = strstr((char *)tempbuff, (char *)userpasswordchange_gothrough_button) ;
		if( c != NULL)
		{
			 index = 0 ;
			 index = c - (char *)tempbuff ;
			 if( tempbuff[index+5] == userpasswordchange_gothrough_button[5] &&  tempbuff[index+4] == userpasswordchange_gothrough_button[4])
			 {
//					 mem_w25qxxDriver_user_password_change( USER_ADDR, auth_UserName, auth_PassWord, uint8_t *newpassword) ;

				 uint8_t data2[30] ;
				  data2[0] = 0x5A ;
				  data2[1] = 0xA5 ;
				  data2[2] = 0x0C ;
				  data2[3] = 0X82 ;
				  data2[4] = 0x23 ;
				  data2[5] = 0xA4 ;
				  for(uint8_t i=6; i<13;i++)
				  {
					data2[i] = '0' ;
				  }
				  HAL_UART_Transmit(&huart1, data2, 15, HAL_MAX_DELAY);
				  data2[4] = 0x23 ;
				  data2[5] = 0xB1 ;
				  HAL_UART_Transmit(&huart1, data2, 15, HAL_MAX_DELAY);

				 DWIN_PAGE_CHANGE(ADMIN_MENU_HOME_ADMIN_PASSWORD_CHANGE_PAGE_NUMBER);
				 *supervisorPageDirectionFlag =  ADMIN_MENU_HOME_ADMIN_PASSWORD_CHANGE_PAGE_NUMBER ;
			 }

		}

		c = 	NULL ;
		c = strstr((char *)tempbuff, (char *)user_passwordChange_present_username ) ;
		if( c != NULL )
		{
			 index = 0 ;
			 index = c - (char *)tempbuff ;
			 if( tempbuff[index+1] == ADMIN_MENU_CHANGE_PASSWORD_PRESENT_ADDR2 )
			 {
				 uint8_t size = tempbuff[index+4] ;
				 for(uint8_t i=0; i< 11 ; i++)
				 {
					 if( i <= (size-1) )
						 admin_changePassword_PresentPassword[i] = tempbuff[index+5+i] ;
					 else
						 admin_changePassword_PresentPassword[i] = '\0' ;
				 }

			 }

		}

		c = 	NULL ;
		c = strstr((char *)tempbuff, (char *)user_passwordChange_New_username ) ;
		if( c != NULL )
		{
			 index = 0 ;
			 index = c - (char *)tempbuff ;
			 if( tempbuff[index+1] == ADMIN_MENU_CHANGE_PASSWORD_NEW_ADDR2 )
			 {
				 uint8_t size = tempbuff[index+4] ;
				 for(uint8_t i=0; i< 11 ; i++)
				 {
					 if( i <= (size-1) )
						 admin_changePassword_NewPassword[i] = tempbuff[index+5+i] ;
					 else
						 admin_changePassword_NewPassword[i] = '\0' ;
				 }
			 }

		}

		c = 	NULL ;
		c = strstr((char *)tempbuff, (char *)user_passwordChange_addButton ) ;
		if( c != NULL )
		{
			 index = 0 ;
			 index = c - (char *)tempbuff ;
			 if( tempbuff[index+5] == ADMIN_MENU_CHANGE_PASSWORD_ADD_BUTTON_ADDR2 )
			 {
				 if( current_page == USER_PRFILE)
				 {
					 mem_w25qxxDriver_user_password_change( USER_ADDR, auth_UserName, admin_changePassword_PresentPassword, admin_changePassword_NewPassword) ;
//					 HAL_UART_Transmit(&huart3, &"user called\n", 14, 1000) ;
				 }
				 else if( current_page == SUPERVISOR_PROFILE)
				 {
					 mem_w25qxxDriver_user_password_change( SUPERUSER_ADDR, auth_UserName, admin_changePassword_PresentPassword, admin_changePassword_NewPassword) ;
//					 HAL_UART_Transmit(&huart3, &"supev called\n", 15, 1000) ;
				 }
			 }


		}


		*displayUartFlag = 0 ;
	}

}
void display_mmhgValueUpdateToDisplay( uint64_t  mmhg,uint8_t runStopFlag)
{

	  uint8_t _mmhgPayLoad[50]  ; //= {0x5A ,0xA5, 0x07 ,0x82 ,0x14 ,0x00};
	  uint8_t _mmhglen 	=	sizeof(_mmhgPayLoad);

	  memset(_mmhgPayLoad,0,_mmhglen);




	  _mmhgPayLoad[0]	=	0x5A	;
	  _mmhgPayLoad[1]	=	0xA5	;
	  _mmhgPayLoad[2]	=	0x06	;
	  _mmhgPayLoad[3]	=	0x82	;
	  _mmhgPayLoad[4]	=	HOMEPAGE_CURRENT_VACCUM_ADDR1	;
	  _mmhgPayLoad[5]	=	HOMEPAGE_CURRENT_VACCUM_ADDR2	;

		uint8_t *gaugeRef[12] ;
		memset(gaugeRef, 0, sizeof(gaugeRef)) ;
		mem_w25qxxDriver_Read_mmhg_GAUGEval( (uint8_t *)gaugeRef, sizeof(gaugeRef)) ;
		int dummymmhg = atoi((const char *)gaugeRef) ;

	  uint64_t 	_mmhg		 = (uint64_t)(mmhg) - dummymmhg ;

	  uint8_t 	iteration 	 =	0 	;
	  uint8_t 	arr[20] ;
	  uint8_t	j			 =	0	;
	  memset(arr, 0 ,sizeof(arr));

	  while(_mmhg>0)
	  {
		  _mmhg 			= _mmhg/10		;

		  iteration++;
	  }

	  //CHANGE: 20-05
//	  uint16_t mmhgdum = (uint16_t)(mmhg) ;

	  sprintf((char *)&arr,"%03ld", mmhg );


	  for(uint8_t i= 6 ;i <=6+iteration ; i++)
	  {
		  _mmhgPayLoad[i] =  arr[j];
		  j++;
	  }

	  if(runStopFlag ==1)//stop button clicked if value is 2
	  {
		  HAL_UART_Transmit(&huart1, _mmhgPayLoad, 9, HAL_MAX_DELAY);
	  }

	  memset(_mmhgPayLoad,0,_mmhglen);


}

void display_secUpdate(uint8_t authPage_sec, uint8_t runstopFlag )
{
	uint8_t _arr[100];
	memset(_arr,0, sizeof(_arr));

	_arr[0]		= 	0x5A 	;
	_arr[1]		= 	0xA5	;
	_arr[2]		=	0x05	;
	_arr[3]		=	0x82	;
	_arr[4]		=	HOMEPAGE_CURRENT_TIME_SEC_ADDR1	;
	_arr[5]		=	HOMEPAGE_CURRENT_TIME_SEC_ADDR2	;


	uint8_t _sec		=	(authPage_sec)	;
	uint8_t arr[10] 			;
	uint8_t iter		=	0	;
	uint8_t j			=	0	;

	memset(arr,0,sizeof(arr));


	while(_sec > 0)
	{
		_sec 		=	_sec/10		;
		iter++;
	}
	sprintf((char *)arr,"%02d",(authPage_sec));

	for(uint8_t i= 6 ; i<=6+iter ;i++)
	{
		_arr[i] = arr[j];
		j++;
	}
	if( (runstopFlag == 1) )
	{
		HAL_UART_Transmit(&huart1, _arr, 8, HAL_MAX_DELAY);
	}

	memset(_arr,0,sizeof(_arr));

}


void display_runTimeUpdateToDisplay(uint8_t runstopFlag, uint8_t uni_sec)
{
	uint8_t _arr[100];
	memset(_arr,0, sizeof(_arr));

	uint8_t buff[100];
	memset(buff,0, sizeof(buff));
	uint64_t  ms = uni_sec - c_runtime	;
	c_sec = (ms);

	display_secUpdate(c_sec , runstopFlag);

	_arr[0]		= 	0x5A 	;
	_arr[1]		= 	0xA5	;
	_arr[2]		=	0x05	;
	_arr[3]		=	0x82	;
	_arr[4]		=	HOMEPAGE_CURRENT_TIME_MIN_ADDR1	;
	_arr[5]		=	HOMEPAGE_CURRENT_TIME_MIN_ADDR2	;


	if( c_sec >= 59 || c_sec <= 1 )
	{

		if(c_sec <=1 && c_min== 0)
		{
			c_min = 0;
		}
		else if( c_sec >= 59)
		{
			c_runtime = uni_sec+1 ;
			c_min++ ;
		}



		uint8_t _min		= c_min	;
		uint8_t arr[10] 			;
		uint8_t iter		=	0	;
		uint8_t j			=	0	;

		memset(arr,0,sizeof(arr));

		while(_min > 0)
		{
			_min 		=	_min/10		;
			iter++;
		}
		sprintf( (char *)arr,"%02d", c_min);

		for(uint8_t i= 6 ; i<=6+iter ;i++)
		{
			_arr[i] = arr[j];
			j++;
		}

		if( ( runstopFlag == 1 )  )//stop button value 2
		{
			HAL_UART_Transmit(&huart1, _arr, 8, HAL_MAX_DELAY);

		}
	}



}



void profile_write_serialNo_read( uint8_t *displayUartFlag, uint8_t *op, uint8_t *serialNo_read_flag,uint8_t *write_button_flag) //,uint8_t rxBufferCount
{
	/** example packate  = 5A A5 0A 83 90 06 03 5A 02 39 38 FF FF #98 */
	uint8_t tempbuff[MAX_MODEM_RX_BUFF] = {0};
	char 	*c 				= NULL ;
	uint8_t indexX = 0;
	uint8_t size = 0 ;
	uint8_t receivedBuff[2]= {0} ;

	uint8_t read_serialNo_addr[] = { 0x90,0x06 } ;
	uint8_t home_page_buff[]	= { 0x5A, 0xA5, 0x06, 0x83, 0x99, 0x99, 0x01, 0x00, 0x01 }	;
	uint8_t write_button_buff[]	= { 0x5A, 0xA5, 0x06, 0x83, 0x90, 0x08, 0x01, 0x00, 0x01 }	;
	home_page_buff[sizeof(home_page_buff) -  1 ] 				=	'\0'	;
	read_serialNo_addr[sizeof(read_serialNo_addr) -  1 ] 		=	'\0'	;
	write_button_buff[sizeof(write_button_buff) -  1 ] 			=	'\0'	;

	if( *displayUartFlag == 1)
	{
		HAL_Delay(1);

		for(int i=0; i< rxBufferCount ; i++)
		{
			tempbuff[i] = modemRxBuff[i];
		}
		display_clearModemBuffer();
//		HAL_UART_Transmit(&huart1, &tempbuff, strlen(tempbuff), HAL_MAX_DELAY);

		c = strstr((char *)tempbuff, (char *)read_serialNo_addr ) ;
		if( c != NULL )
		{
			indexX = 0 ;
			indexX = c - (char *)tempbuff ;


			if(tempbuff[ indexX - 1 ]	==	0x83 )
			{
				if(tempbuff[ indexX - 2 ]	==	0x0A)
				{
					size		= 0 ;
					size 		= tempbuff[ indexX + 4 ] ;
					indexX 		= indexX + 5 ;

					for(uint8_t i =0; i< size; i++)
					{
						receivedBuff[i] = tempbuff[indexX] ;
						indexX++;
					}
					 *serialNo_read_flag = 1 ;//true
					 *op = atoi( (char *)receivedBuff);
//					 HAL_UART_Transmit(&huart1, &*op, 1, HAL_MAX_DELAY);

				}
			}

		}
		c = NULL ;
		c = strstr((char *)tempbuff, (char *)home_page_buff ) ;
		if( c != NULL)
		{
			current_page 	= 	PROFILE_HOME_PAGE	;
		}
		c = NULL ;
		c = strstr((char *)tempbuff, (char *)write_button_buff ) ;
		if( c != NULL)
		{
			*write_button_flag = 1 ;//write command
		}


		*displayUartFlag = 0 ;

	}

}


void Page_validation(uint8_t *displayUartFlag, uint8_t *page_invoke)
{

	uint8_t tempbuff[MAX_MODEM_RX_BUFF];
	char 	*c 				= NULL ;

	uint8_t save_page_buff[]	= { 0x5A, 0xA5, 0x06, 0x83, 0x10, 0x01, 0x01, 0x00, 0x01 }	;
	uint8_t home_page_buff[]	= { 0x5A, 0xA5, 0x06, 0x83, 0x99, 0x99, 0x01, 0x00, 0x01 }	;
	uint8_t load_page_buff[]	= { 0x5A, 0xA5, 0x06, 0x83, 0x10, 0x02, 0x01, 0x00, 0x01 } 	;

	save_page_buff[sizeof(save_page_buff) -  1 ] 				=	'\0'	;
	home_page_buff[sizeof(home_page_buff) -  1 ] 				=	'\0'	;

	if(*displayUartFlag == 1 && *page_invoke == 0)
	{
		HAL_Delay(5);

		for(int i=0; i< rxBufferCount ; i++)
		{
			tempbuff[i] = modemRxBuff[i];
		}
		display_clearModemBuffer();
//		uint8_t data[30];
//		memset(data,0,sizeof(data));
//		sprintf(data," hm %d \n");
//		HAL_UART_Transmit(&huart1, &tempbuff, strlen(tempbuff), HAL_MAX_DELAY);


		c = strstr((char *)tempbuff, (char *)save_page_buff );
		if( c != 	NULL )
		{
			*page_invoke = 1 ;
			current_page = PROFILE_WRITE_PAGE ;
		}
		c = NULL ;
		c = strstr((char *)tempbuff, (char *)home_page_buff );
		if( c != NULL)
		{
			*page_invoke = 1 ;
			current_page = PROFILE_HOME_PAGE ;
		}
		c = NULL ;
		c = strstr((char *)tempbuff, (char *)load_page_buff );
		if( c != NULL)
		{
			current_page = PROFILE_LOAD_PAGE ;
		}
		*displayUartFlag = 0 ;
	}

}

/* ######################################################################################################################################################################## */
/************************************ LOAD PAGE DETAILS BELOW ************************************/
/* ######################################################################################################################################################################## */
//void profile_load_read(uint8_t *displayUartFlag,uint8_t *nextFlag, uint8_t *backFlag, uint8_t *applyFlag)
//{
//
//	uint8_t tempbuff[MAX_MODEM_RX_BUFF] = {0};
//	char 	*c 				= NULL ;
//	uint8_t indexX = 0;
//	uint8_t size = 0 ;
//	uint8_t receivedBuff[10]= {0} ;
//
///*** @LOAD PAGE ADRESS ***/
//	uint8_t loadPage_next_buttonBuff[]  = { 0x5A, 0xA5, 0x06, 0x83, 0x49, 0x50, 0x01, 0x00, 0x01 };
//	uint8_t loadPage_back_buttonBuff[]  = { 0x5A, 0xA5, 0x06, 0x83, 0x49, 0x00, 0x01, 0x00, 0x01 };
////	uint8_t loadPage_home_ButtonBuff[]  = { 0x5A, 0xA5, 0x06, 0x83, 0x99, 0x99, 0x01, 0x00, 0x01 };
//	uint8_t loadPage_first_ApplyBuff[]  = { 0x5A, 0xA5, 0x06, 0x83, 0x42, 0x50, 0x01, 0x00, 0x01 };
//	uint8_t loadPage_second_ApplyBuff[] = { 0x5A, 0xA5, 0x06, 0x83, 0x45, 0x50, 0x01, 0x00, 0x01 };
//	uint8_t loadPage_third_ApplyBuff[]  = { 0x5A, 0xA5, 0x06, 0x83, 0x48, 0x50, 0x01, 0x00, 0x01 };
//
//	if( *displayUartFlag == 1)
//	{
//		HAL_Delay(1);
//
//		for(int i=0; i< rxBufferCount ; i++)
//		{
//			tempbuff[i] = modemRxBuff[i];
//		}
//		HAL_UART_Transmit(&huart1, &tempbuff,rxBufferCount, HAL_MAX_DELAY);
//		display_clearModemBuffer();
//
///*** @next button ***/
//		c = strstr((char *)tempbuff, (char *)loadPage_next_buttonBuff);
//		if( c!= NULL)
//		{
//			*nextFlag = 1;
//			uint8_t dt[30];
//			sprintf(dt,"nextbutton \n");
//			HAL_UART_Transmit(&huart1, &dt, strlen(dt), HAL_MAX_DELAY);
//		}
//
///*** @back button ***/
//		c = 	NULL ;
//		c = strstr((char *)tempbuff, (char *)loadPage_back_buttonBuff ) ;
//		if( c != NULL )
//		{
//			indexX = 0 ;
//			indexX = c - (char )tempbuff ;
//			 if( tempbuff[indexX+4] == loadPage_back_buttonBuff[4] && tempbuff[indexX+5] == loadPage_back_buttonBuff[5] )
//			 {
//				 profile_page_change(tempbuff[indexX+8]);
//			 }
//		}
//
///*** @home button ***/
//		c= NULL ;
//		c = strstr((char *)tempbuff, (char *)loadPage_home_ButtonBuff);
//		if( c!= NULL)
//		{
//			current_page = PROFILE_HOME_PAGE ;
//			uint8_t dt[30];
//			sprintf(dt,"load homeB \n");
//			HAL_UART_Transmit(&huart1, &dt, strlen(dt), HAL_MAX_DELAY);
//		}
//
///*** @first apply button ***/
//		c= NULL ;
//		c = strstr((char *)tempbuff, (char *)loadPage_first_ApplyBuff);//FIRSTAPPLY BUTTON
//		if( c!= NULL)
//		{
//			*applyFlag = 1;
//			uint8_t dt[30];
//			sprintf(dt,"load 1 apply \n");
//			HAL_UART_Transmit(&huart1, &dt, strlen(dt), HAL_MAX_DELAY);
//		}
//
///*** @second apply button ***/
//		c= NULL ;
//		c = strstr((char *)tempbuff, (char *)loadPage_second_ApplyBuff);//FIRSTAPPLY BUTTON
//		if( c!= NULL)
//		{
//			*applyFlag = 2;
//			uint8_t dt[30];
//			sprintf(dt,"load 2 apply \n");
//			HAL_UART_Transmit(&huart1, &dt, strlen(dt), HAL_MAX_DELAY);
//		}
//
///*** @third apply button ***/
//		c= NULL ;
//		c = strstr((char *)tempbuff, (char *)loadPage_third_ApplyBuff);//FIRSTAPPLY BUTTON
//		if( c!= NULL)
//		{
//			*applyFlag = 3;
//			uint8_t dt[30];
//			sprintf(dt,"load 3 apply \n");
//			HAL_UART_Transmit(&huart1, &dt, strlen(dt), HAL_MAX_DELAY);
//		}
//
//
//
//		*displayUartFlag = 0 ;
//
//	}
//}







//void profile_load_update_values(uint8_t *nextFlag , uint8_t *backFlag, uint8_t *applyFlag, uint16_t *homePage_mmhg, uint8_t *homePage_setMin, uint8_t *homePage_setSec)
//{
//	static uint16_t e24_index = 0 ;
//	static uint16_t nextcounter = 0;
//	uint8_t eepromRead[31] = {0};
//	uint8_t data[30];
//
//
//	if(*nextFlag == 9 )
//	{
//		memset(eepromRead,0,sizeof(eepromRead));
////		ee24c64_Read_TestProfiles(eepromRead,sizeof(eepromRead), e24_index);
//		profile_load_update_values_uartFormate_8(&eepromRead, sizeof(eepromRead), 1);
//
//		memset(eepromRead,0,sizeof(eepromRead));
////		ee24c64_Read_TestProfiles(eepromRead,sizeof(eepromRead),e24_index+1);
//		profile_load_update_values_uartFormate_8(&eepromRead, sizeof(eepromRead), 2);
//
//		memset(eepromRead,0,sizeof(eepromRead));
////		ee24c64_Read_TestProfiles(eepromRead,sizeof(eepromRead),e24_index+2);
//		profile_load_update_values_uartFormate_8(&eepromRead, sizeof(eepromRead), 3);
//
//		e24_index = 3 ;
//
//		*nextFlag = 0;
//	}
//
//
//	if(*nextFlag == 1)
//	{
//		nextcounter++ ;
//
//		memset(eepromRead,0,sizeof(eepromRead));
////		ee24c64_Read_TestProfiles(eepromRead,sizeof(eepromRead), ( e24_index * nextcounter ) );
//		profile_load_update_values_uartFormate_8(&eepromRead, sizeof(eepromRead), 1);
//
//		memset(eepromRead,0,sizeof(eepromRead));
////		ee24c64_Read_TestProfiles(eepromRead,sizeof(eepromRead),( e24_index * nextcounter )+1 );
//		profile_load_update_values_uartFormate_8(&eepromRead, sizeof(eepromRead), 2);
//
//		memset(eepromRead,0,sizeof(eepromRead));
////		ee24c64_Read_TestProfiles(eepromRead,sizeof(eepromRead),( e24_index * nextcounter )+2 );
//		profile_load_update_values_uartFormate_8(&eepromRead, sizeof(eepromRead), 3);
//
//		*nextFlag = 0;
//	}
//
//	if(*backFlag == 1)
//	{
//		nextcounter-- ;
//		memset(eepromRead,0,sizeof(eepromRead));
////		ee24c64_Read_TestProfiles(eepromRead,sizeof(eepromRead), ( e24_index * nextcounter ) );
//		profile_load_update_values_uartFormate_8(&eepromRead, sizeof(eepromRead), 1);
//
//		memset(eepromRead,0,sizeof(eepromRead));
////		ee24c64_Read_TestProfiles(eepromRead,sizeof(eepromRead),( e24_index * nextcounter )+1 );
//		profile_load_update_values_uartFormate_8(&eepromRead, sizeof(eepromRead), 2);
//
//		memset(eepromRead,0,sizeof(eepromRead));
////		ee24c64_Read_TestProfiles(eepromRead,sizeof(eepromRead),( e24_index * nextcounter )+2 );
//		profile_load_update_values_uartFormate_8(&eepromRead, sizeof(eepromRead), 3);
//
////		*nextFlag = 0 ;
//
//		*backFlag = 0 ;
//	}
////	if(*applyFlag == 1)
////	{
////		memset(eepromRead,0,sizeof(eepromRead));
////		ee24c64_Read_TestProfiles(eepromRead,sizeof(eepromRead), ( e24_index * nextcounter ) );
////
////		uint16_t __mmhg = eepromRead[1] << 0 | eepromRead[2] << 8 ;
////		homePage_t->mmhg 		= 	__mmhg 			;
////		homePage_t->setMin 		=	eepromRead[3]	;
////		homePage_t->setSec 		=	eepromRead[4]	;
////
////		profile_page_change(HOME_PAGE_NUMBER);
////		push_load_value_homePage(homePage_t->mmhg , homePage_t->setMin, homePage_t->setSec, eepromRead );
////
////		*applyFlag = 0 ;
////
////	}
////
////	if(*applyFlag == 2)
////	{
////		memset(eepromRead,0,sizeof(eepromRead));
////		ee24c64_Read_TestProfiles(eepromRead,sizeof(eepromRead), ( e24_index * nextcounter ) + 1 );
////
////		uint16_t __mmhg = eepromRead[1] << 0 | eepromRead[2] << 8 ;
////		homePage_t->mmhg 		= 	__mmhg 			;
////		homePage_t->setMin 		=	eepromRead[3]	;
////		homePage_t->setSec 		=	eepromRead[4]	;
////		profile_page_change(HOME_PAGE_NUMBER);
////		push_load_value_homePage(homePage_t->mmhg , homePage_t->setMin, homePage_t->setSec, eepromRead );
////
////
////		*applyFlag = 0 ;
////	}
////	if(*applyFlag == 3)
////	{
////		memset(eepromRead,0,sizeof(eepromRead));
////		ee24c64_Read_TestProfiles(eepromRead,sizeof(eepromRead), ( e24_index * nextcounter ) + 2 );
////
////		uint16_t __mmhg = eepromRead[1] << 0 | eepromRead[2] << 8 ;
////		homePage_t->mmhg 		= 	__mmhg 			;
////		homePage_t->setMin 		=	eepromRead[3]	;
////		homePage_t->setSec 		=	eepromRead[4]	;
////
////		profile_page_change(HOME_PAGE_NUMBER);
////		push_load_value_homePage(homePage_t->mmhg , homePage_t->setMin, homePage_t->setSec, eepromRead );
////
////		*applyFlag = 0 ;
////	}
//
//}

/*
 * AUTH: BELOW CODE IS FOR AUTH PAGE
 */
void auth_page_read(uint8_t *displayUartFlag, uint8_t *authFlag, uint8_t *auth_UserName_t  , uint8_t *auth_PassWord_t, uint8_t *aFlag, uint8_t *pageflag)
{
	uint8_t tempbuff[MAX_MODEM_RX_BUFF] = {0};
	memset(tempbuff,0,sizeof(tempbuff));
	char 	*c 				= NULL ;
	uint8_t indexX = 0;
	uint8_t size = 0 ;
	uint8_t receivedBuff[10]= {0} ;

	uint8_t userNameAddr_buff[] = { USERNAME_ADDR1, USERNAME_ADDR2, 0x00} ;
	uint8_t PasswordAddr_buff[] = { USERPASSWORD_ADDR1, USERPASSWORD_ADDR2, 0x00} ;
	uint8_t loginButtonAddr[] = { 0x5A, 0xA5, 0x06, 0x83, LOGIN_B_ADDR1, LOGIN_B_ADDR2, 0x01, 0x00 };



	if(*displayUartFlag == 1)
	{
		HAL_Delay(100);
		for(int i=0; i< rxBufferCount ; i++)
		{
			tempbuff[i] = modemRxBuff[i];
		}
//		HAL_UART_Transmit(&huart3, modemRxBuff, rxBufferCount, HAL_MAX_DELAY);
		display_clearModemBuffer();
//#if 0
		c = NULL ;
		c = strstr( (char *)tempbuff, (char *)loginButtonAddr);
		if( c != NULL)
		{
			mem_w25qxxDriver_user_verify(auth_UserName_t, auth_PassWord_t, pageflag);

//			HAL_UART_Transmit(&huart3, auth_UserName_t, 10, HAL_MAX_DELAY);
//			HAL_UART_Transmit(&huart3, auth_PassWord_t, 10, HAL_MAX_DELAY);
			memset(tempbuff, 0, sizeof(tempbuff));
		}

		c= NULL ;
		c = strstr( (char *)tempbuff, (char *)userNameAddr_buff);
		if( c != NULL)
		{
			indexX =0;
			indexX = c - ( char *)tempbuff ;
			if(tempbuff[ indexX - 1 ]	==	0x83 )
			{

				size 		= tempbuff[ indexX + 4 ] ;
				indexX 	= indexX + 5 ;

				for(uint8_t i = 0; i< 11; i++)
				{
					if(i >= size)
					{
						auth_UserName_t[i] = '\0';
					}
					else
					{
						auth_UserName_t[i] = tempbuff[indexX] ;
						indexX++;
					}
				}
//				HAL_UART_Transmit(&huart3, &size, 1, HAL_MAX_DELAY);
//				HAL_UART_Transmit(&huart3, auth_UserName_t, 10, HAL_MAX_DELAY);
				memset(receivedBuff, 0, sizeof(receivedBuff));
				memset(tempbuff, 0, sizeof(tempbuff));
			}
//			++*aFlag ;
		}

		c = NULL ;
		c = strstr( (char *)tempbuff, (char *)PasswordAddr_buff);

		if( c != NULL)
		{
//			HAL_UART_Transmit(&huart1, c, sizeof(c), HAL_MAX_DELAY);
			indexX =0;
			indexX = c - ( char *)tempbuff ;

//			HAL_UART_Transmit(&huart1, &indexX, sizeof(indexX), HAL_MAX_DELAY);


			if(tempbuff[ indexX - 1 ]	==	0x83 )
			{

				size 		= tempbuff[ indexX + 4 ] ;
				indexX 	= indexX + 5 ;


				for(uint8_t i = 0; i< 11; i++)
				{
					if(i >= size)
					{
						auth_PassWord_t[i] = '\0';
					}
					else
					{
						auth_PassWord_t[i] = tempbuff[indexX] ;
						indexX++;
					}

				}



				uint8_t data2[30] ;
				memset(data2, 0, 30) ;

				data2[0] = 0x5A ;
				data2[1] = 0xA5 ;
				data2[2] = 3+strlen((char *)auth_PassWord_t) ;
				data2[3] = 0X82 ;
				data2[4] = 0x10 ;
				data2[5] = 0x0C ;
				for(uint8_t  j=6; j<6+strlen((char *)auth_PassWord_t); j++)
				{
					data2[j] = '*' ;
				}
				HAL_UART_Transmit(&huart1, data2, 6+strlen((char *)auth_PassWord_t)  , HAL_MAX_DELAY);
				HAL_UART_Transmit(&huart1, data2, 6+strlen((char *)auth_PassWord_t)  , HAL_MAX_DELAY);

				memset(tempbuff, 0, sizeof(tempbuff));
				memset(receivedBuff, 0, sizeof(receivedBuff));
			}

//			++*aFlag ;
		}

//#endif
		*displayUartFlag = 0 ;
	}

}


#if 0
void auth_page_read1(uint8_t *displayUartFlag, uint8_t *authFlag, ee24_Auth_Params_t *ee24_auth_params ,uint8_t *aFlag, uint8_t *pageflag)
{

	uint8_t tempbuff[MAX_MODEM_RX_BUFF] = {0};
	memset(tempbuff,0,sizeof(tempbuff));
	char 	*c 				= NULL ;
	uint8_t indexX = 0;
	uint8_t size = 0 ;
	uint8_t receivedBuff[10]= {0} ;

	uint8_t userNameAddr_buff[] = { 0x0f, 0xff} ;
	uint8_t PasswordAddr_buff[] = { 0x10, 0x3f} ;
	uint8_t loginButtonAddr[] = { 0x5A, 0xA5, 0x06, 0x83, 0x10, 0x80, 0x01, 0x00, 0x00 };

	userNameAddr_buff[ sizeof(userNameAddr_buff) - 1] = '\0'	;
	PasswordAddr_buff[ sizeof(PasswordAddr_buff) - 1] = '\0'	;
	loginButtonAddr[ sizeof(loginButtonAddr) -	1]	  =	'\0'	;


	if(*displayUartFlag == 1)
	{
		HAL_Delay(100);
		for(int i=0; i< rxBufferCount ; i++)
		{
			tempbuff[i] = modemRxBuff[i];
		}
//		HAL_UART_Transmit(&huart1, &tempbuff, rxBufferCount, HAL_MAX_DELAY);
		display_clearModemBuffer();

		c = NULL ;
		c = strstr( (char *)tempbuff, (char *)loginButtonAddr);
		if( c != NULL)
		{
			HAL_UART_Transmit(&huart1, &ee24_auth_params->username, strlen(ee24_auth_params->username), HAL_MAX_DELAY);
			HAL_UART_Transmit(&huart1, &ee24_auth_params->password, strlen(ee24_auth_params->password), HAL_MAX_DELAY);
//			if(*aFlag >= 3)
//			{
//				user_Verify(ee24_auth_params);
//				memset(tempbuff, 0, sizeof(tempbuff));
//				*aFlag =1;
//			}
//			else
//			{
//				*aFlag =1;
//				uint8_t dt[50];
//				sprintf(dt,"enter username or passowrd completely\n");
//				HAL_UART_Transmit(&huart1, &dt, strlen(dt), HAL_MAX_DELAY);
//			}
		}
//
		c = NULL ;
		c = strstr( (char *)tempbuff, (char *)PasswordAddr_buff);

		if( c != NULL)
		{

			indexX =0;
			indexX = c - ( char *)tempbuff ;
			if(tempbuff[ indexX - 1 ]	==	0x83 )
			{
				uint8_t dt[20];
				sprintf(dt,"its in PasswordAddr\n");
				HAL_UART_Transmit(&huart1, &dt, strlen(dt), HAL_MAX_DELAY);
				size 		= tempbuff[ indexX + 4 ] ;
				indexX 	= indexX + 5 ;
//				HAL_UART_Transmit(&huart1, &size, 1, HAL_MAX_DELAY);


				for(uint8_t i = 0; i< 10; i++)
				{
					if(i >= size)
					{
						ee24_auth_params->password[i] = '\0';
					}
					else
					{
						ee24_auth_params->password[i] = tempbuff[indexX] ;
						indexX++;
					}
				}
//				HAL_UART_Transmit(&huart1, &ee24_auth_params->username, strlen(ee24_auth_params->username), HAL_MAX_DELAY);
				HAL_UART_Transmit(&huart1, &ee24_auth_params->password, strlen(ee24_auth_params->password), HAL_MAX_DELAY);
				memset(receivedBuff, 0, sizeof(receivedBuff));
				memset(tempbuff, 0, sizeof(tempbuff));
			}

			++*aFlag ;
		}


		c= NULL ;
		c = strstr( (char *)tempbuff, (char *)userNameAddr_buff);
		if( c != NULL)
		{
			indexX =0;
			indexX = c - ( char *)tempbuff ;
			if(tempbuff[ indexX - 1 ]	==	0x83 )
			{
				uint8_t dt[20];
				sprintf(dt,"its in username\n");
				HAL_UART_Transmit(&huart1, &dt, strlen(dt), HAL_MAX_DELAY);
				size 		= tempbuff[ indexX + 4 ] ;
				indexX 	= indexX + 5 ;
//				HAL_UART_Transmit(&huart1, &size, 1, HAL_MAX_DELAY);


				for(uint8_t i = 0; i< 10; i++)
				{
					if(i >= size)
					{
						ee24_auth_params->username[i] = '\0';
					}
					else
					{
						ee24_auth_params->username[i] = tempbuff[indexX] ;
						indexX++;
					}
				}
				HAL_UART_Transmit(&huart1, &ee24_auth_params->username, strlen(ee24_auth_params->username), HAL_MAX_DELAY);
				memset(receivedBuff, 0, sizeof(receivedBuff));
				memset(tempbuff, 0, sizeof(tempbuff));
			}

			++*aFlag ;
		}





		*displayUartFlag = 0 ;
	}
}
#endif
