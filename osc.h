#ifndef osc_h
#define osc_h
#include "Arduino.h"
#include <stdlib.h> 
#include <stdio.h>
#include <math.h>
#include <string>
#include <cstring>

typedef struct LLNode{ 
  unsigned short int _nodeID;
  char* _controllName;
  float _currentValue;
  LLNode* nextNode; 
}LLNODE;

class osc
{
  public:
    osc();
    void parseOSCPacket();
    void toggleState();
    void clearBuffer();
    void addControll(char* controllName);
    LLNODE* findLast();
    LLNODE* findByID(unsigned short int nodeID);
    float getValue(unsigned short int nodeID);
    LLNODE* findByName(char* controllName);
	
    LLNODE* startPointer;
    unsigned short int totalNodes;
    
    char* packetBuffer;
    unsigned short int maxPacketBufferSize;
    unsigned short int currentPacketSize;
    byte isChanged;
    float primaryControllValue;
    float secondaryControllValue;
    unsigned char* floatCharPointer;
    unsigned short int currentControllID;
	
};

#endif
