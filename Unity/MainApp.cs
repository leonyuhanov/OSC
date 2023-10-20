using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Threading;
using System.Text;
using System.Net;
using System.Net.Sockets;
using System;

public class MainApp : MonoBehaviour
{
    //create a box to controll
	public GameObject[] aBox;
	float startX = -20f, xSpacing = 2f;
	Vector3 pointPos = new Vector3(0, 0, 0);
	Vector3 pointScale = new Vector3(1f, 1f, 1f);
	float xScaler = 20f, yScaler=20f, zScaler=20f;
	float xDecayBy = 0.1f, yDecayBy = 0.1f, zDecayBy = 0.1f;
	float echoDecay = 0.01f;
	
	//Objects for tracking Echo of data
	const int numberOfEchoPoints = 20;
	float [ ,] echoDataArray = new float[numberOfEchoPoints, 2];
	
	//Initiate the OSC System
	oscClass oscSystem = new oscClass();
	
	//UDP RX Stuff
	Thread receiveThread;
	int dataPacketPort=5454;
    UdpClient dataClient;
	IPEndPoint RemoteIpEndPoint = new IPEndPoint(IPAddress.Any, 0);
	
	//Colour
    const byte maxValue = 250;
    static byte[] colourList = new byte[7 * 3] { maxValue, 0, 0, maxValue, maxValue, 0, 0, maxValue, 0, 0, maxValue, maxValue, 0, 0, maxValue, maxValue, 0, maxValue, maxValue, maxValue, maxValue};
    colourObject cObject = new colourObject(maxValue, 7, colourList);
	Color cObj;
	float[] currentColourF = new float[3];
	int cIndex=0;

	
	// Start is called before the first frame update
    void Start()
    {
		int boxCounter=0;
		
		//Add OSC Elements
		oscSystem.addControl("/dataz", 1);
		oscSystem.addControl("/xdecay", 1);
		oscSystem.addControl("/ydecay", 1);
		oscSystem.addControl("/zdecay", 1);
		oscSystem.addControl("/echoDecay", 1);
		
		//init UDP Client
		dataClient = new UdpClient(dataPacketPort);
		receiveThread = new Thread( new ThreadStart(pollUDP) );
        receiveThread.IsBackground = true;
        receiveThread.Start();
		
		//set up the boxes
		aBox = new GameObject[numberOfEchoPoints];
		for(boxCounter=0; boxCounter<numberOfEchoPoints; boxCounter++)
		{
			//Create the object
			aBox[boxCounter] = GameObject.CreatePrimitive(PrimitiveType.Cube);
			//Set up colour
			currentColourF = cObject.getColour(cIndex % cObject._bandWidth, true);
			cObj.r = currentColourF[0];
			cObj.g = currentColourF[1];
			cObj.b = currentColourF[2];
			//Give it a name
			aBox[boxCounter].name = "box_"+boxCounter;
			//COlour it
			aBox[boxCounter].GetComponent<Renderer>().material.SetColor("_Color", cObj);
			//Set its position
			pointPos.x = startX+(xSpacing*boxCounter);
			aBox[boxCounter].transform.position = pointPos;
			//set its Scale
			aBox[boxCounter].transform.localScale = pointScale;
			
		}
		
		//Create the object
		//aBox = GameObject.CreatePrimitive(PrimitiveType.Cube);
		//Give it a name
		//aBox.name = "aBox";
		//COlour it
		//aBox.GetComponent<Renderer>().material.SetColor("_Color", cObj);
		//Set its position
		//aBox.transform.position = pointPos;
		//set its Scale
		//aBox.transform.localScale = pointScale;
    }

    // Update is called once per frame
    void Update()
    {
        float dataPacket=0;
		int boxIndex=0;
		
		//update decay values
		if(oscSystem.dataReady("/ydecay")==1)
		{
			yDecayBy = oscSystem.getValue("/ydecay");
		}
		if(oscSystem.dataReady("/echoDecay")==1)
		{
			echoDecay = oscSystem.getValue("/echoDecay");
		}
		
		//update bars
		if(oscSystem.dataReady("/dataz")==1)
		{
			dataPacket = oscSystem.getValue("/dataz");
			pointScale.y = 1f + (dataPacket*yScaler);
			echoDataArray[0,0] = dataPacket;
			currentColourF = cObject.getColour(((int)(cObject._bandWidth*dataPacket))% cObject._bandWidth, true);
			cObj.r = currentColourF[0];
			cObj.g = currentColourF[1];
			cObj.b = currentColourF[2];
			aBox[0].transform.localScale = pointScale;
			aBox[0].GetComponent<Renderer>().material.SetColor("_Color", cObj);
		}
		
		//echo to other boxes
		if(Time.frameCount%30==29)
		{
			for(boxIndex=1; boxIndex<numberOfEchoPoints; boxIndex++)
			{
				/*
				//echoDataArray[boxIndex,0] = echoDataArray[boxIndex-1,0]-echoDecay;
				echoDataArray[boxIndex,0] = echoDataArray[boxIndex-1,0];
				if(echoDataArray[boxIndex,0]<0)
				{
					echoDataArray[boxIndex,0]=0;
				}
				pointScale = aBox[boxIndex].transform.localScale;
				pointScale.y = 1f + (echoDataArray[boxIndex,0]*yScaler);
				*/
				//Grab previous box scale
				pointScale = aBox[boxIndex-1].transform.localScale;
				//set previous scale to current box
				if(pointScale.y-yDecayBy>1)
				{
					pointScale.y -= yDecayBy;
				}
				aBox[boxIndex].transform.localScale = pointScale;
				echoDataArray[boxIndex,0] = echoDataArray[boxIndex-1,0];
				currentColourF = cObject.getColour(((int)(cObject._bandWidth*echoDataArray[boxIndex,0]))% cObject._bandWidth, true);
				cObj.r = currentColourF[0];
				cObj.g = currentColourF[1];
				cObj.b = currentColourF[2];
				aBox[boxIndex].GetComponent<Renderer>().material.SetColor("_Color", cObj);
			}
			
			//main box decay
			pointScale = aBox[0].transform.localScale;
			if(pointScale.y-yDecayBy>1)
			{
				pointScale.y -= yDecayBy;
				aBox[0].transform.localScale = pointScale;
				if(echoDataArray[0,0]-yDecayBy>=0)
				{
					echoDataArray[0,0] = echoDataArray[0,0]-yDecayBy;
				}
				else
				{
					echoDataArray[0,0]=0;
				}
				currentColourF = cObject.getColour(((int)(cObject._bandWidth*echoDataArray[0,0]))% cObject._bandWidth, true);
				cObj.r = currentColourF[0];
				cObj.g = currentColourF[1];
				cObj.b = currentColourF[2];
				aBox[0].GetComponent<Renderer>().material.SetColor("_Color", cObj);
			}
		}
		
		/*
		float xControl = (oscSystem.controlValues[oscSystem.findIndexByName("/xy1")][1]);
		float yControl = (oscSystem.controlValues[oscSystem.findIndexByName("/xy1")][0]);
		//set the object scale acording to the value of "/fader1"
		pointScale.x = 1f + ((oscSystem.controlValues[oscSystem.findIndexByName("/fader1")][0])*globalMaxScaler);
		pointScale.y = 1f + ((oscSystem.controlValues[oscSystem.findIndexByName("/fader1")][0])*globalMaxScaler);
		pointScale.z = 1f + ((oscSystem.controlValues[oscSystem.findIndexByName("/fader1")][0])*globalMaxScaler);
		//set the object x/y position acording to the value of "/xy1"
		if(xControl<0.5f)
		{
			pointPos.x = 0-globalMaxPosition+(globalMaxPosition*(xControl/0.5f));
		}
		else
		{
			pointPos.x = 0+(globalMaxPosition*((xControl-0.5f)/0.5f));
		}
		if(yControl<0.05f)
		{
			pointPos.y = 0-globalMaxPosition+(globalMaxPosition*(yControl/0.5f));
		}
		else
		{
			pointPos.y = 0+(globalMaxPosition*((yControl-0.5f)/0.5f));
		}
		aBox.transform.localScale = pointScale;
		aBox.transform.position = pointPos;
		*/
    }
	
	void pollUDP()
	{			
		while(true)
		{
			try
			{
				byte[] dataReceived = dataClient.Receive(ref RemoteIpEndPoint); 							
				oscSystem.parseData(dataReceived);
			}
			catch(Exception err)
			{
				Debug.Log(err.ToString());
			}
		}
	}
}
