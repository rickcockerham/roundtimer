#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_LEDBackpack.h>
#include <FlashStorage.h>
#include <Adafruit_DotStar.h>
#include <SPI.h>

#define DEBUG false
#define NUMPIXELS 1
#define BUTTON_MODE  3
#define BUTTON_START 4
#define BUZZER_PIN   1
#define MAX_DURATION 9 //length of the tone
#define MAX_SOUNDS 9  // tone number
#define SLEEP_TIMEOUT 300000
const int intervals[] = {30, 45, 60, 120, 180, 300};
#define NUM_INTERVALS 6

Adafruit_7segment display = Adafruit_7segment();

struct Settings {
  uint8_t duration;      // 1–MAX_DURATION
  uint8_t interval;    // 0–4
  uint8_t soundIndex;  // 0–MAX_SOUNDS
};
FlashStorage(flashStore, Settings);
Settings settings;

int remainingTime = 0;
bool timerRunning = false;
bool sleeping = false;

unsigned long lastUpdate = 0, lastInteraction = 0;
bool lastModeBtn = HIGH, lastStartBtn = HIGH;

Adafruit_DotStar strip(NUMPIXELS, 7, 8, DOTSTAR_BGR);

void setup() {
  if(DEBUG) {Serial.begin(9600); while(!Serial) {}}

  strip.begin();  // Initialize pins for output
  strip.show();   // Turn all LEDs off

  pinMode(BUTTON_MODE, INPUT_PULLUP);
  pinMode(BUTTON_START, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  noTone(BUZZER_PIN);

  Wire.begin();
  display.begin(0x70);
  display.setBrightness(3);
  display.clear();
  display.writeDisplay();
  if(DEBUG) Serial.println("Display initialized");

  settings = flashStore.read();
  if (settings.duration < 1 || settings.duration > MAX_DURATION) settings.duration = 1;
  if (settings.interval >= NUM_INTERVALS) settings.interval = 0;
  if (settings.soundIndex < 0 || settings.soundIndex > MAX_SOUNDS) settings.soundIndex = 0;

  remainingTime = intervals[settings.interval];
  sleeping = false;

  if(DEBUG) {
    Serial.print("Loaded duration: ");
    Serial.println(settings.duration);
    Serial.print("Loaded interval index: ");
    Serial.println(settings.interval);
    Serial.print("Loaded sound index: ");
    Serial.println(settings.soundIndex);
    Serial.print("Initial remainingTime: ");
    Serial.println(remainingTime);
  }
}

void loop() {
  handleButtons();

  if (timerRunning && millis() - lastUpdate >= 1000) {
    lastUpdate = millis();
    remainingTime--;
    if(DEBUG) Serial.print("Timer tick: ");
    if(DEBUG) Serial.println(remainingTime);

    if (remainingTime <= 0) {
      if(DEBUG) Serial.println("Timer reached 0. Buzz + Reset");
      strip.setPixelColor(0, strip.Color(255, 0, 255));  // PURPLE
      strip.show();
      playBuzzerTone(settings.soundIndex);
      blinkDisplay();
      strip.setPixelColor(0, strip.Color(0, 0, 0));    // Off
      strip.show();
      remainingTime = intervals[settings.interval];
    }
  }

  if(!sleeping) displayTime();

  if (!timerRunning && millis() - lastInteraction > SLEEP_TIMEOUT) {
    if(DEBUG) Serial.println("Inactivity timeout. Going to sleep.");
    sleepNow();
  }
}

void handleButtons() {
  bool modeBtn = digitalRead(BUTTON_MODE);
  bool startBtn = digitalRead(BUTTON_START);
  bool save = false;

  if (modeBtn == LOW || startBtn == LOW) {
    lastInteraction = millis();
    sleeping = false;
  }

  // Duration cycle: hold START, tap MODE
  if (startBtn == LOW && lastStartBtn == LOW && modeBtn == LOW && lastModeBtn == HIGH) {
    settings.duration += 1;
    if (settings.duration > MAX_DURATION) settings.duration = 1;
    if(DEBUG) Serial.print("Duration changed: ");
    if(DEBUG) Serial.println(settings.duration);
    playBuzzerAck();
    blinkSetting((char *)"dur", settings.duration);
    save = true;
  }

  // Sound cycle: hold MODE, tap START
  else if (modeBtn == LOW && lastModeBtn == LOW && startBtn == LOW && lastStartBtn == HIGH) {
    settings.soundIndex++;
    if (settings.soundIndex > MAX_SOUNDS) settings.soundIndex = 0;
    if(DEBUG) Serial.print("Sound changed: ");
    if(DEBUG) Serial.println(settings.soundIndex);
    playBuzzerAck();
    blinkSetting((char *)"tun", settings.soundIndex);
    save = true;
  }

  // MODE tapped
  else if (modeBtn == LOW && lastModeBtn == HIGH && startBtn == HIGH) {
    if (timerRunning || remainingTime < intervals[settings.interval]) {
      remainingTime = intervals[settings.interval];
      if(DEBUG) Serial.println("MODE tap: timer running — reset to interval time");
      timerRunning = false;
    } else {
      settings.interval = (settings.interval + 1) % NUM_INTERVALS;
      remainingTime = intervals[settings.interval];
      if(DEBUG) {
        Serial.print("Interval changed: ");
        Serial.print(settings.interval);
        Serial.print(" => ");
        Serial.println(remainingTime);
      }
      save = true;
    }
    playBuzzerAck();
  }

  // START tapped
  else if (startBtn == LOW && lastStartBtn == HIGH && modeBtn == HIGH) {
    timerRunning = !timerRunning;
    if(DEBUG) Serial.print("START tapped: timerRunning = "); Serial.println(timerRunning);
    playBuzzerTone(settings.soundIndex);
  }

  if (save) {
    flashStore.write(settings);
    if(DEBUG) Serial.println("Settings saved to flash.");
  }

  lastModeBtn = modeBtn;
  lastStartBtn = startBtn;
}
void displayTime() {
  display.println(remainingTime);
  display.writeDisplay();
}

void blinkDisplay() {
  display.print(0x8888, HEX);
  display.writeDisplay();
  delay(100);
  display.clear();
  display.writeDisplay();
  delay(60);
}

void blinkSetting(char *c, int v) {
  display.clear();
  char buf[5];
  snprintf(buf, sizeof(buf), "%3s%1d", c, v);

  display.print(buf);
  display.writeDisplay();
  delay(500);
  display.clear();
  display.writeDisplay();
}

void playBuzzerAck() {
  tone(BUZZER_PIN, 50);
  delay(100);
  noTone(BUZZER_PIN);
}

void playBuzzerTone(int idx) {
  int d = (settings.duration+1) * 3;
  if(DEBUG) Serial.print("Playing tone #"); Serial.println(idx);

  switch (idx) {
    case 0:  for (int f = 1; f < 15; f += 1) { tone(BUZZER_PIN, 400); delay(20+d); noTone(BUZZER_PIN); delay(10);} break;
    case 1: tone(BUZZER_PIN, 250); delay(100+d); tone(BUZZER_PIN, 100); delay(100+d); tone(BUZZER_PIN, 250); break;
    case 2: for (int f = 700; f > 200; f -= 20) { tone(BUZZER_PIN, f); delay(5+d); } break;
    case 3: for (int f = 200; f < 700; f += 20) { tone(BUZZER_PIN, f); delay(5+d); } break;
    case 4: tone(BUZZER_PIN, 300); break;
    case 5: tone(BUZZER_PIN, 700); delay(50+d);
            tone(BUZZER_PIN, 900); delay(50+d);
            tone(BUZZER_PIN, 300); break;
    case 6: tone(BUZZER_PIN, 900); delay(70+d);
            tone(BUZZER_PIN, 1100); delay(70+d);
            tone(BUZZER_PIN, 700); break;
    case 7: tone(BUZZER_PIN, 300); delay(90+d);
            tone(BUZZER_PIN, 500); delay(90+d);
            tone(BUZZER_PIN, 800); break;
    case 8: for (int f = 1000; f > 200; f -= 30) { tone(BUZZER_PIN, f); delay(3+d); } break;
    case 9:for (int f = 800; f > 100; f -= 40) { tone(BUZZER_PIN, f); delay(3+d); }
            delay(100+d); tone(BUZZER_PIN, 2000); break;
  }
  delay(100+d);
  noTone(BUZZER_PIN);
}

void sleepNow() {
  display.clear();
  display.writeDisplay();
  noTone(BUZZER_PIN);

  attachInterrupt(BUTTON_MODE, wake, FALLING);
  attachInterrupt(BUTTON_START, wake, FALLING);

  //USBDevice.detach();
  if(DEBUG) Serial.println("Entering sleep...");

  __WFI();  // Wait For Interrupt

  //USBDevice.attach();
  detachInterrupt(BUTTON_MODE);
  detachInterrupt(BUTTON_START);
  if(DEBUG) Serial.println("Woke from sleep.");
  lastInteraction = millis();
  sleeping = true;
}

void wake() {
  // Needed to wake from WFI
  sleeping = false;
  lastInteraction = millis();
  displayTime();
}
