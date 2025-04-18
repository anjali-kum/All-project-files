/*
 * mem_w25qxxDriver.c
 *
 *  Created on: Feb 22, 2024
 *      Author: navee
 */


#include "mem_w25qxxDriver.h"

 uint32_t *spi2GpioPort;
 uint16_t sPI2CsPin;

uint8_t supervisorDeleteIndex = 0 ;
uint8_t supervisorDelete1stButton	=	0	;
uint8_t supervisorDelete2stButton	=	0	;
uint8_t supervisorDelete3stButton	=	0	;

uint8_t supervisorDeleteTrackIndex = 0 ;
uint8_t supervisorDeleteinitVal = 9 ;

uint8_t endUserDeleteIndex = 0 ;
uint8_t endUserDelete1stButton	=	0	;
uint8_t endUserDelete2stButton	=	0	;
uint8_t endUserDelete3stButton	=	0	;

uint8_t endUserDeleteTrackIndex = 0 ;
uint8_t endUserDeleteinitVal = 9 ;

uint8_t testProfileDeleteTrackIndex = 0 ;
uint8_t testProfileDeleteinitVal = 9 ;



static uint8_t mem_w25qxxDriver_endUser_verify(uint8_t *auth_UserName_t, uint8_t *auth_PassWord_t ) ;
static void mem_strcpy(uint8_t initsize, uint8_t *array, uint8_t arraysize, uint8_t stringinit,  uint8_t *_string, uint8_t _strlen);
void read_multiadmin(uint8_t *counter) ;
//#define SHOW_ADMIN_USERNAME_PASSWORD

/*** @#############################################################################################################################################################@ ***/
/*** @@ DRIVER SPECIFIC CODE BELOW. ***/
/*** @#############################################################################################################################################################@ ***/

//&&&&&&&&&&&&&&&&&&& BELOW STATIC CODE, CALLED INSIDE FILE &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&//

 void cs_deselect(uint32_t *spi1_gpio_port, uint16_t SPI1_CS_Pin_t )
{
	HAL_GPIO_WritePin(spi1_gpio_port , SPI1_CS_Pin_t , 1);
}

 void cs_select(uint32_t *spi1_gpio_port, uint16_t SPI1_CS_Pin_t )
{
	HAL_GPIO_WritePin(spi1_gpio_port , SPI1_CS_Pin_t , 0);
}

 uint8_t sPI_Transfer(uint8_t *x)
 {
  uint8_t y = HAL_SPI_Transmit(&hspi2, x, 1, HAL_MAX_DELAY);
  return y;
 }

 uint8_t sPI_Receiver(uint8_t *x)
 {
 	 uint8_t y = HAL_SPI_Receive(&hspi2, x, 1, HAL_MAX_DELAY);
 	 return y;
 }


static void _cs_select()
{
	cs_select(spi2GpioPort, sPI2CsPin);
}

static void _cs_deselect()
{
	cs_deselect(spi2GpioPort, sPI2CsPin);
}

/// return the STATUS register
static uint8_t  mem_W25qxx_Driver_ReadStatus()
{
	uint8_t status_reg = R_STATUS ;
	uint8_t read_status = 0;

	_cs_select();
	sPI_Transfer(&status_reg);
//	uint8_t flag = sPI_Receiver(&read_status) ;
	_cs_deselect();

	return read_status;
}

/// check if the chip is busy erasing/writing
static uint8_t mem_W25qxx_Driver_busy()
{
	return (mem_W25qxx_Driver_ReadStatus() & 1) ;
}


/// Send a command to the flash chip,
static void mem_W25qxx_Driver_sendCommand(uint8_t cmd, uint8_t isWrite)
{
	if(isWrite){
		_cs_select();//Selects the chip by setting the chip select (CS) pin low. This starts the SPI communication.
		uint8_t we = W_ENABLE;
		sPI_Transfer(&we);
		_cs_deselect();//Deselects the chip by setting the chip select (CS) pin high. This ends the SPI communication.

	}
	if(cmd != FLASH_WAKEUP) while( mem_W25qxx_Driver_busy());

	_cs_select(); // Selects the chip by setting the chip select (CS) pin low. This starts the SPI communication.
	sPI_Transfer(&cmd);// TRANSER the data using array name

}

/// erase a 4Kbyte Sector
 void mem_W25qxx_Driver_sectorErase(uint32_t addr)
{
	uint8_t _addr  ;
	mem_W25qxx_Driver_sendCommand(SECTOR_ERASE, 1); // sector Erase
	_addr = addr >> 16 ;
	sPI_Transfer(&_addr);

	_addr = addr >> 8 ;
	sPI_Transfer(&_addr);

	sPI_Transfer( (uint8_t *)&addr);
	_cs_deselect();//Deselects the chip by setting the chip select (CS) pin high. This ends the SPI communication.
}


/// Write 1 byte to flash memory
/// WARNING: you can only write to previously erased memory locations (see datasheet)
///          use the block erase commands to first clear memory (write 0xFFs)
 void mem_W25qxx_Driver_writeByte(uint32_t addr, uint8_t byt)
{
	uint8_t _addr  ;
	mem_W25qxx_Driver_sendCommand(BYTE_PAGE_PROGRAM, 1);  // Byte/Page Program
	 _addr = addr >> 16 ;
	sPI_Transfer(&_addr);

	_addr = addr >> 8 ;
	sPI_Transfer(&_addr);

	sPI_Transfer( (uint8_t *)&addr);
	sPI_Transfer(&byt);
	_cs_deselect();//Deselects the chip by setting the chip select (CS) pin high. This ends the SPI communication.
}

/// read 1 byte from flash memory
uint8_t mem_W25qxx_Driver_ReadByte(uint32_t addr)
{
	uint8_t result = 0 ;
//	uint8_t dummy = 0 ;

	mem_W25qxx_Driver_sendCommand(ARRAY_READ_LOW_FRQ, 0);
	uint8_t _addr = addr >> 16 ; // shift to 16 bits to get the MSB bytes
	sPI_Transfer(&_addr);// transfer to spi higher addr
	_addr = addr >> 8 ;// shift to 8 bits to get the middle bytes
	sPI_Transfer(&_addr);// transfer spi to get lower address
	sPI_Transfer( (uint8_t *)&addr); //sPI_Transfer(&dummy);
	sPI_Receiver(&result);// store received data in results

	_cs_deselect();//Deselects the chip by setting the chip select (CS) pin high. This ends the SPI communication.

	return result;
}

/// read unlimited # of bytes
void mem_W25qxx_Driver_ReadBytes(uint32_t addr, uint8_t *buf, uint16_t len)
{
	uint8_t dummy = 0 ;

	mem_W25qxx_Driver_sendCommand(ARRAY_READ, 0);
	uint8_t _addr = addr >> 16 ; //shift 16 bits right to get high bytes of addr (msb)
	sPI_Transfer(&_addr);//Sends the high byte of the address via SPI.

	_addr = addr >> 8 ; // Shifts the address 8 bits to the right to get the middle byte of the address.
	sPI_Transfer(&_addr);//Sends the high byte of the address via SPI.

	sPI_Transfer( (uint8_t *)&addr);//sends the low bytes of the addr via SPI
	sPI_Transfer(&dummy); //"dont care"(Sends a dummy byte,Often used to initiate SPI read operations.

	for (uint16_t i = 0; i < len; ++i)
	{
		//((uint8_t*) buf)[i] = SPI.transfer(&dummy);
		sPI_Receiver(&buf[i]) ;
	}

	_cs_deselect();//Deselects the chip by setting the chip select (CS) pin high. This ends the SPI communication.

}

/// write multiple bytes to flash memory (up to 64K)
/// WARNING: you can only write to previously erased memory locations (see datasheet)
///          use the block erase commands to first clear memory (write 0xFFs)
/// This version handles both page alignment and data blocks larger than 256 bytes.
///
 void mem_W25qxx_Driver_writeBytes(uint32_t addr, uint8_t* buf, uint16_t len)
{
  uint16_t n;
  uint16_t maxBytes = 256-(addr%256);  // force the first set of bytes to stay within the first page
  uint16_t offset = 0;
  uint8_t _addr ;
  while (len>0)
  {
    n = (len<=maxBytes) ? len : maxBytes;
    mem_W25qxx_Driver_sendCommand(BYTE_PAGE_PROGRAM, 1);  // Byte/Page Program
	 _addr = addr >> 16 ;// getting msb bytes
	sPI_Transfer(&_addr);

	_addr = addr >> 8 ;// getting middle bytes
	sPI_Transfer(&_addr);

	sPI_Transfer(&addr);// getting low address bytes

    for (uint16_t i = 0; i < n; i++)
    	sPI_Transfer(&buf[i]);//Reads a byte from SPI and stores it in the buf array at index i
    _cs_deselect();

    addr += n;  // adjust the addresses and remaining bytes by what we've just transferred.
    offset += n;
    len -= n;
    maxBytes = 256;   // now we can do up to 256 bytes per loop
  }
}


static void mem_W25qxx_Driver_SwapSector_Target_to_Destiny(uint32_t target )
{
	uint8_t data =0 ;
	mem_W25qxx_Driver_sectorErase(SWAP_SECTOR_ADDR);
	HAL_Delay(100);
	for(uint32_t i=0; i<4096; i++)// Upto 4 bytes
	{
		data = mem_W25qxx_Driver_ReadByte(target+i) ;
//		HAL_Delay(1) ;
		mem_W25qxx_Driver_writeByte(( SWAP_SECTOR_ADDR+i), data);
//		HAL_Delay(1) ;
	}



//	for(uint8_t i=0; i<4 ;i++)
//	{
//	    mem_W25qxx_Driver_ReadBytes( SWAP_SECTOR_ADDR + (i * PAGE_SIZE), array, sizeof(array)) ; //SUPERUSER_ADDR + (*counter * PAGE_SIZE)
//
//		HAL_UART_Transmit(&huart3, array, sizeof(array), HAL_MAX_DELAY);
//	}



}

static void mem_find_strlen(uint8_t *in, uint8_t *output)
{
	uint8_t k=0;
	for(uint8_t i=0; in[i] != '\0' ; i++)
	{
		k++;
	}
	*output = k ;
}

static void mem_W25qxx_Driver_SwapSector_Destiny_to_Target_for_TestProfile(uint32_t target, uint8_t *delete_username)
{
	mem_W25qxx_Driver_sectorErase(target);
	HAL_Delay(200) ;
	uint8_t array[31] ;
	uint8_t username[27];
	uint8_t usernameLen  = 0 ;

	uint8_t deletenameLen = 0;
	uint8_t equal_compare = 0 ;
	uint8_t index = 0 ;

	for(uint8_t i=0; i<16 ;i++)
	{
	    mem_W25qxx_Driver_ReadBytes( SWAP_SECTOR_ADDR + (i * PAGE_SIZE), array, sizeof(array)) ; //SUPERUSER_ADDR + (*counter * PAGE_SIZE)
	    HAL_Delay(10) ;

	    for(uint8_t j=0, k=0; k<26; j++,k++)
		{
			username[j] = array[k];
		}
		 mem_find_strlen(username, &usernameLen);

		 mem_find_strlen(delete_username, &deletenameLen);

		if(usernameLen == deletenameLen)
		{
			for(equal_compare = 0; equal_compare < usernameLen ; equal_compare++  )
			{

				if(username[equal_compare] != delete_username[equal_compare])
					break ;

			}

		}
		if(equal_compare == usernameLen)
		{
			continue ;
		}

		mem_W25qxx_Driver_writeBytes((target + (PAGE_SIZE * index)) , array, sizeof(array)) ;
		HAL_Delay(10) ;
		index++ ;

	}

}

static void mem_W25qxx_Driver_SwapSector_Destiny_to_Target_for_userPassword_change(uint32_t target, uint8_t *targetusername, uint8_t *newpassword)
{
	mem_W25qxx_Driver_sectorErase(target);
	HAL_Delay(100) ;
	uint8_t array[22] ;
	uint8_t username[11];
	uint8_t usernameLen  = 0 ;

	uint8_t password[11];
	uint8_t passwordLen = 0 ;

	uint8_t targetnameLen = 0;
	uint8_t equal_compare = 0 ;
	uint8_t index = 0 ;

	for(uint8_t i=0; i<16 ;i++)
	{
		mem_W25qxx_Driver_ReadBytes( SWAP_SECTOR_ADDR + (i * PAGE_SIZE), array, sizeof(array)) ; //SUPERUSER_ADDR + (*counter * PAGE_SIZE)
		HAL_Delay(10) ;
	//	    HAL_UART_Transmit(&huart1, array, sizeof(array), HAL_MAX_DELAY);

		for(uint8_t j=0, k=1; k<11; j++,k++)
		{
			username[j] = array[k];
		}
		 mem_find_strlen(username, &usernameLen);
	//		 HAL_UART_Transmit(&huart1, username, usernameLen, HAL_MAX_DELAY);
		 mem_find_strlen(targetusername, &targetnameLen);
	//		 HAL_UART_Transmit(&huart1, delete_username, deletenameLen, HAL_MAX_DELAY);

		for(uint8_t j=0, k= 11; k<21; j++,k++)
		{
			password[j] = array[k] ;
		}
		mem_find_strlen(password, &passwordLen);
	//		 HAL_UART_Transmit(&huart1, password, passwordLen, HAL_MAX_DELAY);
		if(usernameLen == targetnameLen)
		{
			for(equal_compare = 0; equal_compare < usernameLen ; equal_compare++  )
			{

				if(username[equal_compare] != targetusername[equal_compare])
					break ;

			}

		}
		if(equal_compare == usernameLen)
		{
			mem_strcpy(11, array, 21, 0, newpassword, 11 );
		}

		mem_W25qxx_Driver_writeBytes((target + (PAGE_SIZE * index)) , array, sizeof(array)) ;
		HAL_Delay(10) ;
		index++ ;

	}

}


static void mem_W25qxx_Driver_SwapSector_Destiny_to_Target(uint32_t target, uint8_t *delete_username)
{

	mem_W25qxx_Driver_sectorErase(target);
	HAL_Delay(100);

	uint8_t array[22] ;
	uint8_t username[11];
	uint8_t usernameLen  = 0 ;

	uint8_t password[11];
	uint8_t passwordLen = 0 ;

	uint8_t deletenameLen = 0;
	uint8_t equal_compare = 0 ;
	uint8_t index = 0 ;

	for(uint8_t i=0; i<16 ;i++)
	{
	    mem_W25qxx_Driver_ReadBytes( SWAP_SECTOR_ADDR + (i * PAGE_SIZE), array, sizeof(array)) ; //SUPERUSER_ADDR + (*counter * PAGE_SIZE)
	    HAL_Delay(10) ;

//	    HAL_UART_Transmit(&huart3, array, sizeof(array), HAL_MAX_DELAY);

	    for(uint8_t j=0, k=1; k<11; j++,k++)
		{
			username[j] = array[k];
		}
		 mem_find_strlen(username, &usernameLen);

//		 HAL_UART_Transmit(&huart3, username, usernameLen, HAL_MAX_DELAY);

		 mem_find_strlen(delete_username, &deletenameLen);

//		 HAL_UART_Transmit(&huart3, delete_username, deletenameLen, HAL_MAX_DELAY);

//		 uint8_t data[40] ;
//		 sprintf(data, "name: %s, del name: %s\n", username, delete_username);
//		 HAL_UART_Transmit(&huart3, data, strlen(data), HAL_MAX_DELAY);

		for(uint8_t j=0, k= 11; k<21; j++,k++)
		{
			password[j] = array[k] ;
		}
		mem_find_strlen(password, &passwordLen);
//		 HAL_UART_Transmit(&huart3, password, passwordLen, HAL_MAX_DELAY);
		if(usernameLen == deletenameLen)
		{
			for(equal_compare = 0; equal_compare < usernameLen ; equal_compare++  )
			{

				if(username[equal_compare] != delete_username[equal_compare])
					break ;

			}

		}

		if(equal_compare != usernameLen)
		{
//			HAL_UART_Transmit(&huart3, &"not equal\n", 12, HAL_MAX_DELAY);
			mem_W25qxx_Driver_writeBytes((target + (PAGE_SIZE * index)) , array, sizeof(array)) ;
			index++ ;
		}

	}

}


static void mem_strcpy(uint8_t initsize, uint8_t *array, uint8_t arraysize, uint8_t stringinit,  uint8_t *_string, uint8_t _strlen)
{
    uint8_t j= stringinit;
    for (uint8_t i = initsize; i < arraysize; i++)
    {
        if (i >= (_strlen + initsize))
        {
            array[i] = '\0';
        }
        else
        {
            array[i] = _string[j]; // Fix: Use (i - initsize) to access the correct index in _string
        }
        j++ ;
    }
}




//static uint32_t mem_W25qxx_Driver_readID()
//{
//  uint8_t Temp = 0;
//  uint8_t reg = R_ID;
//  uint8_t data1= 0 ;
//  uint8_t data2= 0 ;
//  uint32_t results = 0 ;
//
//   _cs_select();
//  sPI_Transfer(&reg);
//  Temp = sPI_Receiver(&data1) ;
//  Temp =  sPI_Receiver(&data2);
//  _cs_deselect();
//
//  results = data1 <<8 ;
//  results |= data2 ;
//
//  return results;
//}






/// erase entire flash memory array
/// may take several seconds depending on size, but is non blocking
/// so you may wait for this to complete using busy() or continue doing
/// other things and later check if the chip is done with busy()
/// note that any command will first wait for chip to become available using busy()
/// so no need to do that twice
 void mem_W25qxx_Driver_chipErase()
{
	mem_W25qxx_Driver_sendCommand(FLASH_CHIP_ERASE, 1);
	_cs_deselect();
	 while(mem_W25qxx_Driver_busy());
}
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& STATIC CODE, CALLED INSIDE FILE &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&//

//init function
void mem_W25qxx_Driver_init(uint32_t *spi1_gpio_port, uint16_t SPI1_CS_Pin_t)
{

	spi2GpioPort = spi1_gpio_port ;
	sPI2CsPin = SPI1_CS_Pin_t ;

	uint8_t data1 = 0;
	_cs_select();
	sPI_Transfer(&data1);
	sPI_Transfer(&data1);
	_cs_deselect();

}


/*** @##########################################@@ DRIVER SPECIFIC CODE BELOW.######################################################################@ ***/


/***$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
 * $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
 * $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
 * @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ APPLICATION CODE BELOW @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@S
 */
/*** @#############################################################################################################################################################@ ***/
/*** @@ TEST PROFILE CODE BELOW. ***/
/*** @#############################################################################################################################################################@ ***/

void readtestprofile(uint8_t index)
{
	uint8_t total[31] ;

	mem_W25qxx_Driver_ReadBytes( (TESTPROFILE_ADDR + ( index *256 ) ), total, 31);
	HAL_UART_Transmit(&huart3, total, 31, HAL_MAX_DELAY);
}


void mem_w25qxxDriver_Write_TestProfiles(uint8_t srNo, uint16_t mmHG, uint8_t min, uint8_t sec, uint8_t *profileName, uint8_t index)
{


	uint8_t dbg[40] ;

#ifdef ERASE_TESTPROFILE


	mem_W25qxx_Driver_sectorErase(SUPERVISOR_TEST_PROFILE_INDEX_ADDR);
	HAL_Delay(200) ;
	mem_W25qxx_Driver_sectorErase(TESTPROFILE_ADDR);
	HAL_Delay(200) ;
	sprintf((char *)&dbg,"Test profiles sector erased\n");
	HAL_UART_Transmit(&huart3, dbg, strlen((const char *)dbg), HAL_MAX_DELAY);

#endif

#ifndef ERASE_TESTPROFILE


	memset(dbg, 0, sizeof(dbg)) ;
	uint8_t data1[] = "Please wait." ;

	DWIN_PAGE_CHANGE1(dbg, 0X24) ;
	DWIN_PAGE_CHANGE1(dbg, 0X24) ;

	memset(dbg, 0, sizeof(dbg)) ;
	POPUP_NOTIFICATION(dbg, UNIVERSAL_NOTIFICATION_ADDR2, data1, sizeof(data1)) ;
	POPUP_NOTIFICATION(dbg, UNIVERSAL_NOTIFICATION_ADDR2, data1, sizeof(data1)) ;

//	uint8_t array1[30] ;
//	mem_W25qxx_Driver_sectorErase(TESTPROFILE_ADDR);
	uint8_t cunt = mem_W25qxx_Driver_ReadByte(SUPERVISOR_TEST_PROFILE_INDEX_ADDR) ;
	if( cunt == 255 )
	{
		cunt = 0 ;
	}
//	uint8_t swap_addr = ( ( (cunt) )/16 ) + 1 ;
//    sprintf((char *)&array1,"add test profile COUNTER: %d\n", cunt);
//    HAL_UART_Transmit(&huart3, array1, strlen(array1), HAL_MAX_DELAY);

	if( cunt <= 80)
	{
		uint8_t nameLen = strlen( (char *)profileName) ;
		uint8_t array[31] ;
		if(strlen( (char *)profileName)< 26)
			memcpy(array,profileName,nameLen) ;

		for(uint8_t i= nameLen; i<26; i++)
		{
			array[i] = '\0' ;
		}

		array[26] = cunt ;
		array[27] = mmHG >> 0 ;
		array[28] = mmHG >> 8 ;
		array[29] = min ;
		array[30] = sec ;


	  mem_W25qxx_Driver_writeBytes(TESTPROFILE_ADDR + (cunt * PAGE_SIZE), array, 31) ;
//	  readtestprofile(cunt) ;
	  HAL_Delay(1000) ;
	  cunt++ ;
	  mem_W25qxx_Driver_sectorErase(SUPERVISOR_TEST_PROFILE_INDEX_ADDR);
	  HAL_Delay(200) ;
	  mem_W25qxx_Driver_writeByte(SUPERVISOR_TEST_PROFILE_INDEX_ADDR, cunt) ;


		uint8_t debugBuff[30] ;
		memset(dbg, 0, sizeof(dbg)) ;
		sprintf((char *)&debugBuff, "#Added %s", profileName) ;
		DWIN_PAGE_CHANGE1(dbg, NOTIFICATION_PAGE_NUMBER) ;
		DWIN_PAGE_CHANGE1(dbg, NOTIFICATION_PAGE_NUMBER) ;

		memset(dbg, 0, sizeof(dbg)) ;
		POPUP_NOTIFICATION(dbg, UNIVERSAL_NOTIFICATION_ADDR, debugBuff, strlen( (char *) debugBuff) ) ;
		POPUP_NOTIFICATION(dbg, UNIVERSAL_NOTIFICATION_ADDR, debugBuff, strlen( (char *) debugBuff) ) ;


	}
#endif

}

void mem_w25qxxDriver_Read_TestProfiles(uint8_t *output, uint8_t outputlen, uint8_t index)
{


	mem_W25qxx_Driver_ReadBytes( (TESTPROFILE_ADDR + ( index * 256 ) ), output, outputlen);
	HAL_Delay(100) ;


}




/*** @#########################################################@@ TEST PROFILE CODE BELOW.#########################################################################@ ***/




/*** @#############################################################################################################################################################@ ***/
/*** @@ ADMIN AUTHENTICATION CODE BELOW. ***/
/*** @#############################################################################################################################################################@ ***/

void mem_w25qxxDriver_password_change(uint8_t *auth_username_t, uint8_t *old_auth_PassWord_t, uint8_t *auth_PassWord_t)
{
	uint8_t array[21] ;
	uint8_t _UserName[11]  ;
	uint8_t _userPassword[11] ;
	uint8_t k=0 ;
	uint8_t _dbg[50] ;

	uint8_t dbg[40] ;

	for(uint8_t i=0; i< 50; i++)
	{
		mem_W25qxx_Driver_ReadBytes( MULTIPLE_ADMIN_ADDR + (PAGE_SIZE * i), array, 21);
		HAL_Delay(10) ;
//		HAL_UART_Transmit(&huart3, array, 21, HAL_MAX_DELAY) ;

		for(uint8_t i=1, j =0 ; i<11; i++, j++)
		{
			_UserName[j] = array[i] ;
		}
//		HAL_UART_Transmit(&huart3, _UserName, strlen(_UserName), HAL_MAX_DELAY) ;

		for(uint8_t i=11, j =0 ; i<21; i++, j++)
		{
			_userPassword[j] = array[i] ;
		}
//		HAL_UART_Transmit(&huart3, _userPassword, strlen(_userPassword), HAL_MAX_DELAY) ;

		uint8_t _usernamelen = strlen( (char *)_UserName);
		uint8_t usernamelen = strlen( (char *)auth_username_t);

		if( _usernamelen == usernamelen )
		{
			for( k=0; k< _usernamelen; k++)
			{
				if(_UserName[k] != auth_username_t[k])
				{
					break ;
				}



			}
		}
//		uint8_t dec[20] ;
//		sprintf(dec, "\n k = %d, len = %d \n",k, _usernamelen) ;
//		HAL_UART_Transmit(&huart3, dec, strlen(dec), HAL_MAX_DELAY) ;

		uint8_t _passwordlen = strlen( (char *)_userPassword);
		uint8_t passwordlen = strlen( (char *)old_auth_PassWord_t);

		if(_passwordlen == passwordlen &&( k == _usernamelen) )
		{
			for( k=0; k< _passwordlen; k++)
			{
				if(_userPassword[k] != old_auth_PassWord_t[k])
				{
					break ;
				}



			}
		}

		if(k == _passwordlen )
		{
			uint8_t dbg[40] ;
			memset(dbg, 0, sizeof(dbg)) ;
			uint8_t data1[] = "Please wait.." ;
			DWIN_PAGE_CHANGE(0X24) ; //
			DWIN_PAGE_CHANGE(0X24) ;
			POPUP_NOTIFICATION(dbg, UNIVERSAL_NOTIFICATION_ADDR2, data1, strlen( (char *)data1)) ;
			POPUP_NOTIFICATION(dbg, UNIVERSAL_NOTIFICATION_ADDR2, data1, strlen( (char *)data1)) ;

//			mem_W25qxx_Driver_sectorErase(ADMIN_ADDR);
//			HAL_Delay(1000) ;
			array[0] = '4' ;
			mem_strcpy(1, array, 11, 0, auth_username_t, strlen( (char *)auth_username_t));
			mem_strcpy(11, array, 21, 0, auth_PassWord_t, strlen( (char *)auth_PassWord_t) );
//			mem_W25qxx_Driver_writeBytes(ADMIN_ADDR, array, 21) ;


			 uint8_t swap_addr = ( i/16 ) + 1 ;//decide which sector it is ..  // will tell which sector
			mem_W25qxx_Driver_SwapSector_Target_to_Destiny(MULTIPLE_ADMIN_ADDR * (swap_addr));
			HAL_Delay(200) ;
			mem_W25qxx_Driver_SwapSector_Destiny_to_Target_for_userPassword_change(MULTIPLE_ADMIN_ADDR * (swap_addr), auth_username_t, auth_PassWord_t );
			HAL_Delay(200) ;

			uint8_t debugBuff[30] ;
			memset(dbg, 0, sizeof(dbg)) ;
			sprintf((char *)&debugBuff, "Updated") ;
			DWIN_PAGE_CHANGE1(dbg, NOTIFICATION_PAGE_NUMBER) ;
			DWIN_PAGE_CHANGE1(dbg, NOTIFICATION_PAGE_NUMBER) ;

			memset(dbg, 0, sizeof(dbg)) ;
			POPUP_NOTIFICATION(dbg, UNIVERSAL_NOTIFICATION_ADDR, debugBuff, strlen( (char *) debugBuff) ) ;
			POPUP_NOTIFICATION(dbg, UNIVERSAL_NOTIFICATION_ADDR, debugBuff, strlen( (char *) debugBuff) ) ;

			break ;

		}
		else if( i == 50)
		{

			uint8_t debugBuff[30] ;
			memset(dbg, 0, sizeof(dbg)) ;
			sprintf((char *)&debugBuff, "Not matched") ;
			DWIN_PAGE_CHANGE1(dbg, NOTIFICATION_PAGE_NUMBER) ;
			DWIN_PAGE_CHANGE1(dbg, NOTIFICATION_PAGE_NUMBER) ;

			memset(dbg, 0, sizeof(dbg)) ;
			POPUP_NOTIFICATION(dbg, UNIVERSAL_NOTIFICATION_ADDR, debugBuff, strlen( (char *) debugBuff) ) ;
			POPUP_NOTIFICATION(dbg, UNIVERSAL_NOTIFICATION_ADDR, debugBuff, strlen( (char *) debugBuff) ) ;

		}


	}




}
//void mem_w25qxxDriver_password_change(uint8_t *old_auth_PassWord_t, uint8_t *auth_PassWord_t)
//{
//	  uint8_t array[21] ;
//	  uint8_t auth_UserName_t[] = "ADMIN" ;
//	  uint8_t _userPassword[11] ;
//	  uint8_t k=0 ;
//	  uint8_t _dbg[50] ;
//
//	mem_W25qxx_Driver_ReadBytes( ADMIN_ADDR, array, 21);
//
//	for(uint8_t i=11, j =0 ; i<21; i++, j++)
//	{
//		_userPassword[j] = array[i] ;
//	}
//
//
//	if(strlen( (char *)_userPassword) == strlen( (char *)old_auth_PassWord_t))
//	{
//		for( k=0; k<strlen( (char *)_userPassword); k++)
//		{
//			if(_userPassword[k] != old_auth_PassWord_t[k])
//			{
//				break ;
//			}
//
//
//
//		}
//	}
//
//	if(k == strlen( (char *)_userPassword))
//	{
//		mem_W25qxx_Driver_sectorErase(ADMIN_ADDR);
//		HAL_Delay(1000) ;
//		  array[0] = '1' ;
//		  mem_strcpy(1, array, 11, 0, auth_UserName_t, strlen( (char *)auth_UserName_t));
//		  mem_strcpy(11, array, 21, 0, auth_PassWord_t, strlen( (char *)auth_PassWord_t) );
//		  mem_W25qxx_Driver_writeBytes(ADMIN_ADDR, array, 21) ;
//
//
////		  uint8_t _dbg[50] ;
//		  sprintf((char *)&_dbg, "updated.") ;
//		  POPUP_NOTOFICATION_WITHSTRING(UNIVERSAL_NOTIFICATION_ADDR, NOTIFICATION_PAGE_NUMBER, _dbg) ;
//
//	}
//	else
//	{
//		memset(_dbg, 0, sizeof(_dbg)) ;
//		sprintf((char *)&_dbg, " not matched") ;
//		POPUP_NOTOFICATION_WITHSTRING(UNIVERSAL_NOTIFICATION_ADDR, NOTIFICATION_PAGE_NUMBER, _dbg) ;
//
//	}
//
//
//
//}

uint8_t mem_w25qxxDriver_Admin_write(uint8_t *auth_UserName_t, uint8_t *auth_PassWord_t )
{
	  uint8_t array[21] ;
	  array[0] = '1' ;
	  mem_strcpy(1, array, 11, 0, auth_UserName_t, strlen((char *)auth_UserName_t));
	  mem_strcpy(11, array, 21, 0, auth_PassWord_t, strlen((char *)auth_PassWord_t) );
	  mem_W25qxx_Driver_writeBytes(ADMIN_ADDR, array, 21) ;
	  HAL_Delay(1000) ;

	  memset(array, 0, sizeof(21)) ;
	  mem_W25qxx_Driver_ReadBytes(ADMIN_ADDR, array, 21) ;
	  HAL_Delay(1000) ;
	  HAL_UART_Transmit(&huart3, array, 21, HAL_MAX_DELAY);

	  return 1 ;

}

void mem_w25qxxDriver_multi_Admin_write(uint8_t *auth_UserName_t, uint8_t *auth_PassWord_t)
{

		uint8_t dbg[40] ;

	#ifdef ERASE_MULTIADMIN

		mem_W25qxx_Driver_sectorErase(MULTIADMIN_INDEX_ADDR);
		HAL_Delay(200) ;
		mem_W25qxx_Driver_sectorErase(MULTIPLE_ADMIN_ADDR);
		HAL_Delay(200) ;
		sprintf((char *)&dbg,"MULTIADMIN sector erased\n");
		HAL_UART_Transmit(&huart3, dbg, strlen((const char *)dbg), HAL_MAX_DELAY);

	#endif

	#ifndef  ERASE_MULTIADMIN


		memset(dbg, 0, sizeof(dbg)) ;
		uint8_t data1[] = "Please wait." ;

		DWIN_PAGE_CHANGE1(dbg, 0X24) ;
		DWIN_PAGE_CHANGE1(dbg, 0X24) ;

		memset(dbg, 0, sizeof(dbg)) ;
		POPUP_NOTIFICATION(dbg, UNIVERSAL_NOTIFICATION_ADDR2, data1, sizeof(data1)) ;
		POPUP_NOTIFICATION(dbg, UNIVERSAL_NOTIFICATION_ADDR2, data1, sizeof(data1)) ;

		uint8_t array1[20] ;


		uint16_t cunt = mem_W25qxx_Driver_ReadByte(MULTIADMIN_INDEX_ADDR) ;
		if( cunt == 255 )
		{
			cunt = 0 ;
		}

		uint8_t swap_addr = ( ( (cunt) )/16 ) + 1 ;
	    sprintf((char *)&array1,"hex: %x, %d\n",(MULTIPLE_ADMIN_ADDR *swap_addr), cunt);
//	    HAL_UART_Transmit(&huart3, array1, strlen((char *)array1), HAL_MAX_DELAY);

		if( cunt <= 50)
		{
		  uint8_t array[23] ;

		  array[0] = '4' ;
		  mem_strcpy(1, array, 11, 0, auth_UserName_t,11);
		  mem_strcpy(11, array, 21, 0, auth_PassWord_t, 11 );



		  mem_W25qxx_Driver_writeBytes(MULTIPLE_ADMIN_ADDR + (cunt * PAGE_SIZE), array, 21) ;
//		  read_multiadmin((uint8_t *)&cunt) ;
		  HAL_Delay(1000) ;
		  cunt++ ;
		  mem_W25qxx_Driver_sectorErase(MULTIADMIN_INDEX_ADDR);
		  HAL_Delay(200) ;
		  mem_W25qxx_Driver_writeByte(MULTIADMIN_INDEX_ADDR, cunt) ;


		uint8_t debugBuff[30] ;
		memset(dbg, 0, sizeof(dbg)) ;
		sprintf((char *)&debugBuff, "#Added %s",
				  auth_UserName_t) ;
		DWIN_PAGE_CHANGE1(dbg, NOTIFICATION_PAGE_NUMBER) ;
		DWIN_PAGE_CHANGE1(dbg, NOTIFICATION_PAGE_NUMBER) ;

		memset(dbg, 0, sizeof(dbg)) ;
		POPUP_NOTIFICATION(dbg, UNIVERSAL_NOTIFICATION_ADDR, debugBuff, strlen( (char *) debugBuff) ) ;
		POPUP_NOTIFICATION(dbg, UNIVERSAL_NOTIFICATION_ADDR, debugBuff, strlen( (char *) debugBuff) ) ;


		}


	#endif

}
static uint8_t mem_w25qxxDriver_multiple_Admins_verify(uint8_t *auth_UserName_t, uint8_t *auth_PassWord_t )
{
	uint8_t _userName[11] ;
	uint8_t _userPassword[11] ;
	uint8_t array[22];
	uint8_t ulen = 0;
	uint8_t plen = 0;
	uint8_t c = 0;
	uint8_t i =0 ;

	for(uint8_t x=0; x <50; x++)
	{
		mem_W25qxx_Driver_ReadBytes( ( MULTIPLE_ADMIN_ADDR + (x * PAGE_SIZE) ), array, 21);
//		HAL_UART_Transmit(&huart1, array, 21, HAL_MAX_DELAY);
		for(uint8_t i=0, j=1; j<11; i++, j++)
		{
			_userName[i] = array[j] ;
		}


		for(uint8_t i=11, j =0 ; i<21; i++, j++)
		{
			_userPassword[j] = array[i] ;
		}


		mem_find_strlen(_userName,&ulen);
		mem_find_strlen(_userPassword,&plen);

		if(ulen == strlen( (char *)auth_UserName_t))
		{
			for( i=0; _userName[i] != '\0' ; i++)
			{
				if(_userName[i] == auth_UserName_t[i])
				{
					c = 8 ;
				}
				else
				{
					c = 7 ;//multiple admin username miss matched
					break ;
				}
			}
			if(plen == strlen( (char *)auth_PassWord_t))
			{
				if(c == 8 )
				{
					for( i=0 ; _userPassword[i] != '\0' ; i++)
					{

						if(_userPassword[i] == auth_PassWord_t[i])
						{
							c = array[0] ;
						}
						else
						{
							c =  9;//multiple admin password missmatch
							break ;
						}
					}
				}
			}
		}
		if ( c ==  array[0])
			break ;
	}
	return c ;

}


uint8_t mem_w25qxxDriver_Admin_verify(uint8_t *auth_UserName_t, uint8_t *auth_PassWord_t )
{
	uint8_t _userName[11] ;
	uint8_t _userPassword[11] ;
	uint8_t array[21];
	uint8_t ulen = 0;
	uint8_t plen = 0;
	uint8_t c = 0;
	uint8_t i= 0;

#ifdef SHOW_ADMIN_USERNAME_PASSWORD
	uint8_t dbg[50] ;
	sprintf(dbg, "entered details:\n") ;
	strcat(dbg,auth_UserName_t );
	strcat(dbg,", ");
	strcat(dbg,auth_PassWord_t) ;
	strcat(dbg, "\n from flash :\n") ;
	HAL_UART_Transmit(&huart3, dbg, strlen(dbg), HAL_MAX_DELAY);
	memset(dbg, 0, sizeof(dbg)) ;
#endif
	mem_W25qxx_Driver_ReadBytes( ADMIN_ADDR, array, 21);
#ifdef SHOW_ADMIN_USERNAME_PASSWORD

		HAL_UART_Transmit(&huart3, array, 21, HAL_MAX_DELAY);
#endif
	for(uint8_t i=0, j=1; j<11; i++, j++)
	{
		_userName[i] = array[j] ;
	}

	for(uint8_t i=11, j =0 ; i<21; i++, j++)
	{
		_userPassword[j] = array[i] ;
	}


	mem_find_strlen(_userName,&ulen);
	mem_find_strlen(_userPassword,&plen);

	if(ulen == strlen( (char *)auth_UserName_t))
	{
		for( i=0; _userName[i] != '\0' ; i++)
		{
			if(_userName[i] == auth_UserName_t[i])
			{
				c = 8 ;
			}
			else
			{
				c = 7 ;//admin username miss matched
				break ;
			}
		}
		if(plen == strlen( (char *)auth_PassWord_t))
		{
			if(c == 8 )
			{
				for( i=0 ; _userPassword[i] != '\0' ; i++)
				{

					if(_userPassword[i] == auth_PassWord_t[i])
					{
						c = array[0] ;
					}
					else
					{
						c =  9;//admin password missmatch
						break ;
					}
				}
			}
		}
	}
	return c ;


}

void mem_w25qxxDriver_Admin_password_change(uint8_t *auth_PassWord_t)
{
	mem_W25qxx_Driver_sectorErase(ADMIN_ADDR);
	HAL_Delay(200) ;
	uint8_t auth_UserName_t[] = "ADMIN" ;
	  uint8_t array[21] ;
	  array[0] = '1' ;
	  mem_strcpy(1, array, 11, 0, auth_UserName_t, strlen( (char *)auth_UserName_t));
	  mem_strcpy(11, array, 21, 0, auth_PassWord_t, strlen( (char *)auth_PassWord_t) );
	  mem_W25qxx_Driver_writeBytes(ADMIN_ADDR, array, 21) ;
	  HAL_Delay(200) ;

//	  HAL_UART_Transmit(&huart3, &array, 21, HAL_MAX_DELAY);

}

/*** @#########################################@@ ADMIN AUTHENTICATION CODE.####################################################################@ ***/


/*** @#############################################################################################################################################################@ ***/
/*** @@ SUPERVISOR AUTHENTICATION CODE BELOW.  ***/
/*** @#############################################################################################################################################################@ ***/


void read_multiadmin(uint8_t *counter)
{
	uint8_t readdata[22];
	mem_W25qxx_Driver_ReadBytes( ( MULTIPLE_ADMIN_ADDR + (*counter * PAGE_SIZE) ), readdata, 21);
	HAL_Delay(200) ;
	HAL_UART_Transmit(&huart3, readdata, 21, HAL_MAX_DELAY);
}

void read_supervisor(uint8_t *counter)
{
	uint8_t readdata[22];
	mem_W25qxx_Driver_ReadBytes( ( SUPERUSER_ADDR + (*counter * PAGE_SIZE) ), readdata, 21);
	HAL_Delay(200) ;
	HAL_UART_Transmit(&huart3, readdata, 21, HAL_MAX_DELAY);
}
void mem_w25qxxDriver_read_supervisor(uint8_t counter, uint8_t *op)
{
	mem_W25qxx_Driver_ReadBytes( ( SUPERUSER_ADDR + (counter * PAGE_SIZE) ), op, 21);
//	HAL_UART_Transmit(&huart3, op, 21, HAL_MAX_DELAY);
}

void mem_w25qxxDriver_read_multiadmin(uint8_t counter, uint8_t *op)
{
	mem_W25qxx_Driver_ReadBytes( ( MULTIPLE_ADMIN_ADDR + (counter * PAGE_SIZE) ), op, 21);
//	HAL_UART_Transmit(&huart3, op, 21, HAL_MAX_DELAY);
}

void mem_w25qxxDriver_read_endUser(uint8_t counter, uint8_t *op)
{
	mem_W25qxx_Driver_ReadBytes( ( USER_ADDR + (counter * PAGE_SIZE) ), op, 21);
//	HAL_UART_Transmit(&huart3, op, 21, HAL_MAX_DELAY);
}


void delete_supervisor_w_name(uint8_t *counter, uint8_t *op, uint8_t *supervisorDeleteIndex)
{
	uint8_t array[22] ;
	uint8_t _userName[12] ;
	for(uint8_t i=0;i<5; i++)
	{
		mem_w25qxxDriver_read_supervisor(i,array) ;
		memcpy(_userName, &array[1], 11) ;
		uint8_t j=0 ;
		if(strlen( (char *)op) == strlen( (char *)_userName))
		{
			for(j=0; j<strlen( (char *)_userName); j++)
			{
//				HAL_UART_Transmit(&huart3, &_userName[j], 1, HAL_MAX_DELAY);
				if(op[j] == _userName[j])
				{
//					HAL_UART_Transmit(&huart3, &_userName[j], 1, HAL_MAX_DELAY);

				}
				else
				{
					break ;
//					break ;
				}

			}

			if(j == strlen( (char *)_userName))
			{
				*counter = i ;
				*supervisorDeleteIndex = 9 ;
			}
		}
	}
}

void read_enduser(uint8_t counter)
{
	uint8_t readdata[22];
	mem_W25qxx_Driver_ReadBytes( ( USER_ADDR + (counter * PAGE_SIZE) ), readdata, 21);

	HAL_UART_Transmit(&huart3, readdata, 21, HAL_MAX_DELAY);
}

uint8_t mem_w25qxxDriver_supervisor_write(uint8_t *auth_UserName_t, uint8_t *auth_PassWord_t)
{

	uint8_t dbg[40] ;

#ifdef ERASE_SUPERVISOR


	mem_W25qxx_Driver_sectorErase(SUPERVISOR_INDEX_ADDR);
	HAL_Delay(200) ;
	mem_W25qxx_Driver_sectorErase(SUPERUSER_ADDR);
	HAL_Delay(200) ;
	sprintf((char *)&dbg,"Aupervisor sector erased\n");
	HAL_UART_Transmit(&huart3, dbg, strlen((const char *)dbg), HAL_MAX_DELAY);

#endif

	memset(dbg, 0, sizeof(dbg)) ;
	uint8_t data1[] = "Please wait." ;

	DWIN_PAGE_CHANGE1(dbg, 0X24) ;
	DWIN_PAGE_CHANGE1(dbg, 0X24) ;

	memset(dbg, 0, sizeof(dbg)) ;
	POPUP_NOTIFICATION(dbg, UNIVERSAL_NOTIFICATION_ADDR2, data1, sizeof(data1)) ;
	POPUP_NOTIFICATION(dbg, UNIVERSAL_NOTIFICATION_ADDR2, data1, sizeof(data1)) ;


#ifndef ERASE_SUPERVISOR
	uint8_t array1[20] ;

	uint16_t cunt = mem_W25qxx_Driver_ReadByte(SUPERVISOR_INDEX_ADDR) ;
	if( cunt == 255 )
	{
		cunt = 0 ;
	}

	uint8_t swap_addr = ( ( (cunt) )/16 ) + 1 ;
    sprintf((char *)&array1,"hex: %x, %d\n",(SUPERUSER_ADDR*swap_addr), cunt);
//    HAL_UART_Transmit(&huart3, array1, strlen((char *)array1), HAL_MAX_DELAY);

	if( cunt <= 80)
	{
	  uint8_t array[23] ;

	  array[0] = '2' ;
	  mem_strcpy(1, array, 11, 0, auth_UserName_t,11);
	  mem_strcpy(11, array, 21, 0, auth_PassWord_t, 11 );



	  mem_W25qxx_Driver_writeBytes(SUPERUSER_ADDR + (cunt * PAGE_SIZE), array, 21) ;
//	  read_supervisor(( uint8_t *)&cunt) ;
	  HAL_Delay(1000) ;
	  cunt++ ;
	  mem_W25qxx_Driver_sectorErase(SUPERVISOR_INDEX_ADDR);
	  HAL_Delay(200) ;
	  mem_W25qxx_Driver_writeByte(SUPERVISOR_INDEX_ADDR, cunt) ;


	  uint8_t debugBuff[30] ;
	  memset(dbg, 0, sizeof(dbg)) ;
	  sprintf((char *)&debugBuff, "#Added %s",
				  auth_UserName_t) ;
	  DWIN_PAGE_CHANGE1(dbg, NOTIFICATION_PAGE_NUMBER) ;
	  DWIN_PAGE_CHANGE1(dbg, NOTIFICATION_PAGE_NUMBER) ;

	  memset(dbg, 0, sizeof(dbg)) ;
	  POPUP_NOTIFICATION(dbg, UNIVERSAL_NOTIFICATION_ADDR, debugBuff, strlen( (char *) debugBuff) ) ;
	  POPUP_NOTIFICATION(dbg, UNIVERSAL_NOTIFICATION_ADDR, debugBuff, strlen( (char *) debugBuff) ) ;


	}
	else
	{
		  uint8_t debugBuff[30] ;
		  memset(dbg, 0, sizeof(dbg)) ;
		  sprintf((char *)&debugBuff, "Limit reached") ;
		  DWIN_PAGE_CHANGE1(dbg, NOTIFICATION_PAGE_NUMBER) ;
		  DWIN_PAGE_CHANGE1(dbg, NOTIFICATION_PAGE_NUMBER) ;

		  memset(dbg, 0, sizeof(dbg)) ;
		  POPUP_NOTIFICATION(dbg, UNIVERSAL_NOTIFICATION_ADDR, debugBuff, strlen( (char *) debugBuff) ) ;
		  POPUP_NOTIFICATION(dbg, UNIVERSAL_NOTIFICATION_ADDR, debugBuff, strlen( (char *) debugBuff) ) ;
	}


#endif

	return 1 ;

}




static uint8_t mem_w25qxxDriver_superuser_verify(uint8_t *auth_UserName_t, uint8_t *auth_PassWord_t )
{
	uint8_t _userName[11] ;
	uint8_t _userPassword[11] ;
	uint8_t array[22];
	uint8_t ulen = 0;
	uint8_t plen = 0;
	uint8_t c = 0;
	uint8_t i =0 ;

	for(uint8_t x=0; x <80; x++) //supervisor has only 5 sector in 2nd block of flash
	{
		mem_W25qxx_Driver_ReadBytes( ( SUPERUSER_ADDR + (x * PAGE_SIZE) ), array, 21);
//		HAL_UART_Transmit(&huart1, array, 21, HAL_MAX_DELAY);
		for(uint8_t i=0, j=1; j<11; i++, j++)
		{
			_userName[i] = array[j] ;
		}


		for(uint8_t i=11, j =0 ; i<21; i++, j++)
		{
			_userPassword[j] = array[i] ;
		}


		mem_find_strlen(_userName,&ulen);
		mem_find_strlen(_userPassword,&plen);

		if(ulen == strlen( (char *)auth_UserName_t))
		{
			for( i=0; _userName[i] != '\0' ; i++)
			{
				if(_userName[i] == auth_UserName_t[i])
				{
					c = 8 ;
				}
				else
				{
					c = 7 ;//superviosr username miss matched
					break ;
				}
			}
			if(plen == strlen( (char *)auth_PassWord_t))
			{
				if(c == 8 )
				{
					for( i=0 ; _userPassword[i] != '\0' ; i++)
					{

						if(_userPassword[i] == auth_PassWord_t[i])
						{
							c = array[0] ;
						}
						else
						{
							c =  9;//supervisor password missmatch
							break ;
						}
					}
				}
			}
		}
		if ( c ==  array[0])
			break ;
	}
	return c ;

}


/*** @####################################################@@ SUPERVISOR AUTHENTICATION CODE.########################################################@ ***/


/*** @#############################################################################################################################################################@ ***/
/*** @@ USER AUTHENTICATION CODE BELOW.  ***/
/*** @#############################################################################################################################################################@ ***/

void mem_w25qxxDriver_user_password_change(uint64_t userAddr, uint8_t *auth_UserName_t, uint8_t *auth_PassWord_t, uint8_t *newpassword)
{
	uint8_t _userName[11] ;
	uint8_t _userPassword[11] ;
	uint8_t array[22];
	uint8_t ulen = 0;
	uint8_t plen = 0;
	uint8_t c = 0;
	uint8_t i =0 ;
	uint8_t temp = 0;
	uint8_t x=0 ;

	if( userAddr == USER_ADDR)
	{
		temp = 150 ;
	}
	else if(userAddr == SUPERUSER_ADDR)
	{
		temp = 80;
	}
	for( x=0; x <temp ; x++) //supervisor has only 5 sector in 2nd block of flash
	{
		mem_W25qxx_Driver_ReadBytes( ( userAddr + (x * PAGE_SIZE) ), array, 21);
		HAL_Delay(10) ;
//		HAL_UART_Transmit(&huart1, array, 21, HAL_MAX_DELAY);
		for(uint8_t i=0, j=1; j<11; i++, j++)
		{
			_userName[i] = array[j] ;
		}
//		HAL_UART_Transmit(&huart1, _userName, strlen(_userName), HAL_MAX_DELAY);
//		HAL_UART_Transmit(&huart1, auth_UserName_t, strlen(_userName), HAL_MAX_DELAY);

		for(uint8_t i=11, j =0 ; i<21; i++, j++)
		{
			_userPassword[j] = array[i] ;
		}
//		HAL_UART_Transmit(&huart1, _userPassword, strlen(_userPassword), HAL_MAX_DELAY);
//		HAL_UART_Transmit(&huart1, auth_PassWord_t, strlen(auth_PassWord_t), HAL_MAX_DELAY);

		mem_find_strlen(_userName,&ulen);
		mem_find_strlen(_userPassword,&plen);

		if(ulen == strlen( (char *)auth_UserName_t))
		{
			for( i=0; _userName[i] != '\0' ; i++)
			{
				if(_userName[i] == auth_UserName_t[i])
				{
					c = 8 ;
				}
				else
				{
					c = 7 ;//admin username miss matched
					break ;
				}
			}
			if(plen == strlen( (char *)auth_PassWord_t))
			{
				if(c == 8 )
				{
					for( i=0 ; _userPassword[i] != '\0' ; i++)
					{

						if(_userPassword[i] == auth_PassWord_t[i])
						{
							c = array[0] ;
						}
						else
						{
							c =  9;//admin password missmatch
							break ;
						}
					}
				}
			}
			if ( c ==  array[0])
			{
				uint8_t dbg[40] ;
				memset(dbg, 0, sizeof(dbg)) ;
				uint8_t data1[] = "Please wait.." ;
				DWIN_PAGE_CHANGE(0X24) ; //
				POPUP_NOTIFICATION(dbg, UNIVERSAL_NOTIFICATION_ADDR2, data1, strlen( (char *)data1)) ;

				uint8_t swap_addr = ( i/16 ) + 1 ;//decide which sector it is ..  // will tell which sector
				mem_W25qxx_Driver_SwapSector_Target_to_Destiny(userAddr * (swap_addr));
				HAL_Delay(500) ;
				mem_W25qxx_Driver_SwapSector_Destiny_to_Target_for_userPassword_change(userAddr * (swap_addr), auth_UserName_t, newpassword );
				HAL_Delay(500) ;


				memset(dbg, 0, sizeof(dbg)) ;
				uint8_t data[] = "Updated.." ;
				DWIN_PAGE_CHANGE(NOTIFICATION_PAGE_NUMBER );
				POPUP_NOTIFICATION(dbg, UNIVERSAL_NOTIFICATION_ADDR, data, strlen( (char *)data)) ;

				break ;


			}

		}
	}

	if( x == temp ){
		uint8_t dbg[40] ;
		memset(dbg, 0, sizeof(dbg)) ;
		uint8_t data[] = "P/W not matched." ;
		DWIN_PAGE_CHANGE(NOTIFICATION_PAGE_NUMBER );
		POPUP_NOTIFICATION(dbg, UNIVERSAL_NOTIFICATION_ADDR, data, strlen((char *)data)) ;
	}

}


uint8_t mem_w25qxxDriver_user_write(uint8_t *auth_UserName_t, uint8_t *auth_PassWord_t, uint8_t *counter)
{

	uint8_t array1[30] ;
	uint8_t dbg[40] ;


		#ifdef ERASE_ENDUSER

			mem_W25qxx_Driver_sectorErase(USER_INDEX_ADDR);
			HAL_Delay(200) ;
			mem_W25qxx_Driver_sectorErase(USER_ADDR);
			HAL_Delay(200) ;
			sprintf((char *)&array1,"end user sector erased\n");
			HAL_UART_Transmit(&huart3, array1, strlen((const char *)array1), HAL_MAX_DELAY);

		#endif

		#ifndef ERASE_ENDUSER

			memset(dbg, 0, sizeof(dbg)) ;
			uint8_t data1[] = "Please wait." ;

			DWIN_PAGE_CHANGE1(dbg, 0X24) ;
			DWIN_PAGE_CHANGE1(dbg, 0X24) ;

			memset(dbg, 0, sizeof(dbg)) ;
			POPUP_NOTIFICATION(dbg, UNIVERSAL_NOTIFICATION_ADDR2, data1, sizeof(data1)) ;
			POPUP_NOTIFICATION(dbg, UNIVERSAL_NOTIFICATION_ADDR2, data1, sizeof(data1)) ;

			uint16_t cunt = mem_W25qxx_Driver_ReadByte(USER_INDEX_ADDR) ;
			if( cunt == 255 )
			{
				cunt = 0 ;
			}

			uint8_t swap_addr = ( ( (cunt) )/16 ) + 1 ;
		    sprintf((char *)&array1,"hex: %x, %d\n",(USER_ADDR*swap_addr), cunt);
//		    HAL_UART_Transmit(&huart3, array1, strlen((char *)array1), HAL_MAX_DELAY);

			if( cunt <= 150)
			{
			  uint8_t array[23] ;

			  array[0] = '3' ;
			  mem_strcpy(1, array, 11, 0,auth_UserName_t,11);
			  mem_strcpy(11, array, 21,0, auth_PassWord_t, 11 );



			  mem_W25qxx_Driver_writeBytes(USER_ADDR + (cunt * PAGE_SIZE), array, 21) ;
//			  read_enduser(cunt) ;
			  HAL_Delay(1000) ;
			  cunt++ ;
			  mem_W25qxx_Driver_sectorErase(USER_INDEX_ADDR);
			  HAL_Delay(200) ;
			  mem_W25qxx_Driver_writeByte(USER_INDEX_ADDR, cunt) ;


			  uint8_t debugBuff[30] ;
			  memset(dbg, 0, sizeof(dbg)) ;
			  sprintf((char *)&debugBuff, "#Added %s",
						  auth_UserName_t) ;
			  DWIN_PAGE_CHANGE1(dbg, NOTIFICATION_PAGE_NUMBER) ;
			  DWIN_PAGE_CHANGE1(dbg, NOTIFICATION_PAGE_NUMBER) ;

			  memset(dbg, 0, sizeof(dbg)) ;
			  POPUP_NOTIFICATION(dbg, UNIVERSAL_NOTIFICATION_ADDR, debugBuff, strlen( (char *) debugBuff) ) ;
			  POPUP_NOTIFICATION(dbg, UNIVERSAL_NOTIFICATION_ADDR, debugBuff, strlen( (char *) debugBuff) ) ;

			}
			else
			{

				  uint8_t debugBuff[30] ;
				  memset(dbg, 0, sizeof(dbg)) ;
				  sprintf((char *)&debugBuff, "Limit reached") ;
				  DWIN_PAGE_CHANGE1(dbg, NOTIFICATION_PAGE_NUMBER) ;
				  DWIN_PAGE_CHANGE1(dbg, NOTIFICATION_PAGE_NUMBER) ;

				  memset(dbg, 0, sizeof(dbg)) ;
				  POPUP_NOTIFICATION(dbg, UNIVERSAL_NOTIFICATION_ADDR, debugBuff, strlen( (char *) debugBuff) ) ;
				  POPUP_NOTIFICATION(dbg, UNIVERSAL_NOTIFICATION_ADDR, debugBuff, strlen( (char *) debugBuff) ) ;
			}


		#endif

			return 1 ;

}

static uint8_t mem_w25qxxDriver_endUser_verify(uint8_t *auth_UserName_t, uint8_t *auth_PassWord_t )
{
	uint8_t _userName[11] ;
	uint8_t _userPassword[11] ;
	uint8_t array[22];
	uint8_t ulen = 0;
	uint8_t plen = 0;
	uint8_t c = 0;
	uint8_t i =0 ;
	for(uint8_t x=0; x <150 ; x++) //supervisor has only 5 sector in 2nd block of flash
	{
		mem_W25qxx_Driver_ReadBytes( ( USER_ADDR + (x * PAGE_SIZE) ), array, 21);
//		HAL_UART_Transmit(&huart1, array, 21, HAL_MAX_DELAY);
		for(uint8_t i=0, j=1; j<11; i++, j++)
		{
			_userName[i] = array[j] ;
		}
//		HAL_UART_Transmit(&huart1, _userName, strlen(_userName), HAL_MAX_DELAY);
//		HAL_UART_Transmit(&huart1, auth_UserName_t, strlen(_userName), HAL_MAX_DELAY);

		for(uint8_t i=11, j =0 ; i<21; i++, j++)
		{
			_userPassword[j] = array[i] ;
		}
//		HAL_UART_Transmit(&huart1, _userPassword, strlen(_userPassword), HAL_MAX_DELAY);
//		HAL_UART_Transmit(&huart1, auth_PassWord_t, strlen(auth_PassWord_t), HAL_MAX_DELAY);

		mem_find_strlen(_userName,&ulen);
		mem_find_strlen(_userPassword,&plen);

		if(ulen == strlen( (char *)auth_UserName_t))
		{
			for( i=0; _userName[i] != '\0' ; i++)
			{
				if(_userName[i] == auth_UserName_t[i])
				{
					c = 8 ;
				}
				else
				{
					c = 7 ;//admin username miss matched
					break ;
				}
			}
			if(plen == strlen( (char *)auth_PassWord_t))
			{
				if(c == 8 )
				{
					for( i=0 ; _userPassword[i] != '\0' ; i++)
					{

						if(_userPassword[i] == auth_PassWord_t[i])
						{
							c = array[0] ;
						}
						else
						{
							c =  9;//admin password missmatch
							break ;
						}
					}
				}
			}
		}
		if ( c ==  array[0])
			break ;
	}
	return c ;

}



/*** @####################################################@@ USER AUTHENTICATION CODE.########################################################@ ***/

/*** #################################################################################################################################################### ***/
/*** @@ RP203 THERMAL PRINTER CODE BELOW. ***/
/*** #################################################################################################################################################### ***/

void mem_w25qxxDriver_read_device_details(uint8_t *deviceModel, uint8_t *deviceVersion, uint8_t *clientcompanyName, uint8_t *clientcompanyLocation)
{
	mem_W25qxx_Driver_ReadBytes( ( FACTORY_ADDR + SECTOR_SIZE ), deviceModel, 10);
	HAL_Delay(200) ;
	mem_W25qxx_Driver_ReadBytes( ( FACTORY_ADDR + SECTOR_SIZE + 26), deviceVersion, 10);
	HAL_Delay(200) ;
	mem_W25qxx_Driver_ReadBytes( ( FACTORY_ADDR + SECTOR_SIZE + 52), clientcompanyName, 16);
	HAL_Delay(200) ;
	mem_W25qxx_Driver_ReadBytes( ( FACTORY_ADDR + SECTOR_SIZE + 78), clientcompanyLocation, 16);
	HAL_Delay(200) ;


}

void mem_w25qxxDriver_write_device_details(uint8_t *deviceModel, uint8_t *deviceVersion, uint8_t *clientcompanyName,
		uint8_t *clientcompanyLocation, uint8_t *installedDate, uint8_t *fixedmmHG)
{

	uint8_t dbg[40] ;
	memset(dbg, 0, sizeof(dbg)) ;
	uint8_t data[] = "Please wait.." ;

	mem_W25qxx_Driver_sectorErase(FACTORY_ADDR + SECTOR_SIZE);
	HAL_Delay(200) ;

	DWIN_PAGE_CHANGE(0X24) ;
	DWIN_PAGE_CHANGE(0X24) ;
	POPUP_NOTIFICATION(dbg, UNIVERSAL_NOTIFICATION_ADDR2, data, strlen( (char *)data)) ;
	POPUP_NOTIFICATION(dbg, UNIVERSAL_NOTIFICATION_ADDR2, data, strlen( (char *)data)) ;

	mem_W25qxx_Driver_writeBytes( ( FACTORY_ADDR + SECTOR_SIZE ), deviceModel, 10);
	HAL_Delay(200) ;



	mem_W25qxx_Driver_writeBytes( ( FACTORY_ADDR + SECTOR_SIZE + 26), deviceVersion, 10);
	HAL_Delay(200) ;


	mem_W25qxx_Driver_writeBytes( ( FACTORY_ADDR + SECTOR_SIZE + 52), clientcompanyName, 16);
	HAL_Delay(200) ;


	mem_W25qxx_Driver_writeBytes( ( FACTORY_ADDR + SECTOR_SIZE + 78), clientcompanyLocation, 16);
	HAL_Delay(200) ;


	mem_W25qxx_Driver_writeBytes( ( FACTORY_ADDR + SECTOR_SIZE + 104), installedDate, 10);
	HAL_Delay(200) ;


	mem_W25qxx_Driver_writeBytes( ( FACTORY_ADDR + SECTOR_SIZE + 130), fixedmmHG, 4);
	HAL_Delay(200) ;




	  memset(dbg, 0, sizeof(dbg)) ;
	  uint8_t data1[] = "Updated" ;
	  DWIN_PAGE_CHANGE1(dbg, NOTIFICATION_PAGE_NUMBER) ;
	  DWIN_PAGE_CHANGE1(dbg, NOTIFICATION_PAGE_NUMBER) ;

	  memset(dbg, 0, sizeof(dbg)) ;
	  POPUP_NOTIFICATION(dbg, UNIVERSAL_NOTIFICATION_ADDR, data1, strlen( (char *) data1) ) ;
	  POPUP_NOTIFICATION(dbg, UNIVERSAL_NOTIFICATION_ADDR, data1, strlen( (char *) data1) ) ;


}

//CHANGE:20-05
void  mem_w25qxxDriver_read_fixed_mmhg(uint8_t *arr)
{

	mem_W25qxx_Driver_ReadBytes( ( FACTORY_ADDR + SECTOR_SIZE + 130), arr, 4);
	HAL_Delay(200) ;
}


/*** @####################################################@@ RP203 THERMAL PRINTER CODE. ########################################################@ ***/


/*** #################################################################################################################################################### ***/
/*** @@ IRRESPECTIVE OF AUTHENTICATION STAGES.  ***/
/*** #################################################################################################################################################### ***/
void mem_w25qxxDriver_user_verify(uint8_t *auth_UserName_t, uint8_t *auth_PassWord_t , uint8_t *pageflag)
{
	uint8_t _zFlag = 0 ;
	uint8_t trueFlag = 0 ;

	_zFlag = mem_w25qxxDriver_Admin_verify(auth_UserName_t, auth_PassWord_t) ;
//	HAL_UART_Transmit(&huart3, &_zFlag, 1, HAL_MAX_DELAY);
	if( (_zFlag - '0' ) == 1)
	{
		*pageflag = 1 ;
		trueFlag = 1;

	}
	else
	{
		_zFlag = 0 ;

	}

	_zFlag = mem_w25qxxDriver_multiple_Admins_verify(auth_UserName_t, auth_PassWord_t) ;
//	HAL_UART_Transmit(&huart3, &_zFlag, 1, HAL_MAX_DELAY);
	if( (_zFlag - '0' ) == 4)
	{
		*pageflag = 4 ;
		trueFlag = 1;
	}
	else
	{
		_zFlag = 0 ;

	}// mem_w25qxxDriver_multiple_Admins_verify(uint8_t *auth_UserName_t, uint8_t *auth_PassWord_t )

	_zFlag = mem_w25qxxDriver_superuser_verify(auth_UserName_t, auth_PassWord_t );
//	HAL_UART_Transmit(&huart3, &_zFlag, 1, HAL_MAX_DELAY);
	if( (_zFlag - '0' ) == 2 )
	{
		*pageflag = 2 ;
		trueFlag = 1;
	}
	else
	{
		_zFlag = 0 ;
	}
	_zFlag = mem_w25qxxDriver_endUser_verify(auth_UserName_t, auth_PassWord_t );
//	HAL_UART_Transmit(&huart3, &_zFlag, 1, HAL_MAX_DELAY);
	if( (_zFlag - '0' ) == 3)
	{
		*pageflag = 3 ;
		trueFlag = 1;
	}
	else
	{
		_zFlag = 0 ;

	}

	if( (_zFlag == 0) && (trueFlag == 0)  )
	{
		uint8_t dbg[40] ;
		memset(dbg, 0, sizeof(dbg)) ;
		uint8_t data1[] = "Invalid Details" ;
		memset(dbg, 0, sizeof(dbg)) ;
		DWIN_PAGE_CHANGE1(dbg, 0X24) ;

		POPUP_NOTIFICATION(dbg, UNIVERSAL_NOTIFICATION_ADDR2, data1, sizeof(data1)) ;
		POPUP_NOTIFICATION(dbg, UNIVERSAL_NOTIFICATION_ADDR2, data1, sizeof(data1)) ;
		HAL_Delay(5000) ;

		DWIN_PAGE_CHANGE1(dbg, AUTH_PAGE_NUMBER) ;
		DWIN_PAGE_CHANGE1(dbg, AUTH_PAGE_NUMBER) ;

	}







}

/*** #################################################@ IRRESPECTIVE OF AUTHENTICATION STAGES############################################################# ***/


void deleteTestProfile(uint32_t addr, uint8_t *auth_UserName_t, uint8_t counter)
{
	 uint8_t swap_addr = ( ( (counter) )/16 ) + 1 ;//decide which sector it is ..  // will tell which sector
	mem_W25qxx_Driver_SwapSector_Target_to_Destiny(addr * (swap_addr));
	HAL_Delay(50) ;
	mem_W25qxx_Driver_SwapSector_Destiny_to_Target_for_TestProfile(addr * (swap_addr), auth_UserName_t);
	HAL_Delay(50) ;
}

void deleteAnyUser(uint32_t addr, uint8_t *auth_UserName_t, uint8_t counter)
{
	 uint8_t swap_addr = ( ( (counter) )/16 ) + 1 ;//decide which sector it is ..  // will tell which sector
	mem_W25qxx_Driver_SwapSector_Target_to_Destiny((addr * (swap_addr)) );
	HAL_Delay(50) ;
	mem_W25qxx_Driver_SwapSector_Destiny_to_Target( (addr * (swap_addr)) , auth_UserName_t);
	HAL_Delay(50) ;
//	uint8_t data[50];
//	sprintf(data, "addr: %x, username: %s, cnt: %d\n", addr, auth_UserName_t, counter) ;
//	HAL_UART_Transmit(&huart3, data, strlen(data), HAL_MAX_DELAY) ;
}

uint16_t mem_w25qxxDriver_getTestProfile_idx()
{
	return mem_W25qxx_Driver_ReadByte(SUPERVISOR_TEST_PROFILE_INDEX_ADDR) ;
}

void mem_w25qxxDriver_setTestProfile_idx(uint16_t cunt)
{
	if(cunt <220)
	{
		  mem_W25qxx_Driver_sectorErase(SUPERVISOR_TEST_PROFILE_INDEX_ADDR);
		  HAL_Delay(200) ;
		  mem_W25qxx_Driver_writeByte(SUPERVISOR_TEST_PROFILE_INDEX_ADDR, cunt) ;
	}
}

uint16_t mem_w25qxxDriver_getSupervisor_idx()
{
	return mem_W25qxx_Driver_ReadByte(SUPERVISOR_INDEX_ADDR) ;
}

uint16_t mem_w25qxxDriver_getEndUser_idx()
{
	return mem_W25qxx_Driver_ReadByte(USER_INDEX_ADDR) ;
}

uint16_t mem_w25qxxDriver_getMutiliAdmin_idx()
{
	return mem_W25qxx_Driver_ReadByte(MULTIADMIN_INDEX_ADDR) ;
}

void mem_w25qxxDriver_setSupervisor_idx(uint16_t cunt)
{
	if(cunt <220)
	{
	  mem_W25qxx_Driver_sectorErase(SUPERVISOR_INDEX_ADDR);
	  HAL_Delay(200) ;
	  mem_W25qxx_Driver_writeByte(SUPERVISOR_INDEX_ADDR, cunt) ;
	}
}

void mem_w25qxxDriver_setEndUser_idx(uint16_t cunt)
{
	if(cunt <220)
	{
		  mem_W25qxx_Driver_sectorErase(USER_INDEX_ADDR);
		  HAL_Delay(200) ;
		  mem_W25qxx_Driver_writeByte(USER_INDEX_ADDR, cunt) ;
	}
}

void mem_w25qxxDriver_MutliAdmin_idx(uint16_t cunt)
{
	if(cunt <220)
	{
		  mem_W25qxx_Driver_sectorErase(MULTIADMIN_INDEX_ADDR);
		  HAL_Delay(200) ;
		  mem_W25qxx_Driver_writeByte(MULTIADMIN_INDEX_ADDR, cunt) ;
	}
}

void mem_w25qxxDriver_Write_mmhg_GAUGEval(uint8_t *arr, uint8_t len)
{
	mem_W25qxx_Driver_sectorErase(MMHGGAUGEREFERENCE_ADDR);
	HAL_Delay(2000) ;
	mem_W25qxx_Driver_writeBytes( MMHGGAUGEREFERENCE_ADDR, arr,  len) ;
	HAL_Delay(200) ;
}

void mem_w25qxxDriver_Read_mmhg_GAUGEval(uint8_t *arr, uint8_t len)
{
	mem_W25qxx_Driver_ReadBytes( MMHGGAUGEREFERENCE_ADDR, arr,  len) ;
	HAL_Delay(100) ;
}

/*** #################################################@ RESULTS ############################################################# ***/

void mem_w25qxxDriver_ResultsPage_Read_nd_show_Results(uint8_t index)
{
	uint8_t data[69] ;
	memset(data, 0, sizeof(data)) ;
	uint8_t dbg[20] ;
	uint8_t buff[40] ;


	mem_W25qxx_Driver_ReadBytes( ( ONBOARD_STORED_RESULTS_ADDR + (PAGE_SIZE*index) ), data, 66);
	HAL_Delay(300) ;

	memset(buff, 0, sizeof(buff)) ;
	buff[0] = 0x5A ;
	buff[1] = 0xA5 ;
	buff[3] = 0x82 ;
	buff[4] = (uint8_t)(RESULTS_PAGE_PROFILENAME_ADDR >> 8); // Higher byte
	buff[5] = (uint8_t)(RESULTS_PAGE_PROFILENAME_ADDR & 0xFF); // Lower byte
	/* uncomment if not working */
//	buff[2] = 3+strlen((char *)data) ;
//	memcpy(&buff[6], data, strlen((char *)data)) ;
//	HAL_UART_Transmit( &huart1, buff, 6+strlen((char *)data), HAL_MAX_DELAY) ;
	/* comment if not working */
	buff[2] = 3+10 ;
	memcpy(&buff[6], data, strlen((char *)data)) ;
	strcat(buff,0) ;
	HAL_UART_Transmit( &huart1, buff, 16, HAL_MAX_DELAY) ;

	memset(dbg, 0, sizeof(dbg)) ;
	sprintf((char *)dbg, "%02d",index) ;
	buff[2] = 0x08 ;
	buff[4] = (uint8_t)(RESULTS_PAGE_SRNUMBER_ADDR >> 8); // Higher byte
	buff[5] = (uint8_t)(RESULTS_PAGE_SRNUMBER_ADDR & 0xFF); // Lower byte
	memcpy(&buff[6], dbg, 5) ;
	HAL_UART_Transmit( &huart1, buff, 11, HAL_MAX_DELAY) ;


	memset(dbg, 0, sizeof(dbg)) ;
	memcpy(dbg, &data[28], 5) ;
	buff[2] = 0x08 ;
	buff[4] = (uint8_t)(RESULTS_PAGE_DURATION_ADDR >> 8); // Higher byte
	buff[5] = (uint8_t)(RESULTS_PAGE_DURATION_ADDR & 0xFF); // Lower byte
	memcpy(&buff[6], dbg, 5) ;
	HAL_UART_Transmit( &huart1, buff, 11, HAL_MAX_DELAY) ;

	memset(dbg, 0, sizeof(dbg)) ;
	memcpy(dbg, &data[33], 10) ;

	buff[4] = (uint8_t)(RESULTS_PAGE_TESTEDBY_ADDR >> 8); // Higher byte
	buff[5] = (uint8_t)(RESULTS_PAGE_TESTEDBY_ADDR & 0xFF); // Lower byte
	/* uncomment if not working */
//	buff[2] = 0x0D ;
//	memcpy(&buff[6], dbg, 5) ;
//	HAL_UART_Transmit( &huart1, buff, 16, HAL_MAX_DELAY) ;
	/* comment if not working */
	buff[2] = 3+10 ;
	memcpy(&buff[6], dbg, 5) ;
	strcat(buff,0) ;
	HAL_UART_Transmit( &huart1, buff, 16, HAL_MAX_DELAY) ;

	memset(dbg, 0, sizeof(dbg)) ;
	memcpy(dbg, &data[43], 5) ;
	buff[2] = 0x08 ;
	buff[4] = (uint8_t)(RESULTS_PAGE_DATE_ADDR >> 8); // Higher byte
	buff[5] = (uint8_t)(RESULTS_PAGE_DATE_ADDR & 0xFF); // Lower byte
	memcpy(&buff[6], dbg, 5) ;
	HAL_UART_Transmit( &huart1, buff, 11, HAL_MAX_DELAY) ;

	memset(dbg, 0, sizeof(dbg)) ;
	memcpy(dbg, &data[48], 5) ;
	buff[2] = 0x08 ;
	buff[4] = (uint8_t)(RESULTS_PAGE_TIME_ADDR >> 8); // Higher byte
	buff[5] = (uint8_t)(RESULTS_PAGE_TIME_ADDR & 0xFF); // Lower byte
	memcpy(&buff[6], dbg, 5) ;
	HAL_UART_Transmit( &huart1, buff, 11, HAL_MAX_DELAY) ;

	memset(dbg, 0, sizeof(dbg)) ;
	memcpy(dbg, &data[53], 3) ;
	buff[2] = 0x06 ;
	buff[4] = (uint8_t)(RESULTS_PAGE_MAINTAINED_MMHG_ADDR >> 8); // Higher byte
	buff[5] = (uint8_t)(RESULTS_PAGE_MAINTAINED_MMHG_ADDR & 0xFF); // Lower byte
	memcpy(&buff[6], dbg, 5) ;
	HAL_UART_Transmit( &huart1, buff, 9, HAL_MAX_DELAY) ;

	memset(dbg, 0, sizeof(dbg)) ;
	memcpy(dbg, &data[56], 5) ;
	buff[2] = 0x08 ;
	buff[4] = (uint8_t)(RESULTS_PAGE_NO_OF_SAMPLES_ADDR >> 8); // Higher byte
	buff[5] = (uint8_t)(RESULTS_PAGE_NO_OF_SAMPLES_ADDR & 0xFF); // Lower byte
//	memcpy(&buff[6], dbg, 5) ;
	uint8_t sw = atoi(dbg) ;
	sprintf((char *)&buff[6], "%03d",sw) ;
	HAL_UART_Transmit( &huart1, buff, 11, HAL_MAX_DELAY) ;

	memset(dbg, 0, sizeof(dbg)) ;
	memcpy(dbg, &data[61], 5) ;
	buff[2] = 0x08 ;
	buff[4] = (uint8_t)(RESULTS_PAGE_TEST_PASS_R_FAIL_ADDR >> 8); // Higher byte
	buff[5] = (uint8_t)(RESULTS_PAGE_TEST_PASS_R_FAIL_ADDR & 0xFF); // Lower byte
	memcpy(&buff[6], dbg, 5) ;
	HAL_UART_Transmit( &huart1, buff, 11, HAL_MAX_DELAY) ;


}

void mem_w25qxxDriver_ResultsPage_data_interChange()
{

	uint8_t data =0 ;
	mem_W25qxx_Driver_sectorErase(SWAP_SECTOR_ADDR);
	HAL_Delay(100);

	for(uint32_t i=0; i<4096; i++)
	{
		data = mem_W25qxx_Driver_ReadByte(ONBOARD_STORED_RESULTS_ADDR+i) ;
		mem_W25qxx_Driver_writeByte(( SWAP_SECTOR_ADDR+i), data);

	}
	mem_W25qxx_Driver_sectorErase(ONBOARD_STORED_RESULTS_ADDR);
	HAL_Delay(100);

	uint8_t buff[256] ;
	for(uint8_t i=0; i<16; i++)
	{
		mem_W25qxx_Driver_ReadBytes( ( SWAP_SECTOR_ADDR + (PAGE_SIZE*i) ), buff, 70);
		HAL_Delay(20) ;

		mem_W25qxx_Driver_writeBytes( ( ONBOARD_STORED_RESULTS_ADDR + (PAGE_SIZE*(i+1)) ), buff, 70);
		HAL_Delay(20) ;
		memset(buff, 0, sizeof(buff)) ;
	}
}

void mem_w25qxxDriver_ResultsPage_write(uint8_t *profilename, uint8_t *duration, uint8_t *testedBy, uint8_t *date, uint8_t *time,
		uint8_t *fixedAmp, uint8_t *sampleweight, uint8_t *passRfail)
{
	mem_w25qxxDriver_ResultsPage_data_interChange() ;

	uint8_t data[69] ;
	memset(data, 0, sizeof(data)) ;

	memcpy(data, profilename, 26) ;
	memcpy( &data[28], duration, 5 ) ;
	memcpy( &data[33], testedBy, 10 ) ;
	memcpy( &data[43], date, 5 ) ;
	memcpy( &data[48], time, 5 ) ;
	memcpy( &data[53], fixedAmp, 3 ) ;
	memcpy( &data[56], sampleweight, 5 ) ;
	memcpy( &data[61], passRfail, 5 ) ;

	mem_W25qxx_Driver_writeBytes( ( ONBOARD_STORED_RESULTS_ADDR ), data, 66);
	HAL_Delay(300) ;


}

