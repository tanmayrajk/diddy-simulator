#include <Arduino.h>
#include <TFT_eSPI.h>
#include <bg.h>
#include <random>
#include <vector>
#include <babyoil.h>
#include <police.h>
#include <diddy_left.h>
#include <diddy_right.h>
#include <Bounce2.h>

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite img = TFT_eSprite(&tft);
TFT_eSprite player = TFT_eSprite(&tft);

#define LEFT_BTN 25
#define RIGHT_BTN 26
#define BUZZER_PIN 33

Bounce leftBtnDebouncer = Bounce();
Bounce rightBtnDebouncer = Bounce();

struct Pos {
  float x;
  float y;
};

struct Size {
  int w;
  int h;
};

struct Oil {
  Size size;
  Pos pos;
  float speed;
};

struct Police {
  Size size;
  Pos pos;
  float speed;
};

std::vector<Oil> oils;
std::vector<Police> police;

unsigned long lastTime = millis();

Pos playerPos = { 10, 70 };
Size playerSize = { 30, 51 };
int playerDirection = 1; // 1 = right, -1 = left;
int playerSpeed = 4;
bool isMoving = false;

int score = 0;
int highScore = 0;
bool isGameRunning = false;

int getRandomNumber(int min, int max) {
  static std::random_device rd;           
  static std::mt19937 gen(rd());
  std::uniform_int_distribution<> distr(min, max);
  return distr(gen);
}

bool getBiasedRandom() {
  static std::random_device rd;
  static std::mt19937 gen(rd());
  std::bernoulli_distribution dist(0.3);
  return dist(gen);
}

void setup() {
  Serial.begin(115200);

  pinMode(LEFT_BTN, INPUT_PULLUP);
  pinMode(RIGHT_BTN, INPUT_PULLUP);

  leftBtnDebouncer.attach(LEFT_BTN);
  leftBtnDebouncer.interval(25);

  rightBtnDebouncer.attach(RIGHT_BTN);
  rightBtnDebouncer.interval(25);

  pinMode(BUZZER_PIN, OUTPUT);

  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_GREEN);

  img.createSprite(160, 128);
  img.setSwapBytes(true);
  img.pushImage(0, 0, 160, 128, bg);
  img.pushSprite(0, 0);

  player.createSprite(30, 51);
  player.setSwapBytes(false);
  player.pushImage(0, 0, 30, 51, playerDirection == 1 ? diddy_right : diddy_left);
  player.pushToSprite(&img, playerPos.x, playerPos.y, TFT_WHITE);
}

void loop() {
  leftBtnDebouncer.update();
  rightBtnDebouncer.update();

  if (!isGameRunning) {
    img.pushImage(0, 0, 160, 128, bg);
    player.pushImage(0, 0, 30, 51, diddy_right);
    player.pushToSprite(&img, playerPos.x, playerPos.y, TFT_WHITE);
    img.setTextColor(TFT_BLACK);
    img.setTextSize(2);
    img.drawString("DIDDY", 10, 10);
    img.drawString("SIMULATOR", 10, 27);
    img.setTextSize(1);
    img.drawString("HIGH SCORE: ", 10, 50);
    img.drawString(String(highScore), 80, 50);
    img.pushSprite(0, 0);

    if (leftBtnDebouncer.fell() || rightBtnDebouncer.fell()) {
      oils.clear();
      police.clear();
      score = 0;
      lastTime = millis();
      tone(BUZZER_PIN, 1700, 100);
      isGameRunning = true;
    }

    return;
  }

  if (leftBtnDebouncer.fell()) {
    playerDirection = -1;
    isMoving = true;
  } else if (leftBtnDebouncer.rose()) {
    playerDirection = -1;
    isMoving = false;
  } else if (rightBtnDebouncer.fell()) {
    playerDirection = 1;
    isMoving = true;
  } else if (rightBtnDebouncer.rose()) {
    playerDirection = 1;
    isMoving = false;
  }

  if (isMoving) {
    playerPos.x += (playerDirection * playerSpeed);
  }

  if (playerPos.x + playerSize.w >= 160) {
    playerPos.x = 160 - playerSize.w;
  } else if (playerPos.x <= 0) {
    playerPos.x = 0;
  }

  if (millis() > lastTime) {
    // spawn shit
    const int toBeSpawned = getBiasedRandom(); // true = police, false = oil;
    if (!toBeSpawned) {
      Oil newOil;
      newOil.pos = { float(getRandomNumber(10, 140)), -30 };
      newOil.size = { 13, 28 };
      newOil.speed = 1;
      oils.push_back(newOil);
    } else {
      Police newPolice;
      newPolice.pos = { float(getRandomNumber(10, 130)), -30 };
      newPolice.size = { 24, 30 };
      newPolice.speed = 1;
      police.push_back(newPolice);
    }

    lastTime += getRandomNumber(1500, 2000);
  }

  img.pushImage(0, 0, 160, 128, bg);

  for (int i = oils.size() - 1; i >= 0; --i) {
    if (oils[i].pos.y + oils[i].size.h >= 120) {
      oils.erase(oils.begin() + i);
    }

    if (oils[i].pos.y + oils[i].size.h > playerPos.y && oils[i].pos.y < playerPos.y + playerSize.h) {
      if (oils[i].pos.x + oils[i].size.w > playerPos.x && oils[i].pos.x < playerPos.x + playerSize.w) {
        score += 10;
        tone(BUZZER_PIN, 1700, 100);
        oils.erase(oils.begin() + i);
      }
    }

    oils[i].pos.y += oils[i].speed;
    TFT_eSprite sprite = TFT_eSprite(&tft);
    sprite.createSprite(13, 28);
    sprite.setSwapBytes(false);
    sprite.pushImage(0, 0, 13, 28, babyoil);
    sprite.pushToSprite(&img, oils[i].pos.x, oils[i].pos.y, TFT_BLACK);
  }

  for (int i = police.size() - 1; i >= 0; --i) {
    if (police[i].pos.y + police[i].size.h >= 120) {
      police.erase(police.begin() + i);
    }

    if (police[i].pos.y + police[i].size.h > playerPos.y && police[i].pos.y < playerPos.y + playerSize.h) {
    if (police[i].pos.x + police[i].size.w > playerPos.x && police[i].pos.x < playerPos.x + playerSize.w) {
      if (score > highScore) {
        highScore = score;
      }
      tone(BUZZER_PIN, 1000, 100);
      isGameRunning = false;
      police.erase(police.begin() + i);
    }
  }

    police[i].pos.y += police[i].speed;
    TFT_eSprite sprite = TFT_eSprite(&tft);
    sprite.createSprite(24, 30);
    sprite.setSwapBytes(false);
    sprite.pushImage(0, 0, 24, 30, policeImage);
    sprite.pushToSprite(&img, police[i].pos.x, police[i].pos.y, TFT_WHITE);
  }

  player.pushImage(0, 0, 30, 51, playerDirection == 1 ? diddy_right : diddy_left);
  player.pushToSprite(&img, playerPos.x, playerPos.y, TFT_WHITE);

  img.setTextSize(1);
  img.drawString("SCORE: ", 10, 10);
  img.drawString(String(score), 45, 10);

  img.pushSprite(0, 0);
  delay(50);
}