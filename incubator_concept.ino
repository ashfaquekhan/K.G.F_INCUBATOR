#define CLK 2
#define DT 3
#define SW 4

#include <EEPROM.h>
#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>
LiquidCrystal_PCF8574 lcd(0x27);

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
int opval[3]={37,65,4};

void setup() {
  Wire.begin();
  Wire.beginTransmission(0x27);
  pinMode(CLK,INPUT);
  pinMode(DT,INPUT);
  pinMode(SW, INPUT_PULLUP);
  lcd.begin(16, 2);
  lcd.setBacklight(255);
  lcd.home(); lcd.clear();
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
    disp();
    rot();
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

  int btnState = digitalRead(SW);
  if (btnState == LOW) {
    //if 50ms have passed since last LOW pulse, it means that the
    //button has been pressed, released and pressed again
    if (millis() - lastButtonPress > 50) {
      lcd.clear();
      lcd.setCursor(0, 1);
      writeStringToEEPROM(0,setmode[cnt]);
      delay(1000);
      resetFunc();
      
    }
    int lastPress = millis();
  }
  delay(1);
}
void disp(){
  lcd.setCursor(0,0);
  lcd.print("T:");lcd.print(opval[0]);lcd.print("  H:");lcd.print(opval[1]);lcd.print(" R:");lcd.print(opval[2]);
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
  int btnState = digitalRead(SW);

  //If we detect LOW signal, button is pressed
  if (btnState == LOW){
    if (millis() - lastButtonPress > 50) {
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
        btnState = digitalRead(SW);
        if (btnState == LOW) {
          if (millis() - 0> 50) {
            if(cn<mx){
              lastButtonPress = millis();
              lcd.clear();
              goto A;
             }
          }
       }
   }
      
}
    lastButtonPress = millis();
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
