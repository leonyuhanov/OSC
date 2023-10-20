using System;
using UnityEngine;

public class oscClass
{
    public int numberOfControls;
	public string[] controlNames;
	public int[] varsPerControl;
	public float[][] controlValues;
	public int[] lastRead;
	
	int indexCounter=0;
	int subIndexCounter=0;
	
    public oscClass()
    {
		numberOfControls=0;
		controlNames = new string[0];
		varsPerControl = new int[0];
		controlValues = new float[0][];
		lastRead = new int[0];
    }
	
	public void addControl(string controlName, int numberOfValues)
	{
		//Increase Array sizes by 1 to add new element
		Array.Resize(ref controlNames, controlNames.Length + 1);
		Array.Resize(ref varsPerControl, varsPerControl.Length + 1);
		Array.Resize(ref controlValues, controlValues.Length + 1);
		Array.Resize(ref lastRead, lastRead.Length + 1);
		//Grab new length of arrays
		numberOfControls = controlNames.Length;
		controlNames[numberOfControls-1] = controlName;
		varsPerControl[numberOfControls-1] = numberOfValues;
		lastRead[numberOfControls-1] = 0;
		//Add values to value list
		controlValues[controlValues.Length-1] = new float[numberOfValues];
		for(subIndexCounter=0; subIndexCounter<numberOfValues; subIndexCounter++)
		{
			controlValues[controlValues.Length-1][subIndexCounter] = 0;
		}		
	}
	
	public int findIndexByName(string controlName)
	{
		int localIndexCounter=0;
		
		for(localIndexCounter=0; localIndexCounter<numberOfControls; localIndexCounter++)
		{
			if(controlNames[localIndexCounter] == controlName)
			{
				return localIndexCounter;
			}
		}
		return -1;
	}
	
	public float getValue(string controlName)
	{
		int localIndexCounter=0;
		for(localIndexCounter=0; localIndexCounter<numberOfControls; localIndexCounter++)
		{
			if(controlNames[localIndexCounter] == controlName)
			{
				lastRead[localIndexCounter] = 0;
				return controlValues[localIndexCounter][0];
			}
		}
		return -1;
	}
	
	public int dataReady(string controlName)
	{
		int localIndexCounter=0;
		for(localIndexCounter=0; localIndexCounter<numberOfControls; localIndexCounter++)
		{
			if(controlNames[localIndexCounter] == controlName)
			{
				if(lastRead[localIndexCounter]==1)
				{
					return 1;
				}
				else
				{
					return 0;
				}
			}
		}
		return -1;
	}
	
	public void parseData(byte[] dataIn)
	{
		int zeroIndex = findNeedleCount(0, dataIn, 0);
		string controlName = System.Text.Encoding.ASCII.GetString(dataIn,0,zeroIndex);
		byte[] floatSore = new byte[4];
		int valueCount=0, dataIndex = dataIn.Length;
		for(indexCounter=0; indexCounter<numberOfControls; indexCounter++)
		{
			if(controlNames[indexCounter] == controlName)
			{
				//Count teh number of 'f' in the data packet
				valueCount = countValues(102, dataIn);
				//check that this control has the same nuber of values as received
				if(varsPerControl[indexCounter]==valueCount)
				{
					for(subIndexCounter=0; subIndexCounter<valueCount; subIndexCounter++)
					{
						//copy the 4 bytes represenign the float into the floatStore Array
						Array.Copy(dataIn, dataIndex-4, floatSore, 0, floatSore.Length);
						//reverse the data as its in the wrong byte order
						Array.Reverse(floatSore);
						//store the value at the current index
						controlValues[indexCounter][subIndexCounter] = System.BitConverter.ToSingle(floatSore, 0);
						//decrement the index
						dataIndex-=4;
					}
					lastRead[indexCounter] = 1;
					return;
				}
			}
		}
	}
	
	public int findNeedleCount(byte needle, byte[] hayStack, int needleCount)
	{
		int currentCount=0;
		int hayStackIndex=0;
		
		for(hayStackIndex=0; hayStackIndex<hayStack.Length; hayStackIndex++)
		{
			if(hayStack[hayStackIndex] == needle && currentCount==needleCount)
			{
				return hayStackIndex;
			}
			else if(hayStack[hayStackIndex] == needle && currentCount<needleCount)
			{
				currentCount++;
			}
		}
		return -1;
	}
	
	public int countValues(byte needle, byte[] hayStack)
	{
		int numberOfValuesFound=0;
		int dataIndex = findNeedleCount(44, hayStack, 0)+1;
		while(hayStack[dataIndex]==needle)
		{
			numberOfValuesFound++;
			dataIndex++;
		}
		return numberOfValuesFound;
	}
}

/*
[0][47]
[1][102]
[2][97]
[3][100]
[4][101]
[5][114]
[6][49]
[7][0]
[8][44]
[9][102]
[10][0]
[11][0]
[12][63]
[13][56]
[14][144]
[15][201]
*/