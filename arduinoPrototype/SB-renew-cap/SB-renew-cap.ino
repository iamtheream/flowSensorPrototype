// Connect the red wire to +5V, 
// the black wire to common ground 
// and the yellow sensor wire to pin #2

#include "LiquidCrystal.h"
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

// which pin to use for reading the sensor? can use any pin!
#define FLOWSENSORPIN 5

// count how many pulses!
volatile uint16_t pulses = 0;
// track the state of the pulse pin
volatile uint8_t lastflowpinstate;
// you can try to keep time of how long it is between pulses
volatile uint32_t lastflowratetimer = 0;
// and use that to calculate a flow rate
volatile float flowrate;
// Interrupt is called once a millisecond, looks for any pulses from the sensor!
SIGNAL(TIMER0_COMPA_vect) {
  uint8_t x = digitalRead(FLOWSENSORPIN);
  
  if (x == lastflowpinstate) {
    lastflowratetimer++;
    return; // nothing changed!
  }
  
  if (x == HIGH) {
    //low to high transition!
    pulses++;
  }
  lastflowpinstate = x;
  flowrate = 1000.0;
  flowrate /= lastflowratetimer;  // in hertz
  lastflowratetimer = 0;
}

void useInterrupt(boolean v) {
  if (v) {
    // Timer0 is already used for millis() - we'll just interrupt somewhere
    // in the middle and call the "Compare A" function above
    OCR0A = 0xAF;
    TIMSK0 |= _BV(OCIE0A);
  } else {
    // do not call the interrupt function COMPA anymore
    TIMSK0 &= ~_BV(OCIE0A);
  }
}

void setup() {
   Serial.begin(9600);
   Serial.print("Flow sensor test!");
   lcd.begin(16, 2);
   
   pinMode(FLOWSENSORPIN, INPUT);
   digitalWrite(FLOWSENSORPIN, HIGH);
   lastflowpinstate = digitalRead(FLOWSENSORPIN);
   useInterrupt(true);
}

void loop()
{ 
  lcd.setCursor(0, 0);
  lcd.print("Pulses:"); lcd.print(pulses, DEC);
  lcd.print(" Hz:");
  lcd.print(flowrate);
  //lcd.print(flowrate);
  Serial.print("Freq: "); Serial.println(flowrate);
  Serial.print("Pulses: "); Serial.println(pulses, DEC);
  
  float liters = pulses;
  liters /= 7.5;
  liters /= 60.0;

  float ounces = liters * 33.8;

  Serial.print(liters); Serial.println(" oz");
  lcd.setCursor(0, 1);
  lcd.print(liters); lcd.print(" oz        ");
 
  delay(500);
}
