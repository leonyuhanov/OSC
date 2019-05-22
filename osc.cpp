#include "osc.h"


osc::osc()
{  
  maxPacketBufferSize = 255;
  packetBuffer = new char[maxPacketBufferSize];
  controllID = new char[3];
}

byte osc::parseOSCPacket()
{  
  //  [{/}->[47]{f}->[102]{a}->[97]{d}->[100]{e}->[101]{r}->[114]{B}->[66]{l}->[108]{u}->[117]{e}->[101][0][0]{,}->[44]{f}->[102][0][0]{C}->[67][29][193][138]]
  
  //Look for controll type at index 1
  if(packetBuffer[1]=='f' && packetBuffer[2]=='a' && packetBuffer[3]=='d' && packetBuffer[4]=='e' && packetBuffer[5]=='r')
  {
    //Fader
    //Index 6,7 & 8 are the Controll ID eg 001 maximum is 999
    memcpy(controllID, packetBuffer+(sizeof(char)*6), 3);
    //controllID[0] = packetBuffer
    //Set Current Controll ID
    currentControllID = atoi(controllID);
    //Set Current Controll Value 
    floatCharPointer = (unsigned char*)&currentControllValue;
    floatCharPointer[0] = packetBuffer[currentPacketSize-1];
    floatCharPointer[1] = packetBuffer[currentPacketSize-2];
    floatCharPointer[2] = packetBuffer[currentPacketSize-3];
    floatCharPointer[3] = packetBuffer[currentPacketSize-4];
    toggleState();
  }
}

void osc::toggleState()
{
  if(isChanged)
  {
    isChanged=0;
  }
  else
  {
    isChanged=1;
  }
}

void osc::clearBuffer()
{
  memset ( packetBuffer, 0, maxPacketBufferSize);
}
