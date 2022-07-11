#include "Arduino.h"
#include <WiFi.h>
#include <WiFiUdp.h>
#include "osc.h"


//WIFI
const char *wifi_ssid = "WIFISSID";
const char *wifi_password = "WIFIKEY";

//OSC Data
short int packetSize=0;
osc oscObject;
WiFiUDP oscUDP;
unsigned int oscPort = 5555;


void setup() 
{
  //WiFi.mode(WIFI_OFF);
  Serial.begin(115200);
  Serial.printf("\r\n\r\n\r\nSystem Booting....\r\n");
  
  //Eable WIFI
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

  //Set up UDP server for reciving UDP on port oscPort
  oscUDP.begin(oscPort);
  //Set up your OSC objects by adding in the controll names eg here we add /slider1 /slider2 /rotary1 /fader1
  oscObject.addControll("slider1");
  oscObject.addControll("slider2");
  oscObject.addControll("rotary1");
  oscObject.addControll("fader1"); 
}

void loop() 
{
  //Poll the network for any OSC packets via UDP
  pollOSC();
  //yield
  yield();
  //do something with your data if its changed in pollOSC()
  
}

void pollOSC()
{
   packetSize = oscUDP.parsePacket();
   if(packetSize)
   {
      //if there is an OSC packet, parse it
	  oscObject.clearBuffer();
      oscUDP.read(oscObject.packetBuffer, packetSize);
      oscObject.currentPacketSize = packetSize;
      oscObject.toggleState();
      oscObject.parseOSCPacket();
   }
   
   //Check incoming data and do stuff with your controlls
   if(oscObject.isChanged)
   {
    /*
		If you have new data do something with it
		
	oscObject.getValue(0) will return the FLOAT in /slider1
	oscObject.getValue(1) will return the FLOAT in /slider2
	oscObject.getValue(2) will return the FLOAT in /rotary1
	oscObject.getValue(3) will return the FLOAT in /fader1
	
	*/
	
	//togle the change state so you dont get stuck in a read loop
    oscObject.toggleState();
   }
}

