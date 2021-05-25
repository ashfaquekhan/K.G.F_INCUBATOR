#define ON_INTERVAL 5000
#define OFF_INTERVAL 3000
uint32_t nextTime;
int relayState = LOW;                   // ledState used to set the LED
#define RELAYE  5
void setup() {
  pinMode(RELAYE, OUTPUT);
}

void loop(){
  pulse();
}
void pulse() {
  uint32_t currentTime = millis();
  if (currentTime > nextTime) {
    if (relayState == LOW) {
      relayState = HIGH;
      nextTime = currentTime + OFF_INTERVAL;
    } else {
      relayState = LOW;
      nextTime = currentTime + ON_INTERVAL;
    }
  }
  digitalWrite(RELAYE, relayState);
}
