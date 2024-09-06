/*
      Olimex ESP32 Ethernet OSC Example
        
        
*/
#include "Arduino.h"
#include <WiFi.h>
#include "osc.h"
#include <AsyncUDP.h>
#include <ETH.h>
#include <string>

#define ETH_CLK_MODE ETH_CLOCK_GPIO17_OUT
#define ETH_PHY_POWER 12


//OSC Data
short int packetSize=0;
osc oscObject;
AsyncUDP udp;
unsigned int oscPort = 5555;
LLNODE* anOSCItem;

unsigned short int eth_connected=0;

void setup() 
{
  //INIT Serial 
  Serial.begin(115200);
  Serial.printf("\r\n\r\n\r\nSystem Booting....\r\n");

  //DISABLE WIFI
  WiFi.mode(WIFI_OFF);

  //Enable Etherner
  WiFi.onEvent(WiFiEvent);
  ETH.begin();
  Serial.print("\r\nONLINE via Ethernet\t");
  Serial.print(ETH.localIP());
  
  //Set up UDP listener
  udp.listen(oscPort);
  udp.onPacket(pollOSC);
	
  //Set up your OSC objects by adding in the controll names eg here we add /slider1 /slider2 /rotary1 /fader1
  oscObject.addControll("/slider1", 1, 'f', 100000);
  oscObject.addControll("/slider2", 1, 'f', 100000);
  oscObject.addControll("/rotary1", 1, 'f', 100000);
  oscObject.addControll("/fader1", 1, 'f', 100000); 
}

void loop() 
{

	if(eth_connected==1)
  {
  	if( !oscObject.hasControllTimedOut( (char*)"/slider1" ) )
  	{
  		anOSCItem = oscObject.findByName( (char*)"/slider1" );
  		Serial.printf("\r\nslider1[%f]", anOSCItem->_currentValue);
  		oscObject.timeOutControll( (char*)"/slider1" );
  	}
  	else if(!oscObject.hasControllTimedOut( (char*)"/slider2" ) )
  	{
  		anOSCItem = oscObject.findByName( (char*)"/slider2" );
  		Serial.printf("\r\nslider2[%f]", anOSCItem->_currentValue);
  		oscObject.timeOutControll( (char*)"/slider2" );
  	}
  	else if(!oscObject.hasControllTimedOut( (char*)"/rotary1" ) )
  	{
  		anOSCItem = oscObject.findByName( (char*)"/rotary1" );
  		Serial.printf("\r\nrotary1[%f]", anOSCItem->_currentValue);
  		oscObject.timeOutControll( (char*)"/rotary1" );
  	}
  	else if(!oscObject.hasControllTimedOut( (char*)"/fader1" ) )
  	{
  		anOSCItem = oscObject.findByName( (char*)"/fader1" );
  		Serial.printf("\r\nfader1[%f]", anOSCItem->_currentValue);
  		oscObject.timeOutControll( (char*)"/fader1" );
  	}
  }
  yield();
  
}

void pollOSC(AsyncUDPPacket &packet)
{
   packetSize = packet.length();
   if(packetSize)
   {
      //if there is an OSC packet, parse it
	  oscObject.clearBuffer();
      memcpy(oscObject.packetBuffer, packet.data(), packet.length());
      oscObject.currentPacketSize = packet.length();
      oscObject.toggleState();
      oscObject.parseOSCPacket();
   }
}

void WiFiEvent(WiFiEvent_t event)
{
  switch (event) 
  {
    case ARDUINO_EVENT_ETH_START:           //SYSTEM_EVENT_ETH_START:
                                    Serial.println("ETH Started");
                                    //set eth hostname here
                                    ETH.setHostname("esp32-ethernet");
                                    break;
    case ARDUINO_EVENT_ETH_CONNECTED:       //SYSTEM_EVENT_ETH_CONNECTED:
                                    Serial.println("ETH Connected");
                                    break;
    case ARDUINO_EVENT_ETH_GOT_IP:               //SYSTEM_EVENT_ETH_GOT_IP:
                                    Serial.print("ETH MAC: ");
                                    Serial.print(ETH.macAddress());
                                    Serial.print(", IPv4: ");
                                    Serial.print(ETH.localIP());
                                    if (ETH.fullDuplex()) {
                                      Serial.print(", FULL_DUPLEX");
                                    }
                                    Serial.print(", ");
                                    Serial.print(ETH.linkSpeed());
                                    Serial.println("Mbps");
                                    eth_connected = 1;
                                    break;
    case ARDUINO_EVENT_ETH_DISCONNECTED:      //SYSTEM_EVENT_ETH_DISCONNECTED:
                                    Serial.println("ETH Disconnected");
                                    eth_connected = 0;
                                    break;
    case ARDUINO_EVENT_ETH_STOP:              //SYSTEM_EVENT_ETH_STOP:
                                    Serial.println("ETH Stopped");
                                    eth_connected = 0;
                                    break;
    default:                        break;
  }
}
