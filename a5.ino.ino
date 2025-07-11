#include <LiquidCrystal.h>
#include <IRremote.h>


// LCD (RS, E, D4, D5, D6, D7)
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);


// IR Receiver pin
const int IR_PIN = A3;


// Components
const int buttonPin = A2;
const int ledPin = A0;
const int buzzerPin = A1;


IRrecv irrecv(IR_PIN);
decode_results results;


// Timer variables
int minutes = 0;
int seconds = 0;
bool timerRunning = false;
unsigned long previousMillis = 0;
bool alarmTriggered = false;
bool lastButtonState = HIGH;


void setup() {
 lcd.begin(16, 2);
 lcd.print("Smart Timer Ready");


 pinMode(buttonPin, INPUT_PULLUP);
 pinMode(ledPin, OUTPUT);
 pinMode(buzzerPin, OUTPUT);


 digitalWrite(ledPin, LOW);
 digitalWrite(buzzerPin, LOW);


 irrecv.begin(IR_PIN);
 delay(2000); // show intro for 2 seconds
 lcd.clear();
 updateDisplay();
}


void loop() {
 unsigned long currentMillis = millis();


 // === Timer countdown ===
 if (timerRunning && currentMillis - previousMillis >= 1000) {
   previousMillis = currentMillis;
   if (seconds > 0) {
     seconds--;
   } else if (minutes > 0) {
     minutes--;
     seconds = 59;
   } else {
     // Timer done
     timerRunning = false;
     alarmTriggered = true;
   }
   updateDisplay();
 }


 // === Alarm triggered ===
 if (alarmTriggered) {
   digitalWrite(buzzerPin, HIGH);
   digitalWrite(ledPin, millis() / 300 % 2); // Blink LED every 300 ms
 }


 // === Button press to stop alarm ===
 bool currentButtonState = digitalRead(buttonPin);
 if (lastButtonState == HIGH && currentButtonState == LOW && alarmTriggered) {
   alarmTriggered = false;
   digitalWrite(buzzerPin, LOW);
   digitalWrite(ledPin, LOW);
   lcd.clear();
   lcd.print("Timer Stopped");
   delay(1000);
   lcd.clear();
   updateDisplay();
 }
 lastButtonState = currentButtonState;


 // === IR Remote ===
 if (irrecv.decode()) {
   unsigned long code = irrecv.decodedIRData.command;


   // Replace these with your actual remote codes (use Serial.println to debug)
   if (code == 0x18) { // +1 minute
     minutes++;
   } else if (code == 0x52) { // +10 seconds
     seconds += 10;
     if (seconds >= 60) {
       seconds -= 60;
       minutes++;
     }
   } else if (code == 0xA2) { // -10 seconds
     if (minutes > 0 || seconds >= 10) {
       if (seconds < 10) {
         seconds += 60;
         minutes--;
       }
       seconds -= 10;
     }
   } else if (code == 0x45) { // START button
     timerRunning = true;
   }


   updateDisplay();
   irrecv.resume();
 }
}


void updateDisplay() {
 lcd.setCursor(0, 0);
 lcd.print("Time Left:");


 lcd.setCursor(0, 1);
 if (minutes < 10) lcd.print("0");
 lcd.print(minutes);
 lcd.print(":");
 if (seconds < 10) lcd.print("0");
 lcd.print(seconds);
 lcd.print("      ");
}



