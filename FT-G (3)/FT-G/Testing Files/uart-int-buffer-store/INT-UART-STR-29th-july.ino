#define BAUD_RATE 9600
#define MYUBRR F_CPU/16/BAUD_RATE-1

// Buffer for incoming data
char receivedData[20];
volatile int buffercnt = 0;

// Buffer for formatted string
char outputBuffer[50];

// Data to compare with received data
const char inputstr[] = {0x01, 0x02, 0x03, 0x04, 0x00};

// Flag to indicate whether data has been sent
volatile bool flag = false;

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
}

void loop() {
  // Main loop can perform other tasks while waiting for data
  if (buffercnt != 0) {
    delay(100);

    // Check if received data matches the input data
    if (memcmp(receivedData, inputstr, sizeof(inputstr)) == 0) {
      // If a match is found, proceed with the action
      sprintf(outputBuffer, "Match found: ");
      for (int i = 0; i < buffercnt; i++) {
        sprintf(outputBuffer + strlen(outputBuffer), "%02X ", (unsigned char)receivedData[i]);
      }
      strcat(outputBuffer, "\n");

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
      sprintf(outputBuffer, "Received data: ");
      for (int i = 0; i < buffercnt; i++) {
        sprintf(outputBuffer + strlen(outputBuffer), "%02X ", (unsigned char)receivedData[i]);
      }
      strcat(outputBuffer, "\n");

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

// ISR for receiving data
ISR(USART_RX_vect) {
  // Read the received data from the USART data register
  receivedData[buffercnt] = UDR0;
  buffercnt++;
  // Ensure we don't overflow the buffer
  if (buffercnt >= sizeof(receivedData) - 1) {
    buffercnt = 0;
  }
}
