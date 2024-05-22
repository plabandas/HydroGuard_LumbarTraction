// Include LCD display library for I2C
#include <LiquidCrystal_I2C.h>
// Include NewPing Library
// Include NewPing Library
#include "NewPing.h"

#include <Keypad.h>
// Hook up HC-SR04 with Trig to Arduino Pin 9, Echo to Arduino pin 10
#define TRIGGER_PIN 11
#define ECHO_PIN 12


// Maximum distance we want to ping for (in mm).
#define MAX_DISTANCE 252

// NewPing setup of pins and maximum distance.
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

// Eta Low Level Trigger
// Password Length
const int Password_Length = 7;  // ->>>> Password And Length -> Changing
// Character to hold password input
String Data;
// Password
String Master = "2007111";
// ->>>> Here is the password

//   # for Turn Off and * for Password Reset

// Motor A connections
int pumpMotorOutput = 10;

bool flag = true;
bool lock_state = false;  // true means locked and false means unlocked
// Pin connected to lock relay signal
int gateValveOutput = 13;

// Counter for character entries
byte data_count = 0;

// Character to hold key input
char customKey;

// Constants for row and column sizes
const byte ROWS = 4;
const byte COLS = 4;

// Array to represent keys on keypad
char hexaKeys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};

// Connections to Arduino
byte rowPins[ROWS] = { 9, 8, 7, 6 };
byte colPins[COLS] = { 5, 4, 3, 2 };


// Create keypad object
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

// Create LCD object : Use 0x27 If 0x27 Doesn't work
LiquidCrystal_I2C lcd(0x27, 16, 2);

//Water level
int water1 = A0;
int water2 = A1;
int water3 = A2;
int val1 = 0, val2 = 0, val3 = 0, total = 0;
int level = 0;
//

void setup() {
  // Setup LCD with backlight and initialize
  lcd.init();
  lcd.backlight();
  lcd.print("Enter Password:");

  // Set gateValveOutput as an OUTPUT pin
  pinMode(gateValveOutput, OUTPUT);
  digitalWrite(gateValveOutput, HIGH);

  pinMode(pumpMotorOutput, OUTPUT);

  //start the serial monitor
  Serial.begin(9600);
}

void loop() {

  // Initialize LCD and print
  Serial.println((sonar.ping_median(5) / 2) * 0.343);
  lcd.setCursor(0, 0);
  lcd.print("Enter Password:");

  // Look for keypress
  customKey = customKeypad.getKey();

  if (customKey) {

    // Enter keypress into array and increment counter
    Data += customKey;
    lcd.setCursor(data_count, 1);
    lcd.print(Data[data_count]);

    if (flag) {
      flag = false;
      Serial.print("Your Password : ");
    }
    Serial.print(Data[data_count]);
    data_count++;


    if (customKey == '*') {
      Data = "";
      data_count = 0;
      Serial.print("\n Password Cleaned !! \n");
      flag = true;

      lcd.clear();
      clearData();

      // Incorrect Password
      lcd.print("Pin Cleaned");
      delay(1000);
    }
    if (customKey == '#') {
      Data = "";
      data_count = 0;
      Serial.print("\n Password Cleaned !! \n");
      Serial.print("Power OFF Now !!");
      flag = true;

      digitalWrite(gateValveOutput, HIGH);

      lock_state = true;

      lcd.clear();
      clearData();

      // Incorrect Password
      lcd.print("Lock Btn Press");
      delay(1000);
    }
  }

  // See if we have reached the password length
  if (data_count == Password_Length) {

    lcd.clear();

    if (Data == Master) {

      Serial.print("\n");
      Serial.print("Password Correct !!");
      Serial.print("\n");
      flag = true;

      lock_state = false;  //Unlocked

      // Turn on relay for 5 seconds
      // digitalWrite(gateValveOutput, LOW);

      // Correct Password
      lcd.print("Pin Matched");
      delay(2000);

      enterCheckMethod();

      //delay(10000);
      //digitalWrite(gateValveOutput, HIGH);
    } else {
      Serial.print("\n");
      Serial.print("Password Incorrect !!");
      Serial.print("\n");
      flag = true;

      lock_state = true;

      // Incorrect Password
      lcd.print("Incorrect Pin");
      delay(2000);
    }

    // Clear data and LCD display
    lcd.clear();
    clearData();
  }
}

void clearData() {
  //Reset data_count
  data_count = 0;
  //Reset Data
  Data = "";
}


int getLevel() {

  val1 = analogRead(water1);
  val2 = analogRead(water2);
  val3 = analogRead(water3);
  total = val1 + val2 + val3;

  if (total <= 200) {
    level = 0;
  } else if (total <= 600) {
    level = 1;
  } else if (total <= 700) {
    level = 2;
  } else if (total <= 900) {
    level = 3;
  } else if (total <= 1300) {
    level = 4;
  } else if (total <= 1400) {
    level = 5;
  } else if (total <= 1600) {
    level = 6;
  } else if (total <= 2000) {
    level = 7;
  } else if (total <= 2100) {
    level = 8;
  } else {
    level = 9;
  }

  Serial.print("water level is: ");


  Serial.println(level);

  return level;
}
void enterThreeDigitNumber() {
  // Input from keypad for 3 characters
  lcd.clear();
  lcd.print("Enter 3 Characters:");

  String keypadInput;       // Array to store the input
  int inputCharacters = 0;  // Counter for number of characters entered

  while (inputCharacters < 3) {
    char key = customKeypad.getKey();
    if (key != NO_KEY) {
      // Add the key to the input array
      keypadInput += key;
      inputCharacters++;
      // Display the pressed key
      lcd.clear();  // Clear the display before printing the key
      lcd.print("Enter 3 Characters:");
      lcd.setCursor(0, 1);  // Move cursor to the second line
      lcd.print(keypadInput);
      delay(200);  // Delay to allow for comfortable keypress rate
    }
  }

  // Null-terminate the string
  keypadInput[3] = '\0';

  // Clear remaining keys from the keypad buffer
  customKeypad.getKeys();  // Discard any remaining keys in buffer

  // Display the entered 3-character input
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("Entered: ");
  lcd.print(keypadInput);
  delay(10000);
  // Now you have the 3-character input in keypadInput
}

String takeInput(String d1, String d2, int c) {
  // Input from keypad for 3 characters
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(d1);
  lcd.setCursor(0, 1);
  lcd.print(d2);
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("#: ");
  String keypadInput = "";  // Array to store the input
  int inputCharacters = 0;  // Counter for number of characters entered

  while (inputCharacters < c) {
    char key = customKeypad.getKey();
    if (key != NO_KEY) {
      // Add the key to the input array
      keypadInput += key;
      inputCharacters++;
      // Display the pressed key
      lcd.clear();          // Clear the display before printing the key
      lcd.setCursor(0, 0);  // Move cursor to the second line
      lcd.print(keypadInput);
      delay(200);  // Delay to allow for comfortable keypress rate
    }
  }

  // // Null-terminate the string
  // keypadInput[c] = '\0';

  // // Clear remaining keys from the keypad buffer
  // customKeypad.getKeys();  // Discard any remaining keys in buffer

  // Display the entered 3-character input
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Entered: ");
  lcd.print(keypadInput);
  delay(3000);
  return keypadInput;
}


void enterCheckMethod() {
  String keypadInput;

  keypadInput = takeInput("Enter Check", "Characters:", 1);

  if (keypadInput == "A") {
    gotoManualCheck();
  } else if (keypadInput == "B") {
    gotoDynamicCheck();
  } else if (keypadInput == "C") {

  } else {
    return;
  }

  // Now you have the 3-character input in keypadInput
}


void gotoManualCheck() {

  int getWaterL = atoi(takeInput("Enter Amount Of", "Water:", 3).c_str());
  int getTractionTime = atoi(takeInput("Enter Traction", "Time:", 3).c_str());
  int getGateValveOpenDuration = atoi(takeInput("Enter Gate Valve", "Open Duration:", 2).c_str());

  int pumpSpeed = 6;  // 1 min -> 6 litre

  String getWaterLStr = String(getWaterL);
  String getTractionTimeStr = String(getTractionTime);
  String getGateValveOpenDurationStr = String(getGateValveOpenDuration);

  lcd.clear();
  // lcd.print(getWaterLStr);
  // lcd.print(" ");
  // lcd.print(getTractionTimeStr);
  // lcd.print(" ");
  // lcd.print(getGateValveOpenDurationStr);
  // delay(2000);

  int pumpTime = getWaterL * 60 / pumpSpeed;  //Calculated Time in Seconds
  Serial.print(pumpTime);
  delay(500);
  turnOnPump(pumpTime);  //Takes input in second

  runTraction(getTractionTime * 60);  //Run traction

  turnOnGateValve(getGateValveOpenDuration * 60);  //Takes input in second

  String again = takeInput("Do you want to ", "check again?", 1);

  if (again == "1") {
    enterCheckMethod();
  } else {
    return;
  }
}


void runTraction(int getTractionTime) {

  for (int i = getTractionTime; i > 0; i--) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Traction Running");
    lcd.setCursor(0, 1);
    lcd.print("T:");
    lcd.print(i);
    lcd.print("s");
    delay(1000);  // Wait for one second
  }


  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Traction Finshed");
  delay(2000);
  lcd.clear();
}

void turnOnPump(int pumpTime) {
  digitalWrite(pumpMotorOutput, HIGH);

  for (int i = pumpTime; i > 0; i--) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Pump running");
    lcd.setCursor(0, 1);
    lcd.print("T:");
    lcd.print(i);
    lcd.print("s");
    delay(1000);  // Wait for one second
  }

  digitalWrite(pumpMotorOutput, LOW);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Pump stopped");
  delay(2000);
  lcd.clear();
}


void turnOnGateValve(int getGateValveOpenDuration) {
  digitalWrite(gateValveOutput, LOW);

  for (int i = getGateValveOpenDuration; i > 0; i--) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Draining");
    lcd.setCursor(0, 1);
    lcd.print("T:");
    lcd.print(i);
    lcd.print("s");
    delay(1000);  // Wait for one second
  }

  digitalWrite(gateValveOutput, HIGH);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Draining stopped");
  delay(2000);
  lcd.clear();
}


int calculatePumpTime(int getWeight, int perHeight) {
  double waterL = getWeight/10 ;        //in gm
  double pumpHeight = waterL * perHeight;  //in mm
  Serial.println("PumpHeight:");
  Serial.println(pumpHeight);
  int iterations = 5;
  int dist = (sonar.ping_median(iterations) / 2) * 0.343;
  Serial.println(dist);
  digitalWrite(pumpMotorOutput, HIGH);
  int i=1;
  while ( (MAX_DISTANCE - dist) <= pumpHeight) {
    Serial.println(dist);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Pump running");
    lcd.setCursor(0, 1);
    lcd.print("T:");
    lcd.print(i);
    lcd.print("s");
    delay(1000);  // Wait for one second
    dist = (sonar.ping_median(iterations) / 2) * 0.343;
    i++;
  }

  digitalWrite(pumpMotorOutput, LOW);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Pump stopped");
  delay(2000);
  lcd.clear();
}


void turnOnGateValveDynamic() {
  digitalWrite(gateValveOutput, LOW);
  int iterations = 5;
  int dist = (sonar.ping_median(iterations) / 2) * 0.343;
  int i=1;
  while (dist<=175) {
    Serial.println(dist);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Drain running");
    lcd.setCursor(0, 1);
    lcd.print("T:");
    lcd.print(i);
    lcd.print("s");
    delay(1000);  // Wait for one second
    dist = (sonar.ping_median(iterations) / 2) * 0.343;
    i++;
  }

  digitalWrite(gateValveOutput, HIGH);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Draining stopped");
  delay(2000);
  lcd.clear();
}


void gotoDynamicCheck() {

  int getWeight = atoi(takeInput("Enter Patient", "Weight(KG):", 3).c_str());
  int getTractionTime = atoi(takeInput("Enter Traction", "Time:", 3).c_str());
  // int getGateValveOpenDuration = atoi(takeInput("Enter Gate Valve", "Open Duration:", 2).c_str());

  int perHeight = 25; //actualy 12.75( 127.5 for 5kg)  // x mm per 1 kgram

  String getWeightStr = String(getWeight);
  String getTractionTimeStr = String(getTractionTime);
  // String getGateValveOpenDurationStr = String(getGateValveOpenDuration);

  lcd.clear();
  // lcd.print(getWaterLStr);
  // lcd.print(" ");
  // lcd.print(getTractionTimeStr);
  // lcd.print(" ");
  // lcd.print(getGateValveOpenDurationStr);
  // delay(2000);

  calculatePumpTime(getWeight, perHeight);

  runTraction(getTractionTime * 60);  //Run traction

  turnOnGateValveDynamic();  //dynamic
  String again = takeInput("Do you want to ", "check again?", 1);

  if (again == "1") {
    enterCheckMethod();
  } else {
    return;
  }
}