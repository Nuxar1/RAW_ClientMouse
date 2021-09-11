# RAW_ClientMouse
This program reads your mouse input raw and sends it to an arduino. 
It will also create a window to block all of your mouse input. 
To close it just press alt f4.

Use this for the sender (the arduino connected to the pc running this repo)
```c
#include <Wire.h>
 
// Define Slave I2C Address
#define SLAVE_ADDR 9
 
byte bf[8];
void setup() {
 
  // Initialize I2C communications as Master
  Wire.begin();
  
  // Setup serial monitor
  Serial.begin(115200);
}
 
void loop() {
  if(Serial.available()){
    Serial.readBytes(bf, 8);
    // Write a charatre to the Slave
    Wire.beginTransmission(SLAVE_ADDR);
    Wire.write(bf[0]);
    Wire.write(bf[1]);
    Wire.write(bf[2]);
    Wire.write(bf[3]);
    Wire.write(bf[4]);
    Wire.write(bf[5]);
    Wire.write(bf[6]);
    Wire.write(bf[7]);
    Wire.endTransmission();
  }
}
```


Use this for the reciever:
```c
#include <Wire.h>
#include "HID-Project.h"
byte bf[3];
// Slave I2C Address
#define SLAVE_ADDR 9

void setup() {
 
  // Initialize I2C communications as Slave
  Wire.begin(SLAVE_ADDR);
  // Function to run when data requested from master
  Wire.onRequest(requestEvent); 
  
  // Function to run when data received from master
  Wire.onReceive(receiveEvent);
  
  // Setup Serial Monitor 
  Serial.begin(115200);
  Mouse.begin();
}

void receiveEvent() {
  //Serial.println("receive");
  // Read while data received
  byte x[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  Wire.readBytes(x, 8);

  int dx =  (int)x[0] + (int)bf[0]; 
  int dy =  (int)x[1] + (int)bf[1];
  Mouse.move(dx, dy, x[5]);
  
  if(x[2] != 0 || bf[2] != 0)
    Mouse.press();
  else
    Mouse.release();
    
  if(x[3] != 0)
    Mouse.press(MOUSE_RIGHT);
  else
    Mouse.release(MOUSE_RIGHT);
    
  if(x[4] != 0)
    Mouse.press(MOUSE_MIDDLE);
  else
    Mouse.release(MOUSE_MIDDLE);
    
  if(x[6] != 0)
    Mouse.press(MOUSE_PREV);
  else
    Mouse.release(MOUSE_PREV);
    
  if(x[7] != 0)
    Mouse.press(MOUSE_NEXT);
  else
    Mouse.release(MOUSE_NEXT);


    memset(bf, 0, 3);
}
void requestEvent() {
 
}

void loop() {
  if(Serial.available()){
    Serial.readBytes(bf, 3); //use whatever code you need here! This data will be received from the main pc.
  }
}
```
