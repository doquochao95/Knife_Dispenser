#include <AccelStepper.h>
//#include <HardwareSerial.h>
//#define Motorserial Serial2

#define HALFSTEP 8
int Led_output = 5;
//int zero_button = 4;
int sensor_point = 18;
bool lightStatus;
int led = 0;
int fake = 3;
AccelStepper myStepper(HALFSTEP, 13, 14, 12, 27);
// Pins entered in sequence IN1-IN3-IN2-IN4 for proper step sequence

#define RXD2 16
#define TXD2 17

void setup() 
{
  Serial.begin(115200); 
  //Motorserial.begin(9600);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  pinMode(Led_output, OUTPUT);
  //pinMode(zero_button, INPUT);
  pinMode(sensor_point, INPUT);
  digitalWrite(Led_output, LOW);
  myStepper.setMaxSpeed(500.0);
  myStepper.setAcceleration(100.0);
  myStepper.setSpeed(100);
  myStepper.moveTo(0);
  Serial.println("Installing motor parameter");
  int sensorStatus1 = digitalRead(sensor_point);
  int memory1;
  bool flag=false;
  if (sensorStatus1 == HIGH & myStepper.distanceToGo() == 0)
  {
    myStepper.setMaxSpeed(50.0);
    myStepper.setAcceleration(10.0);
    myStepper.setSpeed(5);
    myStepper.moveTo(myStepper.currentPosition()+ 4080);
    while (true)
    {
      int sensorStatus1 = digitalRead(sensor_point);
      if (sensorStatus1 == LOW)
       {
         memory1 = myStepper.currentPosition();
         myStepper.setMaxSpeed(10.0);
         myStepper.setAcceleration(10.0);
         myStepper.setSpeed(10);
         myStepper.stop();
         myStepper.moveTo(memory1);
         flag=true;
         break;
       }
      else
      {
        myStepper.run(); 
      }
    }
  }
  if (sensorStatus1 == LOW & flag == false)
  {
    myStepper.setMaxSpeed(50.0);
    myStepper.setAcceleration(10.0);
    myStepper.setSpeed(5);
    myStepper.moveTo(myStepper.currentPosition()- 4080);
    while (true)
    {
      int sensorStatus1 = digitalRead(sensor_point);
      if (sensorStatus1 == HIGH)
       {
         memory1 = myStepper.currentPosition();
         myStepper.setMaxSpeed(10.0);
         myStepper.setAcceleration(10.0);
         myStepper.setSpeed(10);
         myStepper.stop();
         myStepper.moveTo(memory1 + 1000);
         flag=false;
         //fake=0;
         break;
       }
       else
       {
         myStepper.run(); 
       }
    }
  }
}

void loop() 
{
  int sensorStatus = digitalRead(sensor_point);
  int memory;
  if (Serial2.available())
  {   
    String command = Serial2.readString();
    command.trim();
    //Serial.println(command.charAt(0));
    switch (command.charAt(0))
      {
        case 'z':
        if (sensorStatus == HIGH)
        {
          if (myStepper.distanceToGo() == 0)
          {
            Serial.println("Command: " + String(command));
            myStepper.setMaxSpeed(50.0);
            myStepper.setAcceleration(10.0);
            myStepper.setSpeed(5);
            myStepper.moveTo(myStepper.currentPosition()+ 4080);
            fake = 0;
            break;
          }
          if (myStepper.distanceToGo() > 0)
          {
            Serial.println("Going to zero");
            break;
          }
        }
        if (sensorStatus == LOW)
        {
          Serial.println("Zero already");
          delay(100);
          Serial2.write("SETTING");
          Serial.println("Write: Setting page");
          break;
        }
        case 'r':
          if (myStepper.distanceToGo() == 0)
          {
            Serial.println("Command: " + String(command));
            myStepper.setMaxSpeed(500.0);
            myStepper.setAcceleration(100.0);
            myStepper.setSpeed(100);
            myStepper.moveTo(myStepper.currentPosition()+680);
            fake = 1;
            break;
          }
          if (myStepper.distanceToGo() > 0)
          {
            Serial.println("Rotating");
            break;
          }
        case 'i':
          if (myStepper.distanceToGo() == 0)
          {
            Serial.println("Command: " + String(command));
            int n = 1;
            for ( int i=1; i<=6; i++)
            {
              myStepper.setMaxSpeed(500.0);
              myStepper.setAcceleration(100.0);
              myStepper.setSpeed(100);
              myStepper.moveTo(myStepper.currentPosition()+680);
              fake = 2;
              Serial.println("Time: " +String(n));
              while (1)
              {
                myStepper.run();
                if (myStepper.distanceToGo() == 0)
                {
                  break;
                }
              }
              n++;
              delay(3000);
            }
            break;
          }
        case 'o':
          digitalWrite(Led_output, HIGH);
          Serial.println("Led: On");
          lightStatus = true;
          led = 1;
          break;
        case 'n':
          digitalWrite(Led_output, LOW);
          Serial.println("Led: Off");
          lightStatus = false;
          led = 1;
          break;
        case 's':
          led = 1;
          break;
      }
  }
  if (sensorStatus == LOW & myStepper.distanceToGo() > 0) 
    {
      if (fake==0)
      {
        memory = myStepper.currentPosition();
        myStepper.setMaxSpeed(10.0);
        myStepper.setAcceleration(10.0);
        myStepper.setSpeed(10);
        myStepper.stop();
        myStepper.moveTo(memory);
        Serial.println("Stopping");
        Serial2.write("SETTING");
        Serial.println("Write: Setting page");
        fake=3;
      }
      if (fake==3)
      {
        memory = myStepper.currentPosition();
        myStepper.setMaxSpeed(10.0);
        myStepper.setAcceleration(10.0);
        myStepper.setSpeed(10);
        myStepper.stop();
        myStepper.moveTo(memory);
        Serial.println("Stopping");
      }
    } 
  if ( myStepper.distanceToGo() == 0)
  {
    switch (fake)
    {
      //case 1:
      //  Serial2.write("SCREEN1");
      //  Serial.println("Write: Screen1");
      //  fake=3;
       // break;
      case 2:
        Serial2.write("SETTING");
        Serial.println("Write: Setting page");
        fake=3;
        delay(1000);
        break;
    }
  }
  if (led == 1)
  {
    if (lightStatus == true & fake != 0)
    {
      Serial2.write("ONLED");
      Serial.println("cmd: ONLED");
      led--;
      delay(100);
    }
    if (lightStatus == false & fake != 0)
    {
      Serial2.write("LEDOFF");
      Serial.println("cmd: LEDOFF");
      led--;
      delay(100);
    }
  }
  myStepper.run();
}
