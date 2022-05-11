
 
// Adafruit GFX Library 
// Graphics library for our display
#include <Adafruit_GFX.h>
#include <Adafruit_GrayOLED.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <gfxfont.h>
 
// Adafruit
#include <Adafruit_SSD1306.h>
#include <splash.h>
 
// DHT sensor library 
#include <DHT.h>
#include <DHT_U.h>
 
// Wire Library 
#include <Wire.h>
 
// Set OLED size in pixels
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
 
// Set OLED parameters
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
 
// Define DHT11 Parameters
#define DHTPIN 8
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
 
// Define variables for Temperature and Humidity
float temp;
float hum;

// Constant for dry sensor
const int DryValue = 2650; 
 
// Constant for wet sensor
const int WetValue = 1800;
 
// Variables for soil moisture
int soilMoistureValue;
int soilMoisturePercent;
 
// Analog input port
#define SENSOR_IN A0
 
// Relay Port
#define RELAY_OUT D3
 
// Pump Status Text
String pump_status_text = "OFF";
 
// Variable for pump trigger
int pump_trigger = 30;
 

 
void setup() {
 
  // Initialize serial and wait for port to open:
  Serial.begin(9600);
  // This delay gives the chance to wait for a Serial Monitor without blocking if none is found
  delay(1500);
 
  // Initialize display using 3.3 volts
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  display.clearDisplay();
 
  // Initialize DHT11
  dht.begin();
 
  // Set ADC to use 12 bits
  analogReadResolution(12);
 
  // Set Relay as Output
  pinMode(RELAY_OUT, OUTPUT);
 
  // Turn off relay
  digitalWrite(RELAY_OUT, LOW);
 
 
}
 
void loop() {
 
 
  // Get temperature and Humidity
  temp = dht.readTemperature();
  hum = dht.readHumidity();
 
 
  // Get soil mositure value
  soilMoistureValue = analogRead(SENSOR_IN);
 
  // Determine soil moisture percentage value
  soilMoisturePercent = map(soilMoistureValue, DryValue, WetValue, 0, 100);
 
  // Keep values between 0 and 100
  soilMoisturePercent = constrain(soilMoisturePercent, 0, 100);
 
  // Print raw value to serial monitor for sensor calibration
  Serial.println(soilMoistureValue);
 
 
 
  // See if pump should be triggered
  // See if moisture is below or equal to threshold
  if (soilMoisturePercent <= pump_trigger) {
    // Turn pump on
    pumpOn();
 
  } else {
    // Turn pump off
    pumpOff();
  }
  
  // Cycle values on OLED Display
  // Pump Status
  printOLED(35, "PUMP", 40, pump_status_text, 2000);
  // Temperature
  printOLED(35, "TEMP", 10, String(temp) + "C", 2000);
  // Humidity
  printOLED(30, "HUMID", 10, String(hum) + "%", 2000);
  // Moisture
  printOLED(35, "MOIST", 30, String(soilMoisturePercent) + "%", 2000);
 
 
}
 
void pumpOn() {
  // Turn pump on
  digitalWrite(RELAY_OUT, HIGH);
  pump_status_text = "ON";

 
}
 
void pumpOff() {
  // Turn pump off
  digitalWrite(RELAY_OUT, LOW);
  pump_status_text = "OFF";
 
 
}
 
void printOLED(int top_cursor, String top_text, int main_cursor, String main_text, int delay_time){
  // Prints to OLED and holds display for delay_time
  display.setCursor(top_cursor, 0);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.println(top_text);
 
  display.setCursor(main_cursor, 40);
  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.print(main_text);
  display.display();
 
  delay(delay_time);
  display.clearDisplay();
 
}
 
