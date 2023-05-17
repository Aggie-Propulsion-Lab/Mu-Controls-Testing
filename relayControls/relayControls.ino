const int relay1 = 1;
char command[12];

void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
delay(1000);

pinMode(relay1, OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  int commandPosition = 0;
  while(Serial.dataAvailable()>0)
  {
      command[commandPosition] = Serial.read();
      commandPosition++;
  }

  switch (command) {
    case "R1ON"
      digtalWrite(relay1, HIGH);
    case "R1OF"
      digitalWrite(relay1, LOW);
    default:
      break;
  }  
}
