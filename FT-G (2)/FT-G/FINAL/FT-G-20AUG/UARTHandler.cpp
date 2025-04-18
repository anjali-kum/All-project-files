#include "UARTHandler.h"   // UART handler
#include "MotorControl.h"  // motor control
#include <string.h>

#define BUFFER_SIZE 128 //size of the buffer for storing received commands

uint8_t commandBuffer[BUFFER_SIZE];  // Buffer to store incoming UART data
volatile int idx = 0;                // Index to track the current position in the buffer
char *c = NULL;                      // Pointer to locate the specific command in the buffer
uint8_t index = 0;                   // Index for command matching
// volatile uint16_t timerCount = 0;
// volatile uint16_t rotationCount = 0; // Counter for motor rotations

// // External interrupt pin
// const int interruptPin = 3;  // Use D3 for external interrupt (INT0 on ATmega328P)


// Predefined command sequences for various actions
const uint8_t StartButton[] = { 0x5A, 0xA5, 0x06, 0x83, 0x10, 0x12, 0x01, 0x00, 0x01, 0x00 };
const uint8_t StopButton[] = { 0x5A, 0xA5, 0x06, 0x83, 0x10, 0x14, 0x01, 0x00, 0x01, 0x00 };
const uint8_t SetSec[] = { 0x10, 0x08, 0x00 };
const uint8_t SetMin[] = { 0x0F, 0xFF, 0x00 };
const uint8_t GotoStop[] = { 0x5A, 0xA5, 0x07, 0x82, 0x00, 0x84, 0x5A, 0x01, 0x00, 0x01 };  // 5A A5 07 82 00 84 5A 01 00 XX
const uint8_t GotoStart[] = { 0x5A, 0xA5, 0x07, 0x82, 0x00, 0x84, 0x5A, 0x01, 0x00, 0x00 };

const size_t GotoStartSize = sizeof(GotoStart);  // Size of the GotoStart command
volatile int statements = 0;
uint8_t homePage_setSec = 0;
uint8_t homePage_setMin = 0;
uint8_t homePage_StartButton = 0;
uint8_t homePage_StopButton = 0;
uint8_t receivedDWINMin = 0;
uint8_t receivedDWINSec = 0;

bool GotoStopSent = false;   // Flag to ensure GotoStop command is sent only once
bool GotoStartSent = false;  // Flag to ensure GotoStart command is sent only once
volatile bool commandReceived = false;


//============================================ISR to handle UART data reception==========================================
ISR(USART_RX_vect) {
  uint8_t receivedByte = UDR0;  // Read the received byte from the UART data register
 // UART_send(receivedByte);      // Echo the received byte back via UART for debugging
  if (idx < BUFFER_SIZE) {
    commandBuffer[idx++] = receivedByte;  //Store the received byte in the buffer and increment the index
    commandReceived = true;               // set the flag to indicate a command has been received
  } else {
    idx = 0;  // Reset index or handle overflow
  }
}
//====================Timer1 ISR=================================//

// ISR(TIMER1_COMPA_vect)
// {
//   timerCount++;
// }

ISR(TIMER1_OVF_vect) {
  TCNT1 = 65536 - (F_CPU / 8 / 1000);  // Reload for 1ms (F_CPU / prescaler / 1000)
  timerCount++;
}

//========================================Function to initialize UART=======================================//
void UART_Begin() {
 // UBRR0 = 8;  // For F_CPU = 16MHz, UBRR = 8 (baud rate for 115200bps)
  UBRR0 = 103 ; //Baudrate for 9600
  // Set frame format: 8 data bits, 1 stop bit
  UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);
  // Enable receiver, transmitter, and RX interrupt
  UCSR0B |= (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
}

// //================ Function to initialize Timer1 for 1ms interrupts=========================//

void Timer1_Init() {
  cli();              // Disable global interrupts
  TCCR1A = 0;         // Clear Timer1 control register A
  TCCR1B = 0;         // Clear Timer1 control register B
  TCNT1 = 65536 - (F_CPU / 8 / 1000);  // Set Timer1 to overflow after 1ms
  TCCR1B |= (1 << CS11); // Set prescaler to 8
  TIMSK1 |= (1 << TOIE1); // Enable Timer1 overflow interrupt
  sei();              // Enable global interrupts
}



//=======================================Send a debug message via UART=================================================//
void sendDebugMessage(const char *message) {
  // Send each character of the message string via UART
  while (*message) {
    UART_send(*message++);
  }
}
//==========================================UART Send============================================================//
void UART_send(char data) {
  while (!(UCSR0A & (1 << UDRE0)))  // Waits until the transmit buffer is empty (UDRE0 flag is set) before sending the data.
    ;
  UDR0 = data;  // Send the data
}

//======================================================================================//

void sendResetValuesToDWIN() {
  uint8_t resetCommandMin[] = { 0x5A, 0xA5, 0x05, 0x82, 0x10, 0x09, 0x00 };
  uint8_t resetCommandSec[] = { 0x5A, 0xA5, 0x05, 0x82, 0x10, 0x00, 0x00 };
  for (size_t i = 0; i < sizeof(resetCommandMin); i++) {
    UART_send(resetCommandMin[i]);
  }
  delay(1000);
  for (size_t i = 0; i < sizeof(resetCommandSec); i++) {
    UART_send(resetCommandSec[i]);
  }

}


//======================================Process commands received via UART=========================================//
void processCommands() {
  if (commandReceived) {      // Reset the command received flag
    commandReceived = false;  // Process the received command
    handleCommand();
    idx = 0;                                          // Reset the buffer index
    memset(commandBuffer, 0, sizeof(commandBuffer));  // Clear the command buffer
    
  }

}
//======================================Handle commands based on the received buffer=================================//

void handleCommand() {
  char debugMsg[BUFFER_SIZE + 1];
  strncpy(debugMsg, (char *)commandBuffer, BUFFER_SIZE);
  debugMsg[BUFFER_SIZE] = '\0';  // Ensure null termination
  sendDebugMessage("Received Data: ");
  sendDebugMessage(debugMsg);

// Check for StartButton command
c = strstr((char *)commandBuffer, (char *)StartButton);
if (c != NULL) {
    index = c - (char *)commandBuffer;
    if (minSet && secSet) {
        statements = 1;             // Start motor
        //previousMillis = millis();  // Reset timing
        rotationCount = 0;     // Reset rotation counter
        timerCount = 0;  // Reset timing using Timer1
        sendDebugMessage("Start button detected, starting motor...\n");
        if (!GotoStopSent) {
            sendDebugMessage("Sending GotoStop command...\n");
            for (size_t i = 0; i < sizeof(GotoStop); ++i) {
                UART_send(GotoStop[i]);
            }
            GotoStopSent = true;    // GotoStop command is sent only once
            GotoStartSent = false;  // Reset GotoStart flag
      // Update button states
      homePage_StartButton = 1;
      homePage_StopButton = 0;
        }
    } else {
        sendDebugMessage("Error: SetMin and SetSec values must be set before starting the motor.\n");
        // Do not send GotoStop if minSet and secSet are not set
        statements = 0;
    }
}
// Check for StopButton command
c = strstr((char *)commandBuffer, (char *)StopButton);
if (c != NULL) {
    index = c - (char *)commandBuffer;
    statements = 2;  // Stop motor
    sendDebugMessage("Stop button detected, stopping motor...\n");
    if (!GotoStartSent) {
        sendDebugMessage("Sending GotoStart command...\n");
        for (size_t i = 0; i < sizeof(GotoStart); i++) {
            UART_send(GotoStart[i]);
        }
        GotoStartSent = true;  // GotoStart command is sent only once
        GotoStopSent = false;  // Reset GotoStop flag
    }
    
    // Reset minSet and secSet flags when the motor stops
    minSet = false;
    secSet = false;
          // Update button states
      homePage_StartButton = 0;
      homePage_StopButton = 1; 

    homePage_setMin += receivedDWINMin;
    homePage_setSec += receivedDWINSec;


}
//===================================== Check for SetSec command==================================================//

  c = strstr((char *)commandBuffer, (char *)SetSec);
  if (c != NULL) {
    index = c - (char *)commandBuffer;
    if ((commandBuffer[index - 1] == 0x83) && (commandBuffer[index + 1] == SetSec[1])) {
      uint8_t size = commandBuffer[index + 4];
      if (index + 4 + size <= BUFFER_SIZE) {
        uint8_t tempSetSec[10];
        for (uint8_t i = 0; i < size; i++) {
          tempSetSec[i] = commandBuffer[index + 5 + i];
        }
        homePage_setSec = 0;
        // Take only the first two digits for seconds
        for (uint8_t i = 0; i < 2 && i < size; i++) {
          homePage_setSec = homePage_setSec * 10 + (tempSetSec[i] - '0');
        }
        char data[30];
        sprintf(data, "data Seconds:%d \n", homePage_setSec);
        sendDebugMessage(data);
        secSet = true;
      }
    }
  }

   //====================================== Check for SetMin command=====================================================//
  c = strstr((char *)commandBuffer, (char *)SetMin);
  if (c != NULL) {
    index = c - (char *)commandBuffer;
    if ((commandBuffer[index - 1] == 0x83) && (commandBuffer[index + 1] == SetMin[1])) {
      uint8_t size = commandBuffer[index + 4];
      if (index + 4 + size <= BUFFER_SIZE) {
        uint8_t tempSetMin[10];
        homePage_setMin = 0;
        // Take only the first two digits for minutes
        for (uint8_t i = 0; i < 2 && i < size; i++) {
          tempSetMin[i] = commandBuffer[index + 5 + i];
        }
        for (uint8_t i = 0; i < 2 && i < size; i++) {
          homePage_setMin = homePage_setMin * 10 + (tempSetMin[i] - '0');
        }
        char data[30];
        sprintf(data, "data minutes:%d \n", homePage_setMin);
        sendDebugMessage(data);
        minSet = true;
      }
    }
  }

}
