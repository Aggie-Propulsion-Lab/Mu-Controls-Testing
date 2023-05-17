const int relay1 = 1;
int command;

void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
delay(1000);

pinMode(relay1, OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  while(Serial.dataAvailable()>0)
  {
    command = Serial.parseInt();
    switch (command) {
      case "11" // relay 1 on
        digtalWrite(relay1, HIGH);
      case "12" // relay 1 off
        digitalWrite(relay1, LOW);
      default:
        break;
    }  
  }
}

// send and receiver command through serial 
// command is set of 2 numbers. First digit indicates relay number; second digit indicates the desired state of the relay (on/off = 1/2)
// make the control code into a function
// 6 relay
// coding topics: serial, input/output mode + arduino
