#define CLK 2
#define DT 3
#define DHT11_PIN 7

#define TEMP_RELAY  10 //IN 1
#define FAN_RELAY  9 // IN 2
#define EXHAUST  6 // IN 3
#define RELAYE  5 //IN 4

#include <EEPROM.h>
#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>
#include <dht.h>
LiquidCrystal_PCF8574 lcd(0x27);
dht DHT;
int period = 2000;
int relayState = LOW;                   // for HUMIDITY
int rotationrelay = LOW;         // for rotation
uint32_t nextTime;    // will store last time updated "HUMIDITY"
uint32_t nextTimeROT;    // will store last time updated "ROTATION" 
boolean HUMPULSE= false;
unsigned long time_now = 0;
int temp,hum;
class Button {
    private:
        bool _state;
        uint8_t _pin;

    public:
        Button(uint8_t pin) : _pin(pin) {}

        void begin() {
            pinMode(_pin, INPUT_PULLUP);
            _state = digitalRead(_pin);
        }

        bool isReleased() {
            bool v = digitalRead(_pin);
            if (v != _state) {
                _state = v;
                if (_state) {
                    return true;
                }
            }
            return false;
        }
};
Button myButton(4);
int counter = 0;
int cnt=0;
int currentStateCLK;
int lastStateCLK;
unsigned long lastButtonPress = 0;
int sizmenu[7] ={40,40,99,99,61,61,1};
char *mainmenu[] ={"TEMP START","TEMP STOP","HUMD START","HUMD STOP","Fan-On","Fan-OFF","RESET"};
int siz;
int opval[7];
bool state=0;
bool hstate=0;
void setup() {
  readIntArrayFromEEPROM(20,opval,7);
  Wire.begin();
  Wire.beginTransmission(0x27);
  pinMode(CLK,INPUT);
  pinMode(DT,INPUT);
  pinMode(TEMP_RELAY, OUTPUT);
  pinMode(FAN_RELAY, OUTPUT);
  pinMode(EXHAUST, OUTPUT);
  pinMode(RELAYE, OUTPUT);
  lcd.begin(16, 2);
  lcd.setBacklight(255);
  lcd.home(); 
  lcd.print("khangoatfarm.in");
  delay(2000);
  lcd.clear();
  myButton.begin();
  Serial.begin(9600);
  lastStateCLK = digitalRead(CLK);
  delay(500);
  digitalWrite(RELAYE,1);
}
void(* resetFunc) (void) = 0;
void loop(){
   static unsigned long currentTime = millis();
   if(myButton.isReleased())
     currentTime = millis();  
   if(millis() - currentTime > 10000) {
    disp();
   }
   else{
    settings();
   }
   pulse();
}
void pulse(){
  uint32_t currentTime = millis();
  if (currentTime > nextTimeROT) {
    if (rotationrelay == LOW) {
      rotationrelay = HIGH;
      nextTimeROT = currentTime + (opval[4]*60000);
    } else {
      rotationrelay = LOW;
      nextTimeROT = currentTime + (opval[5]*60000); //ON INTERVAL
    }
  }
  digitalWrite(FAN_RELAY, rotationrelay);
}

void disp(){
  if(millis() >= time_now + period){
    lcd.clear();
    time_now += period;
    int chk = DHT.read22(DHT11_PIN);
    temp=DHT.temperature;
    hum=DHT.humidity;
    lcd.setCursor(0,0);
    lcd.print("T>");lcd.print(opval[0]);lcd.print("|");lcd.print(int(temp));lcd.print("|");lcd.print(opval[1]);
    lcd.setCursor(0,1);
    lcd.print("H>");lcd.print(opval[2]);lcd.print("|");lcd.print(int(hum));lcd.print("|");lcd.print(opval[3]);
    if(temp<=opval[0] && state==0)
    {
      digitalWrite(EXHAUST,1);
      digitalWrite(TEMP_RELAY,0);
      if(temp>=opval[1])
      {
        state=1;
        digitalWrite(EXHAUST,1);
        digitalWrite(TEMP_RELAY,1);
      }
    }
     else if(temp>=opval[1] && state==0)
     {
      state=1;
      digitalWrite(TEMP_RELAY,1);
    }
    if(hum>opval[3])
    {
      digitalWrite(TEMP_RELAY,0);
      digitalWrite(EXHAUST,1);
      digitalWrite(FAN_RELAY,1);
    }
    if(hum<opval[2])
    {
      digitalWrite(EXHAUST,0);
    }
    if(temp>opval[1])
    {
      digitalWrite(EXHAUST,0);
    }
    else
    {
      digitalWrite(EXHAUST,1);
    }
}
}
void settings() {
  A:
  lcd.setCursor(0,0);
  lcd.print("SETTINGS");
  lcd.setCursor(0,1);
  lcd.print(mainmenu[counter]);lcd.print(" ");lcd.print(opval[counter]);
  int lim=sizeof(mainmenu)/2;
  currentStateCLK = digitalRead(CLK);
  if (currentStateCLK != lastStateCLK  && currentStateCLK == 1){
    lcd.clear();
    if (digitalRead(DT) != currentStateCLK) {
      counter --;
      if(counter<0){
        counter =lim-1;
      }
    } else {
      counter ++;
      if(counter==lim){
        counter=0;
      }
    }
  }
  lastStateCLK = currentStateCLK;
  if (myButton.isReleased()) {
    int cn=0;
    if(mainmenu[counter]=="RESET"){
      resetFunc();
    }
    else{
      while(1){
        unsigned long currentTime = millis();
        int mx=sizmenu[counter];
        currentStateCLK = digitalRead(CLK);
        if (currentStateCLK != lastStateCLK  && currentStateCLK == 1){
          lcd.clear();
          if (digitalRead(DT) != currentStateCLK) {
            cn--;
            currentTime = millis();
            if(cn<0){
              cn=mx-1;}
            }
          else{
            cn++;
            currentTime = millis();
            if(cn==mx){
              cn=0;}
            }
          lcd.setCursor(0,0);
          lcd.print("PRESS TO SAVE");
          lcd.setCursor(0,1);
          lcd.print("VALUE ->");
          lcd.setCursor(10,1);
          lcd.print(cn);
    
         }
        lastStateCLK = currentStateCLK;
        if (myButton.isReleased()) { 
            opval[counter]=cn;
            writeIntArrayIntoEEPROM(20,opval,7);
            lcd.clear();
            currentTime = millis();
            goto A;
         }
        if(millis() - currentTime > 10000){
          goto A;
        }
       delay(1);
     }
    }

  }
  delay(1);
}
void writeIntArrayIntoEEPROM(int address, int numbers[], int arraySize)
{
  int addressIndex = address;
  for (int i = 0; i < arraySize; i++) 
  {
    EEPROM.write(addressIndex, numbers[i] >> 8);
    EEPROM.write(addressIndex + 1, numbers[i] & 0xFF);
    addressIndex += 2;
  }
}
void readIntArrayFromEEPROM(int address, int numbers[], int arraySize)
{
  int addressIndex = address;
  for (int i = 0; i < arraySize; i++)
  {
    numbers[i] = (EEPROM.read(addressIndex) << 8) + EEPROM.read(addressIndex + 1);
    addressIndex += 2;
  }
}
