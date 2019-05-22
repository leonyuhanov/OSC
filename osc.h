#ifndef osc_h
#define osc_h
#include "Arduino.h"

class osc
{
  public:
    osc();
    byte parseOSCPacket();
    void toggleState();
    void clearBuffer();
    
    char* packetBuffer;
    unsigned short int maxPacketBufferSize;
    unsigned short int currentPacketSize;
    byte isChanged;
    float currentControllValue;
    unsigned char* floatCharPointer;
    char* controllID;
    unsigned short int currentControllID;
};

#endif
