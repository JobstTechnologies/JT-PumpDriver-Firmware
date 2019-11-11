# JT Pump Driver Firmware

Firmware for the pump drivers for CPP1 micropumps: https://www.jobst-technologies.com/products/microfluidics/

# Copyrights

Copyright by Jobst Technologies.

This project uses libraries developed by TinyCircuits: https://github.com/TinyCircuits/TinyCircuits-TinyShield-Dual-Motor-ASD2302

# Compilation

- Install the Arduino IDE: https://www.arduino.cc/en/main/software
- For the first run you must prepare the IDE program following Step 1 in this tutorial:
https://learn.tinycircuits.com/Processors/TinyZero_Setup_Tutorial/
- In the IDE use the menu File → Open and open the project “JT-PumpDriver-Firmware.ino”.
- Connect to the processor board by using the menu Tools → Port → COM xx where xx is the COM port number of the board with the description “TinyScreen+”. If this was successful, you see that there is the connection to a TinyScreen+ in the footer line of the IDE.
- Use the menu Sketch → Verify/Compile. If this was successful, you see "Dome compiling." and no warnings or errors in the IDE.
- To compile a binary use the menu Sketch → Export compiled Binary. This will create a file with the suffix ".bin".

# Uploading Firmware to a Pump Driver

- Connect your pump driver to the PC.

We recommend now to
- Run the program JT Pump Driver: https://github.com/JobstTechnologies/JT-PumpDriver-Firmware
- Connect in JT Pump Driver to the pump driver and use the menu Miscellaneous → Firmware Update.

Alternatively you can
- Open the file “JT-PumpDriver-Firmware.ino” in the Arduino IDE.
- Use the menu Sketch → Upload.
