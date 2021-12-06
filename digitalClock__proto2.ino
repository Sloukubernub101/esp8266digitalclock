#include <Arduino.h>
#include <TM1637Display.h>
#include <Wire.h>
#include <RTClib.h>

#define CLK D3
#define DIO D4

const int buttonPin = D5;
const int buttonIncrement = D6;
const int buzzer = D0;
const int sleepButton = D7;

int incrementButtonState = 0;
int incrementLastButtonState = 0;
int buttonstate = 0;
int lastbuttonstate = 0;
int buttonPressCount = 0;
int hours = 0;
int minutes = 0;
int sleepbuttonstate = 0;

RTC_DS1307 rtc;
TM1637Display display(CLK, DIO);

int incrementMinutesButton();
int incrementHoursButton();
void alarmBeepSound();

void setup() {
  pinMode(buzzer , OUTPUT);
  pinMode(buttonPin, INPUT);
  pinMode(buttonIncrement, INPUT);
  pinMode(sleepButton, INPUT);
  Serial.begin(115200);
  // RTC setup
  if (!rtc.begin()){
    Serial.println("RTC is NOT running");
  }

  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // deep sleep esp
  // ESP.deepSleep(0); 
}

void loop() {
  DateTime now = rtc.now();
  uint8_t data[] = {0xff, 0xff, 0xff, 0xff};
  uint8_t black[] = {0x00, 0x00, 0x00, 0x00};
  display.setBrightness(0x0f);
  int displaytime = (now.hour() * 100) + now.minute();

  // read button state
  buttonstate = digitalRead(buttonPin);

  // formatted time for alarm
  int alarmFormatted = (hours * 100) + minutes;
  
  // when second is even
  if (now.second() % 2 == 0 || now.second() == 0){
    if (buttonPressCount == 0){
      display.showNumberDecEx(displaytime, 0b11100000, true);
    }
    else {
      if (buttonPressCount == 1)
      {
        display.showNumberDec(alarmFormatted, true);
        incrementMinutesButton();
        if (minutes > 59){
          minutes = 0;
        }
      }
      else if (buttonPressCount == 2){
        display.showNumberDec(alarmFormatted, true);
        incrementHoursButton();
        if (hours > 23){
          hours = 0;
        }
      }
      else {
        alarmFormatted = (hours * 100) + minutes;
      }
    }
  }
  
  // when second is odd
  if (now.second() % 2 != 0){
    if (buttonPressCount == 0){
      display.showNumberDec(displaytime, true);
    }
    else {
      if(buttonPressCount == 1){
        display.showNumberDec(alarmFormatted, true);
        incrementMinutesButton();
        if (minutes > 59){
          minutes = 0;
        }
        else{
          minutes = incrementMinutesButton();
        }
      }
      else if (buttonPressCount == 2){
        display.showNumberDec(alarmFormatted, true);
        incrementHoursButton();
        if (hours > 23){
          hours = 0;
        }
        else {
          hours = incrementHoursButton();
        }
      }
      else {
        alarmFormatted = (hours * 100) + minutes;
      }     
    }
  }

  if (buttonstate != lastbuttonstate){
    if (buttonstate == HIGH) {
        if (buttonPressCount == 0){
          buttonPressCount++;
          Serial.println(buttonPressCount);
        }

        else if (buttonPressCount == 1){
          buttonPressCount++;
          Serial.println(buttonPressCount);
        }
        else{
          buttonPressCount = 0;
        }
    }
  }
  lastbuttonstate = buttonstate;

  // activate alarm if current time match the alarm time
  if (now.hour() == hours && now.minute() == minutes){
    alarmBeepSound();
  }
  else {
    digitalWrite(buzzer, LOW);
  }
}

// increment button for minutes
int incrementMinutesButton(){
  // increment button for minutes
  incrementButtonState = digitalRead(buttonIncrement);
  if (incrementButtonState != incrementLastButtonState){
    if (incrementButtonState == HIGH){
      minutes++;
      Serial.println(minutes);
      delay(50);
    }
    else{
    }
  }
  incrementLastButtonState = incrementButtonState;
  return minutes;
}

// increment button for hours
int incrementHoursButton(){
  incrementButtonState = digitalRead(buttonIncrement);
  if (incrementButtonState != incrementLastButtonState){
    if(incrementButtonState == HIGH){
      hours++;
      delay(50);
    }
    else{
      
    }
  }
  incrementLastButtonState = incrementButtonState;
  return hours;
}

void alarmBeepSound(){
  DateTime now = rtc.now();
  if (now.second() <= 20){
    digitalWrite(buzzer, HIGH);
    delay(250);
    digitalWrite(buzzer, LOW);
    delay(250);
   }
  else if (now.second() <= 30){
    digitalWrite(buzzer, HIGH);
    delay(100);
    digitalWrite(buzzer, LOW);
    delay(100);
  }
  else if (now.second() <= 40){
    digitalWrite(buzzer, HIGH);
    delay(50);
    digitalWrite(buzzer, LOW);
    delay(50);
  }
  else if (now.second() <= 50){
    digitalWrite(buzzer, HIGH);
    delay(10);
    digitalWrite(buzzer, LOW);
    delay(10);
  }
  else {
    digitalWrite(buzzer, HIGH);
  }
}
