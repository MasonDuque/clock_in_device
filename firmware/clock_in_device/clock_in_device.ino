#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <SD.h>

// =========================
// LCD SETUP
// =========================

LiquidCrystal_I2C lcd(0x27, 20, 4);

// =========================
// BUTTON PINS
// =========================

const int button1 = 1;
const int button2 = 2;
const int button3 = 3;
const int button4 = 4;
const int button5 = 5;
const int button6 = 6;

// =========================
// OUTPUT PINS
// =========================

const int greenLED = 15;
const int redLED = 16;
const int buzzerPin = 21;

// =========================
// SD CARD
// =========================

const int chipSelect = 10;

// =========================
// SETUP
// =========================

void setup() {

  Serial.begin(115200);

  // I2C LCD
  Wire.begin(9, 8);

  lcd.init();
  lcd.backlight();

  // Buttons
  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);
  pinMode(button3, INPUT_PULLUP);
  pinMode(button4, INPUT_PULLUP);
  pinMode(button5, INPUT_PULLUP);
  pinMode(button6, INPUT_PULLUP);

  // LEDs
  pinMode(greenLED, OUTPUT);
  pinMode(redLED, OUTPUT);

  // Buzzer
  pinMode(buzzerPin, OUTPUT);

  // Startup screen
  lcd.setCursor(0, 0);
  lcd.print("Project Tracker");

  lcd.setCursor(0, 1);
  lcd.print("Initializing SD");

  // =========================
  // SD CARD TEST
  // =========================

  SPI.begin(13, 12, 11, 10);

  if (SD.begin(chipSelect)) {

    lcd.setCursor(0, 2);
    lcd.print("SD Card OK      ");

    // Create test file
    File testFile = SD.open("/test.txt", FILE_WRITE);

    if (testFile) {

      testFile.println("ESP32-S3 SD Test");
      testFile.close();

      lcd.setCursor(0, 3);
      lcd.print("File Written OK ");

    } else {

      lcd.setCursor(0, 3);
      lcd.print("Write Failed    ");
    }

  } else {

    lcd.setCursor(0, 2);
    lcd.print("SD Card FAILED  ");
  }

  // Startup chirp
  tone(buzzerPin, 1200, 100);

  delay(2000);

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Press Buttons");
}

// =========================
// LOOP
// =========================

void loop() {

  checkButton(button1, 1);
  checkButton(button2, 2);
  checkButton(button3, 3);
  checkButton(button4, 4);
  checkButton(button5, 5);
  checkButton(button6, 6);
}

// =========================
// BUTTON HANDLER
// =========================

void checkButton(int pin, int number) {

  if (digitalRead(pin) == LOW) {

    // LCD Display
    lcd.setCursor(0, 1);
    lcd.print("Button ");
    lcd.print(number);
    lcd.print(" Pressed   ");

    // =========================
    // EVEN BUTTONS
    // =========================

    if (number % 2 == 0) {

      digitalWrite(greenLED, HIGH);
      digitalWrite(redLED, LOW);

      lcd.setCursor(0, 2);
      lcd.print("GREEN LED       ");

      tone(buzzerPin, 1200, 80);

    }

    // =========================
    // ODD BUTTONS
    // =========================

    else {

      digitalWrite(redLED, HIGH);
      digitalWrite(greenLED, LOW);

      lcd.setCursor(0, 2);
      lcd.print("RED LED         ");

      tone(buzzerPin, 800, 80);
    }

    // =========================
    // LOG BUTTON PRESS TO SD
    // =========================

    File logFile = SD.open("/log.txt", FILE_APPEND);

    if (logFile) {

      logFile.print("Button ");
      logFile.print(number);
      logFile.println(" pressed");

      logFile.close();

      Serial.print("Logged Button ");
      Serial.println(number);

      lcd.setCursor(0, 3);
      lcd.print("Logged to SD    ");

    } else {

      lcd.setCursor(0, 3);
      lcd.print("SD Write Error  ");
    }

    delay(250);

    // Turn LEDs off after press
    digitalWrite(greenLED, LOW);
    digitalWrite(redLED, LOW);
  }
}
