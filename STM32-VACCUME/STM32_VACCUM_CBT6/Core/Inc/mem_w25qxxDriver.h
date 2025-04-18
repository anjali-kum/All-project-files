/*
 * mem_w25qxxDriver.h
 *
 *  Created on: Feb 22, 2024
 *      Author: navee
 */

#ifndef INC_MEM_W25QXXDRIVER_H_
#define INC_MEM_W25QXXDRIVER_H_



//#include "stm32f1xx_hal.h"
//#include "main.h"
#include "myheader.h"


extern SPI_HandleTypeDef hspi2;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;

extern uint8_t supervisorDeleteIndex 		;
extern uint8_t supervisorDelete1stButton	;
extern uint8_t supervisorDelete2stButton	;
extern uint8_t supervisorDelete3stButton	;
extern uint8_t supervisorDeleteTrackIndex  ;

extern uint8_t supervisorDeleteinitVal ;

extern uint8_t endUserDeleteIndex ;
extern uint8_t endUserDelete1stButton	;
extern uint8_t endUserDelete2stButton	;
extern uint8_t endUserDelete3stButton	;

extern uint8_t endUserDeleteTrackIndex  ;
extern uint8_t endUserDeleteinitVal ;

extern uint8_t testProfileDeleteTrackIndex ;
extern uint8_t testProfileDeleteinitVal ;


/*** ARDESS FOR AUTHENTICATION ***/
#define PAGE_SIZE 		256 /* 1 page is equal to 256 bytes */
#define SECTOR_SIZE 	(PAGE_SIZE * 16)
#define BLOCK_SIZE 		(SECTOR_SIZE * 16)

/*###### 1ST BLOCK FOR ADMIN AUTHENTICATION (0X0000H - 0XFFFF) ###### */
/***	write and read admin credentials from this address	***/
#define ADMIN_ADDR				0x0000 		//(0x0000)

#define MULTIPLE_ADMIN_ADDR 	(ADMIN_ADDR + SECTOR_SIZE)

#define RTC_ADDR	3*BLOCK_SIZE


/*###### 2ST BLOCK FOR USER & SUPERVIOSER AUTHENTICATION (0X10000 - 0X1FFFF) ###### */
/***	write and read user & supervisor credentials from this adress	***/
#define SUPERUSER_ADDR		BLOCK_SIZE //(256 * PAGE_SIZE) /* is interms of multiple of pages(256 bytes) */ /* (256 x 256) =65536(0x10000) */


#define USER_ADDR				( BLOCK_SIZE + ( SECTOR_SIZE *6 ) )

//following code for user index
#define USER_INDEX_ADDR 					61 * BLOCK_SIZE
#define SUPERVISOR_INDEX_ADDR 				( (61 * BLOCK_SIZE) + SECTOR_SIZE)  // (62 * BLOCK_SIZE)*( SECTOR_SIZE )
#define SUPERVISOR_TEST_PROFILE_INDEX_ADDR 	( (61 * BLOCK_SIZE)+( 2 * SECTOR_SIZE ) )
#define MULTIADMIN_INDEX_ADDR				( (61 * BLOCK_SIZE)+( 3 * SECTOR_SIZE ) )

#define MMHGGAUGEREFERENCE_ADDR				( (61 * BLOCK_SIZE)+( 5 * SECTOR_SIZE ) )

/*** @@@ FACTORY DETAILS BELOW @@@ ***/
#define FACTORY_ADDR  (4* BLOCK_SIZE)

/*** @@@ ONBOARD_STORED_RESULTS_ADDR DETAILS BELOW @@@ ***/
#define ONBOARD_STORED_RESULTS_ADDR  (5* BLOCK_SIZE)
#define CALIBRATION_DEVICE_ADDORSUBSTRACTVALUE_INDICATION  (6* BLOCK_SIZE - 5 )
#define VALID_AND_CALIB_RESULTS  (6* BLOCK_SIZE )

/*###### 3RD BLOCK TEST PROFILE WRITE ###### */
/***	write and read Test profile details using this block	***/
#define TESTPROFILE_ADDR		(BLOCK_SIZE * 2) //(256 * PAGE_SIZE) /* is interms of multiple of pages(256 bytes)  */



/*###### LAST BLOCK FOR SWAP SECTOR FOR DELETE FUTURE ##### */
#define SWAP_SECTOR_ADDR 		(63*BLOCK_SIZE)

/* w25q32 functionality adresses */
#define R_ID 				0x9F 	/* 	read device ID,	R means Read 			*/
#define R_STATUS 			0X05	/* 	read device status,	R means Read 		*/
#define FLASH_WAKEUP 		0xAB	/*  deep power wake up 						*/
#define ARRAY_READ_LOW_FRQ	0x03	/* 	read array (low frequency)			 	*/
#define ARRAY_READ			0x0B	/*	read array (fast, need to add
 	 	 	 	 	 	 	 	 	 	1 dummy byte after 3 address bytes)		*/
#define W_ENABLE 			0x06	/* 	write enable							*/
#define FLASH_CHIP_ERASE	0x60	/*	chip erase (may take several seconds
										depending on size)						*/
#define SECTOR_ERASE		0x20	/*	erase one 4K block(sector) of flash
										memory									*/
#define BYTE_PAGE_PROGRAM	0x02	/*	write (1 to 256bytes)					*/




 void mem_W25qxx_Driver_init(uint32_t *, uint16_t );


void mem_w25qxxDriver_user_verify(uint8_t *auth_UserName_t, uint8_t *auth_PassWord_t , uint8_t *pageflag);

void mem_w25qxxDriver_Read_TestProfiles(uint8_t *output, uint8_t outputlen, uint8_t index);

void mem_w25qxxDriver_Write_TestProfiles(uint8_t srNo, uint16_t mmHG, uint8_t min, uint8_t sec, uint8_t *profileName, uint8_t );

void read_supervisor(uint8_t *counter);
uint8_t mem_w25qxxDriver_supervisor_write(uint8_t *auth_UserName_t, uint8_t *auth_PassWord_t);

void mem_w25qxxDriver_read_device_details(uint8_t *deviceModel, uint8_t *deviceVersion, uint8_t *clientcompanyName, uint8_t *clientcompanyLocation);

uint8_t mem_w25qxxDriver_user_write(uint8_t *auth_UserName_t, uint8_t *auth_PassWord_t, uint8_t *counter);

void mem_w25qxxDriver_read_supervisor(uint8_t counter, uint8_t *op) ;

void deleteAnyUser(uint32_t addr, uint8_t *auth_UserName_t, uint8_t counter);
uint16_t mem_w25qxxDriver_getSupervisor_idx() ;
void mem_w25qxxDriver_setSupervisor_idx(uint16_t cunt) ;

void mem_w25qxxDriver_Admin_password_change(uint8_t *auth_PassWord_t) ;

void delete_supervisor_w_name(uint8_t *counter, uint8_t *op, uint8_t *supervisorDeleteIndex) ;

//void mem_w25qxxDriver_password_change(uint8_t *old_auth_PassWord_t, uint8_t *auth_PassWord_t) ;
void mem_w25qxxDriver_password_change(uint8_t *auth_username_t, uint8_t *old_auth_PassWord_t, uint8_t *auth_PassWord_t) ;
void mem_w25qxxDriver_read_endUser(uint8_t counter, uint8_t *op);

uint8_t mem_W25qxx_Driver_ReadByte(uint32_t addr) ;
void mem_W25qxx_Driver_writeByte(uint32_t addr, uint8_t byt) ;
void mem_W25qxx_Driver_ReadBytes(uint32_t addr, uint8_t *buf, uint16_t len) ;

void deleteTestProfile(uint32_t addr, uint8_t *auth_UserName_t, uint8_t counter) ;

uint16_t mem_w25qxxDriver_getTestProfile_idx();
void mem_w25qxxDriver_setTestProfile_idx(uint16_t cunt);

uint16_t mem_w25qxxDriver_getEndUser_idx() ;
void mem_w25qxxDriver_setEndUser_idx(uint16_t cunt) ;
void mem_W25qxx_Driver_sectorErase(uint32_t addr) ;

void mem_w25qxxDriver_user_password_change(uint64_t userAddr, uint8_t *auth_UserName_t, uint8_t *auth_PassWord_t, uint8_t *newpassword) ;

void mem_w25qxxDriver_multi_Admin_write(uint8_t *auth_UserName_t, uint8_t *auth_PassWord_t) ;
void mem_w25qxxDriver_read_multiadmin(uint8_t counter, uint8_t *op) ;

uint16_t mem_w25qxxDriver_getMutiliAdmin_idx();
void mem_w25qxxDriver_MutliAdmin_idx(uint16_t cunt) ;
void mem_w25qxxDriver_write_device_details(uint8_t *deviceModel, uint8_t *deviceVersion, uint8_t *clientcompanyName,
		uint8_t *clientcompanyLocation, uint8_t *installedDate, uint8_t *fixedmmHG) ;
//CHANGE:20-05
void  mem_w25qxxDriver_read_fixed_mmhg(uint8_t *arr);


void mem_w25qxxDriver_Write_mmhg_GAUGEval(uint8_t *arr, uint8_t len) ;
void mem_w25qxxDriver_Read_mmhg_GAUGEval(uint8_t *arr, uint8_t len) ;

void mem_w25qxxDriver_ResultsPage_write(uint8_t *profilename, uint8_t *duration, uint8_t *testedBy, uint8_t *date, uint8_t *time,
		uint8_t *fixedAmp, uint8_t *sampleweight, uint8_t *passRfail);
void mem_w25qxxDriver_ResultsPage_Read_nd_show_Results(uint8_t index) ;

void mem_W25qxx_Driver_writeBytes(uint32_t addr, uint8_t* buf, uint16_t len);


#endif /* INC_MEM_W25QXXDRIVER_H_ */
