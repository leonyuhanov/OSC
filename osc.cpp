#include "osc.h"


osc::osc()
{  
  maxPacketBufferSize = 255;
  packetBuffer = new char[maxPacketBufferSize];
  startPointer = NULL;
  totalNodes = 0;
}

void osc::parseOSCPacket()
{  
  //strlen returns up to the 1st [0] which is the terminator for the name of the OSC object
  char* tempString = new char[strlen(packetBuffer)];  
  LLNODE* nodePointer;

  memcpy(tempString, packetBuffer+1, strlen(packetBuffer)-1);
  tempString[strlen(packetBuffer)-1]=0;
    
  nodePointer = findByName(tempString);
  if(nodePointer!=NULL)
  {
    //Set Current Controll Value 
    floatCharPointer = (unsigned char*)&primaryControllValue;
    floatCharPointer[0] = packetBuffer[currentPacketSize-1];
    floatCharPointer[1] = packetBuffer[currentPacketSize-2];
    floatCharPointer[2] = packetBuffer[currentPacketSize-3];
    floatCharPointer[3] = packetBuffer[currentPacketSize-4];
    nodePointer->_currentValue = primaryControllValue;
  }
  else
  {
    //Control not registered, maybe auto add?    
  }

  /*
    //Object  "Steer"
    //2 floats
    //Set Current Controll Value 
    floatCharPointer = (unsigned char*)&secondaryControllValue;
    floatCharPointer[0] = packetBuffer[currentPacketSize-5];
    floatCharPointer[1] = packetBuffer[currentPacketSize-6];
    floatCharPointer[2] = packetBuffer[currentPacketSize-7];
    floatCharPointer[3] = packetBuffer[currentPacketSize-8];
    
    floatCharPointer = (unsigned char*)&primaryControllValue;
    floatCharPointer[0] = packetBuffer[currentPacketSize-1];
    floatCharPointer[1] = packetBuffer[currentPacketSize-2];
    floatCharPointer[2] = packetBuffer[currentPacketSize-3];
    floatCharPointer[3] = packetBuffer[currentPacketSize-4];
    currentControllID=2;
  }
  */
  
}

void osc::addControll(char* controllName)
{
	LLNODE* nodePointer;
	LLNODE* prevNode;
	
	nodePointer = (LLNODE*) malloc(sizeof(LLNODE));
	nodePointer->_nodeID = totalNodes;
	nodePointer->_controllName = new char[strlen(controllName)+1];
	memcpy(nodePointer->_controllName, controllName, strlen(controllName));
	nodePointer->_controllName[strlen(controllName)]=0;
	nodePointer->_currentValue = 0;
	if(totalNodes==0)
	{
		startPointer = nodePointer;
		nodePointer->nextNode = NULL;
	}
	else
	{
		prevNode = findLast();
		prevNode->nextNode = nodePointer;
		nodePointer->nextNode = NULL;
	}
	totalNodes++;
}

float osc::getValue(unsigned short int nodeID)
{
  LLNODE* nodePointer = findByID(nodeID);
  if(nodePointer != NULL)
  {
    return nodePointer->_currentValue;
  }
  return -1;
}

LLNODE* osc::findByID(unsigned short int nodeID)
{
  LLNODE* nodePointer = startPointer;
  while(nodePointer != NULL)
  { 
      if(nodePointer->_nodeID==nodeID)
      {
        return nodePointer;
      }
      nodePointer = nodePointer->nextNode; 
   } 
   return NULL;
}

LLNODE* osc::findByName(char* controllName)
{
  LLNODE* nodePointer = startPointer;
  
  while(nodePointer != NULL)
  { 
      if(strcmp(controllName, nodePointer->_controllName)==0)
      {
        return nodePointer;
      }
      nodePointer = nodePointer->nextNode; 
   } 
   return NULL;
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

LLNODE* osc::findLast()
{
  LLNODE* nodePointer = startPointer;
  while(nodePointer != NULL)
  { 
      if(nodePointer->nextNode==NULL)
      {
        return nodePointer;
      }
      nodePointer = nodePointer->nextNode; 
   } 
   return NULL;
}
