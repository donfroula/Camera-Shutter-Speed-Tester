/*
Shutter Speed Tester
Donald R Froula, 2021

An Arduino-based shutter speed tester

This program uses the Timer 1 oomparator input interrupt capture mode of an AT328p-based Arduino to measure shutter speed. A flicker-free continuous light is directed 
toward one side of the shutter. The photodiode is positioned on the other side of the shutter such that the photodiode is exposed to light only when the shitter is 
fully open. For leaf shutters, open the f/stop control fully and position the photodiode near the periphery of the shutter. This avoids inaccuracies in the readings 
at higher speeds due to the time needed to open and close the shutter, which is not part of the calibrated leaf shutter speed. Focal plane shutters are not subject 
to this distortion. The photodiode and detector are quite sensitive. The test should be conducted in dim room light. Some adjustment of light intensity through distance 
of the source to the shutter may be needed until reliable results are obtained.

Best results are obtained using a photodiode as sensor rather than a phototransistor. Be sure to wire the photodiode with the cathode towards the +5 volt rail 
(reverse biased) as the photodiode is run in photoconductive mode where the reverse-bias current is linear to the light falling on the device. Note the 
schematic shows a phototransistor, rather than a photodiode. Testing indicates that a photodiode works better under varying illumination than a phototransistor.

The voltage divider at pin D7 sets the comparator threshold voltage, 152 millivolts in this case, which is quite sensitive. No A to D conversion is 
necessary when using comparator mode, making sensing very fast and accurate.

The fastest testable shutter speed is limited in software to just less than 1/2000 second. At this speed, inaccuracy is limited to 1% due to the 16 microsend 
tick of Timer 1. Accuracy is better as measured shutter speed increases.

The slowest testable shutter speed is just over 1 second.

The speed of each test is displayed as a conventional fraction as well as in microseconds for additional accuracy.

Error conditions (pulse too short, pulse too long (Timer 1 overflow))are displayed.

Measurements are limited to once every 1.25 seconds to limit bounce and spurious readings.

Accuracy with crystal-controlled Arduinos or Atmega 328p is excellent. Arduinos using resonators may be less accurate and/or temperature stable. 
Accuracy of the circuit and code was checked with a calibrated square wave generator.

WHEN USING ARDUINO UNO USE , AIN0 (D6) as signal in and AIN1 (D7)as Reference level (voltage divider) in.
PINOUT CONNECTIONS

D6 Photodiode signal
D7 Reference Voltage
*/


#include <LiquidCrystal.h>

volatile uint8_t overflows = 0;

volatile uint8_t flash = 0;

volatile unsigned int timercount;

unsigned long period = 0;
float time = 0;

// (C) custom LCD character
byte newCharCopyright[8] = {
 B11111,
 B10001,
 B10101,
 B10111,
 B10101,
 B10001,
 B11111,
 B00000
};

// Custom Greek "mu" for microsecond display.
byte newMicro[8] = {
  B00000,
  B00000,
  B00000,
  B10010,
  B10010,
  B10010,
  B11100,
  B10000
};

/* 16 x 2 LCD wiring
LCD RS pin to digital pin 19
LCD Enable pin to digital pin 8
LCD D4 pin to digital pin 17
LCD D5 pin to digital pin 16
LCD D6 pin to digital pin 15
LCD D7 pin to digital pin 14
*/

//Define Arduino pins for 16x2 LCD module
const int rs = 19, en = 18, d4 = 17, d5 = 16, d6 = 15, d7 = 14;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
    
    //Disable interrupts while we're fiddling with settings.
    cli();
    
    //Timer1 has already been configured in wiring.c, but we want a different speed.
    
    //Mode 0: normal mode, no port outputs. 
    TCCR1A = 0;
    
    //This sets the clock to 256 prescale (16MHz), 16 microseconds/tick
    TCCR1B = _BV(CS12);
    
    //Trigger on rising edge
    TCCR1B |=  _BV(ICES1);
	
	//Enable input capture noise canceller
	TCCR1B |= (1 << ICNC1); 
  
    //Enable analog comparator input capture.
    ACSR = _BV(ACIC);

    TIFR1 |= _BV(ICF1);  //Clear input capture interrupt flag
 
    //Enable input capture interrupts
    TIMSK1  |= _BV(ICIE1);  

    //Turn off digital input on the comparator negative input
    DIDR1 |= _BV(AIN0D);

    //Enable interrupts
    sei(); 
    
    lcd.begin(16, 2); //Initialize the LCD for a 16x2 display.
    lcd.createChar(0, newCharCopyright);
    lcd.createChar(1, newMicro);
    lcd.noCursor(); //Turn off the cursor
    lcd.clear();
    lcd.setCursor(0, 0); //Set cursor at upper left of display
    //Print the brag screen
    lcd.write(byte(0)); //Print copyright character
    lcd.print("2018"); 
    lcd.setCursor(0, 1);
    lcd.print("Don Froula");
    //tone(11, 1500, 150);
    delay(2000);
    lcd.clear();
    lcd.print("Shutter"); 
    lcd.setCursor(0, 1);
    lcd.print("Speed Tester");
    tone(11, 1500, 150);
}

void loop() {

    if (overflows) {               //Rising pulse with no falling pulse, resulting in timer overflow and no event detection
	      tone(11, 250, 500);
          lcd.clear(); 
          lcd.print("Please");
          lcd.setCursor(0, 1);
          lcd.print("try again ");
		  lcd.print("1");          //Error code 1 for overflow with no falling edge (time > 1.05 seconds)
		  
		  delay(1250);             //Wait 1.25 seconds befor re-enabling detection
		
          flash = 0;               //Clear flash flag for next reading         
          overflows = 0;           //Clear the timer1 overflow count
          TCCR1B |= _BV(ICES1);    //We're triggering on fall. Change to rise.
          TIFR1 |= _BV(ICF1);      //Clear input capture interrupt flag
          TIMSK1  |= _BV(ICIE1);   //Re-enable input capture interrupts
		  }
	

	if(flash == 1) {                               //Flash event detected
		if ((timercount <= 29) || overflows) {     //Error condition. Pulse too short (< 1/2000 sec) or timer overflow
		  tone(11, 250, 500);
          lcd.clear(); 
          lcd.print("Please");
          lcd.setCursor(0, 1);
          lcd.print("try again ");
		  if (overflows) {lcd.print("2");}         //Error code 2 for timer overflow if flash detected (rare)
		  if (timercount <= 31) {lcd.print("3");}  //Error code 3 for time ahorter than 1/2000 second limit (512 microseconds)
		  }
		  
		  else if (timercount > 29) {   //Valid event
		    period = (16 * (unsigned long)timercount);       //Period in microseconds is stored timer count * 16 microseconds/count
            time = ((float)period / 1000000);                //Calculate the time in seconds
			
            //Display the result in milliseconds for accuracy to .000016 second, as well as less accurate fractional conventional timing
            tone(11, 800, 150);                    //Play valid data beep
            lcd.clear();                           //Clear the LCD
            lcd.setCursor(0, 0);                   //Move cursor to start of second line
            if ((time < .5) && (time > 0)){        //Only format as a fraction if time is less than 1/2 second
              lcd.print("1/");                     //Display numerator
              lcd.print(1/time, 0);                //Display the denominator with no decimal portion
              }
              else {lcd.print(time, 2);}           //If time is >= 1/2 second, display time as a number to two decimal places
           lcd.print(" Sec");                      //Display unit
           lcd.setCursor(0, 1);                    //Position the cursor
           lcd.print(period);                      //Display the time in microseconds
           lcd.write(byte(1));                     //Print micro character
           lcd.print("S");                         //Display time unit
		  }
		
		delay(1250);             //Wait 1.25 seconds befor re-enabling detection
		
        flash = 0;               //Clear flash flag for next reading         
        overflows = 0;           //Clear the timer1 overflow count
        TCCR1B |= _BV(ICES1);    //We're triggering on fall. Change to rise.
        TIFR1 |= _BV(ICF1);      //Clear input capture interrupt flag
        TIMSK1  |= _BV(ICIE1);   //Re-enable input capture interrupts
		}
	}
		

// Input capture interrupt - Triggers on rising or falling edge of input around value of comparator ref voltage on pin A5
ISR(TIMER1_CAPT_vect) {   
    if(bit_is_set(TCCR1B, ICES1)) {     //Rising edge detected
		TCNT1 -= ICR1;                  //Reset counter
        TCCR1B &= ~_BV(ICES1);          //We're currently triggering on rise. Change to fall
		// Enable timer1 overflow interrupt(TOIE1) (Count greater than 1.04 second limit)
        TIFR1 = _BV(TOV1);              //Clear overflow interrupt flag
        TIMSK1 |= _BV(TOIE1) ;          // Enable timer1 overflow interrupt(TOIE1)
    } else {
	    timercount = ICR1;              //Save stored value of input capture register
	    TIMSK1  &= ~_BV(ICIE1);         //Disable additional input capture interrupts
        TIMSK1 &= ~_BV(TOIE1) ;         // Disable timer1 overflow interrupt(TOIE1)
        flash = 1;                      //Set flag to process readings in main loop
    }   
}

ISR (TIMER1_OVF_vect)    // Timer1 overflow ISR
{
  overflows++;  //Just increment the overflow count (Count greater than 1.04 second limit)
}
