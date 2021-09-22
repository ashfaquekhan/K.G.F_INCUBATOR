#define CLK 2
#define DT 3
#define DHT11_PIN 7
//emergency firmware
#define TEMP_RELAY  10 //IN 1
#define FAN_RELAY  9 // IN 2
#define EXHAUST  6 // IN 3
#define RELAYE  5 //IN 4
#define RESET 12 // reset pin
#include <EEPROM.h>
#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>
#include <dht.h>
LiquidCrystal_PCF8574 lcd(0x27);
dht DHT;
unsigned long period = 2000, minut_res =60000*60;
unsigned long time_now = 0,time_res=0;
float temp,hum,tl=37.2,tu=38.2,ex=38.9;
bool line;
bool exhaust=0;
void setup() {
  Wire.begin();
  Wire.beginTransmission(0x27);
  pinMode(CLK,INPUT);
  pinMode(DT,INPUT);
  pinMode(TEMP_RELAY, OUTPUT);
  pinMode(FAN_RELAY, OUTPUT);
  pinMode(EXHAUST, OUTPUT);
  pinMode(RELAYE, OUTPUT);
  digitalWrite(RESET,1);
  delay(200);
  pinMode(RESET,OUTPUT);
  
  lcd.begin(16, 2);
  lcd.setBacklight(255);
  lcd.home();
  lcd.print("khangoatfarm.in");
  lcd.setCursor(0,1);
  lcd.print("**EMERGENCY MODE");
  delay(2000);
  lcd.clear();
  delay(500);
  digitalWrite(TEMP_RELAY,1);
  digitalWrite(FAN_RELAY,1);
  digitalWrite(EXHAUST,1);
  digitalWrite(RELAYE,1);
}
void(* resetFunc) (void) = 0;
void loop(){
  disp();
}
void disp(){
  if(millis() >=  time_now +period){
    lcd.clear();
    time_now += period;
    float chk = DHT.read22(DHT11_PIN);
    temp=DHT.temperature;
    hum=DHT.humidity;
    lcd.setCursor(0,0);
    lcd.print("T>");lcd.print(tl,1);lcd.print("|");lcd.print(temp,1);lcd.print("|");lcd.print(tu,1);
    lcd.setCursor(0,1);
    lcd.print("H>");lcd.print(hum,1);lcd.print(" Vent:");lcd.print(exhaust?"OFF":"ON");
    if(temp<=tl && temp>0)
    {
      digitalWrite(TEMP_RELAY,0);
      digitalWrite(FAN_RELAY,0);
    }
    if(temp>=tu)
    {
      digitalWrite(TEMP_RELAY,1);
      digitalWrite(FAN_RELAY,1);
    }
    if(temp>ex)
    {
      exhaust=0;
      digitalWrite(EXHAUST,0);
    }
    if(temp<=ex || temp<tl)
    {
      exhaust=1;
      digitalWrite(EXHAUST,1);
    }
    if(temp<0)
    {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("CHECK THE SENSOR");
      lcd.setCursor(0,1);
      lcd.print("RESTARTING");
      delay(2000);
      digitalWrite(RESET,0);
      delay(200);
    }
  }
  if(millis() > time_res + minut_res)
  {
      digitalWrite(RESET,0);
      delay(200);
  }
}
