#include <FastLED.h>

#define NUM_LEDS 260
#define LED_PIN 2
int ledPower = 30;
int fontColor[] = { 60, 60, 60};
CRGB led[NUM_LEDS];

void initBoard() {
  if (false == true) {
    ledPower = 5;
    fontColor[0] = 15;
    fontColor[1] = 15;
    fontColor[2] = 15;
  }
  FastLED.addLeds<NEOPIXEL, LED_PIN>(led, NUM_LEDS);
}


void _updateLedInUse(int from, int to) {
  for (int i = from; i <= to ; i++) {
    led[i] = CRGB(ledPower, 0, 0);
  }
  FastLED.show();
}

void _updateLedInFree(int from, int to) {
  for (int i = from; i <= to ; i++) {
    led[i] = CRGB(0, ledPower, 0);
  }
  FastLED.show();
}

void setOff(int index) {
  led[index] = CRGB(0, 0, 0);
}

int getBoardOffset(int boardIndex) {
  return (boardIndex * 64) + 4;
}
void drawTop(int boardIndex) {
  int board = getBoardOffset(boardIndex);
  for (int i = (2 + board); i <= (5 + board); i++) {
    led[i] = CRGB(fontColor[0], fontColor[1], fontColor[2]);
  }
  FastLED.show();
}

void drawLeftTop(int boardIndex) {
  int board = getBoardOffset(boardIndex);
  for (int i = 0; i < 2; i++) {
    led[9 + i + board] = CRGB(fontColor[0], fontColor[1], fontColor[2]);
    led[17 + i + board] = CRGB(fontColor[0], fontColor[1], fontColor[2]);
  }
  //  led[25] = CRGB(fontColor[0], fontColor[1], fontColor[2]);
  FastLED.show();
}

void drawLeftBottom(int boardIndex) {
  int board = getBoardOffset(boardIndex);
  for (int i = 0; i < 2; i++) {
    led[33 + i + board] = CRGB(fontColor[0], fontColor[1], fontColor[2]);
    led[41 + i + board] = CRGB(fontColor[0], fontColor[1], fontColor[2]);
    led[49 + i + board] = CRGB(fontColor[0], fontColor[1], fontColor[2]);
  }
  FastLED.show();
}

void drawRightTop(int boardIndex) {
  int board = getBoardOffset(boardIndex);
  for (int i = 0; i < 2; i++) {
    led[13 + i + board] = CRGB(fontColor[0], fontColor[1], fontColor[2]);
    led[21 + i + board] = CRGB(fontColor[0], fontColor[1], fontColor[2]);
  }
  FastLED.show();
}

void drawRightBottom(int boardIndex) {
  int board = getBoardOffset(boardIndex);
  for (int i = 0; i < 2; i++) {
    led[37 + i + board] = CRGB(fontColor[0], fontColor[1], fontColor[2]);
    led[45 + i + board] = CRGB(fontColor[0], fontColor[1], fontColor[2]);
    led[53 + i + board] = CRGB(fontColor[0], fontColor[1], fontColor[2]);
  }
  //  led[38 + board] = CRGB(fontColor[0], fontColor[1], fontColor[2]);
  //  led[46 + board] = CRGB(fontColor[0], fontColor[1], fontColor[2]);
  //  led[54 + board] = CRGB(fontColor[0], fontColor[1], fontColor[2]);
  FastLED.show();
}

void drawCenter(int boardIndex) {
  int board = getBoardOffset(boardIndex);
  for (int i = 26; i <= 29; i++) {
    led[i + board] = CRGB(fontColor[0], fontColor[1], fontColor[2]);
  }
  FastLED.show();
}

void drawBottom(int boardIndex) {
  int board = getBoardOffset(boardIndex);
  for (int i = 58; i <= 61; i++) {
    led[i + board] = CRGB(fontColor[0], fontColor[1], fontColor[2]);
  }
  FastLED.show();
}

void drawOne() {
  int index = 0;
  for (int i = 0; i <= 7; i++) {
    led[7 + index] = CRGB(fontColor[0], fontColor[1], fontColor[2]);
    led[8 + index] = CRGB(fontColor[0], fontColor[1], fontColor[2]);
    index += 8;
  }
  led[14] = CRGB(fontColor[0], fontColor[1], fontColor[2]);
  led[22] = CRGB(fontColor[0], fontColor[1], fontColor[2]);
  FastLED.show();
}

void drawThree(int boardIndex) {
  drawTop(boardIndex);
  drawCenter(boardIndex);
  drawRightBottom(boardIndex);
  drawRightTop(boardIndex);
  drawBottom(boardIndex);
  FastLED.show();
}

void drawFive(int boardIndex) {
  drawTop(boardIndex);
  drawCenter(boardIndex);
  drawLeftTop(boardIndex);
  drawRightBottom(boardIndex);
  drawBottom(boardIndex);
  FastLED.show();
}

void drawSix(int boardIndex) {
  drawTop(boardIndex);
  drawCenter(boardIndex);
  drawLeftTop(boardIndex);
  drawLeftBottom(boardIndex);
  drawRightBottom(boardIndex);
  drawBottom(boardIndex);
  //led[25 + (boardIndex * 64)] = CRGB(fontColor[0], fontColor[1], fontColor[2]);
  FastLED.show();
}

void drawSeven(int boardIndex) {
  drawTop(boardIndex);
  drawRightBottom(boardIndex);
  drawRightTop(boardIndex);
  FastLED.show();
}

void drawEight(int boardIndex) {
  drawTop(boardIndex);
  drawCenter(boardIndex);
  drawLeftTop(boardIndex);
  drawLeftBottom(boardIndex);
  drawRightBottom(boardIndex);
  drawRightTop(boardIndex);
  drawBottom(boardIndex);
  FastLED.show();
}

void updateLedInUse(int index) {
  switch (index) {
    case 0:
      _updateLedInUse(0, 0);
      _updateLedInUse(4, 67);
      drawOne();
      break;
    case 1:
      _updateLedInUse(1, 1);
      _updateLedInUse(68, 131);
      drawThree(1);
      break;
    case 2:
      _updateLedInUse(2, 2);
      _updateLedInUse(132, 195);
      drawSix(2);
      break;
    case 3:
      _updateLedInUse(3, 3);
      _updateLedInUse(196, 259);
      drawEight(3);
      break;
  }
}

void updateLedInFree(int index) {
  switch (index) {
    case 0:
      _updateLedInFree(0, 0);
      _updateLedInFree(4, 67);
      drawOne();
      break;
    case 1:
      _updateLedInFree(1, 1);
      _updateLedInFree(68, 131);
      drawThree(1);
      break;
    case 2:
      _updateLedInFree(2, 2);
      _updateLedInFree(132, 195);
      drawSix(2);
      break;
    case 3:
      _updateLedInFree(3, 3);
      _updateLedInFree(196, 259);
      drawEight(3);
      break;
  }
}

