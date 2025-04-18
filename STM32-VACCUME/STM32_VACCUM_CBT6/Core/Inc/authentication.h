/*
 * authentication.h
 *
 *  Created on: Mar 8, 2024
 *      Author: navee
 */

#ifndef INC_AUTHENTICATION_H_
#define INC_AUTHENTICATION_H_

//#include "myheader.h"

#include "mem_w25qxxDriver.h"
#include "display.h"

extern uint8_t authFlag ;

extern ADC_HandleTypeDef hadc1;

extern uint8_t homepage_setprofile_name[27] ;
extern int homePage_setmmhg	;
extern uint8_t homePage_setMin	;
extern uint8_t homePage_setSec	;

//CHAGNE:20-05
extern uint8_t adminPageDirectionFlag ;
extern uint8_t supervisorPageDirectionFlag ;

//controller param's
#define ADC_TO_VOLTAGE 1240.909


extern  uint8_t c_runstopButton	 ;
extern volatile uint8_t c_min	;
extern volatile uint8_t c_sec	;
extern volatile uint64_t c_runtime	;

extern  float c_mmhg	;

extern uint64_t _c_mmhg  ;

//home param's
extern uint8_t runstopFlag ;
extern uint16_t weight_before_test_val 	;
extern uint16_t weight_after_test_val 	;


extern uint8_t modemRxBuff[MAX_MODEM_RX_BUFF];
extern uint8_t current_page ;



/*** @ADMIN MAIN PAGE@ ***/
#define ADMIN_MAIN_PAGE									5
#define ADMIN_MENU_PAGE 								11
#define ADMIN_LOAD_PAGE 								13
#define ADMIN_START_PAGE 								5
#define ADMIN_STOP_PAGE									6

#define ADMIN_MENU_ADDSUPERVISOR_PAGE 					111
#define ADMIN_MENU_ADDSUPERVISOR_PAGE_PASSWORD_ADDR		{0x04, 0xB9}
#define ADMIN_MENU_ADDSUPERVISOR_PAGE_USERNAME_ADDR		{0x12, 0x2F}
#define ADMIN_MENU_ADDSUPERVISOR_PAGE_ADDBUTTON_ADDR	{0x12, 0x50}

/*** @@ADMIN-PAGE VAR***/
//extern uint8_t admin_supervisor_counter ;
extern uint8_t admin_supervisor_username[11] ;
extern uint8_t admin_supervisor_password[11] ;

extern uint8_t admin_changePassword_NewPassword[11] ;
extern uint8_t admin_changePassword_PresentPassword[11] ;


#define ADMIN_MENU_CHANGE_TIME_PAGE			112
#define	ADMIN_MENU_CHANGE_PASSWORD_PAGE		113


#define SUPERVISOR_MAIN_PAGE		ADMIN_MAIN_PAGE
/*** @SUPERVISOR MAIN PAGE@ ***/
#define SUPERVISOR_MENU_PAGE		7
#define SUPERVISOR_LOAD_PAGE		ADMIN_LOAD_PAGE
#define SUPERVISOR_START_PAGE 		ADMIN_START_PAGE
#define SUPERVISOR_STOP_PAGE		ADMIN_STOP_PAGE
/*** @@SUPERVISOR-PAGE VAR***/
extern uint8_t supervisor_username[11] 		;
extern uint8_t supervisor_password[11] 		;
extern uint8_t supervisorPageDirectionFlag 	;
extern uint8_t supervisor_usercounter 		;

extern uint8_t sVP_M_ADD_TSTP_Srch_var ;
extern uint8_t sVP_M_ADD_TSTP_Pname_var[26] ;
extern uint16_t sVP_M_ADD_TSTP_MMHG_var ;
extern uint8_t sVP_M_ADD_TSTP_MIN_var ;
extern uint8_t sVP_M_ADD_TSTP_SEC_var ;
extern uint8_t sVP_TestProfileCounter ;

/*** @ENDUSER DETAILS BELOW ***/
#define ENDUSER_START_PAGE	 	2
#define ENDUSER_STOP_PAGE 		3

#define WEIGHT_BEFORE_TEST 	0x13
#define WEIGHT_AFTER_TEST 	0x12



#define TIMER_OR_STOP_CLICKED 		2
#define PAGECHANGE_TO_AFTERWEIGHT	3
#define RUN_PID						4


void auth_init(uint8_t *displayUartFlag, uint8_t *authFlag, uint8_t *auth_UserName_t, uint8_t *auth_PassWord_t);
void auth_page_read(uint8_t *displayUartFlag, uint8_t *authFlag, uint8_t *auth_UserName_t  , uint8_t *auth_PassWord_t, uint8_t *aFlag, uint8_t *pageflag);



void authentication_admin_init(uint8_t *displayUartFlag);
void authentication_AdminPage_Read(uint8_t *displayUartFlag, uint8_t *, uint8_t *, uint8_t *, uint8_t *, uint8_t *, uint8_t *);
void autherntication_AdminPage_functionality(uint8_t *, uint8_t *, uint8_t *,
											 uint8_t *, uint16_t *, uint8_t *, uint8_t *);


//SUPERVISOR
void authentication_SuperUser_init(uint8_t *displayUartFlag) ;
void authentication_SupervisorPage_Read(uint8_t *displayUartFlag, uint8_t *,uint8_t *, uint8_t *, uint8_t *, uint8_t *, uint8_t *);
void authentication_SupervisorPage_functionality(uint8_t *, uint8_t *, uint8_t *, uint8_t *, uint8_t *, uint8_t *, uint8_t*);





#endif /* INC_AUTHENTICATION_H_ */
