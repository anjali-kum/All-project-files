#include "MotorControl.h"
#include "UARTHandler.h"

void setup() {
  // Initialize motor control pins  as outputs
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(enPin, OUTPUT);
  digitalWrite(enPin, LOW);  // Ensure the motor driver is initially disable

  UART_Begin();  // Initialize UART communication for sending and receiving commands

  //sendDebugMessage("\nFT-G\n");

  sei();  // Enable global interrupts
}

void loop() {

  processCommands();  // Process incoming UART commands

  switch (statements) {  // Handle different motor control states
    case 1:
      if (minSet && secSet) {                                   // Check if minute and second values are set
        setMotorRunDuration(homePage_setMin, homePage_setSec);  // Set duration
        startMotor();                                           // Start the motor
        updateTiming();                                         // Update the timing and monitor the motor status
      } else {
        sendDebugMessage("Error: SetMin and SetSec values must be set before starting the motor.\n");
        statements = 0;

      }
      break;
    case 2:
      if (motorRunning) {
        stopMotor();     // Stop the motor
        statements = 0;  // Reset the state to idle
        
      }
      break;
    default:
      // Reset flags for a fresh start attempt
      GotoStopSent = false;
      GotoStartSent = false;
      break;
  }
}
