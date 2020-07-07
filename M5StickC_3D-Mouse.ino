#include <Arduino.h>
#include <M5StickC.h>
#include <BleMouse.h>
BleMouse bleMouse;
float accX = 0;
float accY = 0;
float accZ = 0;


boolean isActivated = true;
boolean buttonIsPressed = false;
boolean tipped = false;
boolean led = false;

int puffer = 0;

String lcdText = " ";
int x = 0;
int y = 0;
int z = 0;

byte counter = 0;

void writeText(String _Text, int _ColorText) {
  M5.Lcd.setTextColor(BLACK);
  M5.Lcd.setCursor(0, 80);
  M5.Lcd.println(lcdText);
  M5.Lcd.setTextColor(_ColorText);
  M5.Lcd.setCursor(0, 80);
  M5.Lcd.println(_Text);
  lcdText = _Text;
}

void printXY (int _ColorText) {
  M5.Lcd.setTextColor(_ColorText);
  M5.Lcd.setCursor(0, 100);
  M5.Lcd.printf("%d   ", x);
  M5.Lcd.setCursor(0, 120);
  M5.Lcd.printf("%d   ", y);
  M5.Lcd.setCursor(0, 140);
  M5.Lcd.printf("%d   ", z);
}

void setup() {
  pinMode(M5_LED, OUTPUT);
  digitalWrite(M5_LED, HIGH);
  bleMouse.begin();
  M5.begin();
  M5.Lcd.setRotation(2);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextSize(1);
  writeText("3DMaus ein", WHITE);
  M5.MPU6886.Init();
  M5.MPU6886.SetGyroFsr(M5.MPU6886.GFS_250DPS);
  M5.MPU6886.SetAccelFsr(M5.MPU6886.AFS_2G);
}

void loop() {
  if (digitalRead(M5_BUTTON_HOME) == LOW) {
    if (!isActivated) {
      isActivated = true;
      writeText("3DMaus ein", WHITE);
    } else {
      isActivated = false;
      writeText("3DMaus aus", RED);
      bleMouse.release(MOUSE_MIDDLE);
    }
    while (digitalRead(M5_BUTTON_HOME) == LOW) {
    }
  }
  if (bleMouse.isConnected() && isActivated) {
    printXY(BLACK);
    M5.MPU6886.getAccelData(&accX, &accY, &accZ);
    x = accX * 15;
    y = accY * 15;
    if ( accZ * 100 > 110)
      z += 1;
    else {
      if (z > 1)
        z--;
    }
    if (z > 25 && !tipped) {
      tipped = true;

      if (led) {
        digitalWrite(M5_LED, LOW);
        led = false;
      }
      else {
        digitalWrite(M5_LED, HIGH);
        led = true;
      }
    }
    if (z < 5 && tipped) {
      tipped = false;
    }
    printXY(WHITE);
    if (abs(x) + abs(y) == 0 && buttonIsPressed) {
      if (puffer < 1) {
        writeText("3DMaus ein", BLUE);
        bleMouse.release(MOUSE_MIDDLE);
        buttonIsPressed = false;
      } else
        puffer--;
    }
    if (abs(x) > 0 ||  abs(y) > 0 ) {
      writeText("3DMaus ein", GREEN);
      puffer = 100;
      if (!buttonIsPressed && !led) {
        bleMouse.press(MOUSE_MIDDLE);
        buttonIsPressed = true;
      }
      bleMouse.move(x, -y);
    }
    delay(2);
  }
}

