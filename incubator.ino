#define CLK 2
#define DT 3
#include <EEPROM.h>
#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>
#include <dht.h>
LiquidCrystal_PCF8574 lcd(0x27);
dht DHT;
#define DHT11_PIN 7
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
char *setmode[] ={"INCUBATOR","HATCHER"};
int siz;
boolean mD=true;
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
  //memcpy(curr,mainmenu,sizeof(mainmenu));
  Serial.print(readStringFromEEPROM(0));
  if((readStringFromEEPROM(0))==NULL){
    mD=false;
  }

}
void(* resetFunc) (void) = 0;
void loop(){
  if(mD==true){
    rot();
    disp();
  }
  else{
    SET();
    SET_MODE();
  }
}
void SET_MODE(){
  int lim=sizeof(setmode)/2;
  int currentCLK = digitalRead(CLK);
  if (currentCLK != lastStateCLK  && currentCLK == 1){
    lcd.clear();
    lcd.setCursor(0, 1);
    if (digitalRead(DT) != currentCLK) {
      cnt --;
      if(cnt<0){
        cnt=lim-1;
      }
    }
    else {
      // Encoder is rotating CW so increment
      cnt ++;
      if(cnt>lim-1){
        cnt=0;
      }
    }
    lcd.print(setmode[cnt]);

  }
  lastStateCLK = currentCLK;
  if (myButton.isReleased()) {
    lcd.clear();
    lcd.setCursor(0, 1);
    writeStringToEEPROM(0,setmode[cnt]);
    delay(1000);
    resetFunc();
}
delay(1);
}
void disp(){
  int chk = DHT.read11(DHT11_PIN);
  lcd.setCursor(0,0);
  lcd.print(opval[0]);lcd.print("|");lcd.print(int(DHT.temperature));lcd.print(" ");lcd.print(opval[1]);lcd.print("|");lcd.print(int(DHT.humidity));lcd.print(" ");lcd.print(opval[2]);
  delay(100);
}
void SET(){
  lcd.setCursor(0,0);
  lcd.print("SELECT MODE");
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
    //lcd.print(mainmenu[counter]);

  }
  lastStateCLK = currentStateCLK;
  if (myButton.isReleased()) {
    int cn=0;
    if(mainmenu[counter]=="RESET"){
      goto A;
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
          Serial.println("BT Updated with val = ");Serial.print(cn);
          opval[counter]=cn;
          writeIntArrayIntoEEPROM(20,opval,3);
          lcd.clear();
          goto A;
       }
   }

  }
  delay(1);
}
void writeStringToEEPROM(int addrOffset, const String &strToWrite)
{
  byte len = strToWrite.length();
  EEPROM.write(addrOffset, len);
  for (int i = 0; i < len; i++)
  {
    EEPROM.write(addrOffset + 1 + i, strToWrite[i]);
  }
}
String readStringFromEEPROM(int addrOffset)
{
  int newStrLen = EEPROM.read(addrOffset);
  char data[newStrLen + 1];
  for (int i = 0; i < newStrLen; i++)
  {
    data[i] = EEPROM.read(addrOffset + 1 + i);
  }
  data[newStrLen] = '\0';
  return String(data);
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
