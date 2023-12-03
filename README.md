# Proyecto-Maquina-Expendedora-Empotrados
The purpose of this code is to create a simple implementation of a coffe machine, that can be interacted using a joystick and reprogramed on the spot using an admin portal.
## Hardware
For this proyect we are going to need:
1. LCD Display:

    A 16x2 LCD is used to display information and messages to the user.

2. Joystick:

    The joystick is used for navigation. The X and Y analog values are read to determine the movement direction, and a button press is used for selection.

3. Ultrasonic Sensor:

    An ultrasonic sensor is used to measure distance (proximity) from an object, likely to detect the presence of a user.

4. LEDs:

    Two LEDs (LED1 and LED2) are used for visual feedback or indication.

5. DHT Sensor:

    A DHT11 sensor is used to measure temperature and humidity.
   
7. Button:

    A button to enter admin mode
  
8. Potentiometer:
   
    To control the dimness of the LCD screen

10. Arduino UNO:
   
    You can also use a MEGA board or similar, but any board smaller than a UNO wont have enough pins to follow this tutorial and will require more tinkering.
    
11. Breadboard:
    
    A big one fit all our components comfortably

### Wiring
We reccomend wiring and testing each component individually using the sources at the end of this README, and slowly adding components to the breadboard to avoid confusion.

Our wiring diagram ended looking like this:
[ADD PICTURE]

## Software
This program works by using a state machine and some variables to keep track of the position it shuld be in. This allows the main loop to cicle through iself quickly and reduce latency.
### 0. Variables and void setup():

This part is simple as all it does is initialize variables, the pins as input/output and some needed libraries.
The most remarkable part is the creation of a special character for the LCD display, because the UTF character for the EUR can't be directly imported.
```cpp
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
```

### 1. Main Loop (loop()):

The main loop directs the program flow through different states: Initialization, Service, and Administration. A switch-case is used to determine the current state and call the corresponding methods.

```cpp

void loop() {
  switch (state) {
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
      lcd.print("¡¿CÓMO?!");
  }
}
```
## 2. Initialization (Arranque()):

This is the first method called. It manages the machine's startup phase. It displays the "LOADING..." message on the LCD and performs a sequence of LED1 flashes to indicate initialization. The generous use of delay does not damage the reactivity of this program as this first state requires no extra input or change until it's finished.

```cpp

void Arranque() {
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
}
```
## 3. Service (Servicio()):

In this method, the temperature and humidity are displayed on the LCD for a specified time. Then, the distance is checked using an ultrasonic sensor. If a user is nearby, it calls another function to display the menu; otherwise, it waits.


The temperature only shows if it's the first time the loop is called.
```cpp
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
}
```
Otherwise it just checks the distance. If it shows that the user is away from the machine for more than 2 seconds it displays the "ESPERANDO CLIENTE..." message. This wait is done to limit the influence of erroneous readings that the sensor gives periodically.
```cpp
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
```
## 4. Administración (Admin()):

Allows the user to manage the machine using the joystick. Options include viewing temperature and humidity, distance, the time since booting up, and modifying prices. Selection is made with the joystick button.

A switch state is used to keep track of which options to display, and after pressing the joystick button, a method is called to bring the functionality of the option, this way adding options is simple and straightforward.

```cpp
  switch (selectedOption) {
    case 1:
      verTemperatura();
      selectedOption = 0; //Set to cero to avoid reentry to the switch
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
```
## 5. Menú (menu()):

In the service state, this method displays on the LCD the coffee options with their respective prices. Additionally, it gradually increases the brightness of LED1 while preparing coffee and requests the user to remove the drink after preparation.

The first part is the menu display
```cpp
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
```
Next we read the values from the joystick to move between options,
```cpp
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
```

And if the button is pressed, we start "serving" the coffe, while slowly brightening up the LED. At this point in the code, we can also add an extra water pump to serve liquid as I mention in the video.
But for the time being it's outside of the scope for this proyect.
```cpp
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
```
## 6. View Temperature, Distance, and Counter (verTemperatura(), verDistancia(), verContador()):

These methods allow the user to view real-time information (temperature, distance, or counter). The simplest methods called by Admin are simple sensor readings, to exit them we simply move the joystick to the left.

```cpp

void verTemperatura() {
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

void verDistancia() {
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

void verContador() {
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
```
## 7. Modify Prices (modificarPrecios()):

In this method, the user can modify the prices of products using the joystick. It displays the current prices and updates them in real-time. 
Move the joystick to the right to cicle between products, change prices moving it up and down, and discard changes moving it to the left. The modification are confirmed by pressing the joystick button.

```cpp
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
```
## 8. Interrupt Handling (handleInterrupt()):

This method handles hardware interruptions generated by a physical button. It detects the pulse duration and performs specific actions based on the duration. It is used to switch between states and restart the machine.

```cpp
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
```
## Sources
[LCD Screen](https://docs.arduino.cc/learn/electronics/lcd-displays)

[Joystick](https://arduinogetstarted.com/tutorials/arduino-joystick)

[Ultrasonic sensor](https://arduinogetstarted.com/tutorials/arduino-ultrasonic-sensor)

[Temperature/Humidity sensor - code](https://arduinogetstarted.com/tutorials/arduino-temperature-humidity-sensor)

[Temperature/Humidity sensor - wiring](https://d3dzre54m4viff.cloudfront.net/wp-content/uploads/2015/10/02103026/Arduino-DHT11-3.jpg)
