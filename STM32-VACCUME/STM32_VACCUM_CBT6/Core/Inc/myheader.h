 /*
 * myheader.h
 *
 *  Created on: Dec 15, 2023
 *      Author: navee
 */

#ifndef INC_MYHEADER_H_
#define INC_MYHEADER_H_

//#DEFINES
#include "main.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>


#define min(a, b) ((a) < (b) ? (a) : (b))

//BUFFRES
#define MAX_MODEM_RX_BUFF 250
#define MAX_DEBUG_TX_BUFF 50

/***##################################################***/
//Note: EXTERN VARIABLES USED FOR DWIN

extern uint8_t dwin_nitificationFlag;

extern volatile uint8_t uni_sec ;

extern uint8_t changeTimeIndex ;

extern	I2C_HandleTypeDef hi2c1;

/***##################################################***/

/*--------------------------------------------------------------------------------------------------------------------------*/

/***##################################################***/
//Note: DEBUG DEFINE'S HERE
//#define ERASE_SUPERVISOR
//#define ERASE_ENDUSER
//#define ERASE_TESTPROFILE
//#define ERASE_MULTIADMIN
//#define SHOW_ADMIN_USERNAME_PASSWORD

/***##################################################***/

/*--------------------------------------------------------------------------------------------------------------------------*/
/***##################################################***/
//Note: PERIFERAL DEFINE
#define SD_CARD

/***##################################################***/

#define PRINT_RESULTS 255
/*--------------------------------------------------------------------------------------------------------------------------*/

/***##################################################***/
//Note: EXTERNS FROM MAIN.C

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;

extern volatile uint16_t rxBufferCount;
extern uint64_t currenttime	;
//time
extern  uint8_t ds1307_hour ;
extern  uint8_t ds1307_minute ;
extern  uint8_t ds1307_second ;
//date
extern  uint8_t ds1307_date ;
extern  uint8_t ds1307_month ;
extern  uint16_t ds1307_year ;

extern uint8_t auth_UserName[11] ;
extern uint8_t auth_PassWord[11] ;

/***##################################################***/

/*--------------------------------------------------------------------------------------------------------------------------*/

/***##################################################***/
//Note: RP203 THERMAL PRINTER DRIVER
extern uint8_t rp203ThermalPrinterDriver_startTime[10] ;
extern uint8_t rp203ThermalPrinterDriver_endTime[10] ;

extern uint8_t rp203ThermalPrinterDriver_TestDate[15] ;

/***##################################################***/

/*--------------------------------------------------------------------------------------------------------------------------*/
//PAGE:LOAD PAGE
/***##################################################***/
//Note: LOGIN PAGE DEFINE'S
#define USERNAME_ADDR1						0x0F
#define USERNAME_ADDR2						0xFF
#define USERPASSWORD_ADDR1					0X10
#define USERPASSWORD_ADDR2					0x0B
#define LOGIN_B_ADDR1						0X10
#define LOGIN_B_ADDR2						0x19

/***##################################################***/

/***##################################################***/
//Note: LOGIN PAGE DWIN ADDRESS

//FIRST ROW ADDRESS
#define LOADPAGE_FST_R_SRNO_ADDR 		0x1068
#define LOADPAGE_FST_R_PROFILENAME_ADDR	0X106D
#define LOADPAGE_FST_R_MMHG_ADDR 		0x108A
#define LOADPAGE_FST_R_MIN_ADDR 		0x1091
#define LOADPAGE_FST_R_SEC_ADDR			0x1096
#define LOADPAGE_FST_R_APPLY_ADDR1		0x10
#define LOADPAGE_FST_R_APPLY_ADDR2		0x9B

//SECOND ROW ADDRESS
#define LOADPAGE_SND_R_SRNO_ADDR 				0x109E
#define LOADPAGE_SND_R_PROFILENAME_ADDR			0x10A4
#define LOADPAGE_SND_R_MMHG_ADDR 				0x10C1
#define LOADPAGE_SND_R_MIN_ADDR	 				0x10C8
#define LOADPAGE_SND_R_SEC_ADDR					0x10CD
#define LOADPAGE_SND_R_APPLY_ADDR1		0X10
#define LOADPAGE_SND_R_APPLY_ADDR2		0XD2


//THIRD ROW ADREES
#define LOADPAGE_TRD_R_SRNO_ADDR 				0x10D5
#define LOADPAGE_TRD_R_PROFILENAME_ADDR 		0x10DB
#define LOADPAGE_TRD_R_MMHG_ADDR 				0x10F8
#define LOADPAGE_TRD_R_MIN_ADDR 				0x10FF
#define LOADPAGE_TRD_R_SEC_ADDR					0x1104
#define LOADPAGE_TRD_R_APPLY_ADDR1				0X11
#define LOADPAGE_TRD_R_APPLY_ADDR2				0X09

#define LOADPAGE_FST_DELTEBUTTON_ADDR1	0X24
#define LOADPAGE_FST_DELTEBUTTON_ADDR2	0XCF
#define LOADPAGE_SND_DELTEBUTTON_ADDR1	0X24
#define LOADPAGE_SND_DELTEBUTTON_ADDR2	0XD1
#define LOADPAGE_TRD_DELTEBUTTON_ADDR1	0x24
#define LOADPAGE_TRD_DELTEBUTTON_ADDR2	0xD3

#define LOADPAGE_NEXTBUTTON_ADDR1	0X11
#define LOADPAGE_NEXTBUTTON_ADDR2	0X11

#define LOADPAGE_BACKBUTTON_ADDR1	0X11
#define LOADPAGE_BACKBUTTON_ADDR2	0X0D

/***##################################################***/

/*--------------------------------------------------------------------------------------------------------------------------*/


//PAGE:HOME PAGE
/***##################################################***/
//Note: HOME PAGE DEFINE'S

#define UNIVERSAL_NOTIFICATION_ADDR 0X11C6
#define UNIVERSAL_NOTIFICATION_ADDR2 0X2443
#define NOTIFICATION_OK_B1	0x11
#define NOTIFICATION_OK_B2	0xE7

#define UNIVERSAL_TIME_ADDR	0X11F6
#define UNIVERSAL_DATE_ADDR	0X11EB

#define UNIQUE_HOMEPAGE_WEIGHT_BREFORE_ADDR1 0X12
#define UNIQUE_HOMEPAGE_WEIGHT_BREFORE_ADDR2 0X0B

#define UNIQUE_HOMEPAGE_WEIGHT_AFTER_ADDR1 0X12
#define UNIQUE_HOMEPAGE_WEIGHT_AFTER_ADDR2 0X00


#define UNIQUE_HOMEBUTTON_ADDR1				0XFF
#define UNIQUE_HOMEBUTTON_ADDR2				0XFE

#define UNIQUE_BACKBUTTON_ADDR1				0XFF
#define UNIQUE_BACKBUTTON_ADDR2				0XFB

#define BEFORESTART_BATCH_NUMBER_BUFF_ADDR1 0X23
#define BEFORESTART_BATCH_NUMBER_BUFF_ADDR2 0XC6

#define BEFORESTART_NO_OF_SAMPLES_BUFF_ADDR1 0X23
#define BEFORESTART_NO_OF_SAMPLES_BUFF_ADDR2 0XD0

#define BEFORESTART_SAMPLES_BATCHNO_APPY_BUTTON_ADDR1	0X23
#define BEFORESTART_SAMPLES_BATCHNO_APPY_BUTTON_ADDR2 	0XD8

#define HOMEPAGE_RESULTS_BUTTON_ADDR1	0X24
#define HOMEPAGE_RESULTS_BUTTON_ADDR2	0X61

#define HOMEPAGE_NOOFSAMPLESPASSORFAIL_NEXTBUTTON_ADDR1 0X24
#define HOMEPAGE_NOOFSAMPLESPASSORFAIL_NEXTBUTTON_ADDR2 0X9B

#define HOMEPAGE_NOOFSAMPLESPASS_ADDR1 0x24
#define HOMEPAGE_NOOFSAMPLESPASS_ADDR2 0x93

#define HOMEPAGE_NOOFSAMPLESFAIL_ADDR1 0x24
#define HOMEPAGE_NOOFSAMPLESFAIL_ADDR2 0x8C

#define HOME_PAGE_SET_VACCUM_ADDR			0x103D
#define HOME_PAGE_SET_TIME_MIN_ADDR			0x104E
#define HOME_PAGE_SET_TIME_SEC_ADDR			0x1053
#define HOME_PAGE_PROFILENAME_ADDR			0X101B

//UPDATE MMHG AND SET MIN AND SEC TO HOME PAGE
#define MMHG_TO_HOME_PAGE_ADDR				HOME_PAGE_SET_VACCUM_ADDR
#define MIN_TO_HOME_PAGE_ADDR 				HOME_PAGE_SET_TIME_MIN_ADDR
#define SEC_TO_HOME_PAGE_ADDR 				HOME_PAGE_SET_TIME_SEC_ADDR
#define PEOFILENAME_TO_HOME_PAGE_ADDR		HOME_PAGE_PROFILENAME_ADDR


#define HOMEPAGE_CURRENT_VACCUM_ADDR1		0X10
#define HOMEPAGE_CURRENT_VACCUM_ADDR2		0X37

#define HOMEPAGE_CURRENT_TIME_MIN_ADDR1		0X10
#define HOMEPAGE_CURRENT_TIME_MIN_ADDR2		0X44

#define HOMEPAGE_CURRENT_TIME_SEC_ADDR1		0X10
#define HOMEPAGE_CURRENT_TIME_SEC_ADDR2		0X49

#define HOMEPAGE_LOAD_BUTTON_ADDR1			0X10
#define HOMEPAGE_LOAD_BUTTON_ADDR2			0X58

#define HOMEPAGE_BRIGHTNESS_BUTTON_ADDR1	0X10
#define HOMEPAGE_BRIGHTNESS_BUTTON_ADDR2	0X60

#define HOMEPAGE_HOMEPAGE_STARTBUTTON_ADDR1	0X10
#define HOMEPAGE_HOMEPAGE_STARTBUTTON_ADDR2	0X5C

#define HOMEPAGE_HOMEPAGE_STOPBUTTON_ADDR1	0X10
#define HOMEPAGE_HOMEPAGE_STOPBUTTON_ADDR2	0X64

#define HOMEPAGE_MENU_BUTTON_ADDR1			0X11
#define HOMEPAGE_MENU_BUTTON_ADDR2			0X15

/***##################################################***/

/*--------------------------------------------------------------------------------------------------------------------------*/

/***##################################################***/
//Note: DWIN PAGE NUMBERS

#define AUTHENTICATION_PAGE 199
#define VACCUME_TEST_PAGE 	200
#define PROFILE_HOME_PAGE   VACCUME_TEST_PAGE
#define PROFILE_WRITE_PAGE   201
#define PROFILE_LOAD_PAGE    202

#define ADMIN_PROFILE 				1
#define SUPERVISOR_PROFILE			2
#define USER_PRFILE					3
#define MUITI_ADMIN_PROFILE 		4

//#define HOME_PAGE_NUMBER 0X07
#define LOGO_PAGE_NUMBER 0X00
#define AUTH_PAGE_NUMBER 0X01
#define HOME_PAGE_NUMBER 0X02
#define HOME_PAGE_NUMBER_STOP 0X03
#define LOAD_PAGE_NUMBER 0X15

#define NOTIFICATION_PAGE_NUMBER 0x1D
#define NOTIFICATION_W_YES_R_NO_PAGE_NUMBER 0x2D

/***##################################################***/

/*--------------------------------------------------------------------------------------------------------------------------*/

/***#################################################################################################################***/
/***#################################################################################################################***/
/***############### PAGES NUMBERS ###################################################################################***/
/***########################### PAGES NUMBERS #######################################################################***/
/***#################################################################################################################***/
/***#################################################################################################################***/

/***********************************************************************************************************/
/*** AUTHENTICATION PAGE ***/
/************************************************************************************************************/
/*################## SUPERVISOR ##################*/
/*** @MAIN PAGE ***/
#define NO_OF_SAMPLES_TAKE_PAGE_NUMBER 				0X23
#define RESULTS_PAGE 				0X18
#define NOOF_SAMPLES_PASS_OR_FAILS_PAGE_BUMBER 0X25

#define RESULTS_PAGE_SRNUMBER_ADDR 	0x2587
#define RESULTS_PAGE_PROFILENAME_ADDR 	0x23DC
#define RESULTS_PAGE_TESTEDBY_ADDR 		0x2404
#define RESULTS_PAGE_DATE_ADDR 			0x2465
#define RESULTS_PAGE_TIME_ADDR 			0x2479
#define RESULTS_PAGE_DURATION_ADDR 		0x23FB
#define RESULTS_PAGE_MAINTAINED_MMHG_ADDR 		0x2422
#define RESULTS_PAGE_NO_OF_SAMPLES_ADDR 		0x242A
#define RESULTS_PAGE_FAILED_SAMPLES_ADDR 		0x2431
#define RESULTS_PAGE_PASSED_SAMPLES_ADDR 		0x2438

#define RESULTS_PAGE_TEST_PASS_R_FAIL_ADDR 		0x258C

#define SUPERVISOR_HOME_START_PAGE_NUMBER			0X05
#define SUPERVISOR_HOME_STOP_PAGE_NUMBER			0X06
#define SUPERVISOR_MENU_PAGE_NUMBER 				0X07
#define SUPERVISOR_MENU_ADDUSERS_PAGE_NUMBER 		0X09
#define SUPERVISOR_LOAD_PAGE_NUMBER					LOAD_PAGE_NUMBER
#define SUPERVISOR_MENU_ADDTESTPROFILES_PAGE_NUMBER 0X08
#define SUPERVISOR_MENU_CALIBRATE_PAGE_NUMBER		0x0A
#define SUPERVISOR_MENU_ENDUSER_DELETE_PAGE_NUMBER  0X11
#define SUPERVISOR_MENU_TESTPROFILE_DELETE_PAGE_NUMBER  0X26

#define SUPERVISOR_MENU_CALIBRATE_MMHG_ADDR 		0x1190

#define SUPERVISOR_MENU_CALIPOPUP_ADDR				0X2C
#define SUPERVISOR_MENU_VALIDATION_ADDR				0X28
#define SUPERVISOR_MENU_VALIDATION_ADDRSTOP			0X29
#define SUPERVISOR_MENU_VALID_CALID_ADDR			0X2A
#define SUPERVISOR_MENU_VALIDATION_STOPPOPUP_ADDR	0X2B


//MENU PAGE:
#define SUPERVISOR_MENU_ADD_USER_BUTTON_ADDR1				0x11
#define SUPERVISOR_MENU_ADD_USER_BUTTON_ADDR2				0x19
#define SUPERVISOR_MENU_ADD_USER_USERPASSWORD_BUTTON_ADDR1	0X11
#define SUPERVISOR_MENU_ADD_USER_USERPASSWORD_BUTTON_ADDR2	0X69
#define SUPERVISOR_MENU_ADD_USER_USERNAMEE_BUTTON_ADDR1		0X11
#define SUPERVISOR_MENU_ADD_USER_USERNAMEE_BUTTON_ADDR2		0X76
#define SUPERVISOR_MENU_ADD_USER_ADD_BUTTON_ADDR1			0X11
#define SUPERVISOR_MENU_ADD_USER_ADD_BUTTON_ADDR2			0X84

#define SUPERVISOR_MENU_ADD_TESTPROFILE_BUTTON_ADDR1			0x11
#define SUPERVISOR_MENU_ADD_TESTPROFILE_BUTTON_ADDR2			0x25
#define SUPERVISOR_MENU_ADD_TESTPROFILE_SEARCH_BUTTON_ADDR1		0x11
#define SUPERVISOR_MENU_ADD_TESTPROFILE_SEARCH_BUTTON_ADDR2		0x28
#define SUPERVISOR_MENU_ADD_TESTPROFILE_SRNO_BUTTON_ADDR1		0x11
#define SUPERVISOR_MENU_ADD_TESTPROFILE_SRNO_BUTTON_ADDR2		0x2E
#define SUPERVISOR_MENU_ADD_TESTPROFILE_PNAME_BUTTON_ADDR1		0x11
#define SUPERVISOR_MENU_ADD_TESTPROFILE_PNAME_BUTTON_ADDR2		0x32
#define SUPERVISOR_MENU_ADD_TESTPROFILE_MMHG_BUTTON_ADDR1		0x11
#define SUPERVISOR_MENU_ADD_TESTPROFILE_MMHG_BUTTON_ADDR2		0x4F
#define SUPERVISOR_MENU_ADD_TESTPROFILE_MIN_BUTTON_ADDR1		0x11
#define SUPERVISOR_MENU_ADD_TESTPROFILE_MIN_BUTTON_ADDR2		0x56
#define SUPERVISOR_MENU_ADD_TESTPROFILE_SEC_BUTTON_ADDR1		0x11
#define SUPERVISOR_MENU_ADD_TESTPROFILE_SEC_BUTTON_ADDR2		0x5B
#define SUPERVISOR_MENU_ADD_TESTPROFILE_ADD_BUTTON_ADDR1		0x11
#define SUPERVISOR_MENU_ADD_TESTPROFILE_ADD_BUTTON_ADDR2		0x61

#define SUPERVISOR_MENU_DELETEUSERS_BUTTON_ADDR1		0x23
#define SUPERVISOR_MENU_DELETEUSERS_BUTTON_ADDR2		0xC3

#define SUPERVISOR_MENU_DELETETESTPROFILE_BUTTON_ADDR1		0x24
#define SUPERVISOR_MENU_DELETETESTPROFILE_BUTTON_ADDR2		0xCb

#define SUPERVISOR_MENU_DELETEENDUSER_PRESENT_PASSWORD_ADDR1		0x12
#define SUPERVISOR_MENU_DELETEENDUSER_PRESENT_PASSWORD_ADDR2		0x6C

#define SUPERVISOR_MENU_DELETEENDUSER_SRNO1_ADDR					0X1268
#define SUPERVISOR_MENU_DELETEENDUSER_USERNAME1_ADDR				0X126C
#define SUPERVISOR_MENU_DELETEENDUSER_SRNO2_ADDR					0X127E
#define SUPERVISOR_MENU_DELETEENDUSER_USERNAME2_ADDR				0X1282
#define SUPERVISOR_MENU_DELETEENDUSER_SRNO3_ADDR					0X1294
#define SUPERVISOR_MENU_DELETEENDUSER_USERNAME3_ADDR				0X1298

#define SUPERVISOR_MENU_CALIBRAE_BUTTON_ADDR1		0x11
#define SUPERVISOR_MENU_CALIBRAE_BUTTON_ADDR2		0x1D

#define SUPERVISOR_MENU_VALIDATION_BUTTON_ADDR1		0x11
#define SUPERVISOR_MENU_VALIDATION_BUTTON_ADDR2		0x21

#define SUPERVISOR_MENU_VALIDATIONSTOP_BUTTON_ADDR1		0x25
#define SUPERVISOR_MENU_VALIDATIONSTOP_BUTTON_ADDR2		0x5A

#define SUPERVISOR_MENU_VALIDATIONSTART_BUTTON_ADDR1		0x25
#define SUPERVISOR_MENU_VALIDATIONSTART_BUTTON_ADDR2		0x5E

#define SUPERVISOR_MENU_VALIDAFAIL_BUTTON_ADDR1		0x25
#define SUPERVISOR_MENU_VALIDAFAIL_BUTTON_ADDR2		0x56

#define CALIBRATIONGAUGE_INPUTVAL_ADDR1				0x11
#define CALIBRATIONGAUGE_INPUTVAL_ADDR2				0x8F

#define CALIBRATIONGAUGE_INPUTVAL_APPLYBUTTON_ADDR1	0x25
#define CALIBRATIONGAUGE_INPUTVAL_APPLYBUTTON_ADDR2	0x4D

#define SUPERVISOR_MENU_CALIBRAE_INCS_BUTTON_ADDR1		0x11
#define SUPERVISOR_MENU_CALIBRAE_INCS_BUTTON_ADDR2		0x8C
#define SUPERVISOR_MENU_CALIBRAE_DCRS_BUTTON_ADDR1		0x11
#define SUPERVISOR_MENU_CALIBRAE_DCRS_BUTTON_ADDR2		0x88


/*################## ADMIN ##################*/
/*** @MAIN PAGE ***/
#define ADMIN_HOME_START_PAGE_NUMBER		SUPERVISOR_HOME_START_PAGE_NUMBER
#define ADMIN_LOAD_PAGE_NUMBER				LOAD_PAGE_NUMBER
#define ADMIN_MENU_PAGE_NUMBER				0X0B
#define ADMIN_START_PAGE_NUMBER				SUPERVISOR_HOME_START_PAGE_NUMBER
#define ADMIN_STOP_PAGE_NUMBER				SUPERVISOR_HOME_STOP_PAGE_NUMBER

#define ADMIN_MENU_HOME_ADMIN_PASSWORD_CHANGE_PAGE_NUMBER 0X22

//MENU PAGE:
#define ADMIN_ADD_SUPERVISOR_NUMBER				0X0C
#define ADMIN_MENU_HOME_START_PAGE_NUMBER 		ADMIN_HOME_START_PAGE_NUMBER
#define ADMIN_MENU_DELETE_SUERVISOR_PAGE_NUMBER 0X10

#define ADMIN_MENU_FACTORYSETTINGS_PAGE_NUMBER	0x27

#define ADMIN_MENU_CHANGE_TIME_PAGE_NUMBER 	0x0D

//ADDrs:
#define ADMIN_MENU_ADD_SUPERV_BUTTON_ADDR1 0x11
#define ADMIN_MENU_ADD_SUPERV_BUTTON_ADDR2 0x97
#define ADMIN_MENU_ADD_SUPERV_PASSWORD_BUTTON_ADDR1 0x11
#define ADMIN_MENU_ADD_SUPERV_PASSWORD_BUTTON_ADDR2 0xB4
#define ADMIN_MENU_ADD_SUPERV_USERNAME_BUTTON_ADDR1 0x11
#define ADMIN_MENU_ADD_SUPERV_USERNAME_BUTTON_ADDR2 0xA7
#define ADMIN_MENU_ADD_SUPERV_ADD_BUTTON_ADDR1 0x11
#define ADMIN_MENU_ADD_SUPERV_ADD_BUTTON_ADDR2 0xC2

#define ADMIN_MENU_CHANGE_TIME_BUTTON_ADDR1 0x11
#define ADMIN_MENU_CHANGE_TIME_BUTTON_ADDR2 0x9B

#define ADMIN_MENU_CHANGE_TIME_YEAR_ADDR1	0X24
#define ADMIN_MENU_CHANGE_TIME_YEAR_ADDR2	0X9F

#define ADMIN_MENU_CHANGE_TIME_MONTH_ADDR1	0X24
#define ADMIN_MENU_CHANGE_TIME_MONTH_ADDR2	0XA7

#define ADMIN_MENU_CHANGE_TIME_DATE_ADDR1	0X24
#define ADMIN_MENU_CHANGE_TIME_DATE_ADDR2	0XAD

#define ADMIN_MENU_CHANGE_TIME_HOUR_ADDR1	0X24
#define ADMIN_MENU_CHANGE_TIME_HOUR_ADDR2	0XB3

#define ADMIN_MENU_CHANGE_TIME_MINUTE_ADDR1	0X24
#define ADMIN_MENU_CHANGE_TIME_MINUTE_ADDR2	0XB9

#define ADMIN_MENU_CHANGE_TIME_SEC_ADDR1	0X24
#define ADMIN_MENU_CHANGE_TIME_SEC_ADDR2	0XC0

#define ADMIN_MENU_CHANGE_TIME_APPLYBUTTON_ADDR1	0X24
#define ADMIN_MENU_CHANGE_TIME_APPLYBUTTON_ADDR2	0XC7

#define ADMIN_MENU_CHANGE_PASSWORD_BUTTON_ADDR1 0x11
#define ADMIN_MENU_CHANGE_PASSWORD_BUTTON_ADDR2 0x9F

#define ADMIN_MENU_DELETESUPERVISOR_BUTTON_ADDR1 0x23
#define ADMIN_MENU_DELETESUPERVISOR_BUTTON_ADDR2 0xBF

#define ADMIN_MENU_CHANGE_PASSWORD_PRESENT_ADDR1 0x23
#define ADMIN_MENU_CHANGE_PASSWORD_PRESENT_ADDR2 0xA3

#define ADMIN_MENU_CHANGE_PASSWORD_NEW_ADDR1 0x23
#define ADMIN_MENU_CHANGE_PASSWORD_NEW_ADDR2 0xB0

#define ADMIN_MENU_CHANGE_PASSWORD_ADD_BUTTON_ADDR1 0x23
#define ADMIN_MENU_CHANGE_PASSWORD_ADD_BUTTON_ADDR2 0xBD


#define ADMIN_MENU_SUPERVISOR_DELETE_SEARCHNAME_ADDR1			0x12
#define ADMIN_MENU_SUPERVISOR_DELETE_SEARCHNAME_ADDR2			0x00

#define ADMIN_MENU_SUPERVISOR_DELETE_1ST_DELETE_BUTTON_ADDR1	0X12
#define ADMIN_MENU_SUPERVISOR_DELETE_1ST_DELETE_BUTTON_ADDR2	0X22
#define ADMIN_MENU_SUPERVISOR_DELETE_2ND_DELETE_BUTTON_ADDR1	0X12
#define ADMIN_MENU_SUPERVISOR_DELETE_2ND_DELETE_BUTTON_ADDR2	0X38
#define ADMIN_MENU_SUPERVISOR_DELETE_3RD_DELETE_BUTTON_ADDR1	0X12
#define ADMIN_MENU_SUPERVISOR_DELETE_3RD_DELETE_BUTTON_ADDR2	0X4E
#define ADMIN_MENU_SUPERVISOR_DELETE_NEXT_BUTTON_ADDR1			0X12
#define ADMIN_MENU_SUPERVISOR_DELETE_NEXT_BUTTON_ADDR2			0X52
#define ADMIN_MENU_SUPERVISOR_DELETE_BACK_BUTTON_ADDR1			0X12
#define ADMIN_MENU_SUPERVISOR_DELETE_BACK_BUTTON_ADDR2			0X56

#define ADMIN_MENU_SUPERVISOR_DELETE_SRNO1_ADDR					0X120F
#define ADMIN_MENU_SUPERVISOR_DELETE_USERNAME1_ADDR				0X1214
#define ADMIN_MENU_SUPERVISOR_DELETE_SRNO2_ADDR					0X1226
#define ADMIN_MENU_SUPERVISOR_DELETE_USERNAME2_ADDR				0X122A
#define ADMIN_MENU_SUPERVISOR_DELETE_SRNO3_ADDR					0X123C
#define ADMIN_MENU_SUPERVISOR_DELETE_USERNAME3_ADDR				0X1240

#define ADMIN_MENU_ADD_MUITLIADMIN_BUTTON_ADDR1					0X24
#define ADMIN_MENU_ADD_MUITLIADMIN_BUTTON_ADDR2					0XE3

#define ADMIN_MENU_DELETE_MUITLIADMIN_BUTTON_ADDR1				0X24
#define ADMIN_MENU_DELETE_MUITLIADMIN_BUTTON_ADDR2				0XE7

#define ADMIN_MENU_FACTORYSETTINGS_BUTTON_ADDR1					0X24
#define ADMIN_MENU_FACTORYSETTINGS_BUTTON_ADDR2					0XEB

#define ADMIN_FACTORYSETTINGS_SRNO_ADDR1						0X24
#define ADMIN_FACTORYSETTINGS_SRNO_ADDR2						0XEF

#define ADMIN_FACTORYSETTINGS_MODELNO_ADDR1						0X24
#define ADMIN_FACTORYSETTINGS_MODELNO_ADDR2						0XFD

#define ADMIN_FACTORYSETTINGS_COMAPANYNAME_ADDR1				0X25
#define ADMIN_FACTORYSETTINGS_COMAPANYNAME_ADDR2				0X0B

#define ADMIN_FACTORYSETTINGS_COMAPANYLOC_ADDR1					0X25
#define ADMIN_FACTORYSETTINGS_COMAPANYLOC_ADDR2					0X1F

#define ADMIN_FACTORYSETTINGS_INSTALLEDDATE_ADDR1				0X25
#define ADMIN_FACTORYSETTINGS_INSTALLEDDATE_ADDR2				0X33

#define ADMIN_FACTORYSETTINGS_MMHGLIMIT_ADDR1					0X25
#define ADMIN_FACTORYSETTINGS_MMHGLIMIT_ADDR2					0X41

#define ADMIN_FACTORYSETTINGS_APPLYBUTTON_ADDR1					0X25
#define ADMIN_FACTORYSETTINGS_APPLYBUTTON_ADDR2					0X49


/***********************************************************************************************************/

/***##################################################***/
/***@@@ MACRO FUNCTIONS FOR DWIN @@@***/
/***##################################################***/
/**************************************************************************/
//PAGE CHANGE MACRO FUNCTION
/**************************************************************************/
#define DWIN_PAGE_CHANGE( value ) 			\
    do { 									\
        static uint8_t nextPage_buff[10]; 	\
        nextPage_buff[0] = 0x5A; 			\
        nextPage_buff[1] = 0xA5; 			\
        nextPage_buff[2] = 0x07; 			\
        nextPage_buff[3] = 0x82; 			\
        nextPage_buff[4] = 0x00; 			\
        nextPage_buff[5] = 0x84; 			\
        nextPage_buff[6] = 0x5A; 			\
        nextPage_buff[7] = 0x01; 			\
        nextPage_buff[8] = 0x00; 			\
        nextPage_buff[9] = value; 			\
        HAL_UART_Transmit(&huart1, nextPage_buff, 10, HAL_MAX_DELAY); \
    } while(0)

#define DWIN_PAGE_CHANGE1(DBG, value ) 			\
    do { 									\
    	DBG[0] = 0x5A; 			\
    	DBG[1] = 0xA5; 			\
    	DBG[2] = 0x07; 			\
    	DBG[3] = 0x82; 			\
    	DBG[4] = 0x00; 			\
    	DBG[5] = 0x84; 			\
    	DBG[6] = 0x5A; 			\
    	DBG[7] = 0x01; 			\
    	DBG[8] = 0x00; 			\
    	DBG[9] = value; 			\
        HAL_UART_Transmit(&huart1, DBG, 10, HAL_MAX_DELAY); \
    } while(0)


/**************************************************************************/

/**************************************************************************/
// SET DATA IN TO SEND FORMATE TO DWIN, MACRO FUNCTION
/**************************************************************************/


#define ALIGN_SEND_STRING_TO_ADDR_FORMAT_FOR_DWIN(P, ADDR) \
    do { \
        static uint8_t dbg[32]; \
        dbg[0] = 0x5A; \
        dbg[1] = 0xA5; \
        dbg[2] = (6+ (uint8_t )strlen( (char *)P) ) ; \
        dbg[3] = 0x82; \
        dbg[5] = (uint8_t)( ADDR >> 0 ) ; \
        dbg[4] = ( ADDR >> 8 ) ; \
        memcpy(&dbg[6], P, strlen((const char *)P)); \
        for(uint8_t i = ( 6+strlen((const char *)P)); i<17;  i++)	\
		{														\
        	dbg[i] = 0X00 ;										\
		}														\
        HAL_UART_Transmit(&huart1, dbg,( 16 ), HAL_MAX_DELAY); 	\
    } while(0)

#define POPUP_NOTOFICATION_WITHSTRING(FILED_ADDR, PAGE_ADDR, STR) \
	do {												\
		static uint8_t dbg[80] ;						\
		memset(dbg,0,sizeof(dbg)) ;	\
		DWIN_PAGE_CHANGE(PAGE_ADDR);	\
		dbg[0] = 0x5A ;									\
		dbg[1] = 0xA5 ;									\
		dbg[2] = 6+ 24 ;						\
		dbg[3] = 0x82 ;									\
		dbg[5]		=	FILED_ADDR & 0xff	;	\
		dbg[4]		=	FILED_ADDR>>8 	;	\
		memcpy(&dbg[6], STR, strlen((const char *)STR)); \
		HAL_UART_Transmit(&huart1, dbg,( 6+24), HAL_MAX_DELAY);	\
		HAL_Delay(2) ;\
	} while(0) ;

#define MAX_DBG_LEN 26

#define POPUP_NOTIFICATION(DBG, FILE_ADDR, STR, STRLEN) \
    do { \
        DBG[0] = 0x5A; \
        DBG[1] = 0xA5; \
        DBG[2] = 0X1A; \
        DBG[3] = 0x82; \
        DBG[5] = FILE_ADDR & 0xFF; \
        DBG[4] = FILE_ADDR >> 8; \
        memcpy(&DBG[6], STR, (STRLEN < (MAX_DBG_LEN - 6)) ? STRLEN : (MAX_DBG_LEN - 6)); \
        DBG[MAX_DBG_LEN - 1] = '\0'; /* Null-terminate the DBG buffer */ \
        HAL_UART_Transmit(&huart1, DBG, MAX_DBG_LEN, HAL_MAX_DELAY); \
    } while(0)

#endif /* INC_MYHEADER_H_ */
