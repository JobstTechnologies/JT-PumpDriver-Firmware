/*
  Copyright (c) 2022 Uwe Stöhr
  
  This file is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
*/
/*
   Syntax for read commands
   /n.. opening command, n is the address, currently only '0' is allowed as address
   R..execute
   Spnnn..speed, p is the pump number, nnn is the speed in the range 000 - 999 where 999 means a voltage of 3.3V
   Dpppppppp..direction, p is the direction (0 or 1) of the corresonding pump number
   Ipppppppp..set, p is the on/off switch (0 or 1) of the corresonding pump number
   Vvvvvvvvv..set, v is the on/off switch (0 or 1) of the corresonding valve number
   Mnnnn..time, nnnn is the time in milliseconds the pumps should be in the previously defined state,
                the maximal time is 2^31-1 milliseconds (24.85 days)
                (The frontend program "JT pump driver" will take care of this for you.)
   g..start loop
   Gnn..end loop, nn is the number of loops, the numer of loops is only limited by the overall time necessary to execute them,
                  the maximum overall time is 2^32-1 milliseconds (49.71 days)
                  (The frontend program "JT pump driver" will take care of this for you.)
   L..LED ON
   l..LED OFF
   a line feed character (ASCII 10) is expected to finalize the command sequence

  Don't forget to update the string "VersionNumber" when changing the sketch!

  Always test using these commands before releasing a new firmware version:
   General:
  /0LS1999D00000000I10000000M500I00000000M500S1999D00000000I10001000M500I0000000lR - > 2 steps
  /0LS2999D01V1I01M3000V0I00M2000S2999D10V1I01M3000V0I00lR - > 2 steps with 1 pump and 1 valve
  /0LS1999399959997999D1010I1111111M500I00000000M500S1999399959997999D1010I1111111M500I0000lR - > 2 steps, 4 pumps, pumps 1, 3, 5 and 7
  /0LS7999599939991999D10I1111111M500I0000M500S7999599939991999D10I1111111M500I00000000lR - > 2 steps, 4 pumps, pumps 7, 5, 3 and 1
  /0LI11111111R - > nothing executed, LED on
  /0LS39991999I001R - > only pump 3 is run for 1000 ms because this is the setTimeout
  /0LS199929993999D0000I111M500I0000M500S199929993999D0000I1110M500I0000lR - > 2 steps, 3 pumps
  /0LS4999299939991999D0000I1111M500I0000M500S1999D0000I1111M500I0000lR - > 2 steps, 4 pumps
   Only for special pump drivers:
  /0LS4999299939991999D0000I1111M500I0000M500S1999D0000I1111M500I0000lR - > 2 steps, 5 pumps
  /0LS4999299939991999D0000I1111M500I0000M500S1999D0000I1111M500I0000lR - > 2 steps, 6 pumps
  /0LS4999299939991999D0000I1111M500I0000M500S1999D0000I1111M500I0000lR - > 2 steps, 7 pumps
  /0LS4999299939991999D0000I1111M500I0000M500S1999D0000I1111M500I0000lR - > 2 steps, 8 pumps
   Loops:
  /0LgS1999D0000I1000M500I0000M500G3I0000lR -> 1 loop
  /0LgS1999D0000I1000M500I0000M500G0I0000lR -> 1 loop with zero repeats
  /0LgS1999D0000I1000M500I0000M500GI0000lR -> 1 infinite loop
  /0LggS1666D0000I1000M800I0000M200G2S1999D0000I1000M3000G1I0000lR - > 1 inner loop in one outer loop
  /0LggS1666D0000I1000M800I0000M200G2S1999D0000I1000M3000G0I0000lR - > 1 inner loop in one outer loop with zero repeats
  /0LggS1666D0000I1000M800I0000M200G2S1999D0000I1000M3000GI0000llR - > 1 inner loop in one outer infinite loop
  /0LggS1666D0000I1000M800I0000M200G2S1999D0000I1000M3000gS1666D0000I1000M800I0000M200G2S1999D0000I1000M3000G1I0000lR -> 2 inner loops in one outer loop
  /0LggS1666D0000I1000M800I0000M200G2S1999D0000I1000M3000G1ggS1666D0000I1000M800I0000M200G2S1999D0000I1000M3000G1I0000lR -> 2 times 1 inner loop in one outer loop
  /0LggS1999D0000I1000M800I0000M200G00G2I0000lR -> inner loop with zero repeats
   Malformed commands:
  /0LggS1999D0000I1000M800I0000M200GG2I0000lR -> inner loop cannot be infinite
  /0LgggS1999D0000I1000M800I0000M200G0G1G2I0000lR -> only 2 loop levels allowed
  /0LgS1999D0000I1000M800I0000M200G1G2I0000lR -> missing 'g'
  /0LggS1999D0000I1000M800I0000M200G1I0000lR -> missing 'G'
  0LggS1999D0000I1000M800I0000M200GG2I0000lR -> missing begin '/'
  /LggS1999D0000I1000M800I0000M200GG2I0000lR -> missing address
  /3LggS1999D0000I1000M800I0000M200GG2I0000lR -> address is not '0'
  /0LggS1999D0000I1000M800I0000M200GG2I0000l -> missing end 'R'
  /0LggS1999D0000I1000M800I0000M200GG2I0000lRu -> also missing end 'R'
  /0LgSx1999D0000I1000M800I0000M200G2I0000lR -> no digit for 'S'
  /0LgS999D0000I1000M800I0000M200G2I0000lR -> less than 4 digits for 'S'
  /0LgS5999D0000I1000M800I0000M200G2I0000lR -> only 4 pumps are allowed
  /0LgS1999233D0000I1000M800I0000M200G2I0000lR -> less than 4 digits for second pump's 'S'
  /0LgS19996999D0000I1000M800I0000M200G2I0000lR -> only 4 pumps are allowed for second pump
  /0LS3999299919993500D0000I1000M3000I0000lR - > 4 pumps but one pump redefined
  /0LgS1999Dx0000I1000M800I0000M200G2I0000lR -> no digit for 'D'
  /0LS39991999D02I1000M500I0000M500S1999D0000I1000M500I0000lR - > 'D' got not 0 or 1
  /0LgS1999D0000Ix1000M800I0000M200G2I0000lR -> no digit for 'I'
  /0LS39991999D01I02M500I0000M500S1999D0000I1000M500I0000lR - > 'I' got not 0 or 1
  /0LgS1999D0000I1000Mx800I0000M200G2I0000lR -> no digit for 'M'
*/

#include <Wire.h>
#include "EEPROM-PD.h"
#include "MotorDriver.h"
#define Serial SerialUSB

String VersionNumber = "3.1";

MotorDriver motor12(0); // value is the address: removed R1 for 1, R2 for 2, R1 and R2 for 3
MotorDriver motor34(1);
MotorDriver motor56(2);
MotorDriver motor78(3);

int maxPWM = 330;
int timeDelay = 0, endPos = 0, lengthString = 0, i = 0;
int pos = 0, gStart1 = 0, gStart2 = 0, CountLoop1 = -2, CountLoop2 = -2, CounterLoop1 = 0, CounterLoop2 = 0;
int value = 0, value1 = 0, value2 = 0, value3 = 0, value4 = 0, value5 = 0, value6 = 0, value7 = 0, value8 = 0,
    direct1 = 0, direct2 = 0, direct3 = 0, direct4 = 0;
String inputString = "", commandString = " ", SOrder = "0000", ID ="0000";
bool stringComplete = false, broken = false,
 motor1 = false, motor2 = false, motor3 = false, motor4 = false,
 motor5 = false, motor6 = false, motor7 = false, motor8 = false,
 valve1 = false, valve2 = false, valve3 = false, valve4 = false,
 valve5 = false, valve6 = false, valve7 = false, valve8 = false,
 isChangedValve1 = false, isChangedValve2 = false, isChangedValve3 = false, isChangedValve4 = false,
 isChangedValve5 = false, isChangedValve6 = false, isChangedValve7 = false, isChangedValve8 = false;
unsigned long timer = 0;

void setup(){
  // initialize serial:
  Serial.begin(9600);
  while (!Serial)
    ; // wait for serial port to connect. Needed for native USB
  // reserve 1000 bytes for the inputString:
  inputString.reserve(1000);
  // enable 8 digital outputs
  pinMode(1, OUTPUT); pinMode(2, OUTPUT);
  pinMode(3, OUTPUT); pinMode(4, OUTPUT);
  pinMode(5, OUTPUT); pinMode(6, OUTPUT);
  pinMode(7, OUTPUT); pinMode(8, OUTPUT);
  // enable LED
  pinMode(13, OUTPUT);
  // initiate the Wire library and join the I2C bus as a master or slave
  Wire.begin();
  //The value passed to begin() is the maximum counter value, which is 16 bit (up to 65535)
  //This value also determines the output frequency - by default, 8 MHz divided by the counter value
  motor12.begin(maxPWM);
  motor34.begin(maxPWM);
  motor56.begin(maxPWM);
  motor78.begin(maxPWM);
  //The failsafe turns off motors if a command is not sent in a certain amount of time.
  //Failsafe is set in milliseconds - set to 0 to disable
  // we use it because tests showed that the driver can get to states
  // where it runs permanently at full speed (e.g. if the serial connection was interrupted)
  motor12.setFailsafe(1000);
  motor34.setFailsafe(1000);
  motor56.setFailsafe(1000);
  motor78.setFailsafe(1000);
  // set timeout for sending serial commands
  Serial.setTimeout(1000);
  // get data from the virtual EEPROM at position 0
  EEPROM.get(0, ID);
  if (ID == "")
   ID = "0000";
}

void loop(){
  
  // read serial input
  if (Serial.available()) {
    inputString = "";
    serialRead();
  }

  // print the string when a newline arrives:
  if (stringComplete) {
    Serial.println("JT-PumpDriver-ID " + ID +
                   "\nJT-PumpDriver-Firmware " + VersionNumber + 
                   "\n Received command: " + inputString);   
    // reset loop variables
    CountLoop1 = -2;
    CountLoop2 = -2;
    CounterLoop1 = 0;
    CounterLoop2 = 0;
    gStart1 = 0;
    gStart2 = 0;
    // delete values from previous call
    lengthString = inputString.length();
    // check that it was a command
    if (inputString[0] == '/') {
      // check if there is an address
      if (!isDigit(inputString[1])) {
        Serial.println("Error: Command does not start with an address, e.g. with '/0'");
        stringComplete = false;
        broken = true;
        return; // nothing should be executed
      } else {
        if (String(inputString[1]).toInt() > 0) {
          Serial.println("Error: Currently only the address '0' is supported, got '" + String(inputString[1]) + "'");
          stringComplete = false;
          broken = true;
          return; // nothing should be executed
        }
      }
      // check if the command should be executed
      if ( ( (inputString[lengthString - 2] != 'R') and (inputString[lengthString - 2] != '\r') )
        or ( (inputString[lengthString - 3] != 'R') and (inputString[lengthString - 2] != 'R') ) ) {
          // commands end either with #10 (\n) or #13#10 (\r\n)
          Serial.println("Error: Command does not end with 'R'");
          stringComplete = false;
          broken = true;
          return; // nothing should be executed
      }
      commandString = inputString;
      stringComplete = false;
      broken = false;
      pos = 0;
    } else {
      Serial.println("Error: Command does not begin with '/'");
      stringComplete = false;
      broken = true;
      return; // nothing should be executed
    }
  } // end if stringComplete

  if (broken) // do nothing
    return;

  if (!stringComplete) {
    // we can parse and execute the command

    // at first move position
    pos = pos + 1;
    //Serial.println("pos: " + String(pos));

    // command is completely parsed or there is no command
    if ((pos == lengthString - 1) or (lengthString == 0)) { 
      broken = true;
      return;
    }

    // check for loop begin
    if (commandString[pos] == 'g') {
      //Serial.println("g: " + String(pos));
      if ((gStart1 == 0) and (gStart2 == 0))
        gStart1 = pos;
      else if ((gStart1 > 0) and ( gStart2 == 0))
        gStart2 = pos;
      else if ((gStart1 > 0) and ( gStart2 > 0)) {
        Serial.println("Error: only one loop inside a loop allowed");
        stopPumps();
        broken = true;
        return;
      }
    }

    // check for speed
    if (commandString[pos] == 'S') {
      //Serial.println("S: " + String(pos));
      // syntax is "Sxyyy", x = pump number, y = speed
      // it is possible that the user only sends command for one pump
      // the order can be varied, e.g. S35001500
      SOrder = "00000000";
      // delete values from previous call of 'S'
      value1 = int(0); value2 = int(0); value3 = int(0); value4 = int(0);
      value5 = int(0); value6 = int(0); value7 = int(0); value8 = int(0);
      if (isDigit(commandString[pos+1])) {
        // check if the next characters are numbers, then read them
        for (int j = pos+1; j < lengthString; j++) {
          if (isDigit(commandString[j]) == false) {
            endPos = j;
            if (endPos - (pos+1) > 3) // we expect at least 4 digits
              value = commandString.substring(pos+2, pos+5).toInt();
            else {
              value = int(0);
              Serial.println("Error: speed setting 'S' requires at least 4 digits, got " + String(endPos - (pos+1)));
              broken = true;
              stopPumps();
              return;
            }
            // translate value from range 0 - 999 to the range 0 - 330
            value = ceil(value * 0.33);
            // connect the read value to the motor
            if (commandString[pos+1] == '1') {
              value1 = value;
              SOrder[0] = '1';
            } else if (commandString[pos+1] == '2') {
              value2 = value;
              SOrder[0] = '2';
            } else if (commandString[pos+1] == '3') {
              value3 = value;
              SOrder[0] = '3';
            } else if (commandString[pos+1] == '4') {
              value4 = value;
              SOrder[0] = '4';
            } else if (commandString[pos+1] == '5') {
              value5 = value;
              SOrder[0] = '5';
            } else if (commandString[pos+1] == '6') {
              value6 = value;
              SOrder[0] = '6';
            } else if (commandString[pos+1] == '7') {
              value7 = value;
              SOrder[0] = '7';
            } else if (commandString[pos+1] == '8') {
              value8 = value;
              SOrder[0] = '8';
            } else if (String(commandString[pos+1]).toInt() > 8) {
              value = int(0);
              Serial.println("Error: only 8 pumps supported, got a command for pump number '" + String(commandString[pos+1]) + "'");
              broken = true;
              stopPumps();
              return;
            }
            break;
          }   
        }
      } else {
        value = int(0);
        Serial.println("Error: no value given for speed setting 'S'");
        broken = true;
        stopPumps();
        return;
      }
      if (endPos - (pos+1) == 4) // we only got command for one pump and are done
        return;
      // now check for a second speed
      if (isDigit(commandString[pos+5])) {
        for (int j = pos+5; j < lengthString; j++) {
          if (isDigit(commandString[j]) == false) {
            endPos = j;
            if (endPos - (pos+5) > 3)
              value = commandString.substring(pos+6, pos+9).toInt();
            else {
              value = int(0);
              Serial.println("Error: speed setting 'S' for 2 pumps requires 8 digits, got " + String(endPos - (pos+1)));
              broken = true;
              stopPumps();
              return;
            }
            value = ceil(value * 0.33);
            if (commandString[pos+5] == '1') {
              value1 = value;
              SOrder[1] = '1';
            } else if (commandString[pos+5] == '2') {
              value2 = value;
              SOrder[1] = '2';
            } else if (commandString[pos+5] == '3') {
              value3 = value;
              SOrder[1] = '3';
            } else if (commandString[pos+5] == '4') {
              value4 = value;
              SOrder[1] = '4';
            } else if (commandString[pos+5] == '5') {
              value5 = value;
              SOrder[1] = '5';
            } else if (commandString[pos+5] == '6') {
              value6 = value;
              SOrder[1] = '6';
            } else if (commandString[pos+5] == '7') {
              value7 = value;
              SOrder[1] = '7';
            } else if (commandString[pos+5] == '8') {
              value8 = value;
              SOrder[1] = '8';
            } else if (String(commandString[pos+5]).toInt() > 8) {
              value = int(0);
              Serial.println("Error: only 8 pumps supported, got a command for pump number '" + String(commandString[pos+5]) + "'");
              broken = true;
              stopPumps();
              return;
            }
            break;
          }
        }
      } else
        value = int(0);
      if (endPos - (pos+5) == 4) // we only got command for 2 pumps and are done
        return;
      // now check for a third speed
      if (isDigit(commandString[pos+9])) {
        for (int j = pos+9; j < lengthString; j++) {
          if (isDigit(commandString[j]) == false) {
            endPos = j;
            if (endPos - (pos+9) > 3)
              value = commandString.substring(pos+10, pos+13).toInt();
            else {
              value = int(0);
              Serial.println("Error: speed setting 'S' for 3 pumps requires 12 digits, got " + String(endPos - (pos+1)));
              broken = true;
              stopPumps();
              return;
            }
            value = ceil(value * 0.33);
            if (commandString[pos+9] == '1') {
              value1 = value;
              SOrder[2] = '1';
            } else if (commandString[pos+9] == '2') {
              value2 = value;
              SOrder[2] = '2';
            } else if (commandString[pos+9] == '3') {
              value3 = value;
              SOrder[2] = '3';
            } else if (commandString[pos+9] == '4') {
              value4 = value;
              SOrder[2] = '4';
            } else if (commandString[pos+9] == '5') {
              value5 = value;
              SOrder[2] = '5';
            } else if (commandString[pos+9] == '6') {
              value6 = value;
              SOrder[2] = '6';
            } else if (commandString[pos+9] == '7') {
              value7 = value;
              SOrder[2] = '7';
            } else if (commandString[pos+9] == '8') {
              value8 = value;
              SOrder[2] = '8';
            } else if (String(commandString[pos+9]).toInt() > 8) {
              value = int(0);
              Serial.println("Error: only 8 pumps supported, got a command for pump number '" + String(commandString[pos+9]) + "'");
              broken = true;
              stopPumps();
              return;
            }
            break;
          }
        }
      } else
        value = int(0);
      if (endPos - (pos+9) == 4) // we only got command for 3 pumps and are done
        return;
      // now check for a fourth speed
      if (isDigit(commandString[pos+13])) {
        for (int j = pos+13; j < lengthString; j++) {
          if (isDigit(commandString[j]) == false) {
            endPos = j;
            if (endPos - (pos+13) > 3)
              value = commandString.substring(pos+14, pos+17).toInt();
            else {
              value = int(0);
              Serial.println("Error: speed setting 'S' for 4 pumps requires 16 digits, got " + String(endPos - (pos+1)));
              broken = true;
              stopPumps();
              return;
            }
            value = ceil(value * 0.33);
            if (commandString[pos+13] == '1') {
              value1 = value;
              SOrder[3] = '1';
            } else if (commandString[pos+13] == '2') {
              value2 = value;
              SOrder[3] = '2';
            } else if (commandString[pos+13] == '3') {
              value3 = value;
              SOrder[3] = '3';
            } else if (commandString[pos+13] == '4') {
              value4 = value;
              SOrder[3] = '4';
            } else if (commandString[pos+13] == '5') {
              value5 = value;
              SOrder[3] = '5';
            } else if (commandString[pos+13] == '6') {
              value6 = value;
              SOrder[3] = '6';
            } else if (commandString[pos+13] == '7') {
              value7 = value;
              SOrder[3] = '7';
            } else if (commandString[pos+13] == '8') {
              value8 = value;
              SOrder[3] = '8';
            } else if (String(commandString[pos+13]).toInt() > 8) {
              value = int(0);
              Serial.println("Error: only 8 pumps supported, got a command for pump number '" + String(commandString[pos+9]) + "'");
              broken = true;
              stopPumps();
              return;
            }
            break;
          }
        }
      } else
        value = int(0);
      if (endPos - (pos+13) == 4) // we only got command for 4 pumps and are done
        return;
      // now check for a fifth speed
      if (isDigit(commandString[pos+17])) {
        for (int j = pos+17; j < lengthString; j++) {
          if (isDigit(commandString[j]) == false) {
            endPos = j;
            if (endPos - (pos+17) > 3)
              value = commandString.substring(pos+18, pos+21).toInt();
            else {
              value = int(0);
              Serial.println("Error: speed setting 'S' for 5 pumps requires 20 digits, got " + String(endPos - (pos+1)));
              broken = true;
              stopPumps();
              return;
            }
            value = ceil(value * 0.33);
            if (commandString[pos+17] == '1') {
              value1 = value;
              SOrder[4] = '1';
            } else if (commandString[pos+17] == '2') {
              value2 = value;
              SOrder[4] = '2';
            } else if (commandString[pos+17] == '3') {
              value3 = value;
              SOrder[4] = '3';
            } else if (commandString[pos+17] == '4') {
              value4 = value;
              SOrder[4] = '4';
            } else if (commandString[pos+17] == '5') {
              value5 = value;
              SOrder[4] = '5';
            } else if (commandString[pos+17] == '6') {
              value6 = value;
              SOrder[4] = '6';
            } else if (commandString[pos+17] == '7') {
              value7 = value;
              SOrder[4] = '7';
            } else if (commandString[pos+17] == '8') {
              value8 = value;
              SOrder[4] = '8';
            } else if (String(commandString[pos+17]).toInt() > 8) {
              value = int(0);
              Serial.println("Error: only 8 pumps supported, got a command for pump number '" + String(commandString[pos+9]) + "'");
              broken = true;
              stopPumps();
              return;
            }
            break;
          }
        }
      } else
        value = int(0);
      if (endPos - (pos+17) == 4) // we only got command for 5 pumps and are done
        return;
      // now check for a sixth speed
      if (isDigit(commandString[pos+21])) {
        for (int j = pos+21; j < lengthString; j++) {
          if (isDigit(commandString[j]) == false) {
            endPos = j;
            if (endPos - (pos+21) > 3)
              value = commandString.substring(pos+22, pos+25).toInt();
            else {
              value = int(0);
              Serial.println("Error: speed setting 'S' for 6 pumps requires 24 digits, got " + String(endPos - (pos+1)));
              broken = true;
              stopPumps();
              return;
            }
            value = ceil(value * 0.33);
            if (commandString[pos+21] == '1') {
              value1 = value;
              SOrder[5] = '1';
            } else if (commandString[pos+21] == '2') {
              value2 = value;
              SOrder[5] = '2';
            } else if (commandString[pos+21] == '3') {
              value3 = value;
              SOrder[5] = '3';
            } else if (commandString[pos+21] == '4') {
              value4 = value;
              SOrder[5] = '4';
            } else if (commandString[pos+21] == '5') {
              value5 = value;
              SOrder[5] = '5';
            } else if (commandString[pos+21] == '6') {
              value6 = value;
              SOrder[5] = '6';
            } else if (commandString[pos+21] == '7') {
              value7 = value;
              SOrder[5] = '7';
            } else if (commandString[pos+21] == '8') {
              value8 = value;
              SOrder[5] = '8';
            } else if (String(commandString[pos+21]).toInt() > 8) {
              value = int(0);
              Serial.println("Error: only 8 pumps supported, got a command for pump number '" + String(commandString[pos+9]) + "'");
              broken = true;
              stopPumps();
              return;
            }
            break;
          }
        }
      } else
        value = int(0);
      if (endPos - (pos+21) == 4) // we only got command for 6 pumps and are done
        return;
      // now check for a seventh speed
      if (isDigit(commandString[pos+25])) {
        for (int j = pos+25; j < lengthString; j++) {
          if (isDigit(commandString[j]) == false) {
            endPos = j;
            if (endPos - (pos+25) > 3)
              value = commandString.substring(pos+26, pos+29).toInt();
            else {
              value = int(0);
              Serial.println("Error: speed setting 'S' for 7 pumps requires 28 digits, got " + String(endPos - (pos+1)));
              broken = true;
              stopPumps();
              return;
            }
            value = ceil(value * 0.33);
            if (commandString[pos+25] == '1') {
              value1 = value;
              SOrder[6] = '1';
            } else if (commandString[pos+25] == '2') {
              value2 = value;
              SOrder[6] = '2';
            } else if (commandString[pos+25] == '3') {
              value3 = value;
              SOrder[6] = '3';
            } else if (commandString[pos+25] == '4') {
              value4 = value;
              SOrder[6] = '4';
            } else if (commandString[pos+25] == '5') {
              value5 = value;
              SOrder[6] = '5';
            } else if (commandString[pos+25] == '6') {
              value6 = value;
              SOrder[6] = '6';
            } else if (commandString[pos+25] == '7') {
              value7 = value;
              SOrder[6] = '7';
            } else if (commandString[pos+25] == '8') {
              value8 = value;
              SOrder[6] = '8';
            } else if (String(commandString[pos+25]).toInt() > 8) {
              value = int(0);
              Serial.println("Error: only 8 pumps supported, got a command for pump number '" + String(commandString[pos+9]) + "'");
              broken = true;
              stopPumps();
              return;
            }
            break;
          }
        }
      } else
        value = int(0);
      if (endPos - (pos+25) == 4) // we only got command for 7 pumps and are done
        return;
      // now check for an eight speed
      if (isDigit(commandString[pos+29])) {
        for (int j = pos+29; j < lengthString; j++) {
          if (isDigit(commandString[j]) == false) {
            endPos = j;
            if (endPos - (pos+29) > 3)
              value = commandString.substring(pos+30, pos+33).toInt();
            else {
              value = int(0);
              Serial.println("Error: speed setting 'S' for 8 pumps requires 32 digits, got " + String(endPos - (pos+1)));
              broken = true;
              stopPumps();
              return;
            }
            value = ceil(value * 0.33);
            if (commandString[pos+29] == '1') {
              value1 = value;
              SOrder[7] = '1';
            } else if (commandString[pos+29] == '2') {
              value2 = value;
              SOrder[7] = '2';
            } else if (commandString[pos+29] == '3') {
              value3 = value;
              SOrder[7] = '3';
            } else if (commandString[pos+29] == '4') {
              value4 = value;
              SOrder[7] = '4';
            } else if (commandString[pos+29] == '5') {
              value5 = value;
              SOrder[7] = '5';
            } else if (commandString[pos+29] == '6') {
              value6 = value;
              SOrder[7] = '6';
            } else if (commandString[pos+29] == '7') {
              value7 = value;
              SOrder[7] = '7';
            } else if (commandString[pos+29] == '8') {
              value8 = value;
              SOrder[7] = '8';
            } else if (String(commandString[pos+29]).toInt() > 8) {
              value = int(0);
              Serial.println("Error: only 8 pumps supported, got a command for pump number '" + String(commandString[pos+13]) + "'");
              broken = true;
              stopPumps();
              return;
            }
            break;
          }
        }
      } else
        value = int(0);
      // check that no pump is defined twice
      for (int j = 1; j < 8; j++)
        if (SOrder[j] == SOrder[0]) {
          Serial.println("Error: speed setting 'S' for pump " + String(SOrder[j]) + " is redefined");
          broken = true;
        }
      for (int j = 2; j < 8; j++)
        if (SOrder[j] == SOrder[1]) {
          Serial.println("Error: speed setting 'S' for pump " + String(SOrder[j]) + " is redefined");
          broken = true;
        }
      for (int j = 3; j < 8; j++)
        if (SOrder[j] == SOrder[2]) {
          Serial.println("Error: speed setting 'S' for pump " + String(SOrder[j]) + " is redefined");
          broken = true;
        }
      for (int j = 4; j < 8; j++)
        if (SOrder[j] == SOrder[3]) {
          Serial.println("Error: speed setting 'S' for pump " + String(SOrder[j]) + " is redefined");
          broken = true;
        }
      for (int j = 5; j < 8; j++)
        if (SOrder[j] == SOrder[4]) {
          Serial.println("Error: speed setting 'S' for pump " + String(SOrder[j]) + " is redefined");
          broken = true;
        }
      for (int j = 6; j < 8; j++)
        if (SOrder[j] == SOrder[5]) {
          Serial.println("Error: speed setting 'S' for pump " + String(SOrder[j]) + " is redefined");
          broken = true;
        }
      for (int j = 7; j < 8; j++)
        if (SOrder[j] == SOrder[6]) {
          Serial.println("Error: speed setting 'S' for pump " + String(SOrder[j]) + " is redefined");
          broken = true;
        }
      if (broken) {
        stopPumps();
        return;
      }
    } // end if (commandString[pos] == 'S')
    //Serial.println("pump speed: "+String(value1)+" : "+String(value2)+" : "+String(value3)+" : "+String(value4));

    // check for direction
    if (commandString[pos] == 'D') {
      //Serial.println("D: " + String(pos));
      //Serial.println("SOrder: " + SOrder);
      // syntax is "Dxxxxxxxx", x = direction of pump (0 or 1), it is not necessary to set all 8 possible motors
      // this is also possible: S39991999D11, then the first 1 belongs to pump 3
      if ((isDigit(commandString[pos+1])) and (String(SOrder[0]).toInt() > 0) ) { // ignore if there is not any 'S' statement
        if (String(commandString[pos+1]).toInt() > 1) {
          Serial.println("Error: value of direction setting 'D' can only be 0 or 1, got '" + String(commandString[pos+1]) + "'");
          broken = true;
          stopPumps();
          return;
        }
        if (commandString[pos+1] == '1') {
          if (SOrder[0] == '1')
             value1 = int(-1) * value1;
          else if (SOrder[0] == '2')
             value2 = int(-1) * value2;
          else if (SOrder[0] == '3')
             value3 = int(-1) * value3;
          else if (SOrder[0] == '4')
             value4 = int(-1) * value4;
          else if (SOrder[0] == '5')
             value5 = int(-1) * value5;
          else if (SOrder[0] == '6')
             value6 = int(-1) * value6;
          else if (SOrder[0] == '7')
             value7 = int(-1) * value7;
          else if (SOrder[0] == '8')
             value8 = int(-1) * value8;
        }
      } else if (!isDigit(commandString[pos+1])) {
        Serial.println("Error: no value given for direction setting 'D'");
        broken = true;
        stopPumps();
        return;
      }
      // only check for second direction if there is a second pump 'S' command
      if ( (isDigit(commandString[pos+2])) and (String(SOrder[1]).toInt() > 0) ) {
        if (String(commandString[pos+2]).toInt() > 1) {
          Serial.println("Error: value of direction setting 'D' can only be 0 or 1, got '" + String(commandString[pos+2]) + "'");
          broken = true;
          stopPumps();
          return;
        }
        if (commandString[pos+2] == '1') {
          if (SOrder[1] == '1')
             value1 = int(-1) * value1;
          else if (SOrder[1] == '2')
             value2 = int(-1) * value2;
          else if (SOrder[1] == '3')
             value3 = int(-1) * value3;
          else if (SOrder[1] == '4')
             value4 = int(-1) * value4;
          else if (SOrder[1] == '5')
             value5 = int(-1) * value5;
          else if (SOrder[1] == '6')
             value6 = int(-1) * value6;
          else if (SOrder[1] == '7')
             value7 = int(-1) * value7;
          else if (SOrder[1] == '8')
             value8 = int(-1) * value8;
        }
      }
      if ( (isDigit(commandString[pos+3])) and (String(SOrder[2]).toInt() > 0) ) {
        if (String(commandString[pos+3]).toInt() > 1) {
          Serial.println("Error: value of direction setting 'D' can only be 0 or 1, got '" + String(commandString[pos+3]) + "'");
          broken = true;
          stopPumps();
          return;
        }
        if (commandString[pos+3] == '1') {
          if (SOrder[2] == '1')
             value1 = int(-1) * value1;
          else if (SOrder[2] == '2')
             value2 = int(-1) * value2;
          else if (SOrder[2] == '3')
             value3 = int(-1) * value3;
          else if (SOrder[2] == '4')
             value4 = int(-1) * value4;
          else if (SOrder[2] == '5')
             value5 = int(-1) * value5;
          else if (SOrder[2] == '6')
             value6 = int(-1) * value6;
          else if (SOrder[2] == '7')
             value7 = int(-1) * value7;
          else if (SOrder[2] == '8')
             value8 = int(-1) * value8;
        }
      }
      if ( (isDigit(commandString[pos+4])) and (String(SOrder[3]).toInt() > 0) ) {
        if (String(commandString[pos+4]).toInt() > 1) {
          Serial.println("Error: value of direction setting 'D' can only be 0 or 1, got '" + String(commandString[pos+4]) + "'");
          broken = true;
          stopPumps();
          return;
        }
        if (commandString[pos+4] == '1') {
          if (SOrder[3] == '1')
             value1 = int(-1) * value1;
          else if (SOrder[3] == '2')
             value2 = int(-1) * value2;
          else if (SOrder[3] == '3')
             value3 = int(-1) * value3;
          else if (SOrder[3] == '4')
             value4 = int(-1) * value4;
          else if (SOrder[3] == '5')
             value5 = int(-1) * value5;
          else if (SOrder[3] == '6')
             value6 = int(-1) * value6;
          else if (SOrder[3] == '7')
             value7 = int(-1) * value7;
          else if (SOrder[3] == '8')
             value8 = int(-1) * value8;
        }
      }
      if ( (isDigit(commandString[pos+5])) and (String(SOrder[4]).toInt() > 0) ) {
        if (String(commandString[pos+5]).toInt() > 1) {
          Serial.println("Error: value of direction setting 'D' can only be 0 or 1, got '" + String(commandString[pos+4]) + "'");
          broken = true;
          stopPumps();
          return;
        }
        if (commandString[pos+5] == '1') {
          if (SOrder[4] == '1')
             value1 = int(-1) * value1;
          else if (SOrder[4] == '2')
             value2 = int(-1) * value2;
          else if (SOrder[4] == '3')
             value3 = int(-1) * value3;
          else if (SOrder[4] == '4')
             value4 = int(-1) * value4;
          else if (SOrder[4] == '5')
             value5 = int(-1) * value5;
          else if (SOrder[4] == '6')
             value6 = int(-1) * value6;
          else if (SOrder[4] == '7')
             value7 = int(-1) * value7;
          else if (SOrder[4] == '8')
             value8 = int(-1) * value8;
        }
      }
      if ( (isDigit(commandString[pos+6])) and (String(SOrder[5]).toInt() > 0) ) {
        if (String(commandString[pos+6]).toInt() > 1) {
          Serial.println("Error: value of direction setting 'D' can only be 0 or 1, got '" + String(commandString[pos+4]) + "'");
          broken = true;
          stopPumps();
          return;
        }
        if (commandString[pos+6] == '1') {
          if (SOrder[5] == '1')
             value1 = int(-1) * value1;
          else if (SOrder[5] == '2')
             value2 = int(-1) * value2;
          else if (SOrder[5] == '3')
             value3 = int(-1) * value3;
          else if (SOrder[5] == '4')
             value4 = int(-1) * value4;
          else if (SOrder[5] == '5')
             value5 = int(-1) * value5;
          else if (SOrder[5] == '6')
             value6 = int(-1) * value6;
          else if (SOrder[5] == '7')
             value7 = int(-1) * value7;
          else if (SOrder[5] == '8')
             value8 = int(-1) * value8;
        }
      }
      if ( (isDigit(commandString[pos+7])) and (String(SOrder[6]).toInt() > 0) ) {
        if (String(commandString[pos+7]).toInt() > 1) {
          Serial.println("Error: value of direction setting 'D' can only be 0 or 1, got '" + String(commandString[pos+4]) + "'");
          broken = true;
          stopPumps();
          return;
        }
        if (commandString[pos+7] == '1') {
          if (SOrder[6] == '1')
             value1 = int(-1) * value1;
          else if (SOrder[6] == '2')
             value2 = int(-1) * value2;
          else if (SOrder[6] == '3')
             value3 = int(-1) * value3;
          else if (SOrder[6] == '4')
             value4 = int(-1) * value4;
          else if (SOrder[6] == '5')
             value5 = int(-1) * value5;
          else if (SOrder[6] == '6')
             value6 = int(-1) * value6;
          else if (SOrder[6] == '7')
             value7 = int(-1) * value7;
          else if (SOrder[6] == '8')
             value8 = int(-1) * value8;
        }
      }
      if ( (isDigit(commandString[pos+8])) and (String(SOrder[7]).toInt() > 0) ) {
        if (String(commandString[pos+8]).toInt() > 1) {
          Serial.println("Error: value of direction setting 'D' can only be 0 or 1, got '" + String(commandString[pos+4]) + "'");
          broken = true;
          stopPumps();
          return;
        }
        if (commandString[pos+8] == '1') {
          if (SOrder[7] == '1')
             value1 = int(-1) * value1;
          else if (SOrder[7] == '2')
             value2 = int(-1) * value2;
          else if (SOrder[7] == '3')
             value3 = int(-1) * value3;
          else if (SOrder[7] == '4')
             value4 = int(-1) * value4;
          else if (SOrder[7] == '5')
             value5 = int(-1) * value5;
          else if (SOrder[7] == '6')
             value6 = int(-1) * value6;
          else if (SOrder[7] == '7')
             value7 = int(-1) * value7;
          else if (SOrder[7] == '8')
             value8 = int(-1) * value8;
        }
      }
    } // end if commandString[pos] == 'D'

    // check for valve on/off
    if (commandString[pos] == 'V') {
      //Serial.println("V: " + String(pos));
      // syntax is "Vnnnnnnnn" while it is not necessary to set all 8 possible valves

      isChangedValve1 = false; isChangedValve2 = false; isChangedValve3 = false; isChangedValve4 = false;
      isChangedValve5 = false; isChangedValve6 = false; isChangedValve7 = false; isChangedValve8 = false;
      
       if (isDigit(commandString[pos+1])) {
        if (String(commandString[pos+1]).toInt() > 1) {
          Serial.println("Error: value of on/off setting 'V' can only be 0 or 1, got '" + String(commandString[pos+1]) + "'");
          broken = true;
          valve1 = false;
          stopPumps();
          return;
        }
        isChangedValve1 = true;
        if (commandString[pos+1] == '1')
          valve1 = true;
        else
          valve1 = false;
      } else {
        valve1 = false;
        // if there is no digit, we must break
        Serial.println("Error: no value given for on/off setting 'V'");
        broken = true;
        stopPumps();
        return;
      }
      // check for second valve
      if (isDigit(commandString[pos+2])) {
        if (String(commandString[pos+2]).toInt() > 1) {
          Serial.println("Error: value of on/off setting 'V' can only be 0 or 1, got '" + String(commandString[pos+2]) + "'");
          broken = true;
          valve2 = false;
          stopPumps();
          return;
        }
        isChangedValve2 = true;
        if (commandString[pos+2] == '1')
          valve2 = true;
        else
          valve2 = false;
      } else {
        // there was only 1 digit
        goto ValveExecute;
      }
      if (isDigit(commandString[pos+3])) {
        if (String(commandString[pos+3]).toInt() > 1) {
          Serial.println("Error: value of on/off setting 'V' can only be 0 or 1, got '" + String(commandString[pos+3]) + "'");
          broken = true;
          valve3 = false;
          stopPumps();
          return;
        }
        isChangedValve3 = true;
        if (commandString[pos+3] == '1')
          valve3 = true;
        else
          valve3 = false;
      } else {
        // there were only 2 digits
        goto ValveExecute;
      }
      if (isDigit(commandString[pos+4])) {
        if (String(commandString[pos+4]).toInt() > 1) {
          Serial.println("Error: value of on/off setting 'V' can only be 0 or 1, got '" + String(commandString[pos+3]) + "'");
          broken = true;
          valve4 = false;
          stopPumps();
          return;
        }
        isChangedValve4 = true;
        if (commandString[pos+4] == '1')
          valve4 = true;
        else
          valve4 = false;
      } else {
        // there were only 3 digits
        goto ValveExecute;
      }
      if (isDigit(commandString[pos+5])) {
        if (String(commandString[pos+5]).toInt() > 1) {
          Serial.println("Error: value of on/off setting 'V' can only be 0 or 1, got '" + String(commandString[pos+3]) + "'");
          broken = true;
          valve5 = false;
          stopPumps();
          return;
        }
        isChangedValve5 = true;
        if (commandString[pos+5] == '1')
          valve5 = true;
        else
          valve5 = false;
      } else {
        // there were only 4 digits
        goto ValveExecute;
      }
      if (isDigit(commandString[pos+6])) {
        if (String(commandString[pos+6]).toInt() > 1) {
          Serial.println("Error: value of on/off setting 'V' can only be 0 or 1, got '" + String(commandString[pos+3]) + "'");
          broken = true;
          valve6 = false;
          stopPumps();
          return;
        }
        isChangedValve6 = true;
        if (commandString[pos+6] == '1')
          valve6 = true;
        else
          valve6 = false;
      } else {
        // there were only 5 digits
        goto ValveExecute;
      }
      if (isDigit(commandString[pos+7])) {
        if (String(commandString[pos+7]).toInt() > 1) {
          Serial.println("Error: value of on/off setting 'V' can only be 0 or 1, got '" + String(commandString[pos+3]) + "'");
          broken = true;
          valve7 = false;
          stopPumps();
          return;
        }
        isChangedValve7 = true;
        if (commandString[pos+7] == '1')
          valve7 = true;
        else
          valve7 = false;
      } else {
        // there were only 6 digits
        goto ValveExecute;
      }
      if (isDigit(commandString[pos+8])) {
        if (String(commandString[pos+8]).toInt() > 1) {
          Serial.println("Error: value of on/off setting 'V' can only be 0 or 1, got '" + String(commandString[pos+4]) + "'");
          broken = true;
          valve8 = false;
          stopPumps();
          return;
        }
        isChangedValve8 = true;
        if (commandString[pos+8] == '1')
          valve8 = true;
        else
          valve8 = false;
      } else
        // there were only 7 digits
        ;
      // execute the valve actions
      // only execute if there is a state change
      ValveExecute:
      if (isChangedValve1)
        digitalWrite(1, valve1);
      if (isChangedValve2)
        digitalWrite(2, valve2);
      if (isChangedValve3)
        digitalWrite(3, valve3);
      if (isChangedValve4)
        digitalWrite(4, valve4);
      if (isChangedValve5)
        digitalWrite(5, valve5);
      if (isChangedValve6)
        digitalWrite(6, valve6);
      if (isChangedValve7)
        digitalWrite(7, valve7);
      if (isChangedValve8)
        digitalWrite(8, valve8);
      //Serial.println("valve state: "+String(valve1)+" : "+String(valve2)+" : "+String(valve3)+" : "+String(valve4));
    } // end if commandString[pos] == 'V'

    // check for pump on/off
    if (commandString[pos] == 'I') {
      //Serial.println("I: " + String(pos));
      // syntax is "Innnnnnnn" while it is not necessary to set all 8 possible motors
      // however, for security reasons, the first digit after the 'I' is the one for pump 1 and so on
      // so e.g. for /0S39991999I11R only the first pump will run
      if (isDigit(commandString[pos+1])) {
        if (String(commandString[pos+1]).toInt() > 1) {
          Serial.println("Error: value of on/off setting 'I' can only be 0 or 1, got '" + String(commandString[pos+1]) + "'");
          broken = true;
          stopPumps();
          return;
        }
        if (commandString[pos+1] == '1')
          motor1 = true;
        else
          motor1 = false;
      } else {
        motor1 = false;
        // if there is no digit, we must break
        Serial.println("Error: no value given for on/off setting 'I'");
        broken = true;
        stopPumps();
        return;
      }
      // only check for second direction if there is a second pump 'S' command
      if (isDigit(commandString[pos+2])) {
        if (String(commandString[pos+2]).toInt() > 1) {
          Serial.println("Error: value of on/off setting 'I' can only be 0 or 1, got '" + String(commandString[pos+2]) + "'");
          broken = true;
          stopPumps();
          return;
        }
        if (commandString[pos+2] == '1')
          motor2 = true;
        else
          motor2 = false;
      } else {
        // there was only 1 digit
        motor2 = false;
        motor3 = false;
        motor4 = false;
        motor5 = false;
        motor6 = false;
        motor7 = false;
        motor8 = false;
        goto MotorRun;
      }
      if (isDigit(commandString[pos+3])) {
        if (String(commandString[pos+3]).toInt() > 1) {
          Serial.println("Error: value of on/off setting 'I' can only be 0 or 1, got '" + String(commandString[pos+3]) + "'");
          broken = true;
          stopPumps();
          return;
        }
        if (commandString[pos+3] == '1')
          motor3 = true;
        else
          motor3 = false;
      } else {
        // there were only 2 digits
        motor3 = false;
        motor4 = false;
        motor5 = false;
        motor6 = false;
        motor7 = false;
        motor8 = false;
        goto MotorRun;
      }
      if (isDigit(commandString[pos+4])) {
        if (String(commandString[pos+4]).toInt() > 1) {
          Serial.println("Error: value of on/off setting 'I' can only be 0 or 1, got '" + String(commandString[pos+3]) + "'");
          broken = true;
          stopPumps();
          return;
        }
        if (commandString[pos+4] == '1')
          motor4 = true;
        else
          motor4 = false;
      } else {
        // there were only 3 digits
        motor4 = false;
        motor5 = false;
        motor6 = false;
        motor7 = false;
        motor8 = false;
        goto MotorRun;
      }
      if (isDigit(commandString[pos+5])) {
        if (String(commandString[pos+5]).toInt() > 1) {
          Serial.println("Error: value of on/off setting 'I' can only be 0 or 1, got '" + String(commandString[pos+3]) + "'");
          broken = true;
          stopPumps();
          return;
        }
        if (commandString[pos+5] == '1')
          motor5 = true;
        else
          motor5 = false;
      } else {
        // there were only 4 digits
        motor5 = false;
        motor6 = false;
        motor7 = false;
        motor8 = false;
        goto MotorRun;
      }
      if (isDigit(commandString[pos+6])) {
        if (String(commandString[pos+6]).toInt() > 1) {
          Serial.println("Error: value of on/off setting 'I' can only be 0 or 1, got '" + String(commandString[pos+3]) + "'");
          broken = true;
          stopPumps();
          return;
        }
        if (commandString[pos+6] == '1')
          motor6 = true;
        else
          motor6 = false;
      } else {
        // there were only 5 digits
        motor6 = false;
        motor7 = false;
        motor8 = false;
        goto MotorRun;
      }
      if (isDigit(commandString[pos+7])) {
        if (String(commandString[pos+7]).toInt() > 1) {
          Serial.println("Error: value of on/off setting 'I' can only be 0 or 1, got '" + String(commandString[pos+3]) + "'");
          broken = true;
          stopPumps();
          return;
        }
        if (commandString[pos+7] == '1')
          motor7 = true;
        else
          motor7 = false;
      } else {
        // there were only 6 digits
        motor7 = false;
        motor8 = false;
        goto MotorRun;
      }
      if (isDigit(commandString[pos+8])) {
        if (String(commandString[pos+8]).toInt() > 1) {
          Serial.println("Error: value of on/off setting 'I' can only be 0 or 1, got '" + String(commandString[pos+4]) + "'");
          broken = true;
          stopPumps();
          return;
        }
        if (commandString[pos+8] == '1')
          motor8 = true;
        else
          motor8 = false;
      } else
        // there were only 7 digits
        motor8 = false;
      // it is not mandatory that the command has a time specification, thus run the pumps right now
      // moreover if the pumps should be stopped, do this as immediate as possible
      MotorRun:
      if (motor1)
        motor12.setMotor(1, value1, true);
      if (!motor1)
        motor12.setMotor(1, int(0), true);
      if (motor2)
        motor12.setMotor(2, value2, true);
      if (!motor2)
        motor12.setMotor(2, int(0), true);
      if (motor3)
        motor34.setMotor(1, value3, true);
      if (!motor3)
        motor34.setMotor(1, int(0), true);
      if (motor4)
        motor34.setMotor(2, value4, true);
      if (!motor4)
        motor34.setMotor(2, int(0), true);
      if (motor5)
        motor56.setMotor(1, value5, true);
      if (!motor5)
        motor56.setMotor(1, int(0), true);
      if (motor6)
        motor56.setMotor(2, value6, true);
      if (!motor6)
        motor56.setMotor(2, int(0), true);
      if (motor7)
        motor78.setMotor(1, value7, true);
      if (!motor7)
        motor78.setMotor(1, int(0), true);
      if (motor8)
        motor78.setMotor(2, value8, true);
      if (!motor8)
        motor78.setMotor(2, int(0), true);
      //Serial.println("pump state: "+String(motor1)+" : "+String(motor2)+" : "+String(motor3)+" : "+String(motor4));
    } // end if commandString[pos] == 'I'

    // check for time
    if (commandString[pos] == 'M') {
      //Serial.println("M: " + String(pos));
      // syntax is "Mx" where x can have as many digits as long as the user wants
      // we must assure the first character after the 'M' is a digit
      if (!isDigit(commandString[pos+1])) {
        stopPumps();
        Serial.println("Error: no value given for time setting 'M'");
        broken = true;
        return;
      }
      for (int j = pos+1; j < lengthString; j++) {
        if (isDigit(commandString[j]) == false) { // tests if it is a digit
          endPos = j;
          timeDelay = commandString.substring(pos+1, endPos).toInt();
          timer = timeDelay + millis();
          while (millis() < timer) {
            // Note: if failsave is set the motor would stop and need to be turned on here
            if (motor1)
              motor12.setMotor(1, value1, true);
            if (!motor1)
              motor12.setMotor(1, int(0), true);
            if (motor2)
              motor12.setMotor(2, value2, true);
            if (!motor2)
              motor12.setMotor(2, int(0), true);
            if (motor3)
              motor34.setMotor(1, value3, true);
            if (!motor3)
              motor34.setMotor(1, int(0), true);
            if (motor4)
              motor34.setMotor(2, value4, true);
            if (!motor4)
              motor34.setMotor(2, int(0), true);
            if (motor5)
              motor56.setMotor(1, value5, true);
            if (!motor5)
              motor56.setMotor(1, int(0), true);
            if (motor6)
              motor56.setMotor(2, value6, true);
            if (!motor6)
              motor56.setMotor(2, int(0), true);
            if (motor7)
              motor78.setMotor(1, value7, true);
            if (!motor7)
              motor78.setMotor(1, int(0), true);
            if (motor8)
              motor78.setMotor(2, value8, true);
            if (!motor8)
              motor78.setMotor(2, int(0), true);
            // there might be a new command
            if (Serial.available()) {
              inputString = "";
              serialRead();
            }
            if (stringComplete)
              break;
          }
          break;
        }
      } //end for j = pos+1
    } // end if commandString[pos] == 'M'

    // check for LED on/off
    if (commandString[pos] == 'L')
      digitalWrite(13, HIGH); // turn the LED on
    if (commandString[pos] == 'l')
      digitalWrite(13, LOW); // turn the LED off

    // check for loop end
    if (commandString[pos] == 'G') {
      //Serial.println("G: " + String(pos));
      // syntax is "Gx" where x can have as many digits as long as the user wants
      // stop if there is a 'G' without a matching 'g'
      if ((gStart1 == 0) and (gStart2 == 0)) {
        Serial.println("Error: end loop statement 'G' does not match a begin loop statement 'g'");
        broken = true;
        stopPumps();
        return;
        }
      // care of inner loop
      // if we are in inner loop, its counter must be > 0
      if ((CountLoop2 > 0) and (CounterLoop2 < CountLoop2)) {
        CounterLoop2 = CounterLoop2 + 1;
        if (CounterLoop2 == CountLoop2) { // inner loop ended
          //pos = pos + 1; // set position after the loop
          CountLoop2 = -2; // there is no active loop
          CounterLoop2 = 0; // reset counter
          gStart2 = 0;
          return;
        }
        if (CounterLoop2 < CountLoop2) {
          pos = gStart2; // jump to beginning of loop
          return;
        }
      }
      // care of outer loop
      // inner loop must then not exist, so gStart2 = 0
      if ((gStart2 == 0) and (CounterLoop1 < CountLoop1)) {
        CounterLoop1 = CounterLoop1 + 1;
        if (CounterLoop1 == CountLoop1) { // outer loop ended
          //pos = pos + 1; // set position after the loop
          CountLoop1 = -2; // there is no active loop
          CounterLoop1 = 0; // reset counter
          gStart1 = 0;
          return;
        }
        if (CounterLoop1 < CountLoop1) {
          pos = gStart1; // jump to beginning of loop
          return;
        }
      }
      // retrieve loop numbers
      for (int j = pos; j < lengthString; j++) {
        if (isDigit(commandString[j+1]) == false) { // tests if it is a digit
          endPos = j + 1;
          if (endPos == pos + 1) { // infinite repeats
            if ((gStart2 > 0) and ((CountLoop2 == -2))) {
              Serial.println("Error: inner loop must not run infinitely");
              broken = true;
              return;
            }
            pos = gStart1;
            CountLoop1 = -1;
            return;
          } else {
            // set loop counter
            // if there is an inner loop but loop number not yet known, it must be G of inner loop
            if ((gStart2 > 0) and ((CountLoop2 == -2))) { // inner loop, -2 means not yet known
              CountLoop2 = int(commandString.substring(pos+1, endPos).toInt());
              if ((gStart2 > 0) and (CountLoop2 == 0)) {
                pos = endPos - 1;
                CountLoop2 = -2; // there is no active loop
                CounterLoop2 = 0; // reset counter
                gStart2 = 0;
                return; // there is nothing to repeat
              }
              pos = gStart2; // jump to beginning of loop
              return;
            }
            if ( (gStart2 == 0) or ((CountLoop1 == -2) and (gStart2 > 0)) ) { // outer loop, -2 means not yet known
              CountLoop1 = int(commandString.substring(pos+1, endPos).toInt());
              if (gStart2 == 0) { // only if there is no active inner loop set position
                if ((gStart1 > 0) and (CountLoop1 == 0)) {
                  pos = endPos - 1;
                  CountLoop1 = -2; // there is no active loop
                  CounterLoop1 = 0; // reset counter
                  gStart1 = 0;
                  return; // there is nothing to repeat
                }
                pos = gStart1; // jump to beginning of loop
                return;
              }
            }
          } // end if not infinite repeats
        }
      } // end for j
    } // end if commandString[pos] == 'G'

    // only check for command end to catch an error and to inform when command was finished
    if (commandString[pos] == 'R') {
      if ((gStart1 > 0) or (gStart2 > 0)) { // we have an unfinished loop
        Serial.println("Error: missing end loop statement 'G'");
        broken = true;
        motor12.setMotor(1, int(0), true); motor12.setMotor(2, int(0), true);
        motor34.setMotor(1, int(0), true); motor34.setMotor(2, int(0), true);
        motor56.setMotor(1, int(0), true); motor56.setMotor(2, int(0), true);
        motor78.setMotor(1, int(0), true); motor78.setMotor(2, int(0), true);
        return;
      } else
        Serial.println(" Command sucessfully executed and finished");
    }

  } // end if !stringComplete
  
} // end void loop()

//---------------------------------------------------------------------------------------------------------------------

// reads serial values if available
void serialRead() {
  boolean useData = true;
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    if (useData)
     inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something with the command.
    if (inChar == '\n') {
      stringComplete = true;
     // We cannot break because we must assure tha all input is read because the
     // input buffer is just 512 chars, therefor just throw them away
     useData = false;
    }
  }
} // end void serialRead()

//---------------------------------------------------------------------------------------------------------------------

// stops all pumps
void stopPumps() {
  motor12.setMotor(1, int(0), true); motor12.setMotor(2, int(0), true);
  motor34.setMotor(1, int(0), true); motor34.setMotor(2, int(0), true);
  motor56.setMotor(1, int(0), true); motor56.setMotor(2, int(0), true);
  motor78.setMotor(1, int(0), true); motor78.setMotor(2, int(0), true);
} // end void stopPumps()
