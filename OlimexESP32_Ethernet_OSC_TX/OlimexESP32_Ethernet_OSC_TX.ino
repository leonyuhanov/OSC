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
unsigned int oscTXPort = 5556;
LLNODE* anOSCItem;
IPAddress broadcast_ip_address = IPAddress(192,168,1,255);    //Set this to whatever subnet you have

unsigned short int eth_connected=0;
unsigned long globalTimer[3] = {0,0,0};
float exampleData[3] = {0,0,0};

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
  
	
  //Set up your OSC objects by adding in the controll names eg here we add /slider1 /slider2 /rotary1 /fader1
  oscObject.addControll("/fader1", 1, 'f', 100000);
  oscObject.addControll("/fader2", 1, 'f', 100000);
  oscObject.addControll("/fader3", 1, 'f', 100000);

  //Start GLobal timer
  oscObject.startTimer(10, globalTimer);
}

void loop() 
{

	if(eth_connected==1)
  {
  	if(oscObject.hasTimedOut(globalTimer))
    {
      //increment the example data variables
      if(exampleData[0]+0.01>1)
      {
        exampleData[0]=0;
      }
      else
      {
        exampleData[0]+=0.01;
      }
      if(exampleData[1]+0.05>1)
      {
        exampleData[1]=0;
      }
      else
      {
        exampleData[1]+=0.05;
      }
      if(exampleData[2]+0.1>1)
      {
        exampleData[2]=0;
      }
      else
      {
        exampleData[2]+=0.1;
      }
      //Store data into OSC control names
      oscObject.setValue((char*)"/fader1", exampleData[0]);
      oscObject.setValue((char*)"/fader2", exampleData[1]);
      oscObject.setValue((char*)"/fader3", exampleData[2]);
      //Transmit to each control
      oscObject.generateOSCPacket((char*)"/fader1");
      udp.writeTo((uint8_t*)oscObject.txPacketBuffer, oscObject.txPacketBufferLength, broadcast_ip_address, oscTXPort);
      oscObject.generateOSCPacket((char*)"/fader2");
      udp.writeTo((uint8_t*)oscObject.txPacketBuffer, oscObject.txPacketBufferLength, broadcast_ip_address, oscTXPort);
      oscObject.generateOSCPacket((char*)"/fader3");
      udp.writeTo((uint8_t*)oscObject.txPacketBuffer, oscObject.txPacketBufferLength, broadcast_ip_address, oscTXPort);
      //restart timer
      oscObject.startTimer(100, globalTimer);
    }
  }
  yield();
  
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
