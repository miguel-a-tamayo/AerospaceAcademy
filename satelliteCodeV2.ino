#include <LiquidCrystal.h>
#include<DHT.h>

#define DHTTYPE DHT11   // DHT 11 type
#define DELAY 1000 // time delay for data taking
#define BTN_DELAY 500

enum states {
  MOVE = 0,
  RECEIVE
};

uint8_t currentState = MOVE; // initial state

const int DHT_pin = 7; // DHT pin
DHT dht(DHT_pin, DHTTYPE); // DHT sensor declaration

// rgb pins             R  G  B
const int rgb_pins[] = {2, 3, 4};

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 8, en = 9, d4 = 10, d5 = 11, d6 = 12, d7 = 13;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
bool clearFlag = false; // flag to clear screen

// button pins
const int btnL = 5;
const int btnR = 6;
long pastBtn = 0;


long pastTime = 0;

int altitude = 2000; // altitude of satellite
int received = 0; // check if we're done receiving data

void setup() {
  lcd.begin(16, 2); // set up LCD number of columns and rows
  Serial.begin(9600); // baud rate

  pinMode(btnL, INPUT);
  pinMode(btnR, INPUT);
  
  dht.begin(); // start DHT module

  for (int i = 0; i < 2; i++) { // set rbg as outputs
    pinMode(rgb_pins[i], OUTPUT);
  }
}

void loop() {
  float t = dht.readTemperature(true); // read temperature
  
  int lread = digitalRead(btnL);
  int rread = digitalRead(btnR);

  if (lread && rread && (millis() - pastBtn > BTN_DELAY)) {
    lcd.clear();
    pastBtn = millis();
    currentState ^= 1;
  };

  switch(currentState) {
    case MOVE:
      moveSat(lread, rread);
      break;

    case RECEIVE:
      receiveData();
      break;
  }
  
}

void moveSat(int lread, int rread) {

  if (rread && (millis() - pastBtn > BTN_DELAY) && altitude < 4000){
    pastBtn = millis();
    altitude += 100;
  }

  if (lread && (millis() - pastBtn > BTN_DELAY) && altitude > 1000){
    pastBtn = millis();
    altitude -= 100;
  }

  if (altitude < 1500) {
    rgb_color(255, 0, 0);
  }
  else if (altitude > 2500) {
    rgb_color(0, 0, 255);
  }
  else {
    rgb_color(0, 255, 0);
  }
  
  // update LCd screen
  lcd.print("Press L or R");
  lcd.setCursor(0, 1);
  lcd.print("Altitude: ");
  lcd.print(altitude);
  lcd.print("km");
}


/*
 * receiveData()
 * receive state function
 * prints temperature sensor to the LCD screen
 */
void receiveData() {

  if (millis() - pastTime > DELAY) { // take data sample every second
    pastTime = millis(); // update last time we took measurement
    float h = dht.readHumidity();
    float t = dht.readTemperature(true);

    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.setCursor(6, 0);
    lcd.print((int)t);
    lcd.setCursor(0, 1);
    lcd.print("Humidity: ");
    lcd.print((int)h);
    lcd.print("%");
  }
}

/*
 * rgb_color()
 * function to send colors to the RGB led
 * module
 */
void rgb_color(int r, int g, int b) {
  digitalWrite(rgb_pins[0], r);
  digitalWrite(rgb_pins[1], g);
  digitalWrite(rgb_pins[2], b);
}
