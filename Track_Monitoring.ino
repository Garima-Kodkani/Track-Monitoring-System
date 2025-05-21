//For Tilt sensor
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>

//For SD Card
#include <SPI.h>
#include <SD.h>

//FOR LCD
#include <LiquidCrystal.h>//FOR LCD DISPLAY

//FOR PUSH BUTTONS
#define startbutton A1
#define stopbutton A0
int start_new, stop_new;
int flag;

//FOR ENCODER
#define ClockPin 2
#define DataPin 3 
#define readA bitRead(PIND,2)
#define readB bitRead(PIND,3)
volatile long count = 0;
volatile byte LastAB;
long lastCtr;
unsigned long SpareCycles;

void Encoder(bool A) {
  (readB == A)  ? count++ : count--;
   LastAB = A<<1 | readB;
}

//For LCD
const int rs = 9, en = 8, d4 = 7, d5 = 6, d6 = 5, d7 = 4;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//FOR SD CARD
/*
SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 10 (for MKRZero SD: SDCARD_SS_PIN)
Tilt sensor attached as follows:
 **SDA--> pin A4
 **SCL--> pin A5
 **VCC--> 3.3V
*/

File myFile;

Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified();
float Pi = 3.14159;
float angle=0;
float cross_H;

//FOR DISPLACEMENT SENSOR
int potValue;
int pot_pin = A6;
float x=0;//potValue converted to displacement
float displacement=0;//calibrated value of displacement

void setup() 
{
  Serial.begin(9600);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  // set up the LCD's number of columns and rows:
  lcd.begin(20, 4);
  lcd.setCursor(0,1);
  lcd.print("Distance: 0");
  lcd.setCursor(14,1);
  lcd.print("mm");
  lcd.setCursor(0,2);
  lcd.print("Gauge:    0");
  lcd.setCursor(14,2);
  lcd.print("mm");
  lcd.setCursor(0,3);
  //print cross level
  lcd.print("Cant:     0");
  lcd.setCursor(14,3);
  lcd.print("mm");

  //FOR ENCODER
  pinMode(ClockPin, INPUT);
  pinMode(DataPin, INPUT);

  //2 step count
  attachInterrupt(digitalPinToInterrupt(ClockPin), [] {Encoder( readA);}, CHANGE);
 
  if (!SD.begin(10)) 
  {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");

  myFile = SD.open("test.txt", FILE_WRITE);

  if (myFile) 
  {
    myFile.print("DISTANCE = ");
    myFile.print("\t\tGAUGE: ");
    myFile.print("\t\tCANT: ");
    // close the file:
    myFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }

  // re-open the file for reading:
  myFile = SD.open("test.txt");
  if (myFile) {
    Serial.println("test.txt:");
    myFile.close();
  } 
  else 
  {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
 
  //FOR TILT SENSOR
  if(!accel.begin())
  {
    Serial.println("No valid sensor found");
    while(1);
  }
  Serial.println("void setup close");
}

void loop() 
{
  flag = 0;
  while(flag!=2)
  {
    start_new = digitalRead(A1);
    if(start_new == 1)
    {
      flag =1;
    }

    if(flag == 1)
    {
      lcd.setCursor(0,0);
      lcd.print("Recording started! ");
      //FOR ENCODER
      long Counter;
      SpareCycles++;
      noInterrupts ();
      Counter = count;
      interrupts ();
      if ((lastCtr != Counter) & (SpareCycles > 10)) 
      {
        SpareCycles = 0;
      }
      lastCtr = Counter;
      int distance = Counter/0.2;

      //LCD PRINTS DISTANCE
      lcd.setCursor(11,1);
      lcd.print("   ");
      lcd.setCursor(10,1);
      lcd.print(distance);
      lcd.setCursor(14,1);
      lcd.print("mm");
 
      //FOR DISPLACEMENT SENSOR
      potValue = analogRead(pot_pin);//Reading output value of pot
      x = (50./1023.)*potValue; //converting potValue to displacement
  
      //Calibrating the displacement
      if(x==23.17)
      {
        displacement=0;//At sleeper 1
      }
      else
      {
        displacement = x-23.17;
      }

      //print track gauge
      lcd.setCursor(8,2);
      lcd.print("    ");
      lcd.setCursor(7,2);
      lcd.print(displacement);
      lcd.setCursor(14,2);
      lcd.print("mm");

      //FOR TILT SENSOR
      sensors_event_t event;
      accel.getEvent(&event);
      //READING X_OUT
      float X_out = event.acceleration.x/256;
      if(0.0005<=X_out && X_out<=0.0009)
      {
        X_out=0;
      }
      else if(0.0392>=X_out && X_out>= 0.0385)
      {
        X_out = 0.0392;
      }
      //CONVERTING X_out TO ANGLE
      angle = (90.00/0.0392)*X_out;// in terms of theta
      
      if(angle <= 4.22 && angle >= 0.35)
      {
        angle = 0;
      }
      if(angle <= 92.17 && angle >= 90.06)
      {
        angle = 90;
      }
      float angle_rad = (angle*Pi)/180;//converting theta to radians

      //CALCULATING CROSS HEIGHT
      float cross_H = tan(angle_rad)*272;
      if(cross_H>=116.0 && cross_H<=122.0)
      {
        cross_H = 116.0;
      }
      float cross_H_N = cross_H*(100.0/119.0);
      //PRINTING CROSS HEIGHT NEW
      Serial.print("Cross Height: ");
      Serial.println(cross_H_N);
   
      lcd.setCursor(8,3);
      lcd.print("     ");
      lcd.setCursor(7,3);
      lcd.print(cross_H_N);
      lcd.setCursor(14,3);
      lcd.print("mm");
   
      //SD CARD
      myFile = SD.open("test.txt", FILE_WRITE);

      // if the file opened okay, write to it:
      if (myFile) 
      {
        myFile.print(distance);
        myFile.print("\t\t   ");
        myFile.print(displacement);
        myFile.print("\t\t ");
        myFile.println(cross_H_N);
        //myFile.print("mm");

        // close the file:
        myFile.close();
      }
    }
    stop_new = digitalRead(A0);
    if(stop_new == 1)
    {
      flag = 2;
    }
  }
  if(flag == 2)
  {
    lcd.setCursor(0,0);
    lcd.print("Not Recording!");
    myFile.close();
  }
}