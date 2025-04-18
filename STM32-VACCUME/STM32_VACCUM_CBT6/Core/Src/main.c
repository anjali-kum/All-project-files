/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is  licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "fatfs.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "display.h"
#include "ds1307_i2c.h"
#include "myheader.h"
#include "authentication.h"
#include "rp203ThermalPrinterDriver.h"

//#include "fatfs_sd_card.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

//#include "ds1307_i2c.h"




//#include "profile_write.h"
//#include "profile_home.h"
//#include "profile_load.h"
//#include "mem_w25qxxDriver.h"




/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */



#define DS1307



#ifdef DS1307

uint8_t rtc_time[13] ;
uint8_t rtc_date[14] ;


//date
 uint8_t ds1307_date ;
 uint8_t ds1307_month ;
 uint16_t ds1307_year ;

//time
 uint8_t ds1307_hour ;
 uint8_t ds1307_minute ;
 uint8_t ds1307_second ;

#endif




/* ################## variables used for dwin ######## */
uint8_t dwin_send_array[50];
uint8_t dwin_nitificationFlag = 0;

/*##############################################*/


/*@brief this variables are used for control system*/
uint8_t displayUartFlag 	=	0	;
uint8_t authFlag = 5 ;

//authentication variables
//uint8_t readData[21] ;
uint8_t auth_UserName[11] ;
uint8_t auth_PassWord[11] ;

//home page variables
uint8_t homepage_setprofile_name[27] ;
int homePage_setmmhg	=	0	;
uint8_t homePage_setMin		=	0	;
uint8_t homePage_setSec		=	0	;


//VACCUME MANAGE_VARIABLES BELOW//
uint64_t currenttime  ;






/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

I2C_HandleTypeDef hi2c1;

SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi2;

TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */

//volatile unsigned long millis = 0;
volatile uint16_t rxBufferCount = 0;
volatile uint8_t uni_sec = 0  ;

//uint8_t test[300];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_SPI2_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_ADC1_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM3_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


static void profile_page_change1(uint8_t value)
{

	uint8_t nextPage_buff[] = { 0x5A, 0xA5, 0x07, 0x82, 0x00, 0x84, 0x5A, 0x01,0x00,value } ;
	HAL_UART_Transmit(&huart1, nextPage_buff, 10, HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart1, nextPage_buff, 10, HAL_MAX_DELAY);

}




static void get_home_page_addr(uint16_t targetAddr , uint8_t value)
{
	memset(dwin_send_array, 0, sizeof(dwin_send_array)); // resetting or clearing the dwin_send_array

	dwin_send_array[0]		= 	0x5A 	;
	dwin_send_array[1]		= 	0xA5	;
	dwin_send_array[2]		=	0x05	;
	dwin_send_array[3]		=	0x82	;
	dwin_send_array[5]		=	targetAddr>>0	;//shift 0 bits to right gets lower bytes as LSB
	dwin_send_array[4]		=	targetAddr>>8 	;// shift 8 bits to right Gets higher bytes as MSB

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
	sprintf( (char *)&arr,"%02d",value);


	for(uint8_t i= 6 ; i<8 ;i++)
	{
		dwin_send_array[i] = arr[j];
		j++;
	}
}


static void getStringOfDateNdTime(uint16_t targetAddr , uint8_t *string, uint8_t stringLen)
{
	memset(dwin_send_array, 0, sizeof(dwin_send_array));

	dwin_send_array[0]		= 	0x5A 	;
	dwin_send_array[1]		= 	0xA5	;
	dwin_send_array[2]		=	3 + stringLen ;
	dwin_send_array[3]		=	0x82	;
	dwin_send_array[5]		=	targetAddr>>0	;//shift 0 bits to get LSB
	dwin_send_array[4]		=	targetAddr>>8 	;// shift 8 bits to get MSB

//	HAL_UART_Transmit(&huart3, string, (stringLen), HAL_MAX_DELAY);

	for(uint8_t i= 6, j=0 ; j<stringLen ; i++,j++ )
	{
		dwin_send_array[i] = string[j] ;
//		HAL_UART_Transmit(&huart3, &dwin_send_array[i], 1, HAL_MAX_DELAY);
	}
//	HAL_UART_Transmit(&huart1, dwin_send_array, (6+stringLen), HAL_MAX_DELAY);
}

void getTime()
{
#ifdef DS1307

	  ds1307_date = DS1307_GetDate();
	  ds1307_month = DS1307_GetMonth();
	  ds1307_year = DS1307_GetYear();
	  ds1307_hour = DS1307_GetHour();
	  ds1307_minute = DS1307_GetMinute();
	  ds1307_second = DS1307_GetSecond();
	  sprintf( (char *)&rtc_date, "%02d/%02d/%04d", ds1307_date, ds1307_month, ds1307_year) ;
	  sprintf( (char *)&rtc_time, "%02d:%02d:%02d\n", ds1307_hour, ds1307_minute, ds1307_second) ;
//	  HAL_UART_Transmit(&huart3, rtc_time, strlen(rtc_time), HAL_MAX_DELAY);

#endif
}

static void update_date_nd_time()
{

#ifdef DS1307
	getTime() ;

	//send date
	getStringOfDateNdTime(UNIVERSAL_DATE_ADDR, (uint8_t *)rtc_date, strlen((const char*)rtc_date));
	HAL_UART_Transmit(&huart1, dwin_send_array, (6+strlen((char *)rtc_date)), HAL_MAX_DELAY);

	//send time
	getStringOfDateNdTime(UNIVERSAL_TIME_ADDR, (uint8_t *)rtc_time,( (uint8_t )strlen((const char*)rtc_time)));
	HAL_UART_Transmit(&huart1, dwin_send_array, (6+strlen((char *)rtc_date)), HAL_MAX_DELAY);
#endif

}




/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */



  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  MX_SPI2_Init();
  MX_USART1_UART_Init();
  MX_USART3_UART_Init();
  MX_FATFS_Init();
  MX_ADC1_Init();
  MX_I2C1_Init();
  MX_TIM3_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

  HAL_TIM_Base_Start_IT(&htim3) ;
  HAL_Delay(100) ;


	HAL_UART_Receive_IT(&huart1,&modemRxBuff[rxBufferCount],1);
	HAL_Delay(100) ;

	display_clearModemBuffer();

	 mem_W25qxx_Driver_init( (uint32_t *)SPI2_CS_GPIO_Port, SPI2_CS_Pin);

#ifdef DS1307
  /*** ds1307 code init below ***/

//	ds1307_second 	= 	mem_W25qxx_Driver_ReadByte(RTC_ADDR) ;
//	ds1307_minute	=	mem_W25qxx_Driver_ReadByte(RTC_ADDR+1) ;
//	ds1307_hour		=	mem_W25qxx_Driver_ReadByte(RTC_ADDR+2) ;
//	ds1307_date		=	mem_W25qxx_Driver_ReadByte(RTC_ADDR+3) ;
//	ds1307_month	=	mem_W25qxx_Driver_ReadByte(RTC_ADDR+4) ;
//
//	ds1307_year		+=	(mem_W25qxx_Driver_ReadByte(RTC_ADDR+5) <<8 ) ;
//	ds1307_year		+=	mem_W25qxx_Driver_ReadByte(RTC_ADDR+6)  ;

  	/* Start DS1307 timing. Pass user I2C handle pointer to function. */
	DS1307_Init(&hi2c1);
//	/* To test leap year correction. */
//	DS1307_SetTimeZone(+8, 00);
//	DS1307_SetDate(ds1307_date);
//	DS1307_SetMonth(ds1307_month);
//	DS1307_SetYear(ds1307_year);
//	DS1307_SetDayOfWeek(4);
//	DS1307_SetHour(ds1307_hour);
//	DS1307_SetMinute(ds1307_minute);
//	DS1307_SetSecond(ds1307_second);

#endif
//	mem_W25qxx_Driver_chipErase();
//	HAL_Delay(10000) ;

		uint8_t debug1[30];
	  sprintf((char *)&debug1, "application starts here\n");
//	  HAL_UART_Transmit(&huart3, debug1, strlen( (char *)debug1), HAL_MAX_DELAY);

	  /*** setting vaccum pressure and sucker pins high by defualt ***/
	  HAL_ADC_Start(&hadc1);

	  HAL_GPIO_WritePin(VACCUM_PRESSURE_GPIO_Port,VACCUM_PRESSURE_Pin,1);

	  HAL_GPIO_WritePin(VACCUM_SUCKER_GPIO_Port,VACCUM_SUCKER_Pin,1);


	  HAL_GPIO_WritePin(VACCUM_MOTOR_GPIO_Port,VACCUM_MOTOR_Pin,1);





	  /*** change DWIN display page it logo page ***/
	  	DWIN_PAGE_CHANGE(LOGO_PAGE_NUMBER);
	  HAL_Delay(1000) ;

	  /*** changing dwin display page to login page ***/
	  DWIN_PAGE_CHANGE(AUTH_PAGE_NUMBER);  //AUTHENTICATION PAGE ADDRESS
	  HAL_Delay(500) ;



	#ifdef FULL_FLASH_ERASE_AND_WRITE_ADMIN_CRED

	  mem_W25qxx_Driver_chipErase();
	  uint8_t readData[21] ;
	  uint8_t auth_UserName[11] ;
	  uint8_t auth_PassWord[11] ;
	  uint8_t array[21] ;
	  mem_strcpy(0, &array, 10, &auth_UserName, 6);
	  mem_strcpy(11, &array, 20, &auth_PassWord, 6);
	  mem_W25qxx_Driver_writeBytes(ADMIN_ADDR, array, 21) ;
	  mem_W25qxx_Driver_writeBytes(ADMIN_ADDR, &auth_PassWord, strlen(auth_PassWord)) ;
	  mem_W25qxx_Driver_ReadBytes(ADMIN_ADDR, readData,21);
	  for(uint8_t i=0; i<21; i++)
	  {
		  HAL_UART_Transmit(&huart1, &readData[i], 1, HAL_MAX_DELAY);
	  }
	#endif


	//#ifdef DELETE_USER_FUN

//	////  TODO:verifying delete future

//	uint8_t array[30] ;
//	uint8_t auth_UserName_t[] ="ADMIN" ;
//	uint8_t auth_PassWord_t[] = "ADMIN" ;
//
//
//    mem_W25qxx_Driver_sectorErase(( ADMIN_ADDR ));
//    HAL_Delay(1000) ;
////
////	mem_W25qxx_Driver_writeBytes(ADMIN_ADDR, array, 21) ;
//	mem_w25qxxDriver_Admin_write(auth_UserName_t, auth_PassWord_t );
//	uint8_t array1[23] ;
//	mem_W25qxx_Driver_ReadBytes(ADMIN_ADDR, array1, sizeof(array1)) ;
//	HAL_UART_Transmit(&huart3, array1, 22,1000) ;

	#ifdef DELETE_USER_FUN

	mem_W25qxx_Driver_chipErase();
//	mem_w25qxxDriver_Admin_write(auth_UserName_t, auth_PassWord_t );

	  counter = 1 ;
	  mem_w25qxxDriver_supervisor_write(auth_UserName_t, auth_PassWord_t, &counter );
	  counter = 2 ;
	  mem_w25qxxDriver_supervisor_write(auth_UserName_t, auth_PassWord_t, &counter );

	  change password for admin

	  mem_W25qxx_Driver_chipErase();
	  mem_W25qxx_Driver_sectorErase(SWAP_SECTOR_ADDR);

	  mem_W25qxx_Driver_sectorErase(TESTPROFILE_ADDR);

	#endif

	  homePage_setMin = 0 ;
	  homePage_setSec = 0 ;
	  homePage_setmmhg = 0 ;


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */


  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	switch(current_page)
	{
		case ( AUTHENTICATION_PAGE ) :
		{
//			mem_W25qxx_Driver_sectorErase(MMHGGAUGEREFERENCE_ADDR);
//			HAL_Delay(200) ;

//			uint8_t *gaugeRef[12] ;
//			memset(gaugeRef, 0, sizeof(gaugeRef)) ;
//			mem_w25qxxDriver_Read_mmhg_GAUGEval( (uint8_t*)gaugeRef, sizeof(gaugeRef)) ;
//			uint8_t dt[30] ;
//			sprintf(dt, "#@dt: %d\n", atoi((const char *)gaugeRef) );
//			HAL_UART_Transmit(&huart3, dt, strlen(dt), HAL_MAX_DELAY) ;


			auth_init(&displayUartFlag, &authFlag,auth_UserName,auth_PassWord );//authFlag = 5 by defualt

			  if( (authFlag == 1) || (authFlag == 2) || (authFlag == 3) || (authFlag == 4))
			  {
				  if(authFlag == 4)//USER
				  {
					  profile_page_change1(ADMIN_HOME_START_PAGE_NUMBER);
					  current_page = MUITI_ADMIN_PROFILE ;
					  //CHAGNE:20-05
					  adminPageDirectionFlag = ADMIN_HOME_START_PAGE_NUMBER ;

				  }

				  if(authFlag == 3)//USER
				  {
					  profile_page_change1(HOME_PAGE_NUMBER);
					  current_page = USER_PRFILE ;
					  //CHAGNE:20-05
					  supervisorPageDirectionFlag = HOME_PAGE_NUMBER;
				  }

				  else if(authFlag == 2)//SUPER_USER
				  {
					  profile_page_change1(SUPERVISOR_HOME_START_PAGE_NUMBER);
					  current_page = SUPERVISOR_PROFILE ;
					  //CHAGNE:20-05
					  supervisorPageDirectionFlag= SUPERVISOR_HOME_START_PAGE_NUMBER ;
				  }

				  else if(authFlag  == 1)//ADMIN
				  {
					  profile_page_change1(ADMIN_HOME_START_PAGE_NUMBER);
				  	  current_page  = ADMIN_PROFILE ;
//				  	//CHAGNE:20-05
//				  	  adminPageDirectionFlag = ADMIN_HOME_START_PAGE_NUMBER ;
				  }

					 uint8_t dt[30] ;
					 dt[0] = 0x5A ;
					 dt[1] = 0xA5 ;
					 dt[2] = 0x05 ;
					 dt[3] = 0x82 ;
					 dt[4] = HOME_PAGE_PROFILENAME_ADDR>>8 ;// higher bytes in MSB
					 dt[5] = HOME_PAGE_PROFILENAME_ADDR>>0 ;// Lower  bytes in LSB
					 dt[6] = 0x00 ;
					 dt[7] = 0x00 ;
					 HAL_UART_Transmit(&huart1, dt, 8, HAL_MAX_DELAY) ;

					 dt[4] = HOME_PAGE_SET_VACCUM_ADDR>>8 ;
					 dt[5] = HOME_PAGE_SET_VACCUM_ADDR>>0 ;
					 HAL_UART_Transmit(&huart1, dt, 8, HAL_MAX_DELAY) ;

					 dt[4] = HOME_PAGE_SET_TIME_SEC_ADDR>>8 ;
					 dt[5] = HOME_PAGE_SET_TIME_SEC_ADDR>>0 ;
					 HAL_UART_Transmit(&huart1, dt, 8, HAL_MAX_DELAY) ;

					 dt[4] = HOME_PAGE_SET_TIME_MIN_ADDR>>8 ;
					 dt[5] = HOME_PAGE_SET_TIME_MIN_ADDR>>0 ;
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

					 homePage_setMin = 0 ;
					 homePage_setSec = 0 ;
					 homePage_setmmhg = 0 ;

					uint8_t fixedmmhg[5] ;
					mem_w25qxxDriver_read_fixed_mmhg(fixedmmhg) ;
//					uint8_t data[20] ;
//					sprintf(data,"d: %s\n", fixedmmhg) ;
//					HAL_UART_Transmit(&huart3, data, strlen(data), HAL_MAX_DELAY);

					uint8_t data2[30] ;
					data2[0] = 0x5A ;
					data2[1] = 0xA5 ;
					data2[2] = 0x08 ;
					data2[3] = 0X82 ;
					data2[4] = 0x25 ;
					data2[5] = 0x62 ;
					for(uint8_t i=0, j=6; i<4;i++,j++)
					{
						data2[j] = fixedmmhg[i] ;
					}
					HAL_UART_Transmit(&huart1, data2, 9, HAL_MAX_DELAY);
//					HAL_UART_Transmit(&huart3, data2, 9, HAL_MAX_DELAY);


			  }
			break ;
		}
		case (ADMIN_PROFILE) :
		{
			authentication_admin_init(&displayUartFlag);
			HAL_Delay(10) ;
			break ;
		}

		case (SUPERVISOR_PROFILE) :
		{
			authentication_SuperUser_init(&displayUartFlag);
			HAL_Delay(10) ;
			break ;
		}

		case (USER_PRFILE) :
		{
			authentication_SuperUser_init(&displayUartFlag);
			HAL_Delay(10) ;
			break ;
		}

		case (MUITI_ADMIN_PROFILE) :
		{
			authentication_admin_init(&displayUartFlag);
			break ;
		}

		default:
		{
			break ;
		}

	}

  }

  HAL_ADC_Stop(&hadc1);

  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV4;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 8000-1;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 1000-1;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 9600;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, VACCUM_PRESSURE_Pin|VACCUM_SUCKER_Pin|VACCUM_MOTOR_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : SPI1_CS_Pin */
  GPIO_InitStruct.Pin = SPI1_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(SPI1_CS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : VACCUM_PRESSURE_Pin VACCUM_SUCKER_Pin VACCUM_MOTOR_Pin SPI2_CS_Pin */
  GPIO_InitStruct.Pin = VACCUM_PRESSURE_Pin|VACCUM_SUCKER_Pin|VACCUM_MOTOR_Pin|SPI2_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */


//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
//{
//
//
//	if( htim->Instance == TIM2 )
//	{
//		currenttime++;
//
//	}
//
//  /* NOTE : This function should not be modified, when the callback is needed,
//            the HAL_TIM_PeriodElapsedCallback could be implemented in the user file
//   */
//}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
	if( (authFlag == 1) || (authFlag == 2) || ( authFlag == 3 ) || ( authFlag == 4 ) )
	{
		if(changeTimeIndex == 0 )
		{

		   update_date_nd_time();
		}
		 uni_sec++ ;
		 display_runTimeUpdateToDisplay(runstopFlag, uni_sec);
	}

}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

  if(huart == &huart1){
	  displayUartFlag =1;

	if(rxBufferCount > MAX_MODEM_RX_BUFF){
		rxBufferCount = 0;
	}
    modemRxBuff[rxBufferCount] = huart->Instance->DR;
//    HAL_UART_Transmit(&huart3, modemRxBuff, 1, 1);
    rxBufferCount++;



  }
    HAL_UART_Receive_IT(&huart1,&modemRxBuff[rxBufferCount],1);
//    HAL_UART_Transmit(&huart1, &modemRxBuff, strlen(modemRxBuff), 1);

}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
