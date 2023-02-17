#include <ads1248.h>
#include <SPI.h>

ads1248 adc1;

/*
Register addresses
---------------------
0x00h ID
0x01h STATUS
0x02h INPMUX
0x03h PGA
0x04h DATARATE
0x05h REF
0x06h IDACMAG
0x07h IDACMUX
0x08h VBIAS
0x09h SYS
0x0Ah OFCAL0
0x0Bh OFCAL1
0x0Ch OFCAL2
0x0Dh FSCAL0
0x0Eh FSCAL1
0x0Fh FSCAL2
0x10h GPIODAT
0x11h GPIOCON
*/

/*
  - START: set HIGH
  - RESET: set HIGH
  - CLOK: set LOW
  - Need to select pins on teensy 4.1
 */
int  START = 9;
int  CS = 10;
int  DRDY = 12;
int _RESET = 8;

unsigned long startTime = 0;
unsigned long currentTime = 0;
unsigned long elapsedTime = 1/samplingFreq;

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
  adc1.begin();
  adc1.reset();
  delay(10);

  // write to registers
  // configure all the registers here: sampling rate, pga, etc.
  // start with 200sps (assuming 10 packets/s)
  // STATUS(0x1h) need to be clear once POR (power on reset) occur: .begin and .reset
  adc1.setRegister(0x01h, B10000000);
  adc1.setRegister(0x4h, B00011000); 
  

  adc1.directCommand(SELFOCAL);

}

void loop() {

  //clock based to run the loop

  while(record button is hit)
  {
    
    currentTime = millies();
    while(currentTime-startTime <= elapsedTime)
    {
      // select mux
      // get conversion
      // delay relative to chosen frequency
      adc1.setRegisterValue(MUX, value);
      adc1.setRegisterValue(PGA, value);
      PT1 = adc1.GetConversion();
    }
    startTime = currentTime;
  }
  
void setRegister(uint_8t address, value)
{

}

unsigned long getRegister(uint_8t address)
{

}

void reset()
{

}

long getData(uint_8t address)
{

}

void directCommand()
{

}
}
