/*~~~~~~~~~~~~~~~~~~~LIBS~~~~~~~~~~~~~~~~~~*/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#include "TDC7200.h"
#include <ArduinoSort.h>
#include <SendOnlySoftwareSerial.h>
#include <SPI.h>
#include <stdint.h>
#include <MPU6050_tockn.h>
#include <Wire.h>
#include <math.h>
#include <avr/wdt.h>
#include <EEPROM.h>
/*~~~~~~~~~~~~~~~~~~objects~~~~~~~~~~~~~~~~~*/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
String timenow;
SendOnlySoftwareSerial mySerial(PIN_PD0);   // Tx pin
SendOnlySoftwareSerial sensorOut(PIN_PD3);  // Tx pin
#define MPU_addr uint8_t(0x68)
#define WDT_8S 8

/*~~~~~~~~~~~~~~~~~~FILTER-VARS~~~~~~~~~~~~~*/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
float data[100];
float lastval[4];
float filteredValue;
float val;
float calculateFilteredValue(float data[], int dataSize, float range);

/*~~~~~~~~~~~~~~~~~~variables~~~~~~~~~~~~~~~*/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
float instantfuellvl = 0;
float fuellvl = 0;
float x = 0;
float y = 0;
float z = 0;
float anglex;
float angley;
float anglez;
uint8_t stat = EEPROM.read(0);

bool zoneset = true;
bool zonex = true;
bool zoney = false;
uint8_t step = 0;
uint8_t dataSize = 100;

uint16_t minVal = 265;
uint16_t maxVal = 402;

int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;


unsigned long previousMillis = 0;  //will store last time
unsigned long currentMillis = 0;
const long period = 2000;
char snsOutData[100];
#define THREE_SECONDS 3000


/*~~~~~~~~~~~~~~~~~~TDC-7200-INIT~~~~~~~~~~~*/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define STAT_LED uint8_t(PIN_PA6)
#define PIN_TDC7200_INT uint8_t(PIN_PA7)
#define PIN_TDC7200_ENABLE uint8_t(PIN_PD5)
#define PIN_TDC7200_SPI_CS uint8_t(PIN_PC5)
#define TDC7200_CLOCK_FREQ_HZ (4000000UL)  // for 4mhz for 4mhz divide this by 2

static TDC7200 tof(PIN_TDC7200_ENABLE, PIN_TDC7200_SPI_CS, TDC7200_CLOCK_FREQ_HZ);

float calculateHeight();

#define NUM_STOPS (1)


/*~~~~~~~~~~~~~~~~~~TDC-1000-INIT~~~~~~~~~~~*/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define PIN_TDC1000_OSC_ENABLE uint8_t(PIN_PC3)
#define PIN_TDC1000_RESET uint8_t(PIN_PC2)
#define PIN_TDC1000_ENABLE uint8_t(PIN_PD4)
#define PIN_TDC1000_CHSEL uint8_t(PIN_PC4)
#define PIN_TDC1000_SPI_CS uint8_t(PIN_PD6)
#define PIN_TDC1000_ERRB uint8_t(PIN_PC7)

#define CONFIG_0 uint8_t(0x00)
#define CONFIG_1 uint8_t(0x01)
#define CONFIG_2 uint8_t(0x02)
#define CONFIG_3 uint8_t(0x03)
#define CONFIG_4 uint8_t(0X04)
#define TOF_1 uint8_t(0X05)
#define TOF_0 uint8_t(0X06)
#define ERROR_FLAGS uint8_t(0X07)
#define TIMEOUT uint8_t(0X08)
#define CLOCK_RATE uint8_t(0x09)

#define TDC1000_SPI_CLK_MAX (int32_t(10000000))
#define TDC1000_SPI_REG_ADDR_MASK (0x1Fu)
#define TDC1000_SPI_REG_READ (0x00u)
#define TDC1000_SPI_REG_WRITE (0x40u)


/*~~~~~~~~~~~~~~~~~~TDC-7200~~~~~~~~~~~~~~~~*/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define enableTDC7200 uint8_t(PIN_PD5);  //49;
#define intb uint8_t(PIN_PA7);


/*~~~~~~~~~~~~~~~~~~PROTOS~~~~~~~~~~~~~~~~~~*/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void mpuStabilise();
void mpuUpdate();
bool clearSnsOutData();

/*~~~~~~~~~~~~~~~~setup-start~~~~~~~~~~~~~~~*/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void setup() {
  /*~~~~~~~~~~~~~~~~~~boot-stat~~~~~~~~~~~~~*/
  /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  stat = stat + 1;
  EEPROM.write(0, stat);
  if (stat >= 255) {
    EEPROM.write(0, 0);
  }
  wdt_enable(7);
  // wdt_disable();
  wdt_reset();
  pinMode(STAT_LED, OUTPUT); /*satus LED*/

  /*~~~~~init-mpu~~~~~*/
  /*~~~~~~~~~~~~~~~~~*/
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  wdt_reset();
  Wire.write(0x6B);
  Wire.write(0);

  Wire.endTransmission(true);
  /*~~~~~~~~~~~~~~~~~*/
  wdt_reset();
  mySerial.begin(9600);
  wdt_reset();
  mySerial.println(F("~~~~~~~~~~~~~~~~~Diesel-Eye-V1.2~~~~~~~~~~~~~~~~~~"));
  wdt_reset();
  sensorOut.begin(9600);

  SPI.begin();
  Wire.begin();

  mySerial.println(F("-- Starting TDC7200 test --"));

  if (not tof.begin()) {
    mySerial.println(F("Failed to init TDC7200"));
    delay(1000);
  }

  pinMode(PIN_TDC7200_INT, INPUT);  // active low (open drain)

  if (not tof.setupMeasurement(10,         // cal2Periods
                               1,          // avgCycles
                               NUM_STOPS,  // numStops
                               2))         // mode
  {
    mySerial.println(F("Failed to setup measurement"));
  }


  /*~~~~~pin-modes~~~~~*/
  /*~~~~~~~~~~~~~~~~~~~*/
  pinMode(PIN_TDC1000_ENABLE, OUTPUT);
  pinMode(PIN_TDC1000_RESET, OUTPUT);

  pinMode(PIN_TDC1000_ERRB, INPUT_PULLUP);
  pinMode(PIN_TDC1000_SPI_CS, OUTPUT);

  pinMode(PIN_TDC1000_OSC_ENABLE, OUTPUT);

  digitalWrite(PIN_TDC1000_ENABLE, HIGH);
  digitalWrite(PIN_TDC1000_RESET, HIGH);

  delay(12);

  digitalWrite(PIN_TDC1000_RESET, LOW);
  digitalWrite(PIN_TDC1000_CHSEL, LOW);

  digitalWrite(PIN_TDC1000_OSC_ENABLE, HIGH);


  /*~~~~~TDC-1000~~~~~~*/
  /*~~~~~~~~~~~~~~~~~~~*/
  TDC1000Write(CONFIG_0, 0X1F);     //2A//44zz
  TDC1000Write(CONFIG_1, 0X41);     //41
  TDC1000Write(CONFIG_1, 0X41);     //41
  TDC1000Write(CONFIG_2, 0X2);      //02//0
  TDC1000Write(CONFIG_3, 0x0);      //D//C
  TDC1000Write(CONFIG_4, 0X1e);     //5F
  TDC1000Write(TOF_1, 0Xe0);        //80//40
  TDC1000Write(TOF_0, 0X1e);        //1E
  TDC1000Write(ERROR_FLAGS, 0X00);  //0
  TDC1000Write(TIMEOUT, 0x3B);      //33//23
  TDC1000Write(CLOCK_RATE, 0x1);    //6//1
  TDC1000Write(CONFIG_0, 0X1F);
  wdt_reset();

  //cr = 6 , tf1 = e5 ,to =3b for 1280mm
  //cr = 1; tf1 = e0 , to = 3b for less than 153mm

  for (int i = 0; i < dataSize; i++) { /*use memset instead*/
    data[i] = 0.0;
  }
  // memset(data,0, sizeof(data));

  mpuStabilise(); /*changed name to more sensable one*/
  wdt_reset();
}


void loop() {

  wdt_reset();
  mpuUpdate();
  wdt_reset();

  //tdc7200configRead();  //uncomment to print reg config of tdc7200
  tdc1000configRead();  //uncomment to print reg config of tdc1000


  wdt_reset();

  /*~~~~~~~~~~~~ULTRA-SONIC-READINGS~~~~~~~~~~~~~*/
  /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  for (uint8_t i = 0; i <= dataSize; i++) {
    data[i] = round(calculateHeight());
    wdt_reset();
  }
  wdt_reset();
  float val = calculateFilteredValue(data, dataSize, 2);

  wdt_reset();

  if (val > 65) {
    filteredValue = val;
    mySerial.print(F("filtered value:"));
    mySerial.println(String(filteredValue));
    switch (step) {
      wdt_reset();
      case 0: /*Step 0: Store the value in lastval[0]*/

        lastval[0] = filteredValue;
        step = 1;
        break;
        wdt_reset();
      case 1: /*Step 1: Compare with lastval[0]*/
        wdt_reset();
        if (filteredValue >= lastval[0] - 2 && filteredValue <= lastval[0] + 2) {  // Value is within the range of lastval[0], do nothing
          step = 1;
          break;
        } else {  // Step 1: Store the value in lastval[1] and increment counter
          lastval[1] = filteredValue;
          step = 2;
        }

        break;

      case 2: /*Step 2: Compare with lastval[0] and lastval[1]*/
        wdt_reset();
        if (filteredValue >= lastval[0] - 2 && filteredValue <= lastval[0] + 2) {  // Step 2: Decrement counter and delete lastval[1]

          lastval[1] = 0;
          step = 1;

        } else if (filteredValue >= lastval[1] - 2 && filteredValue <= lastval[1] + 2) {  // Step 2: Store the value in lastval[2] and increment counter

          lastval[2] = filteredValue;
          step = 3;

        } else {  // Step 2: Replace the value in lastval[1] with a

          lastval[1] = filteredValue;
          step = 2;
        }

        break;
      case 3: /*Step 3: Compare with lastval[0], lastval[1], and lastval[2]*/
        wdt_reset();
        if (filteredValue >= lastval[0] - 2 && filteredValue <= lastval[0] + 2) {  // Step 3: Set counter to zero and delete lastval[1] and lastval[2]
          wdt_reset();
          lastval[1] = 0;
          lastval[2] = 0;

          step = 1;

          break;

        } else {  // Step 3: Compare with lastval[1], lastval[2], and lastval[3]
          wdt_reset();
          if (filteredValue >= lastval[1] - 2 && filteredValue <= lastval[1] + 2 && filteredValue >= lastval[2] - 2 && filteredValue <= lastval[2] + 2) {  // Step 3: Calculate average and update lastval[0]

            lastval[3] = filteredValue;
            // lastval[1] = 0;
            // lastval[2] = 0;
            step = 4;

            break;

          } else {

            lastval[4] = filteredValue;
            step = 4;
            break;
          }
        }
        break;

      case 4:
        wdt_reset();
        if (filteredValue >= lastval[0] - 2 && filteredValue <= lastval[0] + 2) {  //Step 3: Set counter to zero and delete lastval[1] and lastval[2]

          lastval[1] = 0;
          lastval[2] = 0;

          lastval[3] = 0;
          lastval[4] = 0;

          step = 1;
          break;

        } else {  // Step 3: Compare with lastval[1], lastval[2], and lastval[3]

          if (filteredValue >= lastval[1] - 2 && filteredValue <= lastval[1] + 2 && filteredValue >= lastval[2] - 2 && filteredValue <= lastval[2] + 2 && filteredValue >= lastval[3] - 2 && filteredValue <= lastval[3] + 2) {
            // Step 3: Calculate average and update lastval[0]
            wdt_reset();
            lastval[0] = (lastval[1] + lastval[2] + lastval[3] + filteredValue) / 4;
            step = 1;

            break;
          }
          if (filteredValue >= lastval[4] - 2 && filteredValue <= lastval[4] + 2) {
            // Step 3: Replace lastval[1] with lastval[3] and lastval[2] with a
            wdt_reset();
            lastval[1] = lastval[4];
            lastval[2] = filteredValue;

            step = 3;
            break;
          }
          default:
            //mySerial.println("in switch case ");  // Default case if step value is invalid
            break;
        }
        wdt_reset();
        // Reset the step counter if it exceeds 3
        if (step > 4) {
          step = 0;
        }
    }
  }

  wdt_reset();

  fuellvl = lastval[0] * 10;
  instantfuellvl = filteredValue * 10;
  currentMillis = millis();  // store the current time
  wdt_reset();
  if (currentMillis - previousMillis >= period) {  // check if 10000ms passed
    wdt_reset();
    previousMillis = currentMillis;
    mySerial.println("sensor val:" + String(fuellvl) + "mm");
    sensorPacket(instantfuellvl, fuellvl, x, y, z, 0, 0, 0, stat, 31);
    wdt_reset();
  }

  // for (uint8_t i = 0; i < dataSize; i++) {/*use memset instead*/
  //   data[i] = 0.0;
  // }
  wdt_reset();
  memset(data, 0, sizeof(data));
  wdt_reset();

  //mySerial.println("height:" + String(lastval[0]));
  // mySerial.println("step:" + String(step));
}


void mpuUpdate() {

  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr, 14, true);
  AcX = Wire.read() << 8 | Wire.read();
  AcY = Wire.read() << 8 | Wire.read();
  AcZ = Wire.read() << 8 | Wire.read();
  wdt_reset();
  int16_t xAng = map(AcX, minVal, maxVal, -90, 90);
  int16_t yAng = map(AcY, minVal, maxVal, -90, 90);
  int16_t zAng = map(AcZ, minVal, maxVal, -90, 90);

  x = anglex - (RAD_TO_DEG * (atan2(-yAng, -zAng) + PI));
  y = angley - (RAD_TO_DEG * (atan2(-xAng, -zAng) + PI));
  z = anglez - (RAD_TO_DEG * (atan2(-yAng, -xAng) + PI));
  if (x < 0) {
    x = -(x);
  }
  if (y < 0) {
    y = -(y);
  }
  if (z < 0) {
    z = -(z);
  }
  wdt_reset();
  // mySerial.print("AngleX= ");
  // mySerial.println(x);

  // mySerial.print("AngleY= ");
  // mySerial.println(y);

  // mySerial.print("AngleZ= ");
  // mySerial.println(z);

  // mySerial.println("-----------------------------------------");
}


float calculateFilteredValue(float data[], uint8_t dataSize, float range) {
  // Sort the data in ascending order (using bubble sort algorithm)
  for (uint8_t i = 0; i < dataSize - 1; i++) {
    for (uint8_t j = 0; j < dataSize - i - 1; j++) {
      if (data[j] > data[j + 1]) {
        float temp = data[j];
        data[j] = data[j + 1];
        data[j + 1] = temp;
      }
    }
    wdt_reset();
    // Calculate the filtered value within the specified range
    float filteredValue = 0;
    uint8_t count = 0;
    for (uint8_t i = 0; i < dataSize; i++) {
      if (abs(data[i] - data[dataSize / 2]) <= range) {
        filteredValue += data[i];
        count++;
      }
    }
    return filteredValue / count;
  }
}


void mode_switch() {


  //take a reading, filter the reading
  //if its anything between 160 to 200mm in both the modes then switch to high mode
  //if its anything between 150 to 160 on high mode and below 150mm in low mode switch to low mode
  //if its below 80mm on the low mode and greater than 230mm on high mode switch to high mode 
  
  //     if (filteredValue = > 160 && zonex == true && zoneset == true) {
  //       //change zone to y include maximum reading as well
  //       zonex = false;
  //       zoney = true;
  //       zoneset = false;
  //     }
  //     if (filteredValue = > 160 && zoney == true && zoneset == false) {
  //       //change zone to x
  //       zonex = true;
  //       zoney = false;
  //       zoneset = true;
  //     }

  // if (filteredValue = > 150 && zonex= true){

  // }
}

void mpuStabilise() {
  wdt_reset();
  float ax = 10;
  float ay = 20;
  float az = 30;
  if (EEPROM.read(2) != 3) {
    mySerial.println(F("-----------------------------------------"));
    mySerial.println(F("stabilising gyro please wait"));
    mySerial.println(F("-----------------------------------------"));
    Wire.beginTransmission(MPU_addr);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU_addr, 14, true);
    AcX = Wire.read() << 8 | Wire.read();
    AcY = Wire.read() << 8 | Wire.read();
    AcZ = Wire.read() << 8 | Wire.read();
    int16_t xAng = map(AcX, minVal, maxVal, -90, 90);
    int16_t yAng = map(AcY, minVal, maxVal, -90, 90);
    int16_t zAng = map(AcZ, minVal, maxVal, -90, 90);
    wdt_reset();
    anglex = RAD_TO_DEG * (atan2(-yAng, -zAng) + PI);
    angley = RAD_TO_DEG * (atan2(-xAng, -zAng) + PI);
    anglez = RAD_TO_DEG * (atan2(-yAng, -xAng) + PI);

    EEPROM.put(ax, anglex);
    EEPROM.put(ay, angley);
    EEPROM.put(az, anglez);
    EEPROM.write(2, 0);
  }

  else {
    anglex = EEPROM.get(ax, anglex);
    angley = EEPROM.get(ay, anglez);
    anglez = EEPROM.get(az, anglez);
  }
  delay(1000);
}



void sensorPacket(uint16_t infuel, uint16_t fuelHeight, int16_t gyrox, int16_t gyroy, int16_t gyroz, int16_t accelx, int16_t accely, int16_t accelz, int16_t temp, uint8_t boot) {
  /**XD,0000,00,00000,0000,0000,00000,0000,0000,80#^0000,0000,0000,000,000,000,031!*/
  wdt_reset();
  memset(snsOutData, 0, sizeof(snsOutData)); /*clear snsOutData buff*/
  wdt_reset();
  sprintf(snsOutData, "%s,%05d,%s,%05d,%s%04d,%04d,%04d,%03d,%03d,%03d,%03d,%03d%s", "*XD,0000,00", fuelHeight, "0000,0000", infuel, "0000,0000,80#^", gyrox, gyroy, gyroy, accelx, accely, accelz, boot, temp, "!\r\n");
  wdt_reset();
  mySerial.print(F("sns>"));
  wdt_reset();
  mySerial.print(snsOutData);
  mySerial.print(F("time>"));
  mySerial.print(millis());
  wdt_reset();
  mySerial.println(F("<sns"));
  wdt_reset();
  // sensorOut.flush();
  // sensorOut.println(snsOutData);
  for (uint8_t k = 0; k < strlen(snsOutData); k++) {
    sensorOut.write(snsOutData[k]);
  }
}



float calculateHeight() {
  wdt_reset();
  bool ovrf = false;
  bool undrf = false;
  long timeout = 10000;
  digitalWrite(STAT_LED, !digitalRead(STAT_LED));
  tof.startMeasurement();
  char buff[40];
  unsigned long wait_time = millis() + THREE_SECONDS;
  while (millis() < wait_time) {
    if (digitalRead(PIN_TDC7200_INT) == LOW) {
      break;
    }
    wdt_reset();
  }
  wdt_reset();
  for (uint8_t stop = 1; stop <= NUM_STOPS; ++stop) {
    uint64_t time;
    if (tof.readMeasurement(stop, time)) {
      if (time > 999999999) {
        ovrf = true;
      }
      if (time < 99999999) {
        undrf = true;
      }

      ui64toa(time, buff, 10);
      // mySerial.print("\ttof");
      // mySerial.print(stop);
      // mySerial.print('=');
      // mySerial.print(buff);
    }
  }
  String s1 = "";
  s1 = String(buff[0]);
  s1 = String(s1 + buff[1]);
  s1 = String(s1 + buff[2]);

  int tof = s1.toInt();
  if (ovrf == true) {
    tof = tof * 10;
    ovrf = false;
  }
  if (undrf == true) {
    tof = tof / 10;
    undrf = false;
  }
  float a = tof;
  float height = (((a)*1480) / 2) * 0.001;
  mySerial.println("height:" + String(height) + "mm");
  wdt_reset();

  //mySerial.println(String(height / 2) + ",");
  if (height >= 50) {
    // mySerial.println("height:" + String(height) + "mm");
    return height;
  }

  ///////////////////low mode///////////////////
  // if (height >= 150 && height <= 160) {
  //   TDC1000Write(CONFIG_0, 0X1F);     //2A//44zz
  //   TDC1000Write(CONFIG_1, 0X41);     //41
  //   TDC1000Write(CONFIG_1, 0X41);     //41
  //   TDC1000Write(CONFIG_2, 0X2);      //02//0
  //   TDC1000Write(CONFIG_3, 0x0);      //D//C
  //   TDC1000Write(CONFIG_4, 0X1e);     //5F
  //   TDC1000Write(TOF_1, 0Xe0);        //80//40
  //   TDC1000Write(TOF_0, 0X1e);        //1E
  //   TDC1000Write(ERROR_FLAGS, 0X00);  //0
  //   TDC1000Write(TIMEOUT, 0x3B);      //33//23
  //   TDC1000Write(CLOCK_RATE, 0x1);    //6//1
  //   TDC1000Write(CONFIG_0, 0X1F);
  // }


//////////////////high mode//////////////////
  // if (height >= 160) {
  //   TDC1000Write(CONFIG_0, 0X1F);     //2A//44zz
  //   TDC1000Write(CONFIG_1, 0X41);     //41
  //   TDC1000Write(CONFIG_1, 0X41);     //41
  //   TDC1000Write(CONFIG_2, 0X2);      //02//0
  //   TDC1000Write(CONFIG_3, 0x0);      //D//C
  //   TDC1000Write(CONFIG_4, 0X1e);     //5F
  //   TDC1000Write(TOF_1, 0Xe5);        //80//40
  //   TDC1000Write(TOF_0, 0X1e);        //1E
  //   TDC1000Write(ERROR_FLAGS, 0X00);  //0
  //   TDC1000Write(TIMEOUT, 0x3B);      //33//23
  //   TDC1000Write(CLOCK_RATE, 0x6);    //6//1
  //   TDC1000Write(CONFIG_0, 0X1F);
  // }
}


void tdc1000configRead() {

  uint16_t val;
  uint16_t val1;
  uint16_t val2;
  uint16_t val3;
  uint16_t val4;
  uint16_t val5;
  uint16_t val6;
  uint16_t val7;
  uint16_t val8;
  uint16_t val9;

  val = TDC1000Read(CONFIG_0);
  val1 = TDC1000Read(CONFIG_1);
  val2 = TDC1000Read(CONFIG_2);
  val3 = TDC1000Read(CONFIG_3);
  val4 = TDC1000Read(CONFIG_4);
  val5 = TDC1000Read(TOF_1);
  val6 = TDC1000Read(TOF_0);
  val7 = TDC1000Read(ERROR_FLAGS);
  val8 = TDC1000Read(TIMEOUT);
  val9 = TDC1000Read(CLOCK_RATE);

  mySerial.print("TDC1000 CONFIG 0 = ");
  mySerial.println(val, HEX);
  mySerial.print("\n");
  mySerial.print("TDC1000 CONFIG 1 = ");
  mySerial.println(val1, HEX);
  mySerial.print("\n");
  mySerial.print("TDC1000 CONFIG 2 = ");
  mySerial.println(val2, HEX);
  mySerial.print("\n");
  mySerial.print("TDC1000 CONFIG 3 = ");
  mySerial.println(val3, HEX);
  mySerial.print("\n");
  mySerial.print("TDC1000 CONFIG 4 = ");
  mySerial.println(val4, HEX);
  mySerial.print("\n");
  mySerial.print("TDC1000 TOF1 = ");
  mySerial.println(val5, HEX);
  mySerial.print("\n");
  mySerial.print("TDC1000 TOF0 = ");
  mySerial.println(val6, HEX);
  mySerial.print("\n");
  mySerial.print("TDC1000 ERROR FLAG = ");
  mySerial.println(val7, HEX);
  mySerial.print("\n");
  mySerial.print("TDC1000 TIMEOUT = ");
  mySerial.println(val8, HEX);
  mySerial.print("\n");
  mySerial.print("TDC1000 CLOCK RATE = ");
  mySerial.println(val9, HEX);
  mySerial.print("\n");
}


void tdc7200configRead() {
  uint16_t readCONFIG1;
  uint16_t readCONFIG2;
  uint16_t readINT_STATUS;
  uint16_t readINT_MASK;

  mySerial.print("TDC7200 CONFIG1= ");
  mySerial.println(readCONFIG1, HEX);
  mySerial.print("\n");
  mySerial.print("TDC7200 CONFIG2= ");
  mySerial.println(readCONFIG2, HEX);
  mySerial.print("\n");
  mySerial.print("TDC7200 INT_STATUS= ");
  mySerial.println(readINT_STATUS, HEX);
  mySerial.print("\n");
  mySerial.print("TDC7200 INT_MASK= ");
  mySerial.println(readINT_MASK, HEX);
  mySerial.print("\n");
  mySerial.print("TDC7200 TIME1= ");
  mySerial.println(tof.spiReadReg24(0X10), HEX);
  mySerial.print("\n");
  mySerial.print("TDC7200 COUNT1= ");
  mySerial.println(tof.spiReadReg24(0X11), HEX);
  mySerial.print("\n");
  mySerial.print("TDC7200 TIME2= ");
  mySerial.println(tof.spiReadReg24(0X12), HEX);
  mySerial.print("\n");
  mySerial.print("TDC7200 COUNT2= ");
  mySerial.println(tof.spiReadReg24(0X13), HEX);
  mySerial.print("\n");
  mySerial.print("TDC7200 CALIB1= ");
  mySerial.println(tof.spiReadReg24(0X1Bu), DEC);
  mySerial.print("\n");
  mySerial.print("TDC7200 CALIB2= ");
  mySerial.println(tof.spiReadReg24(0X1Cu), DEC);
  mySerial.print("\n");
  mySerial.print("////////////////////////////////////////////");
  mySerial.print("\n");
}


char* double2s(double f, unsigned int digits) {
  int index = 0;
  static char s[16];  // buffer to build string representation

  // max digits
  if (digits > 6) digits = 6;
  long multiplier = pow(10, digits);  // fix int => long

  int exponent = int(log10(f));
  float g = f / pow(10, exponent);
  if ((g < 1.0) && (g != 0.0)) {
    g *= 10;
    exponent--;
  }

  long whole = long(g);                        // single digit
  long part = long((g - whole) * multiplier);  // # digits
  char format[16];
  sprintf(format, "%%ld.%%0%dld E%%+d", digits);
  sprintf(&s[index], format, whole, part, exponent);

  return s;
}



uint8_t TDC1000Read(const uint8_t addr) {
  SPI.beginTransaction(SPISettings(TDC1000_SPI_CLK_MAX, MSBFIRST, SPI_MODE3));
  digitalWrite(PIN_TDC1000_SPI_CS, LOW);

  SPI.transfer((addr & TDC1000_SPI_REG_ADDR_MASK) | TDC1000_SPI_REG_READ);
  uint8_t val = SPI.transfer(0u);

  digitalWrite(PIN_TDC1000_SPI_CS, HIGH);
  SPI.endTransaction();

  return val;
}



void TDC1000Write(const uint8_t addr, const uint8_t val) {
  SPI.beginTransaction(SPISettings(TDC1000_SPI_CLK_MAX, MSBFIRST, SPI_MODE3));
  digitalWrite(PIN_TDC1000_SPI_CS, LOW);

  (void)SPI.transfer16((((addr & TDC1000_SPI_REG_ADDR_MASK) | TDC1000_SPI_REG_WRITE) << 8) | val);

  digitalWrite(PIN_TDC1000_SPI_CS, HIGH);
  SPI.endTransaction();
}


static void ui64toa(uint64_t v, char* buf, uint8_t base) {
  int idx = 0;
  uint64_t w = 0;
  while (v > 0) {
    w = v / base;
    buf[idx++] = (v - w * base) + '0';
    v = w;
  }
  buf[idx] = 0;
  // reverse char array
  for (int i = 0, j = idx - 1; i < idx / 2; i++, j--) {
    char c = buf[i];
    buf[i] = buf[j];
    buf[j] = c;
  }
}
