/*************User Defined Header Files***************************/
#include "MotorControl.h"
#include "UARTHandler.h"
/*****************Setup Function*********************************/
void setup() {
  // Set baud rate
  unsigned int ubrr = MYUBRR;
  UBRR0H = (unsigned char)(ubrr >> 8);
  UBRR0L = (unsigned char)ubrr;

  // Enable receiver and transmitter, and enable the RX interrupt
  UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);

  // Set frame format: 8 data bits, 1 stop bit
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

  // Enable global interrupts
  sei();

  // Initialize motor control pins
  pinMode(stepPin, OUTPUT); 
  pinMode(dirPin, OUTPUT);
  pinMode(enPin, OUTPUT);
  digitalWrite(enPin, LOW);
}
/*****************************loop function**************************/
void loop() {
  handleReceivedData(); // Function Invocation
  handleEcho(); // Function Invocation

  switch(statement) {
    case 1:
      startMotor();
      break;
    case 2:
      stopMotor();
      break;
    case 0:
    default:
      idleMotor();
      break;
  }
}
