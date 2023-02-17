#include <ads1248.h>
#include <SPI.h>

ads1248 adc1;

/*
  - START: set HIGH
  - RESET: set HIGH
  - CLOK: set LOW
  - Need to select pins on teensy 4.1
 */
int  START = 9;
int  CS = 10;
int  DRDY = 12; //DOUT
int _RESET = 8;

unsigned long startTime = 0;
unsigned long elapsedTime = 0;

samplingFreq = 100; 
samplingPeriod = 1/samplingFreq;

PT1 = 0;

//set up whatever numbers/coefficients needed here for sensors

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while (!Serial) {
    
  }
  Serial.println("Serial online");

  // initialize and calibrate adc
  adc1.begin();
  adc1.reset();
  delay(10);

  // write to registers
  // configure all the registers here: sampling rate, pga, etc.
  // start with 200sps (assuming 10 packets/s)
  // STATUS(0x1h) need to be clear once POR (power on reset) occur: .begin and .reset
  adc1.setRegisterValue(0x01h, B10000000);
  adc1.setRegisterValue(0x4h, B00011000); 
  

  adc1.SendCMD(SELFOCAL);

}

void loop() {

  //clock based to run the loop

  while(record button is hit)
  {
    // select mux
    // get conversion
    // delay relative to chosen frequency
    adc1.setRegisterValue(MUX, value);
    adc1.setRegisterValue(PGA, value);
    PT1 = adc1.GetConversion();
    delay(samplingPeriod);  
  }
  
}
