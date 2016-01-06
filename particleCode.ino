unsigned long oldTime;
volatile unsigned int WaterPulseCount = 0;

// add a global variable for the accumulated pulse count
unsigned int accumPulseCount = 0;

// and a helper variable for the hour timing
unsigned long msStartOfHour = 0;

// conversion from pps to litres
const float pulsesPerLiter = 450;

// Particle Digial Pin D3 (D2 did not work)
#define WATER_SENSOR_PIN    D3

// The decimal is shifted left with * 100 (so xx.yy becomes xxyy)
float ounces = 1 * 33.8;
char ShaveButter[6];

//-----------------------------------------------------------------------------
// Water Sensor interrupts
//-----------------------------------------------------------------------------
void WaterPulseCounter(void)
{
  // Increment the water pulse counter
  WaterPulseCount++;
}

void setup()
{
  Serial.begin(9600);
  
  Particle.variable("ShaveButter", ShaveButter, STRING);

  // interrupt vector (water flow sensor) for FALLING edge interrupt
  pinMode(WATER_SENSOR_PIN, INPUT);
  attachInterrupt(WATER_SENSOR_PIN, WaterPulseCounter, FALLING) ;
  oldTime = millis();
}

void loop()
{
  unsigned long t;
  static unsigned int pc;

  t = (millis() - oldTime);
  if(t >= 1000)         // Only process counters once per second
  {
      oldTime = millis();                // Reset base delay time
      detachInterrupt (WATER_SENSOR_PIN);    // Disable water flow interrupt to read value
      pc = WaterPulseCount;
      attachInterrupt(WATER_SENSOR_PIN, WaterPulseCounter, FALLING);
      //Calculate litres and adjust for 1 sec offset, if any
      ounces = (pc / pulsesPerLiter) * (t / 1000);
      accumPulseCount += pc;  // accumulate the readings

    sprintf(ShaveButter, "%4.3f", ounces);
    
    Particle.publish("shaveButterLow", "Looks like your Shave Butter is running low. Wanna purchase more?", 60, PRIVATE);
    
    Serial.print(pc);
    Serial.print(", oz= ");
    Serial.println(ounces);
    
  }
}
