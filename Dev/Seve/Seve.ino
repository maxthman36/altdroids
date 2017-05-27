
#include <Servo.h>

const int RIGHTSENSOR = A0;
const int LEFTSENSOR = A1;
const int INTERNALSENSOR = A2;

Servo myservoRight; 
Servo myservoLeft; // create servo object to control a servo
// twelve servo objects can be created on most boards

int pos = 0;    // variable to store the servo position
const int FORWARD = 0;
const int REVERSE = 1;
const int RIGHT = 2;
const int LEFT = 3;

const double VARIANCE_CONSIDERED_STUCK = .05; // Higher the number the more he thinks he's stuck
const int AMBIENT_LIGHT = 50; //Raise this number if he doesn't recognize opsticals
const double STRAIGHTNESS = .25;//Higher the number, the less he will turn.
const int BRIGHT_LIGHT = 750; //If there is bright light, we will steer away from it if we don't have a ball. 

int rightSensorReading = 0;
int leftSensorReading = 0; 
int SensorReading = 0;
int insideSensorReading = 0;
int baselineInternalReading=0;
bool hasBall = false;

bool possiblyStuck = false;
int previousLeftSensorReading = false;
int previousRightSensorReading = false;
unsigned long timeWindowBegin = 0; //time window from when we think we might be stuck


int dir = FORWARD; //inital Direction

void setup() {
  pinMode(12, OUTPUT);
  digitalWrite(12, HIGH);
  myservoRight.attach(9);
  myservoLeft.attach(10); // attaches the servo on pin 9 to the servo object
  baselineInternalReading = analogRead(INTERNALSENSOR); //Set the baseline where no ball is inserted
  
}

void loop() {

  rightSensorReading = analogRead(RIGHTSENSOR); // Get Values from Light Sensors
  leftSensorReading = analogRead(LEFTSENSOR);
  insideSensorReading = analogRead(INTERNALSENSOR);

  if(insideSensorReading < baselineInternalReading && !withinPercentage(insideSensorReading, baselineInternalReading, .01)) //if light in bay dims, there is a ball in the bay
  hasBall=true;
  else
  hasBall=false;

//hasBall=false; //COMMENT THIS OUT LATER

  //This is where we Decide what the robot will do next after the sensor data is collected

  
  if(leftSensorReading < AMBIENT_LIGHT  || rightSensorReading < AMBIENT_LIGHT) // if collision then reverse for 3 seconds
  {
     dir = REVERSE;
  }
  else
    {
    if(withinPercentage(leftSensorReading, rightSensorReading, STRAIGHTNESS)) //If 30% of smaller value go straight!
    {dir = FORWARD;}
    else if (leftSensorReading > rightSensorReading) //OR check for left and right..
      { //brighter on the Left
        if(hasBall) //Go twords light if it has a ball & away if not!
        dir = LEFT;
        else
        {
          if(leftSensorReading >= BRIGHT_LIGHT)
          dir = RIGHT;
          else
          dir = LEFT;
        }
      }
    else 
      { //brighter on the right
        if(hasBall)
        dir = RIGHT;
        else
        {
          if(rightSensorReading >=BRIGHT_LIGHT)
            dir = LEFT;
          else
          dir = RIGHT;
        }  
      }
    }

  //Check to see if there is the robot is stuck. This is deterimined by seeing if the sensor reading don't change at all for 3 seconds.
  if(possiblyStuck && millis() > timeWindowBegin+3000)
  {
    dir = REVERSE;
  }

  //see if we're stuck
  if (dir == REVERSE) {
    //can't be stuck if we're reversing
    possiblyStuck = false;
  } else {
    //not reversing
    
    if (!possiblyStuck) {
      possiblyStuck = true;
      timeWindowBegin = millis();
      previousLeftSensorReading = leftSensorReading;
      previousRightSensorReading = rightSensorReading;
    } else if (possiblyStuck && (!withinPercentage(leftSensorReading, previousLeftSensorReading, VARIANCE_CONSIDERED_STUCK) || !withinPercentage(rightSensorReading, previousRightSensorReading, VARIANCE_CONSIDERED_STUCK))) {
      possiblyStuck = false;
      timeWindowBegin = millis();
 
 //     possiblyStuck = true;
 //     timeWindowBegin = millis();
 //     previousLeftSensorReading = leftSensorReading;
 //     previousRightSensorReading = rightSensorReading;
    }
  }


//dir = REVERSE; 

  switch (dir)
  {
    case FORWARD:
         myservoLeft.write(180);  
         myservoRight.write(0); 
         delay(10); 
          //delay(100);
    break;
    case REVERSE:
          myservoRight.write(180);  
          myservoLeft.write(0);  
          delay(random(100, 1000));  // Increased delay to 2 seconds to allow bot to get away from any obstacle
          //Rotate backwards left or right, based on a random #.
          if(random(2) == 0)
          {
            myservoRight.write(180);  
            myservoLeft.write(90);  
          }
          else
          {
            myservoRight.write(90);  
            myservoLeft.write(0);  
          }
          delay(random(300, 1500)); //turn for a variable amount
    break;
    case RIGHT:
          myservoRight.write(180);  
          myservoLeft.write(180); 
          delay(200);
    break;
    case LEFT:
          myservoRight.write(90);  
          myservoLeft.write(90);  
          delay(200);
    break;
  }
}

bool withinPercentage(int left, int right, double percentage) //Function to determine if both values are within percentage% of eachother.
{
  // ----------------------- Get Higher Value ---------------- //
  int higher = 0;
  int lower = 0;
  if(right <= left)
  {lower = right; higher = left;} 
  else {lower = left; higher = right;}
  // ------------------------------------------------------ //
  if((higher - lower)/((double)lower) <= percentage)
    {return true;}
  else 
    {return false;}

}



