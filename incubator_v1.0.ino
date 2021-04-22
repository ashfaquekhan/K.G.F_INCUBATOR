#define CLK 2
#define DT 3
#define DHT11_PIN 7

#include <EEPROM.h>
#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>
#include <dht.h>
LiquidCrystal_PCF8574 lcd(0x27);
dht DHT;

int period = 2000;
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
int sizmenu[3] ={51,96,9};
char *mainmenu[] ={"SET TEMP","SET HUMD","SERVO","RESET"};
int siz;
int opval[3];

void setup() {
  readIntArrayFromEEPROM(20,opval,3);
  Wire.begin();
  Wire.beginTransmission(0x27);
  pinMode(CLK,INPUT);
  pinMode(DT,INPUT);
  lcd.begin(16, 2);
  lcd.setBacklight(255);
  lcd.home(); lcd.clear();
  myButton.begin();
  Serial.begin(9600);
  lastStateCLK = digitalRead(CLK);


}
void(* resetFunc) (void) = 0;
void loop(){
  disp();
   static unsigned long currentTime = millis();
   if(myButton.isReleased())        // if button pressed,
     currentTime = millis();  // reset timer
   if(millis() - currentTime > 10000) {
    dispVal();
   }
   else{
    rot();
   }
}
void dispVal(){
  lcd.setCursor(0,1);
  lcd.print("TEMP  HUMD  TURN");
}
void disp(){
  if(millis() >= time_now + period){
    lcd.clear();
    time_now += period;
    int chk = DHT.read11(DHT11_PIN);
    temp=DHT.temperature;
    hum=DHT.humidity;
    
    }
lcd.setCursor(0,0);
lcd.print(opval[0]);lcd.print("|");lcd.print(int(temp));lcd.print(" ");lcd.print(opval[1]);lcd.print("|");lcd.print(int(hum));lcd.print("  ");lcd.print(opval[2]);lcd.print("x");
}
void rot() {
  A:
  lcd.setCursor(0,1);
  lcd.print(mainmenu[counter]);
  int lim=sizeof(mainmenu)/2;
  currentStateCLK = digitalRead(CLK);
  if (currentStateCLK != lastStateCLK  && currentStateCLK == 1){
    lcd.clear();
    //lcd.setCursor(0,1);
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
    while(1){
      int mx=sizmenu[counter];
      currentStateCLK = digitalRead(CLK);
      if (currentStateCLK != lastStateCLK  && currentStateCLK == 1){
        lcd.clear();
        if (digitalRead(DT) != currentStateCLK) {
          cn--;
          if(cn<0){
            cn=mx-1;}
          }
        else{
          cn++;
          if(cn==mx){
            cn=0;}
          }
        lcd.setCursor(10,1);
        lcd.print(cn);

       }
      lastStateCLK = currentStateCLK;
      if (myButton.isReleased()) {
          opval[counter]=cn;
          writeIntArrayIntoEEPROM(20,opval,3);
          lcd.clear();
          goto A;
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
