### version A 0.95
Working project in it´s end of alpha face.

- fix set temperature interference that occurred after a couple of hours.
- fix water temperature interference.

### version A 0.9
Working project in it´s end of alpha face.

- Possible to change temp, control pumps and lights.

- Detect filters curculation, heater, standard mode and eco mode.

- Defined different button for up/down with temp up/down.

- Over The Air update now working
  http://unitsIPadress/update
  
Identifing bits are still ongoing. List of definition you will find in the Balboa_GS_Interface.h file

This is a mix of the great work by MagnusPer project with GS510SZ and Shuraxxx, GS523DZ.

Could possibly work with the GS523DZ models as well, as its the same display, I have no ability to try this.

<img src="https://github.com/micke85/Balboa-GS510DZ-with-panel-VL801D/blob/main/Images/BalboaGS510DZVl801D.jpg" width="900">

## RJ45 pins
<img src="https://github.com/Shuraxxx/-Balboa-GS523SZ-with-panel-VL801D-DeluxeSerie-/blob/main/src/img/RJ45.png" width="200">

| PIN           | Description             | 
| ------------- | ------------------------|
| PIN 1         | Functionality unclear!  |
| PIN 2         | Functionality unclear!  |
| PIN 3         | Button data             |  
| PIN 4         | GND                     |
| PIN 5         | Display data            | 
| PIN 6         | Clock                   |
| PIN 7         | 5 VDC                   |  
| PIN 8         | Floating output         |

##PIN 6 (Yellow - clock), PIN 3 (Red - button data), PIN 5 (Green - display data)

<img src="https://github.com/Shuraxxx/-Balboa-GS523SZ-with-panel-VL801D-DeluxeSerie-/blob/main/src/img/DATA-1.jpg" width="800"> 
<img src="https://github.com/Shuraxxx/-Balboa-GS523SZ-with-panel-VL801D-DeluxeSerie-/blob/main/src/img/DATA-2.jpg" width="800"> 

#### Conclusion

- The clock is pulsing 7 times for each chunck of display data, totaly 71 bits and 4 times for button data. A complete cycle 75 pulses. <br />
- Totaly of 6 display data chunks in each cycle <br />
- Each cycle is repeted every 10 ms <br />
- One total cycle duration 3.54 ms
- Bit duration for display data is 23.75 microseconds for display data <br />
- Bit duration for button data is 71.5 microseconds <br />

#### Display Data

Display shows **29.5C** - How does that translates into the bitstream? The first 4 chunks is coded in BCD to 7 segment LCD (see reference) but only with 7 bits sinse the LSB always is (0). <br />
2 = 1101101(0) = (Chunk 4) <br />
9 = 1110011(0) = (Chunk 3) <br />
5 = 1011011(0) = (Chunk 2) <br />
C = 1001110(0) = (Chunk 1) <br />

| Chunk 4 - bit(0-7) | Chunk 3 - bit(7-14) | Chunk 2 - bit(14-21) | Chunk 1 - bit(21-28) | Chunk 5 - bit(29-71) | 
| ------------------ | ------------------- | ------------------   | -------------------- | -------------------- |                     
| LCD segment 1      |  LCD segment 2      |   LCD Segment 3      |   LCD segment 4      | 29 - Button down/up  |                    
|                    |                     |                      |                      |                      |       
|                    |                     |                      |                      |                      | 
|                    |                     |                      |                      | 59 - Standard mode   | 
|                    |                     |                      |                      |                      |  
|                    |                     |                      |                      |                      |   
|                    |                     |                      |                      |                      | 
<img src="https://github.com/MagnusPer/Balboa-GS510SZ/blob/main/extras/docs/Balboa_GS_Interface.jpg" width="800">

## PCB fabrication
This uses the same PCB developed by MagnusPer work on gs510SZ

Dont forget to give space between PCB and ESP8266 for the USB cable connection

- https://github.com/MagnusPer/Balboa-GS510SZ/
  
## Installing PCB
Be surent you are using a standard Patch cable and not a crossover cable, or you will destroy the panel unit.

GS510DZ J1 Port to PCB control system port

Display port to vl801D panel

Magnus PCB board files you will find here: [PCB Balboa-GS510SZ](https://github.com/MagnusPer/Balboa-GS510SZ/tree/main/extras/PCB%20layout)

## Balboa Display panels information
- http://service.balboa-instruments.com/appnotes/index.html?app_notes_vl801d.htm

## Display Paternt nr 8550643
- https://ppubs.uspto.gov/dirsearch-public/print/downloadPdf/8550643


## References of great help!
- https://github.com/NickB1/OpenSpa/blob/master/documents/Balboa/Balboa_Display_Measurements.pdf
- https://www.olivierhill.ca/archives/72-The-Internet-of-Spas.html
- https://create.arduino.cc/projecthub/meljr/7-segment-led-displays-102-using-a-shift-register-6b6976

## Other Balboa projects 
- GL2000 Series https://github.com/netmindz/balboa_GL_ML_spa_control
- BP Series https://github.com/ccutrer/balboa_worldwide_app
- https://github.com/MagnusPer/Balboa-GS510SZ/
- https://github.com/Shuraxxx/-Balboa-GS523DZ-with-panel-VL801D-DeluxeSerie--MQTT?tab=readme-ov-file
