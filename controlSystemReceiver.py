from socket import *
import time
portNum=2 #Match to teensy
serverAddr = {"1.2.3.4.5",portNum} #Match to teensy
clientSocket = socket(AF_INET,SOCK_DGRAM)
clientSocket.settimeout(1) #wait 1 Second then time out

while(True):
    dataToSend="pressure"#data to send
    clientSocket.sendto(dataToSend,serverAddr)#Send command
    try:
        rcvData,serverAddr = clientSocket.recvfrom(2048); #Tune 2048 to max data size
        print(rcvData)
    except:
        print("Failed data return")
    #Uncomment to sleep between auto commands
    #time.sleep(2);
