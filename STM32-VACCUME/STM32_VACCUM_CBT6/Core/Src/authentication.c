/*
 * authentication.c
 *
 *  Created on: Mar 8, 2024
 *      Author: navee
 */

#include "authentication.h"
#include <string.h>
#include <stdio.h>

/*** @@AUTHENTICATION VAR ***/
uint8_t aFlag = 1;
uint8_t auth_flag_switch_page = 0 ;

/*** @@ UNIVERSAL VAR FOR ALL AUTHENTICATION***/
uint8_t loadpage_nextFlag 	= 9 ;//9 IS FOR INITIAL PRINT LOAD PROFILE VALUE INSTANTLY WHEN ENTER TO LOAD PAGE
uint8_t loadpage_backFlag 	= 0 ;
uint8_t loadpage_applyFlag 	= 0;
uint8_t loadpage_deleteFlag = 0;

/*** @@ADMIN-PAGE VAR***/
uint8_t admin_supervisor_username[11] ;
uint8_t admin_supervisor_password[11] ;
uint8_t adminPageDirectionFlag 		= ADMIN_MAIN_PAGE ;

uint8_t admin_changePassword_NewPassword[11] ;
uint8_t admin_changePassword_PresentPassword[11] ;


/*** @@SUPERVISOR-PAGE VAR***/
uint8_t supervisor_username[11] ;
uint8_t supervisor_password[11] ;
uint8_t supervisorPageDirectionFlag	= SUPERVISOR_MAIN_PAGE ;
uint8_t supervisor_usercounter 		= 0;

uint8_t sVP_M_ADD_TSTP_Srch_var  	= 0 ;
uint8_t sVP_M_ADD_TSTP_Pname_var[26] ;
uint16_t sVP_M_ADD_TSTP_MMHG_var 	= 0 ;
uint8_t sVP_M_ADD_TSTP_MIN_var 		= 0 ;
uint8_t sVP_M_ADD_TSTP_SEC_var 		= 0 ;
uint8_t sVP_TestProfileCounter 		= 0;

//controller var's
volatile float c_kpa				=	0 ;
float c_mmhg						=	0 ;
volatile float c_adc				=	0 ;
volatile float c_volt				=	0 ;
volatile uint8_t c_min				=	0 ;
volatile uint8_t c_sec				=	0 ;
volatile uint64_t c_runtime			=	0 ;
 uint8_t c_runstopButton	= 	0 ;

//pid param's
float pid_kp			= 2  ;
float pid_ki 			= 0.01175 ;
float pid_kd 			= 0.05 ;
float pid_dt			= 0 ;
float pid_i 			= 0.0f ;
float pid_error			= 0.0f ;
float pid_prev_error	= 0.0f ;
float pid_ut			=0.0f ;

//home profile var's
uint8_t controlLogicFlag 	=	1 ;
uint64_t lastTime 			=	0 ;
uint8_t runstopFlag 		= 	2 ;
uint8_t timeoutFlag 		= 	0 ;
uint8_t timerAcceptFlag 	= 	0 ;

uint16_t weight_before_test_val 	= 0 ;
uint16_t weight_after_test_val 		= 0 ;

uint8_t pid_logic_pin = 0 ;





/*#######################################################################################*/
/*** @CONTROLLER FUNCTIONS***/
/*#######################################################################################*/

void pid_core_functionality(uint8_t runStopFlag, float ut, uint8_t *control, float currentMmhg, float targetMmhg, uint8_t *displayUartFlag)
{

	if( runStopFlag == 1)
	{

		if(pid_logic_pin == 0)
		{
			if( currentMmhg <= targetMmhg+1 )
			{

				HAL_GPIO_WritePin(VACCUM_SUCKER_GPIO_Port,VACCUM_SUCKER_Pin,0);
				HAL_GPIO_WritePin(VACCUM_PRESSURE_GPIO_Port,VACCUM_PRESSURE_Pin,1);
				HAL_GPIO_WritePin(VACCUM_MOTOR_GPIO_Port,VACCUM_MOTOR_Pin,0);
			}
			if(currentMmhg > targetMmhg+1){
				pid_logic_pin =1 ;
			}
		}
		else if( pid_logic_pin == 1)
		{
			if( currentMmhg <= targetMmhg-2 )
			{
				HAL_GPIO_WritePin(VACCUM_SUCKER_GPIO_Port,VACCUM_SUCKER_Pin,0);
				HAL_GPIO_WritePin(VACCUM_PRESSURE_GPIO_Port,VACCUM_PRESSURE_Pin,1);
				HAL_GPIO_WritePin(VACCUM_MOTOR_GPIO_Port,VACCUM_MOTOR_Pin,0);
			}
			else
			{

			  HAL_GPIO_WritePin(VACCUM_PRESSURE_GPIO_Port,VACCUM_PRESSURE_Pin,1);
			  HAL_GPIO_WritePin(VACCUM_MOTOR_GPIO_Port,VACCUM_MOTOR_Pin,1);
			  HAL_Delay (5);

			  if( currentMmhg >= targetMmhg+10 )
			  {
				  HAL_GPIO_WritePin(VACCUM_PRESSURE_GPIO_Port, VACCUM_PRESSURE_Pin, 0);
				  HAL_Delay(500) ;
				  HAL_GPIO_WritePin(VACCUM_PRESSURE_GPIO_Port, VACCUM_PRESSURE_Pin, 1);

			  }


			}
		}

	}
	else if( ( runStopFlag == 2 ) )//STOP BUTTON
	{

		HAL_GPIO_WritePin(VACCUM_SUCKER_GPIO_Port,VACCUM_SUCKER_Pin,1);
		HAL_GPIO_WritePin(VACCUM_PRESSURE_GPIO_Port,VACCUM_PRESSURE_Pin,0);
		HAL_GPIO_WritePin(VACCUM_MOTOR_GPIO_Port,VACCUM_MOTOR_Pin,1);
		pid_logic_pin = 0 ;
		HAL_Delay (5);
	}


}

uint32_t controller_calculateFilteredValue(uint32_t *data, uint8_t dataSize, float prevAdc)
{
	uint32_t filterValue =0;
	uint8_t count =0;

	for(uint8_t i=0 ; i<200 ;i++)
	{
		filterValue = filterValue + data[i];
		count++ ;
	}
	filterValue = filterValue /count ;



	uint32_t op = (prevAdc + filterValue )/2 ;

	return  op ;
}

float controller_calculateFilteredValueFloat(float arr[],uint8_t dataSize, float prevVal)
{
	float val =0;
	uint8_t counter =0;

	for(uint8_t i=0; i<dataSize ; i++)
	{

		val = val + arr[i];
		counter++;
	}
	val = val/counter ;

	return ( val + prevVal )/2 ;
}

float PID( float pid_error)
{
	float P = pid_kp * ( pid_error ) ;

	  float i =pid_i * pid_ki * pid_dt * ( pid_error );
//	  float D = kd * (previouserror);

	  pid_prev_error  = pid_error ;

	 return P+i ;

}

void controller_getCalculated_PID_value(uint32_t *arr)
{

	c_adc = controller_calculateFilteredValue(arr,200, c_adc);


	c_volt = c_adc /ADC_TO_VOLTAGE ;

	c_kpa 	= ( ( c_volt + 0.3135 )/(0.0297) ) ;

//	float temp_mmhg  =  ( c_kpa ) * 7.50062 ;
//	float temp_mmhg  = 1.272507955 *(  ( c_kpa ) * 7.50062 )  + (-161.1633971 ) ; //FOR mBAR
//	float temp_mmhg  = (0.941341178) *( ( c_kpa ) * (7.50062) )  + (71.41085978) ; //FOR mmHG TEST 1
//	float temp_mmhg  = (0.939505075) *( ( c_kpa ) * (7.50062) )  + (84.68013413) ; //FOR mmHG TEST 2
//	float temp_mmhg  = (1.021959412) *( ( c_kpa ) * (7.50062) )  + (60.0681908) ; //FOR mmHG TEST 3 new sensor
//	float temp_mmhg  = (1.081582641) *( ( c_kpa ) * (7.50062) )  + (57.84124308) ; //FOR mmHG TEST 4 new sensor

//	float temp_mmhg  = (0.950147802) *( ( c_kpa ) * (7.50062) )  + (68.68414853) ; //BOARD-1 DELIVERED ON 05/07/24

	float temp_mmhg  = (0.946591008) *( ( c_kpa ) * (7.50062) )  + (61.92966016) ; //BOARD-2 DELIVERED ON 05/07/24

	c_mmhg  = 760 - ( (temp_mmhg) ) ;


	pid_error =	homePage_setmmhg - c_mmhg;

	float _ut = PID( pid_error) ;
	pid_ut = _ut	;




}

void pid_init(uint8_t *PageDirectionFlag, uint8_t *displayUartFlag)
{
	currenttime = c_sec;
	pid_dt = currenttime - lastTime ;
	lastTime = currenttime ;


	uint32_t adc[200] ;
	memset( adc, 0, sizeof(adc));
    for(uint8_t i=0 ; i<sizeof(adc) / sizeof(adc[0]) ; i++)
    {
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1,1);
    	adc[i] = HAL_ADC_GetValue(&hadc1) ;
    }

    controller_getCalculated_PID_value(adc);
//    HAL_Delay(100) ;

//	  uint8_t data[60];
//	  sprintf(data, "LP Time : %.2f ; ADC : %.2f ; volt :%.2f, kPa : %.2f : mmhg : %.2f : error : %.2f, UT :%.2f, target :%d,  Min:%d , sec:%d \n ",	pid_dt, c_adc, c_volt, c_kpa, c_mmhg, pid_error, pid_ut, homePage_setmmhg, homePage_setMin, homePage_setSec);
//    HAL_UART_Transmit(&huart3, &data, strlen(data), HAL_MAX_DELAY);
//
	    uint8_t *gaugeRef[12] ;
		memset(gaugeRef, 0, sizeof(gaugeRef)) ;
		mem_w25qxxDriver_Read_mmhg_GAUGEval( (uint8_t *)gaugeRef, sizeof(gaugeRef)) ;
		int dummymmhg = atoi((const char *)&gaugeRef) ;

//		  memset(data,0,sizeof(data));
//		  sprintf(data, ", dt:%d \n", dummymmhg ) ;
//		  HAL_UART_Transmit(&huart3, &data, strlen(data), HAL_MAX_DELAY);

//		int target = 0 ;
//		if( mem_W25qxx_Driver_ReadByte(CALIBRATION_DEVICE_ADDORSUBSTRACTVALUE_INDICATION) == 1)
//		{
//			target = homePage_setmmhg - dummymmhg ;
//		}

	  if( ( (homePage_setmmhg+dummymmhg) <= c_mmhg) && (timerAcceptFlag == 0) && (runstopFlag == 1) )
	  {
		  timerAcceptFlag 			= 	1	;
		  c_sec						=	0	;
		  c_min						=	0	;
		  currenttime				=	0	;
		  c_runtime					=	0	;
		  uni_sec= 0;

//		  uint8_t debug_buff1[30];
//		  memset(debug_buff1,0,sizeof(debug_buff1));
	  }



	  if( (homePage_setSec == c_sec) && (homePage_setMin== c_min) && (timerAcceptFlag == 1) )
	  {
		  _c_mmhg = c_mmhg ;
		  uint8_t debug_buff1[30];
		  memset(debug_buff1,0,sizeof(debug_buff1));
//		  sprintf(debug_buff1,"min :%d, sec:%d, HP_Min %d, HPsec %d,\n", c_min, c_sec, homePage_setMin, homePage_setSec);
//		  HAL_UART_Transmit(&huart3, &debug_buff1, strlen(debug_buff1), HAL_MAX_DELAY);
		  runstopFlag = 2 ;
		  timerAcceptFlag = 0 ;
		  quickTestFlag = 3 ;
		  *PageDirectionFlag = PRINT_RESULTS ;
//		  testResultFlag = 1 ;
//		  homePage_setmmhg = 0 ;

		  if( (validationTestVar > 0) && (validationTestVar < 4) )
		  {
				profile_page_change(SUPERVISOR_MENU_VALIDATION_STOPPOPUP_ADDR);
				*PageDirectionFlag =	SUPERVISOR_MENU_VALIDATION_STOPPOPUP_ADDR ;
		  }
		  else
		  {
			  if( (authFlag == 1) || (authFlag == 4)  )
			  {
				  DWIN_PAGE_CHANGE(ADMIN_HOME_START_PAGE_NUMBER) ;
			  }
			  else if(authFlag == 2)
			  {
				  DWIN_PAGE_CHANGE(ADMIN_HOME_START_PAGE_NUMBER) ;
			  }
			  else if(authFlag == 3)
			  {
				  DWIN_PAGE_CHANGE(ENDUSER_START_PAGE) ;
			  }

			  testdoneRnotFlag = 2 ;

			   uint8_t dbg[50] ;

			  memset(dbg, 0, sizeof(dbg)) ;
			  uint8_t data1[] = "Print results?" ;

			  DWIN_PAGE_CHANGE1(dbg, NOTIFICATION_W_YES_R_NO_PAGE_NUMBER) ;
			  DWIN_PAGE_CHANGE1(dbg, NOTIFICATION_W_YES_R_NO_PAGE_NUMBER) ;

			  memset(dbg, 0, sizeof(dbg)) ;
			  POPUP_NOTIFICATION(dbg, UNIVERSAL_NOTIFICATION_ADDR, data1, strlen( (char *) data1) ) ;
			  POPUP_NOTIFICATION(dbg, UNIVERSAL_NOTIFICATION_ADDR, data1, strlen( (char *) data1) ) ;
		  }


		   rp203ThermalPrinterDriver_endHour = ds1307_hour ;
		   rp203ThermalPrinterDriver_endMin = ds1307_minute ;
		   rp203ThermalPrinterDriver_endSec = ds1307_second ;

	  }
	  //CHECK TIME BEFORE GOING FOR PID CONTROL

	  /* @breif this condition is for control pressure based on RUN & STOP Command */
	  pid_core_functionality(runstopFlag, pid_ut, &controlLogicFlag,  c_mmhg, (homePage_setmmhg + dummymmhg) , displayUartFlag);


	 if( (*PageDirectionFlag == ADMIN_STOP_PAGE) || ( *PageDirectionFlag == SUPERVISOR_MENU_VALIDATION_ADDR) )
	 {
		  /* @Display function call start from here */
		  display_mmhgValueUpdateToDisplay( (uint64_t)(c_mmhg-dummymmhg), runstopFlag);
		  HAL_Delay(100) ;

	 }



}

/***...........................@CONTROLLER FUNCTIONS END@***................................................................*/


/*#######################################################################################*/
/*** @AUTHENTICATION FUNCTIONS***/
/*#######################################################################################*/


void authentication_admin_init(uint8_t *displayUartFlag)
{
	authentication_AdminPage_Read(displayUartFlag,  &adminPageDirectionFlag, &loadpage_nextFlag, &loadpage_backFlag, &loadpage_applyFlag, &c_runstopButton, &loadpage_deleteFlag);
	if(homePage_setmmhg >0)
	{
		pid_init(&adminPageDirectionFlag, displayUartFlag) ;
	}

}

void authentication_SuperUser_init(uint8_t *displayUartFlag)// SUPERVISOR & USER
{
	authentication_SupervisorPage_Read(displayUartFlag,&supervisorPageDirectionFlag, &loadpage_nextFlag, &loadpage_backFlag, &loadpage_applyFlag, &c_runstopButton, &loadpage_deleteFlag);
	if(homePage_setmmhg >0)
	{
		pid_init(&supervisorPageDirectionFlag, displayUartFlag);
	}

}


void auth_init(uint8_t *displayUartFlag, uint8_t *authFlag, uint8_t *auth_UserName_t, uint8_t *auth_PassWord_t)
{

	if(*authFlag == 5)
	{
		auth_page_read(displayUartFlag, &auth_flag_switch_page, auth_UserName_t, auth_PassWord_t,  &aFlag, authFlag);
	}

}
