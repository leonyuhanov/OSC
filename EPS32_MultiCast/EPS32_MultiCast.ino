#include "Arduino.h"
#include <WiFi.h>
#include <AsyncUDP.h>
#include "osc.h"


//WIFI
const char *wifi_ssid = "YOURSSID";
const char *wifi_password = "YOURKEY";

//OSC Vars
osc oscObject;
AsyncUDP oscUDP;
unsigned int oscRXPort = 5555;    //port that this device will RECIVE packets on
unsigned int oscTXPort = 5556;    //port that this device will SEND packets to
//Multicast address to TX to
IPAddress multicast_ip_address = IPAddress(239,1,1,1);    //Muticast IP
float exampleData[2] = {0,0};

void setup() 
{
  Serial.begin(115200);
  Serial.printf("\r\n\r\n\r\nSystem Booting....\r\n");
  
  //Eable & connect to WIFI
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi_ssid, wifi_password);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.printf(".");
    delay(100);
  }
  delay(100);
  Serial.printf("\r\n\tConnected to WIFI\t");
  Serial.print(WiFi.localIP());

  //Set up UDP server for reciving UDP on port oscRXPort
  oscUDP.listenMulticast(multicast_ip_address, oscRXPort);
  //Register method that handles INCOMING OSC data
  oscUDP.onPacket(handleData);

  //Set up your OSC objects by adding in the controll names eg here we add /slider1 /slider2 /rotary1 /fader1
  oscObject.addControll((char*)"/fader1", 1, 'f');
  oscObject.addControll((char*)"/fader2", 1, 'f');
}

void loop() 
{
  //Modify value of 'fader1'
  exampleData[0]+=0.01;
  oscObject.setValue((char*)"/fader1", exampleData[0]);
  //Modify value of 'fader2'
  exampleData[1]+=0.01;
  oscObject.setValue((char*)"/fader2", exampleData[1]);

  //send value to "/fader1"  
  oscObject.generateOSCPacket((char*)"/fader1");
  oscUDP.writeTo((uint8_t*)oscObject.txPacketBuffer, oscObject.txPacketBufferLength, multicast_ip_address, oscTXPort);
  //send value to "/fader2" 
  oscObject.generateOSCPacket((char*)"/fader2");
  oscUDP.writeTo((uint8_t*)oscObject.txPacketBuffer, oscObject.txPacketBufferLength, multicast_ip_address, oscTXPort);

  //reset example data to 0 is gerater than 1
  if(exampleData[0]>1)
  {
    exampleData[0]=0;
  }
  if(exampleData[1]>1)
  {
    exampleData[1]=0;
  }
  //Wait for 1/4 second
  delay(1000);
}

//debug to check your packet
void dumpString(char* tempString, unsigned short int stringLength)
{
  unsigned short int stringCnt=0;
  for(stringCnt=0; stringCnt<stringLength; stringCnt++)
  {
    Serial.printf("[%d]->(%c)", tempString[stringCnt], tempString[stringCnt]);
  }
}

void handleData(AsyncUDPPacket packet)
{
  memcpy(oscObject.packetBuffer, packet.data(), packet.length());
  oscObject.currentPacketSize = packet.length();
  oscObject.toggleState();
  oscObject.parseOSCPacket();
  Serial.printf("\r\n\tGot\t%d\tBytes", packet.length());
}
