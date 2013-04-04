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

//#define SPEED 255
// times in milliseconds for driving forward/backward
int forwardTime = 0;  
int backTime = 0;

int motorSpeed = 180;

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
  
  calibrate();
  printStats();
  
  while(true)
  {
    if(Serial.available())
    {
      calibrate();
      printStats();
      Serial.read();
    }
     else
       delay(100);
  }
  
}

// measure how long it takes to extend and retract
void calibrate()
{
  int counter = 0;
    
  analogWrite(PIN_PWM_A, motorSpeed);
  digitalWrite(PIN_DIR_A, LOW); // extend 
  delay(2000); // go forward for 2 seconds
  
  digitalWrite(PIN_DIR_A, HIGH); // retract until limit trigger
  while(!digitalRead(PIN_LIMITA_B)) 
  {
    delay(1);
    counter++;
  }
  Serial.println(counter);
  digitalWrite(PIN_PWM_A, 0);
  
  counter = 0;
  digitalWrite(PIN_DIR_A, LOW); // extend 
  analogWrite(PIN_PWM_A, motorSpeed);
  while(!digitalRead(PIN_LIMITA_A))
  {
    counter++;
    delay(1);
  }
  forwardTime = counter;
  digitalWrite(PIN_PWM_A, 0);  
  Serial.println(counter);
  
  counter = 0;
  digitalWrite(PIN_DIR_A, HIGH); // retract until tripped  
  digitalWrite(PIN_PWM_A, motorSpeed);  
   while(!digitalRead(PIN_LIMITA_B))
  {
    delay(1);
    counter++;
  }
 
  backTime = counter;
  digitalWrite(PIN_PWM_A, 0);
  Serial.println(counter);
}


// print out various status parameters
void printStats()
{
  Serial.print("Backward time: ");
  Serial.print(backTime);
  Serial.print("ms, Forward time: ");
  Serial.print(forwardTime);
  Serial.print("ms");

}
