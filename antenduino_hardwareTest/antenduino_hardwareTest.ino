#include "math.h"

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

int REFLECT_MIN = 255; // as far away as possible 
int REFLECT_MAX = 0; // as close as possible



//#define SPEED 255
// times in milliseconds for driving forward/backward
int forwardTime = 0;  
int backTime = 0;

int Speed = 230;
double Position = 0;

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
  
  while(true)
  {
   // int dist = analogRead(0);
  
    double destPos = getDestination();
    if(destPos < 0) 
       destPos = 0;
    updateMotor(destPos);
  }
  
  calibrate();
  printStats();  
}

// measure how long it takes to extend and retract
void calibrate()
{
  int counter = 0;
    
  analogWrite(PIN_PWM_A, Speed);
  digitalWrite(PIN_DIR_A, LOW); // extend 
  delay(2000); // go forward for 2 seconds
  
  digitalWrite(PIN_DIR_A, HIGH); // retract until limit trigger
  while(!digitalRead(PIN_LIMITA_B)) 
  {
    delay(1);
    counter++;
    Serial.println(analogRead(0));
  }
  digitalWrite(PIN_PWM_A, 0);
  
  counter = 0;
  digitalWrite(PIN_DIR_A, LOW); // extend 
  analogWrite(PIN_PWM_A, Speed);
  while(!digitalRead(PIN_LIMITA_A))
  {
    Serial.println(analogRead(0));
    counter++;
    delay(1);
  }
  forwardTime = counter;
  digitalWrite(PIN_PWM_A, 0);  
  
  counter = 0;
  digitalWrite(PIN_DIR_A, HIGH); // retract until tripped  
  digitalWrite(PIN_PWM_A, Speed);  
   while(!digitalRead(PIN_LIMITA_B))
  {
    Serial.println(analogRead(0));
    delay(1);
    counter++;
  }
 
  Position = 0;
  backTime = counter;
  digitalWrite(PIN_PWM_A, 0);
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

void printSensors()
{
  Serial.print("Reflectance: ");
  Serial.println(analogRead(0));
}

// returns a number, 0 to 100% for antenna extension based on reflectance 
double getDestination()
{
  double destPos = 0; // destination postiion
  double m = 100.0/(REFLECT_MAX - REFLECT_MIN);
  double b = -1.0 * (m * REFLECT_MIN);

  for(int i = 0; i < 10; i++)
  {
    destPos += analogRead(0);
  }
  destPos /= 10;
  
  if(destPos < REFLECT_MIN) REFLECT_MIN = destPos;
  else if (destPos > REFLECT_MAX) REFLECT_MAX = destPos;
  
  destPos = (m * destPos) + b;
  
  
  Serial.print(m);
    Serial.print(",");
    Serial.print(b);
    Serial.print(",");
    Serial.print(analogRead(0));
    Serial.print(",");
    Serial.print(destPos);
    Serial.print(",");
    Serial.println(Position);
  return destPos;
}

void updateMotor(double destPos)
{
  Serial.println("updating motors");
   // do nothing
   if(abs(destPos-Position) < 2)
   {  
     analogWrite(PIN_PWM_A, 0);
     return; 
   }
   
   // extend
   else if (destPos > Position)
   {
     digitalWrite(PIN_DIR_A, LOW); // extend 
     analogWrite(PIN_PWM_A, Speed);
     //Serial.println(Speed);

      
        if(digitalRead(PIN_LIMITA_A))
        {
          Position = 100;
          destPos = 100;
          analogWrite(PIN_PWM_A, 0);
          return;
        }
        if(Position >= 100)        
        {
          destPos = Position = 100;
          analogWrite(PIN_PWM_A, 0);        
          return;
        }
        else if (Position >= destPos)
          delay(100);
          Serial.print(forwardTime);
          Serial.print(",");
          Position += 100.0 / (forwardTime/100);
          Serial.println(Position);
          return;       
   }
   
   // retract
   else if (destPos < Position)
   {
      digitalWrite(PIN_DIR_A, HIGH); // retract
      analogWrite(PIN_PWM_A, Speed);
     
        if(digitalRead(PIN_LIMITA_B))
        {
          Position = 0;
          destPos = 0;
          analogWrite(PIN_PWM_A, 0);
          return;
        }
        if(Position <= 0)
        {
          analogWrite(PIN_PWM_A, 0);        
          return;
        }
        else if (Position > destPos)
        {
          delay(100);
          Position -= 100.0 / (backTime/100);
          return;
        }
      
      //analogWrite(PIN_PWM_A, 0);
   }

}
