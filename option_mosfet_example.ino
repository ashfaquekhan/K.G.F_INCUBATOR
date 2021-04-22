int outPin = 9;
 
// Input from potentiometer
int potIn = A0;
 
// Variable to hold speed value
int speedVal;
 
void setup()
{
    Serial.begin(9600);
    // Setup transistor pin as output
    pinMode(outPin, OUTPUT);
 
}
 
void loop()
{
  speedVal=150;
  // Read values from potentiometer
  //speedVal = analogRead(potIn);
  Serial.println(speedVal);
  
  // Map value to range of 0-255
  //speedVal = map(speedVal, 0, 1023, 0, 255);
  
  // Write PWM to transistor
  analogWrite(outPin, speedVal);
  
  delay(20);
  
}
