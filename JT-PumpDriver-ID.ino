/*
  Copyright (c) 2022 Uwe Stöhr
  
  This file is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
*/
#include "EEPROM-PD.h"
#define Serial SerialUSB

String ID = "0254"; // ID number to write to the pump driver

void setup()
{
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB
  }
  EEPROM.put(0, ID);
  delay(2000);
  String testID;
  EEPROM.get(0, testID);
  Serial.println("Success. Pump driver ID is now: " + testID);
}

void loop() {
}
