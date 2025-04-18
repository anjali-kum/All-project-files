/*******************Header Files**********************************/
#include "UARTHandler.h"
#include "MotorControl.h"
/*************Variable Declaration and initialization************/
char receivedData[20];
volatile int buffercnt = 0;
char outputBuffer[50];
volatile bool flag = false;
/****************************Function for Handling ReceivedData*********/
void handleReceivedData() {
  if (buffercnt != 0) {
    delay(100);
    readstatement(statement, receivedData);

    buffercnt = 0;
    memset(receivedData, 0, sizeof(receivedData));
  }
}
/************Function for comparing ReceivedData and inputStr and handle buffercnt******/
void handleEcho() {
  const char inputstr[] = {0x01, 0x02, 0x03, 0x04, 0x00};

  if (buffercnt != 0) {
    delay(100);

    // Check if received data contains the input string
    if (strstr(receivedData, inputstr) != NULL) {
      // If a match is found, proceed with the action
      sprintf(outputBuffer, "Match found: %s\n", receivedData);
      for (int i = 0; outputBuffer[i] != '\0'; i++) {
        // Wait for empty transmit buffer
        while (!(UCSR0A & (1 << UDRE0)));
        // Put data into buffer, sends the data
        UDR0 = outputBuffer[i];
      }
      // Set flag to indicate data has been sent
      flag = true;
    } else {
      // If no match, print the received data
      sprintf(outputBuffer, "Received data: %s\n", receivedData);
      for (int i = 0; outputBuffer[i] != '\0'; i++) {
        // Wait for empty transmit buffer
        while (!(UCSR0A & (1 << UDRE0)));
        // Put data into buffer, sends the data
        UDR0 = outputBuffer[i];
      }
      // Set flag to indicate data has been sent
      flag = true;
    }

    buffercnt = 0;
    memset(receivedData, 0, sizeof(receivedData));
  } else {
    // If no data has been sent, set flag to false
    flag = false;
  }
}

/*****************************ISR for receiving data**************************/
ISR(USART_RX_vect) {
  // Read the received data from the USART data register
  receivedData[buffercnt] = UDR0;
  buffercnt++;
  // Ensure we don't overflow the buffer
  if (buffercnt >= sizeof(receivedData) - 1) {
    buffercnt = 0;
  }
}
