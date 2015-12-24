#include <Q2Balance.h>
#include <Q2HX711.h>

const byte hx711_data_pin = A2;
const byte hx711_clock_pin = A3;
const byte hx711_clock_gain = 64;

const byte tare_button_pin = 2;
const byte zero_button_pin = 3;
const byte calb_button_pin = 4;

Q2HX711 hx711(hx711_data_pin, hx711_clock_pin);
Q2Balance balance = Q2Balance();

void setup() {
  Serial.begin(9600);
  pinMode(tare_button_pin, INPUT);
  digitalWrite(tare_button_pin, HIGH);
  pinMode(zero_button_pin, INPUT);
  digitalWrite(zero_button_pin, HIGH);
  pinMode(calb_button_pin, INPUT);
  digitalWrite(calb_button_pin, HIGH);
}


void loop() {
  balance.measure(hx711.read());
  balance.tick();
  if(digitalRead(tare_button_pin) == LOW)
  {
    Serial.println("taring");  
    balance.tare(100);
  }

  if(digitalRead(zero_button_pin) == LOW)
  {
    Serial.println("calibrating zero weight");  
    balance.calibrateZero(100);
  }

  if(digitalRead(calb_button_pin) == LOW)
  {
    Serial.println("calibrating 100g weight");  
    balance.calibrate(2,100,1000);
  }

  char str_temp[9];
  char buffer[100];
  dtostrf(balance.adjustedValue(), 6, 2, str_temp);  
  sprintf(buffer, "Weight: %s %s", str_temp, balance.tared() ? "[tared]" :"");
  Serial.println(buffer);  
  delay(500);
}



