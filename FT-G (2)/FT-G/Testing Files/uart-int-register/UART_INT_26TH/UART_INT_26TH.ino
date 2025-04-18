// /*****************************@Declare Private Macro*******************************************/
#define BAUD_RATE 9600
#define MYUBRR F_CPU/16/BAUD_RATE-1

// /* **********************@Private Variable Declaration******************/
char receivedData[20];                                    // Buffer for incoming data
volatile int buffercnt = 0;

/********************************Setup function*************************************/
void setup() {
  unsigned int ubrr = MYUBRR;                             // Set baud rate
  UBRR0H = (unsigned char)(ubrr >> 8);
  UBRR0L = (unsigned char)ubrr;
  UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);  // Enable receiver and transmitter, and enable the RX interrupt
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);               // Set frame format: 8 data bits, 1 stop bit

  sei();                                               // Enable global interrupts
}
/*
   Main loop can perform other tasks while waiting for data, we'll just echo back the received data
*/
void loop() {
  if (buffercnt != 0) {
    delay(100);  
    for (int i = 0; i < buffercnt; i++) {           // Print the received data
      while (!(UCSR0A & (1 << UDRE0)));             // Wait for empty transmit buffer
      UDR0 = receivedData[i];                       // Put data into buffer, sends the data
    }
    buffercnt = 0;
    memset(receivedData, 0, sizeof(receivedData));// Reset the receivedData
  }
}
/******************ISR for receiving data********************************************/
ISR(USART_RX_vect) {
  receivedData[buffercnt] = UDR0;                // Read the received data from the USART data register
  buffercnt++;
  if (buffercnt >= sizeof(receivedData) - 1) {  // Ensure we don't overflow the buffer
    buffercnt = 0;
  }
}
