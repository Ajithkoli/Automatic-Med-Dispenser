#include <Wire.h>
#include <Adafruit_Keypad.h>
#include <Servo.h>
#include <TimeLib.h>
#include <RTClib.h>

// Define keypad configuration
const byte ROW_NUM = 4;    // Four rows
const byte COLUMN_NUM = 4; // Four columns
char keys[ROW_NUM][COLUMN_NUM] = {
  {'D', 'C', 'B', 'A'},
  {'#', '9', '6', '3'},
  {'0', '8', '5', '2'},
  {'*', '7', '4', '1'}
};
byte pin_rows[ROW_NUM] = {9, 8, 7, 6};      // Row pins connected to Arduino
byte pin_column[COLUMN_NUM] = {5, 4, 3, 10}; // Column pins connected to Arduino

// Create Keypad object
Adafruit_Keypad keypad = Adafruit_Keypad(makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM);

// Define Servo and sensor pins
Servo servo1, servo2;        // Two Servo motors
const int SERVO1_PIN = A0;   // Servo1 attached to Analog Pin 0
const int SERVO2_PIN = A1;   // Servo2 attached to Analog Pin 1
const int IR_PIN = 12;       // IR sensor pin
const int BUZZER_PIN = 13;   // Buzzer pin
const int RESET_BUTTON = 10; // Reset button pin
const int REFILL_BUTTON = 11;// Refill button pin

// RTC object
RTC_DS3231 rtc; // Using DS3231 RTC

// Time variables for dispensing
int setHour1 = 0, setMinute1 = 0;  // Schedule for Servo 1
int setHour2 = 0, setMinute2 = 0;  // Schedule for Servo 2
bool isTimeSet = false;

// Dispensing flags
bool servo1Dispensed = false;
bool servo2Dispensed = false;

void setup() {
  Serial.begin(9600);  // Start Serial communication
  delay(2000);         // Wait for Serial Monitor to start

  // Attach servos to their respective pins
  servo1.attach(SERVO1_PIN);
  servo2.attach(SERVO2_PIN);

  // Initialize pin modes
  pinMode(IR_PIN, INPUT);               // IR sensor as input
  pinMode(BUZZER_PIN, OUTPUT);          // Buzzer as output
  pinMode(RESET_BUTTON, INPUT_PULLUP);  // Reset button with internal pull-up
  pinMode(REFILL_BUTTON, INPUT_PULLUP); // Refill button with internal pull-up

  keypad.begin(); // Initialize the keypad
  Serial.println("Medicine Dispenser Initialized");

  // Check if RTC is connected
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  // Set the current time using keypad only once
  DateTime now = rtc.now();
  if (now.year() == 2000) { // RTC not set yet
    setCurrentTimeUsingKeypad();
  }

  isTimeSet = false; // Initial state for dispensing times
}

void loop() {
  keypad.tick(); // Update the keypad state to detect key presses

  // Display current time every 5 seconds
  static unsigned long previousMillis = 0;
  if (millis() - previousMillis >= 5000) {
    displayCurrentTime();
    previousMillis = millis();
  }

  // Check for reset button press
  if (digitalRead(RESET_BUTTON) == LOW) {
    resetTimes();
    delay(500);  // Debounce delay
  }

  // Check for refill button press
  if (digitalRead(REFILL_BUTTON) == LOW) {
    refillServos();
    delay(500);  // Debounce delay
  }

  if (!isTimeSet) {
    setTimeUsingKeypad();
  } else {
    checkAndDispenseMedicine();
  }
}

// Function to display the current time
void displayCurrentTime() {
  DateTime now = rtc.now(); // Get the current time from RTC
  Serial.print("Current Time: ");
  Serial.print(now.hour());
  Serial.print(":");
  Serial.print(now.minute());
  Serial.print(":");
  Serial.println(now.second());
}

// Function to set the current time using keypad
void setCurrentTimeUsingKeypad() {
  Serial.println("Set current time (HH:MM):");

  int currentHour = getKeypadInput("Hour (0-23): ");
  if (currentHour == -1) return; // Cancelled input

  int currentMinute = getKeypadInput("Minute (0-59): ");
  if (currentMinute == -1) return; // Cancelled input

  if (currentHour < 0 || currentHour > 23 || currentMinute < 0 || currentMinute > 59) {
    Serial.println("Invalid current time entered.");
    return;
  }

  rtc.adjust(DateTime(2024, 1, 1, currentHour, currentMinute, 0)); // Set current time to RTC
  Serial.print("Current Time Set: ");
  Serial.print(currentHour);
  Serial.print(":");
  Serial.println(currentMinute);
  delay(2000);
}

// Function to set dispensing times using keypad
void setTimeUsingKeypad() {
  Serial.println("Set time for Servo 1 (HH:MM):");

  setHour1 = getKeypadInput("Hour (0-23): ");
  if (setHour1 == -1) return; // Cancelled input
  
  setMinute1 = getKeypadInput("Minute (0-59): ");
  if (setMinute1 == -1) return; // Cancelled input

  if (setHour1 < 0 || setHour1 > 23 || setMinute1 < 0 || setMinute1 > 59) {
    Serial.println("Invalid time entered for Servo 1.");
    return;
  }

  Serial.print("Time Set for Servo 1: ");
  Serial.print(setHour1);
  Serial.print(":");
  Serial.println(setMinute1);
  delay(2000);

  Serial.println("Set time for Servo 2 (HH:MM):");

  setHour2 = getKeypadInput("Hour (0-23): ");
  if (setHour2 == -1) return; // Cancelled input
  
  setMinute2 = getKeypadInput("Minute (0-59): ");
  if (setMinute2 == -1) return; // Cancelled input

  if (setHour2 < 0 || setHour2 > 23 || setMinute2 < 0 || setMinute2 > 59) {
    Serial.println("Invalid time entered for Servo 2.");
    return;
  }

  Serial.print("Time Set for Servo 2: ");
  Serial.print(setHour2);
  Serial.print(":");
  Serial.println(setMinute2);

  isTimeSet = true; // Mark time as set
  delay(2000);
}

// Function to get input from keypad (HH or MM)
int getKeypadInput(String prompt) {
  int value = 0;
  Serial.print(prompt);

  unsigned long startTime = millis(); // Start a timeout timer

  while (true) {
    keypad.tick(); // Poll the keypad for key events
    while (keypad.available()) {
      keypadEvent e = keypad.read(); // Read the key event

      if (e.bit.EVENT == KEY_JUST_PRESSED) {
        char key = keys[e.bit.ROW][e.bit.COL]; 

        if (key >= '0' && key <= '9') {
          value = value * 10 + (key - '0');
          Serial.print(key);
        } else if (key == '#') { // Confirm input
          Serial.println();
          return value;
        } else if (key == '*') { // Cancel input
          Serial.println("\nCancelled");
          return -1;
        }
      }
    }

    if (millis() - startTime > 30000) { // 30 seconds timeout
      Serial.println("\nTimeout!");
      return -1; // Cancel on timeout
    }
    delay(100);
  }
}

// Function to reset times
void resetTimes() {
  setHour1 = 0;
  setMinute1 = 0;
  setHour2 = 0;
  setMinute2 = 0;
  isTimeSet = false;
  servo1Dispensed = false;
  servo2Dispensed = false;
  Serial.println("Times Reset!");
  delay(2000);
}

// Function to refill servos
void refillServos() {
  Serial.println("Refilling...");
  servo1.write(90);  
  servo2.write(90);  
  delay(3000);       
  servo1.write(0);   
  servo2.write(0);   
  Serial.println("Refill Done!");
  delay(2000);
}

// Function to check time and dispense medicine
void checkAndDispenseMedicine() {
  DateTime now = rtc.now(); 
  int currentHour = now.hour();
  int currentMinute = now.minute();

  if (currentHour == setHour1 && currentMinute == setMinute1 && !servo1Dispensed) {
     tone(BUZZER_PIN, 1000, 2000); // Buzzer beeps for 2 seconds
    delay(2000); 
    if (digitalRead(IR_PIN) != HIGH) { 
      servo1.write(180); 
      delay(8000);      
      servo1.write(90);  
      delay(1000);      
      servo1Dispensed = true; 
      Serial.println("Dispensed from Servo 1");
    } else {
      Serial.println("No one detected for Servo 1!");
    }
    delay(2000);
  } else if (currentHour != setHour1 || currentMinute != setMinute1) {
    servo1Dispensed = false; 
  }

  if (currentHour == setHour2 && currentMinute == setMinute2 && !servo2Dispensed) {
     tone(BUZZER_PIN, 1000, 2000); // Buzzer beeps for 2 seconds
    delay(2000); 
    if (digitalRead(IR_PIN) != HIGH) { 
      servo2.write(180); 
      delay(8000);      
      servo2.write(90);  
      delay(1000);      
      servo2Dispensed = true; 
      Serial.println("Dispensed from Servo 2");
    } else {
      Serial.println("No one detected for Servo 2!");
    }
    delay(2000);
  } else if (currentHour != setHour2 || currentMinute != setMinute2) {
    servo2Dispensed = false; 
  }
}