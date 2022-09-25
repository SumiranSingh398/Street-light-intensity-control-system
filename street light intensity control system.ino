#include <Wire.h>
#include <LiquidCrystal.h>
#include "RTClib.h"
#define ON  0
#define OFF 1
DateTime now;

RTC_DS3231 rtc;
LiquidCrystal lcd(8, 7, 6, 5, 4, 3); // (rs, e, d4, d5, d6, d7)

const int buttonPin = 2;
const int led=11;
int nob = A3;
int val = 0;
int val1 = 0;
int path=1;
int a=1;
int previousState = HIGH;
unsigned int previousPress;
volatile int buttonFlag;
int buttonDebounce = 20;

int on_hour=22;
int on_minute=45;
int on_second=15;

int off_hour=22;
int off_minute=45;
int off_second=5;

int c_hour=0;
int c_minute=0;
int c_second=0;

int onOrOffFlag = ON;

void showDate(void);
void showTime(void);
void showDay(void); 

void loadHandler(int, int , int , int , int , int ,  int , int , int );

typedef struct userTime 
{
    int temp_hour;
    int temp_minute;
    int temp_second;
}userTime_t;
unsigned char checkLessThanOrEqual(userTime_t , userTime_t);


void setup ()
{
  Serial.begin(9600);
  lcd.begin(16,2);
  
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(led,OUTPUT);
  attachInterrupt(digitalPinToInterrupt(buttonPin), button_ISR, CHANGE);
  
  if (! rtc.begin()) 
  {
    Serial.println("Couldn't find RTC Module");
    while (1);
  }

  if (rtc.lostPower()) 
  {
    Serial.println("RTC lost power, lets set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
        
}

void loop () 
{
 if(path)
  {
    if(a==1)
    {
       lcd.setCursor(0,0);
        lcd.print("       RTC      ");
        lcd.setCursor(0,1);
        lcd.print("     MODE ON    ");
        delay(2000);
        a=0;
    }
    now = rtc.now();
    showTime();
    c_hour=now.hour();
    c_minute=now.minute();
    c_second=now.second();
    loadHandler( on_hour,  on_minute,  on_second,  off_hour,  off_minute,  off_second,  c_hour,  c_minute,  c_second);
    delay(1000);
  }
 else
  {
       if(a==0)
    {
       lcd.setCursor(0,0);
        lcd.print("       LDR      ");
        lcd.setCursor(0,1);
        lcd.print("     MODE ON    ");
        delay(2000);
        a=1;
    }
       val = analogRead(nob);    
       if(val>300 && val<450)
       {
        lcd.setCursor(0,0);
        lcd.print("       30%      ");
        lcd.setCursor(0,1);
        lcd.print("   Brightness   ");
        analogWrite(led, 400);
       }
       else if(val>450 && val<550)
       {
        lcd.setCursor(0,0);
        lcd.print("       60%      ");
        lcd.setCursor(0,1);
        lcd.print("   Brightness   ");
        analogWrite(led, 600);
       }
       else if(val>550 && val<600)
       {
        lcd.setCursor(0,0);
        lcd.print("      100%      ");;
        lcd.setCursor(0,1);
        lcd.print("   Brightness   ");
        analogWrite(led, 1023);
       }
       else if(val<300)
       {
        lcd.setCursor(0,0);
        lcd.print("       0%      ");
        lcd.setCursor(0,1);
        lcd.print("   Brightness   ");
        analogWrite(led, 0);
       }
   }
}
   

 void showTime()
 {
  lcd.setCursor(0,0);
  lcd.print("   Time:");
  lcd.print(now.hour());
  lcd.print(':');
  lcd.print(now.minute());
  lcd.print(':');
  lcd.print(now.second());
  lcd.print("    ");
 } 


void button_ISR()
{
  buttonFlag = 1;
  if((millis() - previousPress) > buttonDebounce && buttonFlag)
  {
    previousPress = millis();
    if(digitalRead(buttonPin) == LOW && previousState == HIGH)
    {
      path =! path;
      previousState = LOW;
    }
    
    else if(digitalRead(buttonPin) == HIGH && previousState == LOW)
    {
      previousState = HIGH;
    }
    buttonFlag = 0;
  }
}

unsigned char checkLessThanOrEqual(userTime_t a, userTime_t b)
{
    if(a.temp_hour < b.temp_hour)
        return true;
    else
    {
        if ((a.temp_hour == b.temp_hour) && (a.temp_minute < b.temp_minute)) 
        {
            return true;
        }
        else 
        {
            if(a.temp_hour > b.temp_hour)
                return false;
            else 
            {
                if((a.temp_minute == b.temp_minute) && (a.temp_second < b.temp_second)) 
                {
                    return true;
                }
                else 
                {
                    if(a.temp_minute > b.temp_minute)
                        return false;
                    else 
                    {
                        if(a.temp_second == b.temp_second) 
                        {
                            return true;
                        }
                        else 
                        {
                            return false;
                        }
                    }
                }
            }
        }
    }
}

void loadHandler(int onTimeHr, int onTimeMin, int onTimeSec, int offTimeHr, int offTimeMin, int offTimeSec,  int rtcTimeHr, int rtcTimeMin, int rtcTimeSec)
{

    userTime_t in1 = {onTimeHr, onTimeMin, onTimeSec}, in2 = {offTimeHr, offTimeMin, offTimeSec}, rtc_hr = {rtcTimeHr, rtcTimeMin, rtcTimeSec}, a = {}, b = {};

    if(checkLessThanOrEqual(in1, in2)) 
    {
        onOrOffFlag = ON;
        memcpy(&a, &in1, sizeof(userTime_t));
        memcpy(&b, &in2, sizeof(userTime_t));
    }
    else 
    {
        onOrOffFlag = OFF;
        memcpy(&a, &in2, sizeof(userTime_t));
        memcpy(&b, &in1, sizeof(userTime_t));

    }
   
    if((checkLessThanOrEqual(a, rtc_hr)) && (checkLessThanOrEqual(rtc_hr, b))) 
    {
        if(onOrOffFlag == ON) 
        {
            // Switch on the load
            digitalWrite(led,HIGH);
            lcd.setCursor(0,1);
            lcd.print("OffTime:");
            lcd.print(off_hour);
            lcd.print(':');
            lcd.print(off_minute);
            lcd.print(':');
            lcd.print(off_second);
           
            
        }
        else
        {
            // Switch off the load
             digitalWrite(led,LOW);
            lcd.setCursor(0,1);
            lcd.print(" OnTime:");
            lcd.print(on_hour);
            lcd.print(':');
            lcd.print(on_minute);
            lcd.print(':');
            lcd.print(on_second);
            
        }
    }
    else 
    {
        if(onOrOffFlag == ON) 
        {
            // Switch off the load
            digitalWrite(led,LOW);
            lcd.setCursor(0,1);
            lcd.print(" OnTime:");
            lcd.print(on_hour);
            lcd.print(':');
            lcd.print(on_minute);
            lcd.print(':');
            lcd.print(on_second);
        }
        else 
        {
            // Switch on the load
            digitalWrite(led,HIGH);
            lcd.setCursor(0,1);
            lcd.print("OffTime:");
            lcd.print(off_hour);
            lcd.print(':');
            lcd.print(off_minute);
            lcd.print(':');
            lcd.print(off_second);
        }
    }
}
