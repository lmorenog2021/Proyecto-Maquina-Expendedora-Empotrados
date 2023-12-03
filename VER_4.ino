#include <avr/wdt.h>
#include <LiquidCrystal.h>
#include "DHT.h"
#define VRX_PIN  A0 // Arduino pin connected to VRX pin
#define VRY_PIN  A1 // Arduino pin connected to VRY pin
#define DHTPIN 13
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

//LCD Components
const int rs = 12, en = 11, d4 = 6, d5 = 5, d6 = 4, d7 = 3;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
byte customEUR[] = {
  B00000,
  B00111,
  B01000,
  B11111,
  B01000,
  B11111,
  B01000,
  B00111
};

//Ultrasound sensor components
int trigPin = 9;    // TRIG pin
int echoPin = 8;    // ECHO pin
float duration_us, distance_cm;

//Joystick components
int xValue = 0; // To store value of the X axis
int yValue = 0; // To store value of the Y axis
int bValue;
int SW_pin = 7;

//LED components
int LED1 = 10;
int LED2 = 19; //AnalogPin A5

//Humidity sensor components
float humidity;
float tempC;

//States Components
volatile int state = 0;
int menuOption = 1; // Initial menu option
int selectedOption = 0;
unsigned long currentMillis = 0;
unsigned long previousMillis = 0;  // Variable to store the last time the user was farther than 1m
const long exitDelay = 2000;      // Delay in milliseconds (2 seconds)
const long tempDelay = 5000;      // Delay in milliseconds (5 seconds)
int firstLoop = 1;

//Interruption components
const int buttonPin = 2;
volatile unsigned long pressTime = 0;
volatile unsigned long releaseTime = 0;

//Prices for products
float cafSolo = 1;
float cafCortado = 1.10;
float cafDoble = 1.25;
float cafPremium = 1.5;
float chocolate = 2;
//New prices for products
float cafSoloNew;
float cafCortadoNew;
float cafDobleNew;
float cafPremiumNew;
float chocolateNew;


void setup() {
  Serial.begin(9600);
  //Set up the LCD's number of columns and rows, create the custom char and turn on the cursor:
  lcd.createChar(0, customEUR);
  lcd.begin(16, 2);
  lcd.cursor();

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(SW_pin, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(buttonPin), handleInterrupt, CHANGE);

  dht.begin();
}

void loop() {
  wdt_disable();
  wdt_enable(WDTO_8S);
  switch(state){
    case 0:
      Arranque();
      state = 1;
      break;
    case 1:
      Servicio();
      break;
    case 2:
      Admin();
      break;
    default:
    lcd.print("HOW?!?!");
  }
}

void Arranque(){
  lcd.print("CARGANDO...");
  // Leds Parpadean tres veces
  digitalWrite(LED2, HIGH);
  delay(500);
  digitalWrite(LED2, LOW);
  delay(500);
  digitalWrite(LED2, HIGH);
  delay(500);
  digitalWrite(LED2, LOW);
  delay(500);
  digitalWrite(LED2, HIGH);
  delay(500);
  digitalWrite(LED2, LOW);
  delay(500);
  //Leds Apagan
}

void Servicio(){
  
  if(firstLoop){
    previousMillis = millis();
    currentMillis = millis();
    while(currentMillis - previousMillis < tempDelay){
      humidity = dht.readHumidity();
      tempC = dht.readTemperature();
      lcd.clear();
      lcd.print("Temp: ");
      lcd.print(tempC, 2);
      lcd.print("C");
      lcd.setCursor(0, 1);
      lcd.print("Hum: ");
      lcd.print(humidity, 2);
      lcd.print("%");
      currentMillis = millis();
      delay(200);
    }
    previousMillis = 0;
    firstLoop = 0;
  }

  distance_cm = distanceSensor();
  if(distance_cm > 100){
    currentMillis = millis();
    if (currentMillis - previousMillis >= exitDelay){
      lcd.clear();
      lcd.print("ESPERANDO");
      lcd.setCursor(0, 1);
      lcd.print("CLIENTE");
      delay(300);
    }
  }else{
    previousMillis = millis();  // Update the last time the user was within 1m
    menu();
  }

}

void Admin(){
  delay(100);
  lcd.clear();
  lcd.setCursor(0, 0);
  // read analog X and Y analog values
  xValue = analogRead(VRX_PIN);
  yValue = analogRead(VRY_PIN);
  bValue = digitalRead(SW_pin);

  switch (menuOption) {
    case 1:
      lcd.print("i.  Ver");
      lcd.setCursor(5, 1);
      lcd.print("temperatura");
      break;
    case 2:
      lcd.print("ii. Ver");
      lcd.setCursor(5, 1);
      lcd.print("distancia");
      break;
    case 3:
      lcd.print("iii.Ver");
      lcd.setCursor(5, 1);
      lcd.print("contador");
      break;
    case 4:
      lcd.print("iv. Modificar");
      lcd.setCursor(5, 1);
      lcd.print("precios");
      break;
  }

  if (yValue < 100) {
    menuOption++;
    if (menuOption > 4) {
      menuOption = 1;
    }
    delay(300); // Add a small delay to avoid rapid changes
  }else if (yValue > 900) {
    menuOption--;
    if (menuOption < 1) {
      menuOption = 4;
    }
    delay(300);
  }

  // Button press to select menu option
  if (bValue == LOW) {
    // Add code to perform the selected menu option
    selectedOption = menuOption;
  }
  switch (selectedOption) {
    case 1:
      verTemperatura();
      selectedOption = 0;
      break;
    case 2:
      verDistancia();
      selectedOption = 0;
      break;
    case 3:
      verContador();
      selectedOption = 0;
      break;
    case 4:
      modificarPrecios();
      selectedOption = 0;
      break;
  }
}

void menu(){
  lcd.clear();
  lcd.setCursor(0, 0);
  // read analog X and Y analog values
  xValue = analogRead(VRX_PIN);
  yValue = analogRead(VRY_PIN);
  bValue = digitalRead(SW_pin);

  switch (menuOption) {
    case 1:
      lcd.print("i.  Cafe Solo");
      lcd.setCursor(11, 1);
      lcd.print(cafSolo, 2);  // Display with 2 decimal places
      lcd.write(byte(0));
      break;
    case 2:
      lcd.print("ii. Cafe Cortado");
      lcd.setCursor(11, 1);
      lcd.print(cafCortado, 2);  // Display with 2 decimal places
      lcd.write(byte(0));
      break;
    case 3:
      lcd.print("iii.Cafe Doble");
      lcd.setCursor(11, 1);
      lcd.print(cafDoble, 2);  // Display with 2 decimal places
      lcd.write(byte(0));
      break;
    case 4:
      lcd.print("iv. Cafe Premium");
      lcd.setCursor(11, 1);
      lcd.print(cafPremium, 2);  // Display with 2 decimal places
      lcd.write(byte(0));
      break;
    case 5:
      lcd.print("v.  Chocolate");
      lcd.setCursor(11, 1);
      lcd.print(chocolate, 2);  // Display with 2 decimal places
      lcd.write(byte(0));
      break;
  }

  if (yValue < 100) {
    menuOption++;
    if (menuOption > 5) {
      menuOption = 1;
    }
    delay(300);
  }else if (yValue > 900) {
    menuOption--;
    if (menuOption < 1) {
      menuOption = 5;
    }
    delay(300); // Add a small delay to avoid rapid changes
  }

  // Button press to select menu option
  if (bValue == LOW) {
    // Add code to perform the selected menu option
    lcd.clear();
    lcd.print("Preparando");
    lcd.setCursor(0, 1);
    lcd.print("Cafe...");

    previousMillis = millis();
    currentMillis = 0;
    while (currentMillis < random(4000, 8001)) {
      int brightness = map(currentMillis, 0, random(4000, 8001), 0, 255);
      analogWrite(LED1, brightness);
      currentMillis = millis() - previousMillis;
    }
    digitalWrite(LED1, LOW);
    lcd.clear();
    lcd.print("RETIRE BEBIDA");
    delay(3000);
  }
}

float distanceSensor(){
  // generate 10-microsecond pulse to TRIG pin
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // measure duration of pulse from ECHO pin
  duration_us = pulseIn(echoPin, HIGH);

  // calculate the distance
  return(0.017 * duration_us); 
}

void verTemperatura(){
  while(xValue > 100){
    wdt_reset();
    humidity = dht.readHumidity();
    tempC = dht.readTemperature();
    xValue = analogRead(VRX_PIN);

    lcd.clear();
    lcd.print("Temp: ");
    lcd.print(tempC, 2);
    lcd.print("C");
    lcd.setCursor(0, 1);
    lcd.print("Hum: ");
    lcd.print(humidity, 2);
    lcd.print("%");
    currentMillis = millis();
    delay(200);
  }
}
void verDistancia(){
  while(xValue > 100){
    wdt_reset();
    distance_cm = distanceSensor();
    xValue = analogRead(VRX_PIN);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Distancia:");

    lcd.setCursor(0, 1);
    lcd.print(distance_cm, 2);  // Display with 2 decimal places
    lcd.setCursor(13,1);
    lcd.print("cm");
    delay(200);
  }
}
void verContador(){
  while(xValue > 100){
    wdt_reset();
    unsigned long elapsedTime = millis() / 1000;
    xValue = analogRead(VRX_PIN);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Tiempo pasado:");

    lcd.setCursor(0, 1);
    lcd.print(elapsedTime);
    lcd.print(" secs");
    delay(200);
  }
}
void modificarPrecios(){
  cafSoloNew = cafSolo;
  cafCortadoNew = cafCortado;
  cafDobleNew = cafDoble;
  cafPremiumNew = cafPremium;
  chocolateNew = chocolate;
  menuOption = 1;

  while(xValue > 100){
    wdt_reset();
    delay(200);
    lcd.clear();
    lcd.setCursor(0, 0);
    // read analog X and Y analog values
    xValue = analogRead(VRX_PIN);
    yValue = analogRead(VRY_PIN);
    bValue = digitalRead(SW_pin);

    switch (menuOption) {
      case 1:
        lcd.print("i.  Cafe Solo");
        lcd.setCursor(11, 1);
        lcd.print(cafSoloNew, 2);  // Display with 2 decimal places
        lcd.write(byte(0));
        if (yValue < 100) {
          cafSoloNew = cafSoloNew - 0.05;
          delay(300);
        } else if (yValue > 900) {
          cafSoloNew = cafSoloNew + 0.05;  // Corrected the variable to update
          delay(300);
        }
        break;
      case 2:
        lcd.print("ii. Cafe Cortado");
        lcd.setCursor(11, 1);
        lcd.print(cafCortadoNew, 2);  // Display with 2 decimal places
        lcd.write(byte(0));
        if (yValue < 100) {
          cafCortadoNew = cafCortadoNew - 0.05;
          delay(300);
        } else if (yValue > 900) {
          cafCortadoNew = cafCortadoNew + 0.05;  // Corrected the variable to update
          delay(300);
        }
        break;
      case 3:
        lcd.print("iii.Cafe Doble");
        lcd.setCursor(11, 1);
        lcd.print(cafDobleNew, 2);  // Display with 2 decimal places
        lcd.write(byte(0));
        if (yValue < 100) {
          cafDobleNew = cafDobleNew - 0.05;
          delay(300);
        } else if (yValue > 900) {
          cafDobleNew = cafDobleNew + 0.05;  // Corrected the variable to update
          delay(300);
        }
        break;
      case 4:
        lcd.print("iv. Cafe Premium");
        lcd.setCursor(11, 1);
        lcd.print(cafPremiumNew, 2);  // Display with 2 decimal places
        lcd.write(byte(0));
        if (yValue < 100) {
          cafPremiumNew = cafPremiumNew - 0.05;
          delay(300);
        } else if (yValue > 900) {
          cafPremiumNew = cafPremiumNew + 0.05;  // Corrected the variable to update
          delay(300);
        }
        break;
      case 5:
        lcd.print("v.  Chocolate");
        lcd.setCursor(11, 1);
        lcd.print(chocolateNew, 2);  // Display with 2 decimal places
        lcd.write(byte(0));
        if (yValue < 100) {
          chocolateNew = chocolateNew - 0.05;
          delay(300);
        } else if (yValue > 900) {
          chocolateNew = chocolateNew + 0.05;  // Corrected the variable to update
          delay(300);
        }
        break;
      }
  
    if (xValue > 900) {
      menuOption++;
      if (menuOption > 5) {
        menuOption = 1;
      }
      delay(300);
    }
    if (bValue == LOW) {
      cafSolo = cafSoloNew;
      cafCortado = cafCortadoNew;
      cafDoble = cafDobleNew;
      cafPremium = cafPremiumNew;
      chocolate = chocolateNew;
      break;
    }
  }
}

void handleInterrupt() {
  unsigned long currentTime = millis();

  if (digitalRead(buttonPin) == HIGH) {
    pressTime = currentTime;
  }

  if (digitalRead(buttonPin) == LOW) {
    releaseTime = currentTime;

    // Debounce the button
    if (releaseTime - pressTime >= 5000) {
      // Valid button press duration
      if (state == 1) {
        state = 2;
      } else if (state == 2) {
        state = 1;
      }
    } else if ((releaseTime - pressTime >= 2000) && (releaseTime - pressTime <= 3000)){
      firstLoop = 1;
    }
  }
}
