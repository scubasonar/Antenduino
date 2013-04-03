/* Pins */

#define PIN_PWM_A     3  // motor A speed
#define PIN_DIR_A     12 // motor A direction 
#define PIN_LIMITA_A  6  // red, normally open, connects to ground
#define PIN_LIMITA_B  7  // blue, normally open, connects to ground

#define PIN_PWM_B     11 // controls motor B speed
#define PIN_DIR_B     13 // motor B direction
#define PIN_LIMITB_A  4  // red, normally open, connects to ground
#define PIN_LIMITB_B  5  // blue, normally open, connects to ground

#define PIN_SENSOR_IR 14 // analog 0

// set up the pins
void setup()
{
  pinMode(PIN_PWM_A, OUTPUT);
  pinMode(PIN_DIR_A, OUTPUT);
  analogWrite(PIN_PWM_A, 0);
  
  pinMode(PIN_PWM_B, OUTPUT);
  pinMode(PIN_DIR_B, OUTPUT);
  analogWrite(PIN_PWM_B, 0);
  
  pinMode(PIN_SENSOR_IR, INPUT);
  
  pinMode(PIN_LIMITA_A, INPUT);
  digitalWrite(PIN_LIMITA_A, HIGH);
  pinMode(PIN_LIMITA_B, INPUT);
  digitalWrite(PIN_LIMITA_B, HIGH);
  
  pinMode(PIN_LIMITB_A, INPUT);
  digitalWrite(PIN_LIMITB_A, HIGH);
  pinMode(PIN_LIMITB_B, INPUT);
  digitalWrite(PIN_LIMITB_B, HIGH);
  Serial.begin(9600);
}

void loop()
{
  Serial.println(analogRead(0));
   digitalWrite(PIN_DIR_A, LOW);  //Set motor direction, 1 low, 2 high
  digitalWrite(PIN_DIR_B, LOW);  //Set motor direction, 3 high, 4 low
  

  Serial.println(analogRead(0));
  analogWrite(PIN_PWM_A, 100);  //set both motors to run at 100% duty cycle (fast)
  analogWrite(PIN_PWM_B, 0);
  while(!digitalRead (PIN_LIMITA_B))
  {
    delay(10);
  }
  delay(1000);
}

