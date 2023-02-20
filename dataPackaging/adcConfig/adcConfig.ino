#include <SPI.h>
#include <math.h>

/*
Register addresses
---------------------
*/
#define ID = 0x00h 
#define STATUS =  0x01h STATUS
#define INPUTMUX = 0x02h 
#define PGA = 0x03h 
#define DATARATE = 0x04h 
#define REF = 0x05h
#define IDACMAG = 0x06h 
#define IDACMUX = 0x07h
#define VBIAS = 0x08h
#define SYS = 0x09h
#define OFCAL0 = 0x0Ah
#define OFCAL1 = 0x0Bh
#define OFCAL2 = 0x0Ch 
#define FSCAL0 = 0x0Dh 
#define FSCAL1 = 0x0Eh
#define FSCAL2 = 0x0Fh
#define GPIODAT = 0x10h 
#define GPIOCON = 0x11h 

int  ADC1 [4] = [10, 12, 8, 9]; //[CS, DRDY, START, RESET]

float samplingFreq = 200; //sampling at 200Hz
unsigned long elapsedTime = (1.0/samplingFreq)*pow(10,6); //exponential math function

// change the pga values appropriately
PT_PGA_value = b01010101;
LC_PGA_value = b01010101;

PT1 = 0;
PT2 = 0; 
PT3 = 0;
PT4 = 0;

//set up whatever numbers/coefficients needed here for sensors

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while (!Serial) {
    
  }
  Serial.println("Serial online");

  // initialize and calibrate adc
  begin(ADC1);
  reset(ADC1);
  delay(10);

  // write to registers
  // configure all the registers here: sampling rate, pga, etc.
  // start with 200sps (assuming 10 packets/s)
  // STATUS(0x1h) need to be clear once POR (power on reset) occur: .begin and .reset
  setRegister(ADC1, 0x01h, B10000000);
  setRegister(ADC1, 0x4h, B00011000);   

  directCommand(ADC1, SELFOCAL);

  elapsedMicros adcCycle = 0;
}

void loop() {

  //clock based to run the loop

  while(data record pin is high) // toggle switch to start/stop recording data
  {
    if(adcCycle >= elapsedTime)
    {
      adcCycle = 0;
      // select mux
      // get conversion
      // delay relative to chosen frequency
      setRegisterValue(ADC1, MUX, value);
      setRegisterValue(ADC1, PGA, value);
      PT1 = GetConversion(ADC1);
    }
  }
  
}

void setRegister(uint_8t adc,uint_8t address, value)
{
  while(digitalRead(adc[1])) {}
  SPI.beginTransaction(SPISettings(1920000,MSBFIRST,SPI_MODE1));
  
  digitalWrite(adc[0], LOW); //pull CS low
  delayNanoseconds(22); //td(CSSC) refer to timing characteristics
  
  SPI.transfer(WREG|address); // send write register command with address; 2 bytes
  SPI.transfer(value); // send the third byte as data byte
  
  digitalWrite(adc[0], HIGH); // end the process by pulling CS high
  delayNanoseconds(22); //td(SCCS) refer to timing characteristics
  
  SPI.endTransaction();
}

unsigned long getRegister(uint_8t adc, uint_8t address)
{

}

// send reset command
void reset(uint_8t adc)
{

}

long getData(uint_8t adc, uint_8t address)
{

}

void directCommand(uint_8t adc)
{

}