
// SPI pins
int  ADC1 [4] = [10, 12, 8, 9]; //[CS, DRDY, START, RESET]; remember to change value for arduino nano

//Variables
double VREF = 2.50; //Value of V_ref. In case of internal V_ref, it is 2.5 V
double voltage = 0; //Converted RAW bits. 
int CS_Value; //we use this to store the value of the bool, since we don't want to directly modify the CS_pin

//Values for registers
uint8_t registerAddress; //address of the register, both for reading and writing - selects the register
uint8_t registerValueR; //this is used to READ a register
uint8_t registerValueW; //this is used to WRTIE a register
int32_t registerData; //this is used to store the data read from the register (for the AD-conversion)
uint8_t directCommand; //this is used to store the direct command for sending a command to the ADS1256
String ConversionResults; //Stores the result of the AD conversion
String PrintMessage; //this is used to concatenate stuff into before printing it out. 

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);  //We will need high datarate, so it should be a high baud rate
  delay(1000);
  
	Serial.println("*ADS1256 Initialization...."); //Some message
	initialize_ADS1256(); //run the initialization function 
 
  delay(1000);
	Serial.println("*Initialization finished!"); //Confirmation message
 
	reset_ADS1256(); //Reset the ADS1256
	userDefaultRegisters(); //Set up the default registers
	printInstructions(); //Print the instructions for the commands used in the code
}

void loop() {
  // put your main code here, to run repeatedly:

}

// Functions
void reset_ADS1256()
{
	SPI.beginTransaction(SPISettings(1920000, MSBFIRST, SPI_MODE1)); // initialize SPI with  clock, MSB first, SPI Mode1

	digitalWrite(ADC1[0], LOW); //CS_pin goes LOW

	delayMicroseconds(10); //wait

	SPI.transfer(0xFE); //Reset

	delay(2); //Minimum 0.6 ms required for Reset to finish.

	SPI.transfer(0x0F); //Issue SDATAC: stop read data continuously

	delayMicroseconds(100);

	digitalWrite(ADC1[0], HIGH); //CS_pin goes HIGH

	SPI.endTransaction();

 Serial.println("*Reset DONE!"); //confirmation message
}
void initialize_ADS1256()	//starting up the chip by making the necessary steps. This is in the setup() of the Arduino code.
{
	//Setting up the pins first
	//Chip select
	pinMode(ADC1[0], OUTPUT); //Chip select is an output
	digitalWrite(ADC1[0], LOW); //Chip select LOW

	SPI.begin(); //start SPI (Arduino/STM32 - ADS1256 communication protocol)
  //The STM32-ADS1256 development board uses a different SPI channel (SPI_2)
  //For more info: https://youtu.be/3Rlr0FCffr0

	CS_Value = ADC1[0]; //We store the value of the CS_pin in a variable

	//DRDY
	pinMode(ADC1[1], INPUT); //DRDY is an input
	pinMode(ADC[3], OUTPUT); //RESET pin is an output
	digitalWrite(ADC1[3], LOW); //RESET is set to low 

	delay(500); // Wait

	digitalWrite(ADC1[3], HIGH); //RESET is set to high

	delay(500); // Wait

}

void cycleDifferential() 
{
  //Relevant viodeo: https://youtu.be/GBWJdyjRIdM
  
  int cycle = 1;  

  //outside while() loop, we have to switch to the first differential channel ([AIN0+AIN1]) 
  //writeRegister(1, 1); //B00000001 = 1;  [AIN0+AIN1]
  
  registerData = 0;
  
  SPI.beginTransaction(SPISettings(1920000, MSBFIRST, SPI_MODE1)); //We start this SPI.beginTransaction once.
  
  digitalWrite(ADC1[0], LOW); //CS must stay LOW during the entire sequence [Ref: P34, T24]
  SPI.transfer(0x50 | 1); // 0x50 = WREG //1 = MUX
  SPI.transfer(0x00); 
  SPI.transfer(B00000001);  //AIN0+AIN1
  digitalWrite(ADC1[0], HIGH);
  SPI.endTransaction();
  

  while (Serial.read() != 's')
  {   
    for (cycle = 1; cycle < 5; cycle++)
    {
      //we cycle through all the 4 differential channels with the RDATA

      //RDATA = B00000001
      //SYNC = B11111100
      //WAKEUP = B11111111

      //Steps are on Page21
      //Step 1. - Updating MUX 
      
      
      while (digitalRead(ADC1[1])) {} 
      
      switch (cycle)
      {
      case 1: //Channel 2        
        digitalWrite(ADC1[0], LOW); //CS must stay LOW during the entire sequence [Ref: P34, T24]
        SPI.transfer(0x50 | 1); // 0x50 = WREG //1 = MUX
        SPI.transfer(0x00); 
        SPI.transfer(B00100011);  //AIN2+AIN3
        break;

      case 2: //Channel 3        
        digitalWrite(ADC1[0], LOW); //CS must stay LOW during the entire sequence [Ref: P34, T24]
        SPI.transfer(0x50 | 1); // 0x50 = WREG //1 = MUX
        SPI.transfer(0x00); 
        SPI.transfer(B01000101); //AIN4+AIN5
        break;

      case 3: //Channel 4
        digitalWrite(ADC1[0], LOW); //CS must stay LOW during the entire sequence [Ref: P34, T24]
        SPI.transfer(0x50 | 1); // 0x50 = WREG //1 = MUX
        SPI.transfer(0x00); 
        SPI.transfer(B01100111); //AIN6+AIN7          
        break;      

      case 4: //Channel 1       
        digitalWrite(ADC1[0], LOW); //CS must stay LOW during the entire sequence [Ref: P34, T24]
        SPI.transfer(0x50 | 1); // 0x50 = WREG //1 = MUX
        SPI.transfer(0x00); 
        SPI.transfer(B00000001); //AIN0+AIN1
        break;
      }            
      
      SPI.transfer(B11111100); //SYNC      

      delayMicroseconds(4); //t11 delay 24*tau = 3.125 us //delay should be larger, so we delay by 4 us

      SPI.transfer(B11111111); //WAKEUP

      //Step 3. 
      //Issue RDATA (0000 0001) command      
      SPI.transfer(B00000001);

      //Wait t6 time (~6.51 us) REF: P34, FIG:30.
      delayMicroseconds(5);

      //step out the data: MSB | mid-byte | LSB,

      //registerData is ZERO
      registerData |= SPI.transfer(0x0F); //MSB comes in, first 8 bit is updated // '|=' compound bitwise OR operator
      registerData <<= 8;         //MSB gets shifted LEFT by 8 bits
      registerData |= SPI.transfer(0x0F); //MSB | Mid-byte
      registerData <<= 8;         //MSB | Mid-byte gets shifted LEFT by 8 bits
      registerData |= SPI.transfer(0x0F); //(MSB | Mid-byte) | LSB - final result
      //After this, DRDY should go HIGH automatically   
      
      //Constructing an output  
      ConversionResults = ConversionResults + registerData;
      ConversionResults = ConversionResults + "\t";     
      //---------------------      
      registerData = 0;
      digitalWrite(ADC1[0], HIGH); //We finished the command sequence, so we switch it back to HIGH                  

      //Expected output when using a resistor ladder of 1k resistors and the ~+5V output of the Arduino:
      //Formatting  Channel 1 Channel 2 Channel 3 Channel 4 
      /*
      16:14:23.066 -> 4.79074764  4.16625738  3.55839943  2.96235866  
      16:14:23.136 -> 4.79277801  4.16681241  3.55990862  2.96264190  
      16:14:23.238 -> 4.79327344  4.16698741  3.55968427  2.96277694  
      */

    }
    Serial.println(ConversionResults);
    ConversionResults = "";
  }
  SPI.endTransaction();
}

void continuousConversion() //Cycling through all (8) single ended channels
{ 
  int cycle = 1;  
  registerData = 0;
  SPI.beginTransaction(SPISettings(1920000, MSBFIRST, SPI_MODE1));

  while (Serial.read() != 's')
  {
    for (cycle = 1; cycle < 9; cycle++)
    {
      //we cycle through all the 8 single-ended channels with the RDATAC
      //INFO:
      //RDATAC = B00000011
      //SYNC = B11111100
      //WAKEUP = B11111111     
      //---------------------------------------------------------------------------------------------
      /*Some comments regarding the cycling:
      When we start the ADS1256, the preconfiguration already sets the MUX to [AIN0+AINCOM].
      When we start the RDATAC (this function), the default MUX ([AIN0+AINCOM]) will be included in the
      cycling which means that the first readout will be the [AIN0+AINCOM]. But, before we read the data
      from the [AIN0+AINCOM], we have to switch to the next register already, then start RDATA. This is
      demonstrated in Figure 19 on Page 21 of the datasheet. 

      Therefore, in order to get the 8 channels nicely read and formatted, we have to start the cycle
      with the 2nd input of the ADS1256 ([AIN1+AINCOM]) and finish with the first ([AIN0+AINCOM]).

         \ CH1 | CH2 CH3 CH4 CH5 CH6 CH7 CH8 \ CH1 | CH2 CH3 ...

      The switch-case is between the  two '|' characters
      The output (one line of values) is between the two '\' characters.
      *///-------------------------------------------------------------------------------------------
      //Steps are on Page21
      //Step 1. - Updating MUX       
      while (digitalRead(ADC1[1])) {} //waiting for DRDY
      
      switch (cycle) 
      {
        //Channels are written manually, so we save time on switching the SPI.beginTransaction on and off.
        case 1: //Channel 2          
            digitalWrite(ADC1[0], LOW); //CS must stay LOW during the entire sequence [Ref: P34, T24]
            SPI.transfer(0x50 | 1); // 0x50 = WREG //1 = MUX
            SPI.transfer(0x00); 
            SPI.transfer(B00011000);  //AIN1+AINCOM           
          break;

        case 2: //Channel 3
            digitalWrite(ADC1[0], LOW); //CS must stay LOW during the entire sequence [Ref: P34, T24]
            SPI.transfer(0x50 | 1); // 0x50 = WREG //1 = MUX
            SPI.transfer(0x00); 
            SPI.transfer(B00101000);  //AIN2+AINCOM            
          break;

        case 3: //Channel 4
            digitalWrite(ADC1[0], LOW); //CS must stay LOW during the entire sequence [Ref: P34, T24]
            SPI.transfer(0x50 | 1); // 0x50 = WREG //1 = MUX
            SPI.transfer(0x00); 
            SPI.transfer(B00111000);  //AIN3+AINCOM            
          break;

        case 4: //Channel 5
            digitalWrite(ADC1[0], LOW); //CS must stay LOW during the entire sequence [Ref: P34, T24]
            SPI.transfer(0x50 | 1); // 0x50 = WREG //1 = MUX
            SPI.transfer(0x00); 
            SPI.transfer(B01001000);  //AIN4+AINCOM 
          break;

        case 5: //Channel 6
            digitalWrite(ADC1[0], LOW); //CS must stay LOW during the entire sequence [Ref: P34, T24]
            SPI.transfer(0x50 | 1); // 0x50 = WREG //1 = MUX
            SPI.transfer(0x00); 
            SPI.transfer(B01011000);  //AIN5+AINCOM            
          break;

        case 6: //Channel 7
            digitalWrite(ADC1[0], LOW); //CS must stay LOW during the entire sequence [Ref: P34, T24]
            SPI.transfer(0x50 | 1); // 0x50 = WREG //1 = MUX
            SPI.transfer(0x00); 
            SPI.transfer(B01101000);  //AIN6+AINCOM            
          break;

        case 7: //Channel 8
            digitalWrite(ADC1[0], LOW); //CS must stay LOW during the entire sequence [Ref: P34, T24]
            SPI.transfer(0x50 | 1); // 0x50 = WREG //1 = MUX
            SPI.transfer(0x00); 
            SPI.transfer(B01111000);  //AIN7+AINCOM            
          break;

        case 8: //Channel 1
            digitalWrite(ADC1[0], LOW); //CS must stay LOW during the entire sequence [Ref: P34, T24]
            SPI.transfer(0x50 | 1); // 0x50 = WREG //1 = MUX
            SPI.transfer(0x00); 
            SPI.transfer(B00001000); //AIN0+AINCOM              
          break;
      }

      //Step 2.     

      //Issue RDATA (0000 0001) command
      SPI.transfer(B11111100); //SYNC ~ stop current conversion and restart digital filter

      delayMicroseconds(4); //t11 delay 24*tau = 3.125 us //delay should be larger, so we delay by 4 us
      
      SPI.transfer(B11111111); //WAKEUP

      //Step 3. 
      //Issue RDATA (0000 0001) command
      SPI.transfer(B00000001);

      //Wait t6 time (~6.51 us) REF: P34, FIG:30.
      delayMicroseconds(5);

      //step out the data: MSB | mid-byte | LSB,

      //registerData is ZERO
      registerData |= SPI.transfer(0x0F); //MSB comes in, first 8 bit is updated // '|=' compound bitwise OR operator
      registerData <<= 8;         //MSB gets shifted LEFT by 8 bits
      registerData |= SPI.transfer(0x0F); //MSB | Mid-byte
      registerData <<= 8;         //MSB | Mid-byte gets shifted LEFT by 8 bits
      registerData |= SPI.transfer(0x0F); //(MSB | Mid-byte) | LSB - final result
      //After this, DRDY should go HIGH automatically   

      //Constructing an output  
      ConversionResults = ConversionResults + registerData;
      ConversionResults = ConversionResults + "\t";     
      //---------------------      
      registerData = 0;

      digitalWrite(ADC1[0], HIGH); //We finished the command sequence, so we switch it back to HIGH
          
      //Expected output when using a resistor ladder of 1k resistors and the ~+5V output of the Arduino:
      //Formatting  Channel 1 Channel 2 Channel 3 Channel 4 Channel 5 Channel 6 Channel 7 Channel 8
      /*
      12:41:40.280 -> 4.78714609  4.16558074  3.55143761  2.96154289  2.37305951  1.78396224  1.19539093  0.60204453
      12:41:40.450 -> 4.78708410  4.16603088  3.55298733  2.96177434  2.37242603  1.78440055  1.19551980  0.60218434
      12:41:40.620 -> 4.78826045  4.16563510  3.55332374  2.96192693  2.37245225  1.78419756  1.19552350  0.60213699
      */                    
    }
    Serial.println(ConversionResults);   
    ConversionResults="";
  } 
  SPI.endTransaction(); 
}
void sendDirectCommand(uint8_t directCommand)
{
	//Direct commands can be found in the datasheet Page 34, Table 24. 
  //Use binary, hex or dec format. 
	//Here, we want to use everything EXCEPT: RDATA, RDATAC, SDATAC, RREG, WREG
	//We don't want to involve DRDY here. We just write, but don't read anything.

	//Start SPI
	SPI.beginTransaction(SPISettings(1700000, MSBFIRST, SPI_MODE1));

	digitalWrite(CS_pin, LOW); //REF: P34: "CS must stay low during the entire command sequence"

	delayMicroseconds(5); //t6 - maybe not necessary

	SPI.transfer(directCommand); //Send Command

	delayMicroseconds(5); //t6 - maybe not necessary

	digitalWrite(CS_pin, HIGH); //REF: P34: "CS must stay low during the entire command sequence"

	SPI.endTransaction();

}

void userDefaultRegisters()
{
	// This function is "manually" updating the values of the registers then reads them back.
	// This function should be used in the setup() after performing an initialization-reset process 
  // I use the below listed settings for my "startup configuration"
	/*
		REG   VAL     USE
		0     54      Status Register, Everything Is Default, Except ACAL and BUFEN
		1     1       Multiplexer Register, AIN0 POS, AIN1 POS
		2     0       ADCON, Everything is OFF, PGA = 1
		3     99      DataRate = 50 SPS		
    */	
    
	//We update the 4 registers that we are going to use
  
	delay(500);
  
  writeRegister(0x00, B00110110); //STATUS                      
	delay(200);
	writeRegister(0x01, B00000001); //MUX AIN0+AIN1
	delay(200);
	writeRegister(0x02, B00000000); //ADCON
	delay(200);
	writeRegister(0x03, B01100011); //DRATE - DEC[99] - 50 SPS
	delay(500);
  sendDirectCommand(B11110000);	// SELFCAL
	Serial.println("*Register defaults updated!");
}