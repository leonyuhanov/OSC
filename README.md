# OSC
Simplistic implementation of OSC speicifcly designed for use with Touch OSC https://hexler.net/touchosc Layouts

# Currently Supported

Any ELement named as follows:

  - /Fader001
  - Must use FLOAT as the data type

# Currently NOT Supported

Everything else.

# Accesing OSC Packet Data

```C++
osc oscObject;

//  ID of the fader eg /Fader001 this will be int(1)
oscObject.currentControllID;


// Value of the control element as a float()
oscObject.currentControllValue
```
