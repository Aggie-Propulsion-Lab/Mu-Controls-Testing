
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
