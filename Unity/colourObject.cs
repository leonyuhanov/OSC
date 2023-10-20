using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;

public class colourObject
{
    byte[,] _primaryColours;
    byte[,] _colourTable = new byte[3, 3];
    int[,] _modifierTable = new int[2, 3];
    byte[] _tempColour = new byte[3];
    float[] _tempColourF = new float[3];
    byte _colourBlockCount;
    int _coloursPerBlock;
    public int _bandWidth;
    byte _cnt;
    int _primColIndex;
    float _mIndex;
    string bString;

    public colourObject()
    {

    }

    public colourObject(byte maxValue)
    {
        _colourBlockCount = 6;
        _primaryColours = new byte[_colourBlockCount, 3];
        _coloursPerBlock = maxValue;
        _bandWidth = _coloursPerBlock * _colourBlockCount;
        //Fill out default primary colours
        //[[255,0,0],[255,255,0],[0,255,0],[0,255,255],[0,0,255],[255,0,255]]
        _primaryColours[0, 0] = maxValue;
        _primaryColours[0, 1] = 0;
        _primaryColours[0, 2] = 0;

        _primaryColours[1, 0] = maxValue;
        _primaryColours[1, 1] = maxValue;
        _primaryColours[1, 2] = 0;

        _primaryColours[2, 0] = 0;
        _primaryColours[2, 1] = maxValue;
        _primaryColours[2, 2] = 0;

        _primaryColours[3, 0] = 0;
        _primaryColours[3, 1] = maxValue;
        _primaryColours[3, 2] = maxValue;

        _primaryColours[4, 0] = 0;
        _primaryColours[4, 1] = 0;
        _primaryColours[4, 2] = maxValue;

        _primaryColours[5, 0] = maxValue;
        _primaryColours[5, 1] = 0;
        _primaryColours[5, 2] = maxValue;
    }

    public colourObject(byte maxValue, byte colourBlockCount, byte[] rgbColourArray)
    {
        _colourBlockCount = colourBlockCount;
        _primaryColours = new byte[_colourBlockCount, 3];
        _coloursPerBlock = maxValue;
        _bandWidth = _coloursPerBlock * _colourBlockCount;
        for (_cnt = 0; _cnt < colourBlockCount; _cnt++)
        {
            _primaryColours[_cnt, 0] = rgbColourArray[_cnt * 3];
            _primaryColours[_cnt, 1] = rgbColourArray[(_cnt * 3) + 1];
            _primaryColours[_cnt, 2] = rgbColourArray[(_cnt * 3) + 2];
        }
    }

    public float[] getColour(int colourIndex, bool returnFloat)
    {
        _primColIndex = colourIndex / _coloursPerBlock;
        _colourTable[0, 0] = _primaryColours[_primColIndex, 0];
        _colourTable[0, 1] = _primaryColours[_primColIndex, 1];
        _colourTable[0, 2] = _primaryColours[_primColIndex, 2];

        _colourTable[1, 0] = _primaryColours[(_primColIndex + 1) % _colourBlockCount, 0];
        _colourTable[1, 1] = _primaryColours[(_primColIndex + 1) % _colourBlockCount, 1];
        _colourTable[1, 2] = _primaryColours[(_primColIndex + 1) % _colourBlockCount, 2];

        gradientGenerator(colourIndex % _coloursPerBlock, _coloursPerBlock);

        _tempColour[0] = _colourTable[2, 0];
        _tempColour[1] = _colourTable[2, 1];
        _tempColour[2] = _colourTable[2, 2];

        if (returnFloat == false)
        {
            _tempColourF[0] = (float)_tempColour[0];
            _tempColourF[1] = (float)_tempColour[1];
            _tempColourF[2] = (float)_tempColour[2];
            return _tempColourF;
        }
        else
        {
            _tempColourF[0] = (float)((float)_tempColour[0] / _coloursPerBlock);
            _tempColourF[1] = (float)((float)_tempColour[1] / _coloursPerBlock);
            _tempColourF[2] = (float)((float)_tempColour[2] / _coloursPerBlock);
            return _tempColourF;
        }
    }
    public void gradientGenerator(int colourIndex, int bandwidth)
    {
        for (_cnt = 0; _cnt < 3; _cnt++)
        {
            //fill modifier
            if (_colourTable[1, _cnt] > _colourTable[0, _cnt]) { _modifierTable[0, _cnt] = 1; }
            else if (_colourTable[1, _cnt] < _colourTable[0, _cnt]) { _modifierTable[0, _cnt] = -1; }
            else if (_colourTable[1, _cnt] == _colourTable[0, _cnt]) { _modifierTable[0, _cnt] = 0; }


            //fill step value
            if (_modifierTable[0, _cnt] == 1)
            {
                _modifierTable[1, _cnt] = _colourTable[1, _cnt] - _colourTable[0, _cnt];
            }
            else if (_modifierTable[0, _cnt] == -1)
            {
                _modifierTable[1, _cnt] = _colourTable[0, _cnt] - _colourTable[1, _cnt];
            }
            else if (_modifierTable[0, _cnt] == 0)
            {
                _modifierTable[1, _cnt] = 0;
            }
            _colourTable[2, _cnt] = (byte)(_colourTable[0, _cnt] + (((float)_modifierTable[1, _cnt] * ((float)colourIndex / (float)bandwidth)) * _modifierTable[0, _cnt]));

        }
    }
}