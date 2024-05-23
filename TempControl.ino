#include <dht.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define DHTPIN 2
#define DHTTYPE DHT11  // Use DHT11 instead of dht11

dht DHT;

const int potPin = A0;
const int fanPin = 3;                // Connect the fan to this pin
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Set the LCD address and dimensions

void setup() {
  Serial.begin(9600);
  pinMode(DHTPIN, INPUT);  // Set DHTPIN as input
  pinMode(fanPin, OUTPUT);
  lcd.init();       // Initialize the LCD
  lcd.backlight();  // Turn on the backlight
  lcd.setCursor(0, 0);
  lcd.print("Temp Fan Control");
  lcd.setCursor(0, 1);
  lcd.print("by Your Name");
  delay(2000);
  lcd.clear();
}

void loop() {
  int threshold = map(analogRead(potPin), 0, 1023, 20, 40);  // Map potentiometer value to temperature range

  int brightness = map(analogRead(potPin),0,1023,0,255);

  int chk = DHT.read11(DHTPIN);  // Read data from DHT sensor

  float temperature = DHT.temperature;

  if (isnan(temperature)) {
    lcd.clear();
    lcd.print("Failed to read from DHT sensor!");
    return;
  }

  if (temperature > threshold) {
    //analogWrite(fanPin, brightness);  // Turn on the fan
    digitalWrite(fanPin, LOW); 

  } else {
    digitalWrite(fanPin, HIGH);  // Turn off the fan
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temperature);
  lcd.print("C");

  lcd.setCursor(0, 1);
  lcd.print("Threshold: ");
  lcd.print(threshold);
  lcd.print("C");

  delay(1000);
}
