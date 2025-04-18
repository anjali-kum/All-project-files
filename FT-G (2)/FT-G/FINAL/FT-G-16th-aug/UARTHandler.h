#ifndef UARTHANDLER_H
#define UARTHANDLER_H

#include <Arduino.h>
#include <avr/interrupt.h>
#include <avr/io.h>

#define BUFFER_SIZE 500

// Buffer and flags for UART communication
extern uint8_t commandBuffer[BUFFER_SIZE];
extern volatile int idx;
extern volatile bool commandReceived;
extern const uint8_t GotoStart[];
extern bool GotoStopSent;
extern bool GotoStartSent;

// External variables for motor control settings
extern volatile int statements;
extern uint8_t homePage_setSec;
extern uint8_t homePage_setMin;
// extern uint8_t homePage_speed;

// Function prototypes
void UART_Begin();
void UART_send(char data);
void handleCommand();
void processCommands();
void sendDebugMessage(const char *message);
void sendResetValuesToDWIN();

#endif  // UARTHANDLER_H
