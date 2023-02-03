#include <ads1248.h>
#include <SPI.h>

ads1248 adc1;

/*
  - START: set HIGH
  - RESET: set HIGH
  - CLOK: set LOW
 */
int  START = 0;
int  CS = 0;
int  DRDY = 0;
int _RESET = 0;

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
  // start with 160sps (assuming 10 packets/s)
  adc1.setRegisterValue(adress, B00000101); 

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
