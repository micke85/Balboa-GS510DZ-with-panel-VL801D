
#ifndef Balboa_GS_Interface_h
#define Balboa_GS_Interface_h

#include <Arduino.h>
// Problem with showing SetTemp as its a float

// Balboa 510DZ
const byte displayDataBufferSize       		= 74;	// 74 Size of display data buffer|
const byte displayDataBits             		= 71;   // 71 71 bits length of display data within a cycle
const byte buttonDataBits              		= 4;    // 0-4  4  bits length of button data within a cycle
const byte totalDataBits               		= 75;   // 56-75, 75 total number of pulses within a cycle 
const unsigned int durationNewCycle    		= 5000;	// How many microsecounds to detect new cycle if no interrupt occurs 
const unsigned long buttonPressTimerMillis  	= 500; 	// Timer in milliseconds between update temperature button presses 


class BalboaInterface {

  public:
	
	BalboaInterface(byte setClockPin, byte setReadPin, byte setWritePin);
	
	// Interface control
	void begin();					// Initializes the stream output to Serial by default
    	bool loop();                                    // Returns true if valid data is available
    	void stop();                                    // Disables the clock hardware interrupt 
    	void resetStatus();                             // Resets the state of all status components as changed for sketches to get the current status	
	void updateTemperature(float Temperature);	// Function to set the water temperature 	

	

	// Status tracking
	float waterTemperature;                		// Water temperatur
	float SetTemp;                			// Set water temp  
	float setTemperature;                		// The wanted set temperature 

	String LCD_display;				// The text shown on display			| Bit 1-28
	bool displayButton;        			// Up/Down button pressed 			| Bit 29
	bool TimeMenu;        				// Display Time menu				| Bit 30
	bool displayBit31;        			// Still unknown functionality, if at all used! | Bit 31 	      
	bool displayBit32;        			// Still unknown functionality, if at all used!	| Bit 32
	bool displayBit33;        			// ? ON ?					| Bit 33
	bool displayBit34;				// Still unknown functionality, if at all used! | Bit 34 
	bool displayBit35;				// Still unknown functionality, if at all used!	| Bit 35 
	bool Filter1;        				// Filter 1					| Bit 36
	bool Filter2;					// Filter 2 					| Bit 37
   	bool TempUp;					// Temp UP 					| Bit 38 
   	bool Heater;					// Heater running or not  			| Bit 39
   	bool TempMenu;					// Display Temp menu / Set			| Bit 40
    	bool displayBit41;				// Still unknown functionality, if at all used! | Bit 41		
	bool Blower;                             	// Blower running or not 			| Bit 42
	bool displayBit43;				// Mode/program button somthing?		| Bit 43
	bool Filtration;				// Show Filtrations 				| Bit 44
	bool displayBit45;				// Still unknown functionality, if at all used! | Bit 45
	bool displayBit46;				// Still unknown functionality, if at all used! | Bit 46
	bool Lights;        				// SPA lights activated or not 			| Bit 47
	bool Pump1;        				// Pump 1 running or not 			| Bit 48
	bool Pump2;        				// Pump 2 running or not			| Bit 49
	bool displayBit50;				// Still unknown functionality, if at all used! | Bit 50			
	bool displayBit51;				// Still unknown functionality, if at all used! | Bit 51
	bool displayBit52;				// Still unknown functionality, if at all used! | Bit 52
	bool displayBit53;				// Still unknown functionality, if at all used! | Bit 53
	bool displayBit54;				// Still unknown functionality, if at all used! | Bit 54
	bool displayBit55;				// Display show TEXT "TIME" small I belive	| Bit 55 
	bool displayBit56;				// Display show TEXT "SET" small I belive	| Bit 56
	bool displayBit57;				// Still unknown functionality, if at all used! | Bit 57
	bool displayBit58; 				// Still unknown functionality, if at all used! | Bit 58
	bool StandardMode;                 	   	// Standard mode activated or not 		| Bit 59
	bool EcoMode;                   	      	// Eco mode mode activated or not 		| Bit 60 
	bool displayBit61;				// Display PM under TIME menu			| Bit 61
	bool displayBit62;				// Display SET under Prog/mode menu 		| Bit 62
	bool displayBit63;				// Display AM under TIME menu			| Bit 63
	bool TempDown;					// Temp DOWN 					| Bit 64
	bool displayBit65;				// Menu2? SLP ? (Time button (0 on | 1 off ?))	| Bit 65
	bool displayBit66;				// Still unknown functionality, if at all used! | Bit 66 
	bool displayBit67;				// Still unknown functionality, if at all used! | Bit 67
	bool displayBit68;				// Still unknown functionality, if at all used! | Bit 68
	bool displayBit69;				// Still unknown functionality, if at all used! | Bit 69
	bool displayBit70;				// Still unknown functionality, if at all used! | Bit 70
	bool displayBit71;				// Still unknown functionality, if at all used! | Bit 71

	
	

	static bool displayDataBufferOverflow;		
	
	// Write button data to control unit  
	static bool writeDisplayData;            	// If something should be written to button data line  
	static bool writeButtonUp;
	static bool writeButtonDown;
	static bool writeTempUp;
	static bool writeTempDown;
	static bool writeLights;
	static bool writePump1;
	static bool writePump2;			
	static bool writeBlower;
	static bool writeSTDMode;			
	static bool writeEcoMode;

  private:
	
	static void clockPinInterrupt();
	void decodeDisplayData();
	String lockup_LCD_character(int LCD_character);
	int LCD_segment_1;
	int LCD_segment_2;
	int LCD_segment_3;
	int LCD_segment_4;
	String LCD_display_1;
	String LCD_display_2;
	String LCD_display_3;
	String LCD_display_4;  
	static byte displayDataBuffer[displayDataBufferSize]; 		// Array of display data measurements 
	static unsigned long clockInterruptTime;
	static int clockBitCounter;               		 	// Counter of pulses within a cycle
	static byte dataIndex; 									
	static bool displayDataBufferReady;            			// Is buffer available to be decoded
	static byte clockPin;
    	static byte displayPin;
    	static byte buttonPin;

	int updateTempDirection;
	int updateTempButtonPresses;
	unsigned long buttonPressTimerPrevMillis; 
};

  
#endif  // Balboa_GS_Interface_h