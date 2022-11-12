#include <Ethernet.h>
#include <EthernetUdp.h>
#include <SPI.h>
#include "TeensyThreads.h"
byte mac[] = {x};//Assign available mac on network
IPAddress ip(1,2,3,4,5);//Assign address
uint16_t portNumber = 1;//Assign available port on network
int packetBuffer[UDP_TX_MAX_PACKET_SIZE];//Change to sum of all data resolutions
EthernetUdp mainConnection;//Library object, assign packet size and string variable for parsing data
//Input from control frame
int reqSize;
String rcvdReq;

//Var
const int chipSel=10;
int16_t loadC;

#include "HX711.h"
HX711 cell;
uint8_t dataPin = 6;
uint8_t clockPin = 7;

void initSpi(){
  pinMode(chipSel,OUTPUT);
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE1);
  }

void readADC(){
  //Set ~DRDY low, then send 16 sclks 
  //Set continuous mode
  //SPI.transfer(0b00010100); 
  //Synch readings
  //SPI.transfer(0b00000100);

  //First test
  SPI.transfer(B0010000000001111);
  threads.yield();
  //           True, 4 bit reg, True, # reg to read 
  }

void beginLC(){
  if(cell.begin(dataPin, clockPin)){
      cell.set_scale(31.79);       // Experimentally determine calibrate coefficient. 20 kg => 127.15, 5 kg => 420.0983
      cell.tare();
    }else{
      Serial.println("Failed to initialize load cell, check data lines");
      }
  }

void beginADC(){
  //Min 16ms
  delayMicroseconds(20); //Vd
  SPI.transfer(B00000110); //Res
  //Last bit = x
  delayMicroseconds(1); //Fres
  //PGA=16,Internal Ref, 20SPS
  }

void readIncoming(EthernetUDP mainConnection){
  if(mainConnection.parsePacket()>0){
    mainConnection.read(packetBuffer,UDP_TX_MAX_PACKET_SIZE);//Change secondary to sum of peripheral resolution
    String rcvdReq(packetBuffer);
    Serial.print("Received command: ");
    Serial.println(rcvdReq);
    }
  memset(packetBuffer,0,UDP_TX_MAX_PACKET_SIZE);
  threads.yield();
  }

void sendData(EthernetUDP mainConnection, string dataToSend){
    mainConnection.beginPacket(mainConnection.remoteIP(),mainConnection.remotePort()); //Potetntially change remote to specific IP
    mainConnection.print(dataToSend);//Concatenate all before sending
    mainConnection.endPacket();
    memset(packetBuffer,0,UDP_TX_MAX_PACKET_SIZE);
    threads.yield();
  }

void actuate(int pinToActuate, bool controlSignal){
  if(controlSignal){
    digitalWrite(pinToActuate,HIGH);
  }
  else{
    digitalWrite(pinToActuate,LOW)
    }
   threads.yield();
  }

void setup() {
//Initiate main connection
Ethernet.begin(mac,ip);
mainConnection.begin(portNumber);
threads.addThread(readIncoming);
threads.addThread(readADC);
threads.addThread(sendData);
threads.addThread(actuate);
//Load Cell Calibration
  beginLC();
}

void loop() {
  // put your main code here, to run repeatedly:

}
