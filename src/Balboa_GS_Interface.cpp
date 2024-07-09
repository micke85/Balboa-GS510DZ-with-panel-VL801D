// 2024-07-09 Version 0.95

#include "Balboa_GS_Interface.h" 


byte BalboaInterface::clockPin;
byte BalboaInterface::displayPin;
byte BalboaInterface::buttonPin;
bool BalboaInterface::displayDataBufferOverflow;
bool BalboaInterface::writeDisplayData;       
bool BalboaInterface::writeButtonUp;
bool BalboaInterface::writeButtonDown;
bool BalboaInterface::writeTempUp;
bool BalboaInterface::writeTempDown;
bool BalboaInterface::writeLights;
bool BalboaInterface::writePump1;
bool BalboaInterface::writePump2;
bool BalboaInterface::writeBlower;
bool BalboaInterface::writeTimeMenu;
unsigned long BalboaInterface::clockInterruptTime;
int  BalboaInterface::clockBitCounter;  
byte BalboaInterface::displayDataBuffer[displayDataBufferSize];
byte BalboaInterface::dataIndex; 
bool BalboaInterface::displayDataBufferReady;  


BalboaInterface::BalboaInterface(byte setClockPin, byte setReadPin, byte setWritePin) {
  clockPin = setClockPin;
  displayPin = setReadPin;
  buttonPin = setWritePin;
     
}

void BalboaInterface::begin() { 
  pinMode(clockPin, INPUT);
  pinMode(displayPin, INPUT);
  pinMode(buttonPin, OUTPUT);
  digitalWrite(buttonPin,LOW);   
   
  attachInterrupt(clockPin, clockPinInterrupt, CHANGE);
  
 }

void BalboaInterface::stop() {
  
  detachInterrupt(digitalPinToInterrupt(clockPin));    

}

bool BalboaInterface::loop() {

// Update

 if (displayDataBufferReady) { 
    // Decode data once available 
    decodeDisplayData(); 
    
   	 // Get setTemperature if not known on start up
 	if (!isInitialized) {
   	   TempMenu = true;
   	   writeDisplayData = true;
	   writeTempUp = true;
  	  }
  	  isInitialized = true;				// Set the flag to true to prevent re-running


		// Update tempreture
		if (TempMenu && !TimeMenu) {
			if(millis() - buttonPressTimerPrevMillis  > buttonPressTimerMillis) {
				buttonPressTimerPrevMillis = millis();
				
				if (updateTempDirection == 1) {
					writeDisplayData = true;
					writeTempDown = true;

				}else if (updateTempDirection == 2) {
					writeDisplayData = true;
					writeTempUp = true;		
				}
			
			}
		}
 	 }
		
	return true;
	}




void BalboaInterface::updateTemperature(float Temperature){
	
	 
	float updateTempDifference = Temperature - setTemperature;
	if (updateTempDifference < 0 && TempMenu == true){ 
	updateTempDirection = 1; }													// Temp down
	else if (updateTempDifference > 0 && TempMenu == true){ 
	updateTempDirection = 2; }													// Temp up
	else if (updateTempDifference == 0) { 
	updateTempDirection = 0; 
	}																
}
	
	
void BalboaInterface::decodeDisplayData() {

      LCD_segment_1 = 0;
      LCD_segment_2 = 0;
      LCD_segment_3 = 0;
      LCD_segment_4 = 0;
      
      LCD_display = "";
            
      for (int x = 0; x <= displayDataBits; x++) {
      
                  if ( x > 0 && x <= 7 ) {
                        LCD_segment_1 <<= 1;
                        if (displayDataBuffer[x] == 1){
                            LCD_segment_1 |= 1;
                        }
                        else LCD_segment_1 |= 0;
                  }
                  else if (x > 7 && x <= 14) {
                        LCD_segment_2 <<= 1;
                        if ( displayDataBuffer[x] == 1){
                            LCD_segment_2 |= 1;
                        }
                        else LCD_segment_2 |= 0;
                  }   
                  else if (x > 14 && x <= 21) {
                        LCD_segment_3 <<= 1;
                        if ( displayDataBuffer[x] == 1){
                            LCD_segment_3 |= 1;
                        }
                        else LCD_segment_3 |= 0;
                  }   
                  else if (x > 21 && x <= 28) {
                        LCD_segment_4 <<= 1;
                        if ( displayDataBuffer[x] == 1){
                            LCD_segment_4 |= 1;
                        }
                        else LCD_segment_4 |= 0;     
                  }  
                  else if (x == 29) {
                        if ( displayDataBuffer[x] == 1){
                            displayButton = true;
                        }
                        else displayButton = false;
                  }  
				  else if (x == 30) {
                        if ( displayDataBuffer[x] == 1){
                            TimeMenu = true;
                        }
                        else TimeMenu = false;
                  } 
                  else if (x == 31) {
                        if ( displayDataBuffer[x] == 1){
                            StandardMode = true;
                        }
                        else StandardMode = false;
                  } 
                  else if (x == 32) {
                        if ( displayDataBuffer[x] == 1){
                            displayBit32 = true;
                        }
                        else displayBit32 = false;
                  } 
                  else if (x == 33) {
                        if ( displayDataBuffer[x] == 1){
                            displayBit33 = true;
                        }
                        else displayBit33 = false;
                  } 
                  else if (x == 34) {
                        if ( displayDataBuffer[x] == 1){
                            displayBit34 = true;
                        }
                        else displayBit34 = false;
                  } 
                  else if (x == 35) {
                        if ( displayDataBuffer[x] == 1){
                            displayBit35 = true;
                        }
                        else displayBit35 = false;
                  } 
                  else if (x == 36) {
                        if ( displayDataBuffer[x] == 1){
                            Filter1 = true;
                        }
                        else Filter1 = false;
                  } 
                  else if (x == 37) {
                        if ( displayDataBuffer[x] == 1){
                            Filter2 = true;
                        }
                        else Filter2 = false;
                  }   
                  else if (x == 38) {
                        if ( displayDataBuffer[x] == 1){
                            TempUp = true;
                        }
                        else TempDown = false;
                  }   
				  else if (x == 39) {
                        if ( displayDataBuffer[x] == 1){
                            Heater = true;
                        }
                        else Heater = false;
                  }  
				  else if (x == 40) {
                        if ( displayDataBuffer[x] == 1){
                            TempMenu = true;
                        }
                        else TempMenu = false;
                  }  
				  else if (x == 41) {
                        if ( displayDataBuffer[x] == 1){
                            displayBit41 = true;
                        }
                        else displayBit41 = false;
                  }  
				  else if (x == 42) {
                        if ( displayDataBuffer[x] == 1){
                            Blower = true;
                        }
                        else Blower = false;
                  } 
				  else if (x == 43) {
                        if ( displayDataBuffer[x] == 1){
                            displayBit43 = true;
                        }
                        else displayBit43 = false;
                  } 
				  else if (x == 44) {
                        if ( displayDataBuffer[x] == 1){
                            Filtration = true;
                        }
                        else Filtration = false;
                  } 
				  else if (x == 45) {
                        if ( displayDataBuffer[x] == 1){
                            displayBit45 = true;
                        }
                        else displayBit45 = false;
                  }
				  else if (x == 46) {
                        if ( displayDataBuffer[x] == 1){
                            displayBit46 = true;
                        }
                        else displayBit46 = false;
                  }
				  else if (x == 47) {
                        if ( displayDataBuffer[x] == 1){
                            Lights = true;
                        }
                        else Lights = false;
                  }
				  else if (x == 48) {
                        if ( displayDataBuffer[x] == 1){
                            Pump1 = true;
                        }
                        else Pump1 = false;
                  } 
				  else if (x == 49) {
                        if ( displayDataBuffer[x] == 1){
                            Pump2 = true;
                        }
                        else Pump2 = false;
                  } 
				  else if (x == 50) {
                        if ( displayDataBuffer[x] == 1){
                            displayBit50 = true;
                        }
                        else displayBit50 = false;
                  } 
				  else if (x == 51) {
                        if ( displayDataBuffer[x] == 1){
                            displayBit51 = true;
                        }
                        else displayBit51 = false;
                  } 
				  else if (x == 52) {
                        if ( displayDataBuffer[x] == 1){
                            displayBit52 = true;
                        }
                        else displayBit52 = false;
                  } 
				  else if (x == 53) {
                        if ( displayDataBuffer[x] == 1){
                            displayBit53 = true;
                        }
                        else displayBit53 = false;
                  } 
				  else if (x == 54) {
                        if ( displayDataBuffer[x] == 1){
                            displayBit54 = true;
                        }
                        else displayBit54 = false;
                  } 
				  else if (x == 55) {
                        if ( displayDataBuffer[x] == 1){
                            displayTIME = true;
                        }
                        else displayTIME = false;
                  } 
				  else if (x == 56) {
                        if ( displayDataBuffer[x] == 1){
                            displaySET = true;
                        }
                        else displaySET = false;
                  } 
				  else if (x == 57) {
                        if ( displayDataBuffer[x] == 1){
                            displayBit57 = true;
                        }
                        else displayBit57 = false;
                  } 
				  else if (x == 58) {
                        if ( displayDataBuffer[x] == 1){
                            displayBit58 = true;
                        }
                        else displayBit58 = false;
                  } 
				  else if (x == 59) {
                        if ( displayDataBuffer[x] == 1){
                            StandardMode = true;
                        }
                        else StandardMode = false;
                  } 
				  else if (x == 60) {
                        if ( displayDataBuffer[x] == 1){
                            EcoMode = true;
                        }
                        else EcoMode = false;
                  } 
				  else if (x == 61) {
                        if ( displayDataBuffer[x] == 1){
                            displayPM = true;
                        }
                        else displayPM = false;
                  } 
				  else if (x == 62) {
                        if ( displayDataBuffer[x] == 1){
                            displayBit62 = true;
                        }
                        else displayBit62 = false;
                  } 
				  else if (x == 63) {
                        if ( displayDataBuffer[x] == 1){
                            displayAM = true;
                        }
                        else displayAM = false;
                  } 
				  else if (x == 64) {
                        if ( displayDataBuffer[x] == 1){
                            TempDown = true;
                        }
                        else TempDown = false;
                  } 
				  else if (x == 65) {
                        if ( displayDataBuffer[x] == 1){
                            displayBit65 = true;
                        }
                        else displayBit65 = false;
                  } 
				  else if (x == 66) {
                        if ( displayDataBuffer[x] == 1){
                            displayBit66 = true;
                        }
                        else displayBit66 = false;
                  } 
				  else if (x == 67) {
                        if ( displayDataBuffer[x] == 1){
                            displayBit67 = true;
                        }
                        else displayBit67 = false;
                  } 
				  else if (x == 68) {
                        if ( displayDataBuffer[x] == 1){
                            displayBit68 = true;
                        }
                        else displayBit68 = false;
                  } 
				  else if (x == 69) {
                        if ( displayDataBuffer[x] == 1){
                            displayBit69 = true;
                        }
                        else displayBit69 = false;
                  } 
				  else if (x == 70) {
                        if ( displayDataBuffer[x] == 1){
                            displayBit70 = true;
                        }
                        else displayBit70 = false;
                  } 
				  else if (x == 71) {
                        if ( displayDataBuffer[x] == 1){
                            displayBit71 = true;
                        }
                        else displayBit71 = false;
                  } 
				  
				  
				  
            } 
        
           LCD_display_1 = lockup_LCD_character(LCD_segment_4);
           LCD_display_2 = lockup_LCD_character(LCD_segment_3); 
           LCD_display_3 = lockup_LCD_character(LCD_segment_2);
           LCD_display_4 = lockup_LCD_character(LCD_segment_1);  
      
           
             // check if temperature or something else is shown on LCD display
           
		    // No temperature is shown
			 if(LCD_segment_4 == 0) {   
                  LCD_display = LCD_display_1 + LCD_display_2 + LCD_display_3 + LCD_display_4; 
             } 
             
			 // Temperature is shown
			else {
                 
				float Temperature = (10 * LCD_display_1.toInt() + LCD_display_2.toInt() + 0.1 * LCD_display_3.toInt());
				 
				if (TempMenu == true && Temperature>=1) {setTemperature = Temperature;}
				else {waterTemperature = Temperature;}  
				
				LCD_display = LCD_display_1 + LCD_display_2 + "." + LCD_display_3 + LCD_display_4; 
             }
                         
            displayDataBufferReady = false;
            attachInterrupt(clockPin, clockPinInterrupt, CHANGE);
}

 ICACHE_RAM_ATTR void BalboaInterface::clockPinInterrupt() {
	  
        
     if (!displayDataBufferReady) {
            
            if ((micros() - clockInterruptTime) >= durationNewCycle ) {             // New cycle detected  
                    dataIndex = 0;
                    clockBitCounter = 0;
                    displayDataBufferReady = false;
            }
            
            clockInterruptTime = micros();
            
            if (digitalRead(clockPin) == LOW) { digitalWrite(buttonPin,LOW); }
                     
            if (digitalRead(clockPin) == HIGH) {

                                         
     
                   // Write button data if requested
                  
                  if (writeDisplayData == true && clockBitCounter >= 72 && clockBitCounter <= 75){
                            
                          if (clockBitCounter == 72) {
                                 
                                  if (writeButtonUp)    		{ digitalWrite(buttonPin,HIGH);  }
                                  else if (writeButtonDown)		{ digitalWrite(buttonPin,HIGH);  }
                                  else if (writeTempUp)     	{ digitalWrite(buttonPin,HIGH);  }
                                  else if (writeTempDown)   	{ digitalWrite(buttonPin,HIGH);  }
								  else if (writeBlower)     	{ digitalWrite(buttonPin,HIGH);  }
                                  else if (writeLights)      	{ digitalWrite(buttonPin,HIGH);  }
                                  else if (writePump1)     		{ digitalWrite(buttonPin,HIGH);  }
                                  else if (writePump2)     		{ digitalWrite(buttonPin,HIGH);  }
							//	  else if (writeTimeMenu)     	{ digitalWrite(buttonPin,HIGH);  }
                                  
                          }

                          else if (clockBitCounter == 73) {
                                 
                                  if (writeButtonUp)    		{ digitalWrite(buttonPin,HIGH);  }
                                  else if (writeButtonDown)   	{ digitalWrite(buttonPin,HIGH);  }
                                  else if (writeTempUp)    		{ digitalWrite(buttonPin,HIGH);  }
                                  else if (writeTempDown)  		{ digitalWrite(buttonPin,HIGH);  }
								  else if (writeBlower)     	{ digitalWrite(buttonPin,HIGH);  }
                                  else if (writeLights)				{ digitalWrite(buttonPin,LOW);   }
                                  else if (writePump1)     	  		{ digitalWrite(buttonPin,LOW);   }
                                  else if (writePump2)     	  		{ digitalWrite(buttonPin,LOW);   }
							//	  else if (writeTimeMenu)     	{ digitalWrite(buttonPin,HIGH);  }
                          }
						   

                          else if (clockBitCounter == 74) {
                                 
                                  if (writeButtonUp)  			{ digitalWrite(buttonPin,HIGH);  }
                                  else if (writeButtonDown)  	{ digitalWrite(buttonPin,HIGH);  }
                                  else if (writeTempUp)    		{ digitalWrite(buttonPin,HIGH);  }
                                  else if (writeTempDown)		{ digitalWrite(buttonPin,HIGH);  }
								  else if (writeBlower)    	  		{ digitalWrite(buttonPin,LOW);   }
                                  else if (writeLights)			{ digitalWrite(buttonPin,HIGH);  }
                                  else if (writePump1)    	  		{ digitalWrite(buttonPin,LOW);   }
                                  else if (writePump2)			{ digitalWrite(buttonPin,HIGH);  }
							//	  else if (writeTimeMenu)     	{ digitalWrite(buttonPin,HIGH);  }         
                          }

                          else if (clockBitCounter == 75) {
                                  
                                  if (writeButtonUp)				{ digitalWrite(buttonPin,LOW);   }
                                  else if (writeButtonDown) 	{ digitalWrite(buttonPin,HIGH);  }
                                  else if (writeTempUp)   	   		{ digitalWrite(buttonPin,LOW);   }
                                  else if (writeTempDown) 	 	{ digitalWrite(buttonPin,HIGH);  }
								  else if (writeBlower)    	 	{ digitalWrite(buttonPin,HIGH);  }
                                  else if (writeLights)   		{ digitalWrite(buttonPin,HIGH);  }
                                  else if (writePump1)   	 	{ digitalWrite(buttonPin,HIGH);  }
                                  else if (writePump2)				{ digitalWrite(buttonPin,LOW);   }
                          //	  else if (writeTimeMenu)     	{ digitalWrite(buttonPin,HIGH);  }        

                                  writeButtonUp = false;
                                  writeButtonDown = false;
                                  writeTempUp = false;
                                  writeTempDown = false;
                                  writeLights = false;
                                  writePump1 = false;
                                  writePump2 = false;
                                  writeBlower = false;
						 //		writeTimeMenu = false;
                        }
                  }

                  // Read display data   
                                 
                  if ( clockBitCounter <= displayDataBits ) {
                            displayDataBuffer[dataIndex] = digitalRead(displayPin);
                            dataIndex++;
                  }
                  else if ( clockBitCounter == totalDataBits ){          // Total cycle has passed  
                           displayDataBufferReady = true;
                           detachInterrupt(digitalPinToInterrupt(clockPin));                         
                  }
                  else if ( clockBitCounter > totalDataBits ){
                          displayDataBufferOverflow  = true;
                  }
                  
                 clockBitCounter++; 
				 
           }
      }
} 

String BalboaInterface::lockup_LCD_character(int LCD_character) {

      
      switch (LCD_character) {
          case B0000000: return " ";  break;
          case B1111110: return "0";  break;
          case B0110000: return "1";  break;
          case B1101101: return "2";  break;
          case B1111001: return "3";  break;
          case B0110011: return "4";  break;
          case B1011011: return "5";  break;
          case B1011111: return "6";  break;
          case B1110000: return "7";  break;
		  case B1111111: return "8";  break;
		  case B1110011: return "9";  break;    
          case B1110111: return "A";  break;
       // case B0011111: return "B";  break;	//Same binary as b
          case B1001110: return "C";  break;
       // case B0111101: return "D";  break;	//Same binary as d
          case B1001111: return "E";  break;
       // case B1000111: return "F";  break;  	//Same binary as f
          case B1011110: return "G";  break; 
          case B0110111: return "H";  break;
       // case B0000110: return "I";  break;	//Same binary as l
          case B0111100: return "J";  break;
       // case B1010111: return "K";  break;	//Same binary as k
          case B0001110: return "L";  break;
          case B1010100: return "M";  break;
          case B1110110: return "N";  break;
       // case B1111110: return "O";  break;	//Same binary as 0
       // case B1100111: return "P";  break;	//Same binary as p
          case B1101011: return "Q";  break;
          case B1100110: return "R";  break;
       // case B1011011: return "S";  break;	//Same binary as 5 & s
       // case B0001111: return "T";  break;
          case B0111110: return "U";  break;
       // case B0111110: return "V";  break;	//Same binary as U
          case B0101010: return "W";  break;
       // case B0110111: return "X";  break;	//Same binary as H & x
       // case B0111011: return "Y";  break;	//Same binary as y
       // case B1101101: return "Z";  break;    //Same binary as 2 
          case B1111101: return "a";  break;
          case B0011111: return "b";  break;
          case B0001101: return "c";  break; 
          case B0111101: return "d";  break;
          case B1101111: return "e";  break;
          case B1000111: return "f";  break;
          case B1111011: return "g";  break;	
          case B0010111: return "h";  break;
          case B0000100: return "i";  break;
          case B0000001: return "j";  break;
          case B1010111: return "k";  break;
          case B0000110: return "l";  break;
          case B0010100: return "m";  break;
          case B0010101: return "n";  break;
          case B0011101: return "o";  break;
          case B1100111: return "p";  break;
       // case B1110011: return "q";  break;	//Same binary as 9 
          case B0000101: return "r";  break;
       // case B1011011: return "s";  break;	//Same binary as 5
          case B0001111: return "t";  break;
          case B0011100: return "u";  break;
       // case B0011100: return "v";  break;	//Same binary as u
       // case B0010100: return "w";  break;	//Same binary as m
       // case B0110111: return "x";  break;	//Same binary as H
          case B0111011: return "y";  break;
          default:       return "-";  break; // Error condition, displays vertical bars
             
      }
}

