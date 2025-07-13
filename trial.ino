#include <TM1637Display.h>

// ---------- Pomodoro Definitions ----------
#define CLK 3
#define DIO 4
TM1637Display display(CLK, DIO);

#define START_BTN 9
#define PAUSE_BTN 10
#define RESET_BTN 11
#define STOP_BTN 12
#define BUZZER_PIN 8

const unsigned long pomodoroDuration = 25 * 60 * 1000UL;
const unsigned long breakDuration = 5 * 60 * 1000UL;
unsigned long duration = pomodoroDuration;

unsigned long startTime = 0;
unsigned long elapsedTime = 0;

bool isRunning = false;
bool isPaused = false;
bool isBreak = false;

// ---------- PIR Definitions ----------
#define PIR_PIN 7
const unsigned long firstNoMotionPeriod = 2UL * 60UL * 1000UL;  // 2 mins
const unsigned long secondNoMotionPeriod = 1UL * 60UL * 1000UL; // 1 min

unsigned long lastMotionTime = 0;
bool warningIssued = false;
bool alarmTriggered = false;

// ---------- Setup ----------
void setup() {
  // Pomodoro Setup
  display.setBrightness(7);
  pinMode(START_BTN, INPUT_PULLUP);
  pinMode(PAUSE_BTN, INPUT_PULLUP);
  pinMode(RESET_BTN, INPUT_PULLUP);
  pinMode(STOP_BTN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);

  showTime(duration);

  // PIR Setup
  pinMode(PIR_PIN, INPUT);
  digitalWrite(BUZZER_PIN, LOW); // Ensure buzzer is off initially
  Serial.begin(9600);
  Serial.println("🔍 PIR Inactivity Alarm System Initialized...");
  lastMotionTime = millis();
}

// ---------- Loop ----------
void loop() {
  handlePomodoro();   // Run Pomodoro logic
  handlePIR();        // Run PIR inactivity detection
}

// ---------- Pomodoro Logic ----------
void handlePomodoro() {
  if (digitalRead(START_BTN) == LOW && !isRunning && !isPaused) {
    startTime = millis();
    elapsedTime = 0;
    isRunning = true;
    delay(200);
  }

  if (digitalRead(PAUSE_BTN) == LOW) {
    if (isRunning) {
      elapsedTime = millis() - startTime;
      isRunning = false;
      isPaused = true;
    } else if (isPaused) {
      startTime = millis() - elapsedTime;
      isRunning = true;
      isPaused = false;
    }
    delay(200);
  }

  if (digitalRead(STOP_BTN) == LOW) {
    isRunning = false;
    isPaused = false;
    isBreak = false;
    elapsedTime = 0;
    duration = pomodoroDuration;
    showTime(duration);
    buzzEnd();
    delay(200);
  }

  if (digitalRead(RESET_BTN) == LOW) {
    isBreak = !isBreak;
    duration = isBreak ? breakDuration : pomodoroDuration;
    elapsedTime = 0;
    startTime = millis();
    isRunning = true;
    isPaused = false;
    showTime(duration);
    buzzEnd();
    delay(300);
  }

  if (isRunning) {
    elapsedTime = millis() - startTime;
    if (elapsedTime >= duration) {
      isRunning = false;
      isPaused = false;
      elapsedTime = duration;
      buzzEnd();
      if (!isBreak) {
        isBreak = true;
        duration = breakDuration;
        elapsedTime = 0;
        startTime = millis();
        isRunning = true;
      }
    }
  }

  showTime(duration - elapsedTime);
}

void showTime(unsigned long msLeft) {
  unsigned int seconds = (msLeft / 1000) % 60;
  unsigned int minutes = (msLeft / 60000);
  int timeValue = minutes * 100 + seconds;
  display.showNumberDecEx(timeValue, 0b01000000, true);
}

void buzzEnd() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(300);
    digitalWrite(BUZZER_PIN, LOW);
    delay(200);
  }
}

// ---------- PIR Logic ----------
void handlePIR() {
  int pirState = digitalRead(PIR_PIN);
  unsigned long currentTime = millis();

  if (pirState == HIGH) {
    Serial.println("✅ Motion Detected — Resetting Timers");
    lastMotionTime = currentTime;
    warningIssued = false;
    alarmTriggered = false;
    digitalWrite(BUZZER_PIN, LOW); // Ensure buzzer is off
  }

  unsigned long noMotionDuration = currentTime - lastMotionTime;

  if (!warningIssued && noMotionDuration >= firstNoMotionPeriod) {
    Serial.println("⚠️ No Motion for 2 minutes — Warning Buzz!");
    tone(BUZZER_PIN, 1000, 1000); // 1 sec beep
    warningIssued = true;
  }

  if (warningIssued && !alarmTriggered && noMotionDuration >= (firstNoMotionPeriod + secondNoMotionPeriod)) {
    Serial.println("🚨 No Motion for 3 minutes — Triggering Continuous Alarm!");
    digitalWrite(BUZZER_PIN, HIGH); // Continuous alarm
    alarmTriggered = true;
  }

  delay(200); // Stabilization
}
