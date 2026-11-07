#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "BluetoothSerial.h"

#define TRIG 5
#define ECHO 18
#define MAXD 40.0f

Adafruit_SSD1306 d(128, 64, &Wire, -1);
BluetoothSerial bt;

float dist = 99;
float prevDist = 99;
float birdY = 40;
unsigned long jumpFlash = 0;
unsigned long lastDraw = 0;
unsigned long jumps = 0;
bool btOn = false;

float readOnce() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(3);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  unsigned long t = pulseIn(ECHO, HIGH, 12000);
  if (t == 0) return -1;
  return t * 0.0343f / 2.0f;
}

float readMedian() {
  float v[3];
  int n = 0;
  for (int i = 0; i < 3; i++) {
    float r = readOnce();
    if (r > 0) v[n++] = r;
    delay(3);
  }
  if (n == 0) return -1;
  if (n == 1) return v[0];
  if (n == 2) return (v[0] + v[1]) / 2;
  if (v[0] > v[1]) { float t = v[0]; v[0] = v[1]; v[1] = t; }
  if (v[1] > v[2]) { float t = v[1]; v[1] = v[2]; v[2] = t; }
  if (v[0] > v[1]) { float t = v[0]; v[0] = v[1]; v[1] = t; }
  return v[1];
}

void drawBT(int x, int y, int s, uint16_t col) {
  d.drawLine(x + 3 * s, y, x + 3 * s, y + 9 * s, col);
  d.drawLine(x + 3 * s, y, x + 6 * s, y + 2 * s, col);
  d.drawLine(x + 6 * s, y + 2 * s, x, y + 7 * s, col);
  d.drawLine(x, y + 2 * s, x + 6 * s, y + 7 * s, col);
  d.drawLine(x + 6 * s, y + 7 * s, x + 3 * s, y + 9 * s, col);
}

void centerText(const char* s, int y, int size) {
  int w = strlen(s) * 6 * size;
  d.setTextSize(size);
  d.setCursor((128 - w) / 2, y);
  d.print(s);
}

void splash() {
  d.clearDisplay();
  d.setTextColor(WHITE);
  d.drawRoundRect(0, 0, 128, 64, 6, WHITE);
  centerText("ECHO FLAP", 12, 2);
  centerText("ULTRASONIC CONTROLLER", 36, 1);
  centerText("TechTadka360", 50, 1);
  d.display();
  delay(1500);
}

void drawBird(int cx, int cy, bool up) {
  d.fillCircle(cx, cy, 6, WHITE);
  d.fillCircle(cx + 2, cy - 2, 2, BLACK);
  d.fillTriangle(cx + 6, cy - 1, cx + 13, cy + 1, cx + 6, cy + 3, WHITE);
  if (up) d.drawLine(cx - 5, cy, cx - 1, cy - 4, BLACK);
  else d.drawLine(cx - 5, cy + 1, cx - 1, cy + 4, BLACK);
}

void drawScreen(bool jumping) {
  float cl = dist < 0 ? MAXD : (dist > MAXD ? MAXD : dist);
  float target = 60 - (1.0f - cl / MAXD) * 40;
  birdY += (target - birdY) * 0.4f;

  d.clearDisplay();
  d.setTextColor(BLACK);
  d.fillRect(0, 0, 128, 12, WHITE);
  d.setTextSize(1);
  d.setCursor(3, 2);
  d.print("TECHTADKA360");
  if (btOn) {
    drawBT(117, 1, 1, BLACK);
  } else {
    if ((millis() / 500) % 2 == 0) drawBT(117, 1, 1, BLACK);
  }
  d.setTextColor(WHITE);

  char buf[8];
  if (dist >= 0 && dist < 99) snprintf(buf, sizeof(buf), "%d", (int)dist);
  else snprintf(buf, sizeof(buf), "--");
  d.setTextSize(3);
  d.setCursor(4, 17);
  d.print(buf);
  d.setTextSize(1);
  d.setCursor(4 + strlen(buf) * 18 + 2, 34);
  d.print("cm");

  d.setCursor(4, 44);
  d.print(btOn ? "BT LINKED" : "BT WAITING");
  d.drawRect(4, 54, 68, 8, WHITE);
  int w = (int)((1.0f - cl / MAXD) * 64);
  if (w > 0) d.fillRect(6, 56, w, 4, WHITE);

  d.drawLine(77, 15, 77, 63, WHITE);

  d.drawLine(120, 16, 120, 62, WHITE);
  d.drawLine(118, 16, 122, 16, WHITE);
  d.drawLine(118, 62, 122, 62, WHITE);

  if (jumping) {
    d.fillTriangle(84, 17, 79, 25, 89, 25, WHITE);
    d.fillRect(83, 25, 3, 8, WHITE);
  }
  drawBird(100, (int)birdY, jumping);

  if (jumping) {
    d.fillRoundRect(82, 52, 34, 11, 3, WHITE);
    d.setTextColor(BLACK);
    d.setCursor(88, 54);
    d.print("JUMP");
  } else {
    d.drawRoundRect(82, 52, 34, 11, 3, WHITE);
    d.setCursor(86, 54);
    d.print("READY");
  }
  d.setTextColor(WHITE);
  d.display();
}

void setup() {
  Serial.begin(115200);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  Wire.begin(21, 22);
  Wire.setClock(400000);
  d.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  bt.begin("TT360 Flappy");
  splash();
}

void loop() {
  dist = readMedian();
  btOn = bt.hasClient();

  if (dist > 0) {
    Serial.print("D:");
    Serial.println(dist, 1);
    if (btOn) {
      bt.print("D:");
      bt.println(dist, 1);
    }
    if (prevDist < 90 && prevDist - dist > 3.0f) {
      jumpFlash = millis();
      jumps++;
    }
    prevDist = dist;
  } else {
    prevDist = 99;
  }

  unsigned long now = millis();
  if (now - lastDraw > 60) {
    lastDraw = now;
    drawScreen(now - jumpFlash < 250);
  }
}
