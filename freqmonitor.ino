#include <U8glib.h>
#include <Arduino.h>
#include "Config.h"
#include "Rx5808Fns.h"
#include "BoardControl.h"

struct samplePoint {
  int freq;
  int rssi;
  int on;
  int failsafe;
};

const int fromFreqScan = 5645;
const int toFreqScan = 5945;
const PROGMEM int scanFreqArray[] = {5645, 5658, 5665, 5685, 5695, 5705, 5725, 5732, 5733, 5740, 5745, 5752, 5760, 5765, 5769, 5771, 5780, 5785, 5790, 5800, 5805, 5806, 5809, 5820, 5825, 5828, 5840, 5843, 5845, 5847, 5860, 5865, 5866, 5880, 5885, 5905, 5917, 5925, 5945};
int samplesAmount = 40;
samplePoint samples[40];
int freqArray[] = {5658, 5732, 5843, 5917};
//int freqArray[] = {5658, 5732, 5806, 5880};
int freqRssiArray[4];
const boolean scaled = true;
const int failesafe = -4;
int threshold = 195;
uint16_t currentTunerFreqInMhz = 0;
char out[8];
char minScale[8];
char maxScale[8];
int fidx = 0;

const int baseColumn = 61;
const int endColumn = 128;
const int baseRow = 5;
const int endRow = 63;

// If using software SPI (the default case):
#define OLED_MOSI   6
#define OLED_CLK   7
#define OLED_DC    4
#define OLED_CS    3
#define OLED_RESET 5

U8GLIB_SH1106_128X64 u8g(OLED_CLK, OLED_MOSI, OLED_CS, OLED_DC, OLED_RESET);

void setup() {
//  analogReference(INTERNAL);
  if (scaled) {
    threshold = 28;
  }
  initBoard();
  for (int i = 0 ; i < 4 ; i++) {
    updateLedInFree(i);
  }
  Serial.begin(115200);
  Serial.print("Initializing ");
  initializeSamples();
  rx5808setup();
  setCurrentFreqByMhz(pgm_read_word_near(&scanFreqArray[0]));
  displayLogo();
}

void loop() {
  setCurrentFreqByMhz(samples[fidx].freq);
  waitRssiReady();
  if (isInFPVUseFreq(samples[fidx]) == true) {
    processFPVChannels(samples[fidx]);
  } else {
//    ponele();
    samples[fidx].rssi = readRssiValue();
  }
  fidx++;
  if (fidx == samplesAmount) {
    fidx = 0;
    updateDisplay();
  }

}

void initializeSamples() {

  for (int a = 0; a < samplesAmount; a++) {
    samplePoint sample;
    samples[a] = sample;
    samples[a].freq = pgm_read_word_near(&scanFreqArray[a]);
    samples[a].rssi = 0;
    samples[a].on = 0;
    samples[a].failsafe = 0;
  }

}

void ponele() {
  delay(25);
}

void processFPVChannels(samplePoint& sample) {
//  waitRssiReady();
  sample.rssi = readRssiValue();
  freqRssiArray[isInFPVUseFreqIndex(sample)] = sample.rssi;
  if (sample.rssi > threshold) {
    if (sample.failsafe == 2) {
      if (sample.on == 0) {
        updateLedInUse(isInFPVUseFreqIndex(sample));
        sample.on = 1;
      }
    } else {
      if (sample.failsafe < 0) {
        sample.failsafe = 1;
      }
      sample.failsafe += 1;
    }
  } else {
    if (sample.failsafe == failesafe) {
      if (sample.on == 1) {
        updateLedInFree(isInFPVUseFreqIndex(sample));
        sample.on = 0;
      }
    } else {
      sample.failsafe -= 1;
    }
  }
}

boolean isInFPVUseFreq(samplePoint sample) {
  for (int i = 0 ; i < 4 ; i++) {
    if (sample.freq == freqArray[i]) {
      return true;
    }
  }
  return false;
}

int isInFPVUseFreqIndex(samplePoint sample) {
  int out = 0;
  for (int i = 0 ; i < 4 ; i++) {
    if (sample.freq == freqArray[i]) {
      out = i;
    }
  }
  return out;
}

void updateDisplay() {
  u8g.firstPage();
  do {
    draw();
  } while ( u8g.nextPage() );
}

void displayLogo() {
  u8g.firstPage();
  do {
    u8g.drawXBMP(0, 0, 127, 62, rsz_argentos2);
  } while ( u8g.nextPage() );
  delay(1500);
}

void draw(void) {
  u8g.setFont(u8g_font_6x13);
  int pos = 16;
  int margin = 2;
  for (int _index = 0; _index < 4; _index++) {
    u8g.drawStr(2, pos - margin, convertToCharString(freqArray[_index]));
    u8g.drawStr(33, pos - margin, convertToCharString(freqRssiArray[_index]));
    int freqFromCol = map(freqArray[_index], fromFreqScan, toFreqScan, baseColumn, endColumn );
    u8g.drawVLine(freqFromCol, 2, 4);
    pos = pos + 14;
  }
  int rssiLine = 0;
  if (scaled == true) {
    rssiLine = map(threshold, 0, 100, baseRow, endRow );
  } else {
    rssiLine = map(threshold, DEF_RAWRSSI_MIN, DEF_RAWRSSI_MAX, baseRow, endRow );
  }
  for (int y = 61; y <= 128; y += 4) {
    u8g.drawPixel(y, (endRow - rssiLine));
  }
  for (int x = 0; x < 40; x++) {
    spectroLine(samples[x].freq, samples[x].rssi);
  }
}

char* convertToCharString(uint16_t toConvert) {
  itoa(toConvert, out, 10);
  return out;
}

char* charMinScale(uint16_t toConvert) {
  itoa(toConvert, minScale, 10);
  return minScale;
}

char* charMaxScale(uint16_t toConvert) {
  itoa(toConvert, maxScale, 10);
  return maxScale;
}

void setCurrentFreqByMhz(uint16_t freqInMHz) {
  setChannelByFreq(freqInMHz);                   //set tuner
  currentTunerFreqInMhz = freqInMHz;             //save freq in MHz
}

//Reads and averages a set of RSSI samples for the currently-tuned channel.
// Returns:  An averaged RSSI value from MIN_RSSI_VAL to MAX_RSSI_VAL.
uint16_t readRssiValue() {
  uint16_t rawVal = readRawRssiValue();
  if (scaled) {
    return trimRssi(scaleRawRssiValue(rawVal));    //scale MIN_RSSI_VAL to MAX_RSSI_VAL
  }
  return rawVal;
}

int trimRssi(int rawRssi) {
  return rawRssi < 0 ? 0 : (rawRssi > 100 ? 100 : rawRssi);
}

void spectroLine(int freq, int &rssi) {
  int freqFromCol = map(freq, fromFreqScan, toFreqScan, baseColumn, endColumn );
  int lineHeight = map(rssi, 0, 100, baseRow, endRow);
  u8g.drawLine(freqFromCol , (endRow - lineHeight) + baseRow, freqFromCol, endRow);
}

long readVcc() {
  long result; // Read 1.1V reference against AVcc
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Convert
  while (bit_is_set(ADCSRA, ADSC));
  result = ADCL;
  result |= ADCH << 8;
  result = 1126400L / result; // Back-calculate AVcc in mV
  return result;
}

long readTemp() {
  long result; // Read temperature sensor against 1.1V reference
  ADMUX = _BV(REFS1) | _BV(REFS0) | _BV(MUX3);
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Convert
  while (bit_is_set(ADCSRA, ADSC));
  result = ADCL; result |= ADCH << 8;
  result = (result - 125) * 1075;
  return result;
}
