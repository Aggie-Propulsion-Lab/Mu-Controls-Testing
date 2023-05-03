
switch(command) // This is the output pin on the Arduino we are using
  case R1:
  int solenoidPin = 1; 
  break;
  case R2:
  int solenoidPin = 2;
  break;
  case R3:
  int solenoidPin = 3;
  break;
  case R4:
  int solenoidPin = 4;
  break;
  case R5:
  int solenoidPin = 5;
  break;
  case R6:
  int solenoidPin = 6;
  break;
  case R7: 
  int solenoidPin = 7;
  break;

void setup() {
  // put your setup code here, to run once:
pinMode(solenoidPin, OUTPUT); //Sets the pin as an output
}

void loop() {
  // put your main code here, to run repeatedly:
digitalWrite(solenoidPin, HIGH); //Switch Solenoid ON
delay(1000); //Wait 1 Second
digitalWrite(solenoidPin, LOW); //Switch Solenoid OFF
delay(1000); //Wait 1 Second
}
