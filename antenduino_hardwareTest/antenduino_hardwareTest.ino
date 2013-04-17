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

int REFLECT_MIN = 0; // as far away as possible 
int REFLECT_MAX = 500; // as close as possible



//#define SPEED 255
// times in milliseconds for driving forward/backward
int forwardTime = 0;  
int backTime = 0;

int Speed = 255;
double Position = 0;
double destPos = 0;

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


  delay(20000);
  calibrate();
}

// our main tracking loop that gets called over and over!
void loop()
{ 
  analogWrite(PIN_PWM_A, 0);
  bool driving = false;
  destPos = getDestination();

  while(abs(destPos-Position) > 2)
  { 
    if (destPos > Position)
    {
      digitalWrite(PIN_DIR_A, LOW); // extend 
      digitalWrite(PIN_DIR_B, LOW); // extend 
      analogWrite(PIN_PWM_A, Speed);
      analogWrite(PIN_PWM_B, Speed);

      // antenna A max limit switch
      if(!digitalRead(PIN_LIMITA_A))
      {
        Position += 0.1;
        delay(forwardTime / 1000);
        Serial.println(forwardTime);
        Serial.println(backTime);
      }
      else
      {
        Position = 100;
        analogWrite(PIN_PWM_A, 0);
      }

      // antenna B max limit switch
      if(!digitalRead(PIN_LIMITB_A))
      {        
      }
      else
      {         
        analogWrite(PIN_PWM_B, 0);
      }        
    }
      
    else if(destPos < Position)
    {
      digitalWrite(PIN_DIR_A, HIGH); // extend 
      digitalWrite(PIN_DIR_B, HIGH); // extend 
      analogWrite(PIN_PWM_A, Speed);
      analogWrite(PIN_PWM_B, Speed);

      if(!digitalRead(PIN_LIMITA_B))
      {         
        Position -= .1;
        delay(backTime / 1000);
      }
      else
      {
        Position = 0;
        analogWrite(PIN_PWM_A, 0);
      }

      // antenna B bottom limit
      if(!digitalRead(PIN_LIMITB_B))
      {
      }
      else
      {
        analogWrite(PIN_PWM_B, 0);
      }        
    }


    destPos = getDestination();
    Serial.print("Destionation: ");
    Serial.print(destPos);
    Serial.print(", Current: " );
    Serial.println(Position);
  }
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

  // antenna B
  counter = 0;

  analogWrite(PIN_PWM_B, Speed);
  digitalWrite(PIN_DIR_B, LOW); // extend 
  delay(2000); // go forward for 2 seconds

  digitalWrite(PIN_DIR_B, HIGH); // retract until limit trigger
  while(!digitalRead(PIN_LIMITB_B)) 
  {
    delay(1);
    counter++;
    Serial.println(analogRead(0));
  }
  digitalWrite(PIN_PWM_B, 0);

  counter = 0;
  digitalWrite(PIN_DIR_B, LOW); // extend 
  analogWrite(PIN_PWM_B, Speed);
  while(!digitalRead(PIN_LIMITB_A))
  {
    Serial.println(analogRead(0));
    counter++;
    delay(1);
  }
  //forwardTime = counter;
  digitalWrite(PIN_PWM_B, 0);  

  counter = 0;
  digitalWrite(PIN_DIR_B, HIGH); // retract until tripped  
  digitalWrite(PIN_PWM_B, Speed);  
  while(!digitalRead(PIN_LIMITB_B))
  {
    Serial.println(analogRead(0));
    delay(1);
    counter++;
  }

  Position = 0;
  //backTime = counter;
  digitalWrite(PIN_PWM_B, 0);
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
  double m = 100.0/REFLECT_MAX;
  double b = -1.0 * m;

  for(int i = 0; i < 10; i++)
  {
    destPos += analogRead(0);
  }
  destPos /= 10;

  destPos = (m * destPos) + b;

  if(destPos < 0) 
    destPos = 0;
  if(destPos > 100)
    destPos = 100;

  /*Serial.print(m);
   Serial.print(",");
   Serial.print(b);
   Serial.print(",");
   Serial.print(analogRead(0));
   Serial.print(",");
   Serial.print(destPos);
   Serial.print(",");
   Serial.println(Position);
   */
  return destPos;
}



