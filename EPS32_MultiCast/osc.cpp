#include "osc.h"


osc::osc()
{  
  maxPacketBufferSize = 255;
  packetBuffer = new char[maxPacketBufferSize];
  startPointer = NULL;
  totalNodes = 0;
  txPacketBuffer = new char[maxPacketBufferSize];
  txPacketBufferLength=0;
}

void osc::parseOSCPacket()
{  
  //strlen returns up to the 1st [0] which is the terminator for the name of the OSC object
  int tempInt=0;
  char* tempString = new char[strlen(packetBuffer)+1];  
  LLNODE* nodePointer;

  memcpy(tempString, packetBuffer, strlen(packetBuffer)+1);
  tempString[strlen(packetBuffer)]=0;  
  
  nodePointer = findByName(tempString);
 
  if(nodePointer!=NULL)
  {
    currentControllID = nodePointer->_nodeID;
	if(nodePointer->_varType=='f')
    {
      //Set Current Controll Values using Floats
      for(counter=0; counter<nodePointer->_numOfVars; counter++)
    	{
    		packetIndex = ((counter+1)*4);
    		floatCharPointer = (unsigned char*)&primaryControllValue;
    		floatCharPointer[0] = packetBuffer[currentPacketSize-(packetIndex-3)];
    		floatCharPointer[1] = packetBuffer[currentPacketSize-(packetIndex-2)];
    		floatCharPointer[2] = packetBuffer[currentPacketSize-(packetIndex-1)];
    		floatCharPointer[3] = packetBuffer[currentPacketSize-(packetIndex)];
    		nodePointer->_controllValueArray[counter] = primaryControllValue;
  	  }
  	  nodePointer->_currentValue = nodePointer->_controllValueArray[0];
      nodePointer->_timer[0] = millis();
  	  primaryControllValue = nodePointer->_currentValue;
    }
    else if(nodePointer->_varType=='i')
    {
      //Set Current Controll Values using Floats
      for(counter=0; counter<nodePointer->_numOfVars; counter++)
      {
        packetIndex = ((counter+1)*4);
        floatCharPointer = (unsigned char*)&tempInt;
        floatCharPointer[0] = packetBuffer[currentPacketSize-(packetIndex-3)];
        floatCharPointer[1] = packetBuffer[currentPacketSize-(packetIndex-2)];
        floatCharPointer[2] = packetBuffer[currentPacketSize-(packetIndex-1)];
        floatCharPointer[3] = packetBuffer[currentPacketSize-(packetIndex)];
        nodePointer->_controllValueArray[counter] = tempInt;
      }
      nodePointer->_currentValue = nodePointer->_controllValueArray[0];
      nodePointer->_timer[0] = millis();
      primaryControllValue = nodePointer->_currentValue;
    }
  }
  else
  {
    //Control not registered, maybe auto add?    
  }  
}

void osc::generateOSCPacket(char* controllName)
{
  unsigned short int txPacketSize = 8, bufferIndex=0, dataSent=0;
  double fractpart, intpart, padding=0;
  char oscBufferPadding[4] = {44,102,0,0}, padBuffer[3] = {0,0,0};
  unsigned char* floatCharPointer;

  LLNODE* oscItem = findByName(controllName);
  if(oscItem!=NULL)
  {
    //Clear the TX buffer
    clearTXBuffer();
    txPacketSize += strlen(oscItem->_controllName)+1;
    //Calculate Padding to make the string 32bit able as per osc spec https://opensoundcontrol.stanford.edu/spec-1_0.html#osc-packets
    padding = (strlen(oscItem->_controllName)+1)*8;
    padding = padding/32;
    fractpart = modf(padding , &intpart);
    if(fractpart==0.75)
    {
      padding=1;
    }
    else if(fractpart==0.5)
    {
      padding=2;
    }
    else if(fractpart==0.25)
    {
      padding=3;
    }
    else
    {
      padding=0;
    }
    txPacketSize+=padding;
    //txPacketSize is now the total number of bytes tha tthe osc packet to send will be
    txPacketBufferLength = txPacketSize;
    //Store Control Name
    memcpy(txPacketBuffer, oscItem->_controllName, strlen(oscItem->_controllName));
    bufferIndex+=strlen(oscItem->_controllName);
    txPacketBuffer[bufferIndex]=0;
    bufferIndex++;
    //Add padding to make it 32bitable
    if(padding!=0)
    {
      memcpy(txPacketBuffer+bufferIndex, padBuffer, padding);
      bufferIndex+=padding;
    }
    //Add middle of packet
    memcpy(txPacketBuffer+bufferIndex, oscBufferPadding, 4);
    bufferIndex+=4;
    //Grab 4 bytes of te value float
    floatCharPointer = (unsigned char*)&oscItem->_currentValue;
    
    txPacketBuffer[bufferIndex] = floatCharPointer[3];
    txPacketBuffer[bufferIndex+1] = floatCharPointer[2];
    txPacketBuffer[bufferIndex+2] = floatCharPointer[1];
    txPacketBuffer[bufferIndex+3] = floatCharPointer[0];
  }
}

void osc::addControll(char* controllName)
{
  addControll(controllName, 1);
}

void osc::addControll(char* controllName, unsigned short int numOfValues)
{
	LLNODE* nodePointer;
	LLNODE* prevNode;
	
	nodePointer = (LLNODE*) malloc(sizeof(LLNODE));
	nodePointer->_nodeID = totalNodes;
	nodePointer->_controllName = new char[strlen(controllName)+1];
	memcpy(nodePointer->_controllName, controllName, strlen(controllName));
	nodePointer->_controllName[strlen(controllName)]=0;
	nodePointer->_currentValue = 0;
	//Init Controll Value Array
	nodePointer->_numOfVars = numOfValues;
	nodePointer->_controllValueArray = new float[numOfValues];
  nodePointer->_varType='f';
	for(counter=0; counter<numOfValues; counter++)
	{
		nodePointer->_controllValueArray[counter] = 0;
	}
	
	//Set up Linked List stuff
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

void osc::addControll(char* controllName, unsigned short int numOfValues, char valueType)
{
  LLNODE* nodePointer;
  if(valueType=='f')
  {
    addControll(controllName, numOfValues);
  }
  else if(valueType=='i')
  {
    addControll(controllName, numOfValues);
    nodePointer = findByName(controllName);
    nodePointer->_varType = valueType;
  }
}

void osc::addControll(char* controllName, unsigned short int numOfValues, char valueType, unsigned long timeOut)
{
  LLNODE* nodePointer;
  if(valueType=='f')
  {
    addControll(controllName, numOfValues);
    nodePointer = findByName(controllName);
    nodePointer->_timer[2] = timeOut;
  }
  else if(valueType=='i')
  {
    addControll(controllName, numOfValues);
    nodePointer = findByName(controllName);
    nodePointer->_varType = valueType;
  }
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

void osc::setValue(char* controllName, float valueToSet)
{
  LLNODE* nodePointer = findByName(controllName);
  if(nodePointer!=NULL)
  {
    nodePointer->_currentValue = valueToSet;
  }
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

void osc::clearTXBuffer()
{
  memset ( txPacketBuffer, 0, maxPacketBufferSize);
}

void osc::deleteNode(unsigned short int nodeID)
{
  LLNODE* currentNode = findByID(nodeID);
  if(currentNode==NULL)
  {
    return;
  }
  LLNODE* prevNode = findPrev(nodeID);
  

  //check if Prev node exists or its the last node
  if(totalNodes==1)
  {
    currentNode = startPointer;
    delete currentNode;
    startPointer = NULL;
    totalNodes=0;
  }
  else if(prevNode==NULL)
  {
    startPointer = currentNode->nextNode;
    delete currentNode;
    totalNodes--;
  }
  else
  {
    prevNode->nextNode = currentNode->nextNode;
    delete currentNode;
    totalNodes--;
  }
}

LLNODE* osc::findPrev(unsigned short int nodeID)
{
  LLNODE* currentNode = startPointer;
  LLNODE* nextNode = NULL;
  
  while(currentNode != NULL)
  { 
    nextNode = currentNode->nextNode;
    if(nextNode!=NULL)
    {
      if(nextNode->_nodeID==nodeID)
      {
        return currentNode;
      }
    }
    currentNode = nextNode; 
  } 
  return NULL;
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
