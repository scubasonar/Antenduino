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

int REFLECT_MIN = 80; // as far away as possible 
int REFLECT_MAX = 200; // as close as possible



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
  analogWrite(PIN_PWM_B, 0);
  bool driving = false;
  destPos = getDestination();
  
   destPos = getDestination();
   Serial.print("Destionation: ");
   Serial.print(destPos);
   Serial.print(", Current: " );
   Serial.println(Position);
  
  
  // --------- begin zeroing section
  // this section makes sure we fully retract when idle
  if((destPos == 0) && (Position <= 3))
  {
    digitalWrite(PIN_DIR_A, LOW); // retract 
    digitalWrite(PIN_DIR_B, HIGH); // retract 
    bool drive = false;
    
    Serial.println("I need to be at 0!");
    // no limit triggered
    if(!digitalRead(PIN_LIMITA_B))
    {
      analogWrite(PIN_PWM_A, Speed);             
      drive = true;
    }       
      
    if(!digitalRead(PIN_LIMITB_B))
    {
      analogWrite(PIN_PWM_B, Speed);
      drive = true;
    }  
      
    if(drive)
    {
      if(Position > 0)
        Position -= .8;
       delay(4 * backTime / 1000.0);      
      analogWrite(PIN_PWM_A, 0);
      analogWrite(PIN_PWM_B, 0);                  
    }
  } // ------------- end zeroing section
  
  while(abs(destPos-Position) > 2)
  { 
    destPos = getDestination();
    Serial.print("Destionation: ");
    Serial.print(destPos);
    Serial.print(", Current: " );
    Serial.println(Position);
    
   /* if((Position > 0) && (Position <= 4) && (destPos == 0))
    {
      Serial.println("Special retraction!");
      digitalWrite(PIN_DIR_A, LOW); // retract 
      digitalWrite(PIN_DIR_B, HIGH); // retract 
      analogWrite(PIN_PWM_A, Speed);
      analogWrite(PIN_PWM_B, Speed);

      delay(1000);
      Position -=1;
      analogWrite(PIN_PWM_A, 0);
      analogWrite(PIN_PWM_B, 0);  
    }*/
  
    if (destPos > Position)
    {
      digitalWrite(PIN_DIR_A, HIGH); // extend 
      digitalWrite(PIN_DIR_B, LOW); // extend 
      analogWrite(PIN_PWM_A, Speed);
      analogWrite(PIN_PWM_B, Speed);

      // antenna A max limit switch
      if(!digitalRead(PIN_LIMITA_A))
      {
        Position += 0.2;
        delay(forwardTime / 1000.0);
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
        delay(100);
      }        
    }
      
    else if(destPos < Position)
    {
      digitalWrite(PIN_DIR_A, LOW); // retract 
      digitalWrite(PIN_DIR_B, HIGH); // retract 
      analogWrite(PIN_PWM_A, Speed);
      analogWrite(PIN_PWM_B, Speed);

      if(!digitalRead(PIN_LIMITA_B))
      {         
        Position -= .2;
        delay(backTime / 1000.0);
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
        delay(100);
      }        
    }  
  }
  
}

// measure how long it takes to extend and retract
void calibrate()
{
  int counter = 0;

  analogWrite(PIN_PWM_A, Speed);
  digitalWrite(PIN_DIR_A, HIGH); // extend 
  delay(2000); // go forward for 2 seconds

  digitalWrite(PIN_DIR_A, LOW); // retract until limit trigger
  while(!digitalRead(PIN_LIMITA_B)) 
  {
    delay(1);
    counter++;
    Serial.println(analogRead(0));
  }
  digitalWrite(PIN_PWM_A, 0);

  counter = 0;
  digitalWrite(PIN_DIR_A, HIGH); // extend 
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
  digitalWrite(PIN_DIR_A, LOW); // retract until tripped  
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
  double m = 100.0/(REFLECT_MAX-REFLECT_MIN);
  double b = -1.0 * (m*REFLECT_MIN);

  for(unsigned int i = 0; i < 100; i++)
  {
    destPos += analogRead(0);
  }
  destPos /= 100;

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



