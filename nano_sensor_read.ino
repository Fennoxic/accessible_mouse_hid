/*
 * Dual Ultrasonic Distance Sensor - Arduino Nano 33 BLE Rev2
 * Outputs PWM on A0 and A1 with duty cycle mapped to distance percentage.
 * Duty cycle is the rolling average of the last 40 readings per sensor,
 * rounded to the nearest whole percent.
 *
 * Distance range: 38 cm (0%) to 50 cm (100%)
 * Anything <= 38 cm = 0%, anything >= 50 cm = 100%
 *
 * PWM frequency: 500 Hz (below 1000 Hz requirement)
 *
 * Wiring - Sensor 1:
 *   HC-SR04 VCC  --> 5V pin
 *   HC-SR04 GND  --> GND
 *   HC-SR04 TRIG --> D9
 *   HC-SR04 ECHO --> D10
 *
 * Wiring - Sensor 2:
 *   HC-SR04 VCC  --> 5V pin
 *   HC-SR04 GND  --> GND
 *   HC-SR04 TRIG --> D7
 *   HC-SR04 ECHO --> D8
 *
 * PWM Output:
 *   Sensor 1 --> A0
 *   Sensor 2 --> A1
 */

#include <mbed.h>
#include <pinDefinitions.h>

// ----- Ultrasonic Pins -----
const int TRIG_1 = 9;
const int ECHO_1 = 10;
const int TRIG_2 = 7;
const int ECHO_2 = 8;

// ----- Distance Range -----
const float DIST_MIN_CM = 38.0;
const float DIST_MAX_CM = 50.0;

// ----- PWM Settings -----
const int PWM_PERIOD_US = 2000; // 2000µs = 500 Hz

// ----- Averaging -----
const int NUM_SAMPLES = 50;

float samples1[NUM_SAMPLES];
float samples2[NUM_SAMPLES];
int   sampleIndex1 = 0;
int   sampleIndex2 = 0;
int   sampleCount1 = 0;
int   sampleCount2 = 0;

// ----- Ultrasonic Constants -----
const float SOUND_SPEED_CM_US       = 0.03432;
const unsigned long ECHO_TIMEOUT_US = 25000UL;

// PWM objects — instantiated in setup() to avoid global init issues
mbed::PwmOut* pwm1 = nullptr;
mbed::PwmOut* pwm2 = nullptr;

// ----- Function Declarations -----
float readDistanceCm(int trigPin, int echoPin);
float distanceToPercent(float distanceCm);
float addSample(float* samples, int& index, int& count, float newPct);
void  printSensorLine(float distCm, float pct, int avgPct);

void setup() {
  Serial.begin(115200);

  pinMode(TRIG_1, OUTPUT); digitalWrite(TRIG_1, LOW);
  pinMode(ECHO_1, INPUT);
  pinMode(TRIG_2, OUTPUT); digitalWrite(TRIG_2, LOW);
  pinMode(ECHO_2, INPUT);

  for (int i = 0; i < NUM_SAMPLES; i++) {
    samples1[i] = 0.0;
    samples2[i] = 0.0;
  }

  pwm1 = new mbed::PwmOut(digitalPinToPinName(A0));
  pwm1->period_us(PWM_PERIOD_US);
  pwm1->write(0.0f);

  pwm2 = new mbed::PwmOut(digitalPinToPinName(A1));
  pwm2->period_us(PWM_PERIOD_US);
  pwm2->write(0.0f);

  Serial.println("=== Dual Ultrasonic -> PWM Output (40-sample avg) ===");
  Serial.println("Range: 38 cm (0%) to 50 cm (100%)");
  Serial.println("PWM:   500 Hz on A0 (S1) and A1 (S2)");
  Serial.println("-----------------------------------------------------");
}

void loop() {
  float dist1 = readDistanceCm(TRIG_1, ECHO_1);
  delay(30); // Prevent cross-talk between sensors
  float dist2 = readDistanceCm(TRIG_2, ECHO_2);

  float pct1 = distanceToPercent(dist1);
  float pct2 = distanceToPercent(dist2);

  // Average, then round to nearest whole percent
  int avg1 = (int)round(addSample(samples1, sampleIndex1, sampleCount1, pct1));
  int avg2 = (int)round(addSample(samples2, sampleIndex2, sampleCount2, pct2));

  // Clamp to [0, 100] after rounding
  avg1 = constrain(avg1, 0, 100);
  avg2 = constrain(avg2, 0, 100);

  pwm1->write(avg1 / 100.0f);
  pwm2->write(avg2 / 100.0f);

  Serial.print("Sensor 1: ");
  printSensorLine(dist1, pct1, avg1);
  Serial.print("    Sensor 2: ");
  printSensorLine(dist2, pct2, avg2);
  Serial.println();

  delay(30);
}

float addSample(float* samples, int& index, int& count, float newPct) {
  samples[index] = newPct;
  index = (index + 1) % NUM_SAMPLES;
  if (count < NUM_SAMPLES) count++;

  float sum = 0.0;
  for (int i = 0; i < count; i++) {
    sum += samples[i];
  }
  return sum / count;
}

float distanceToPercent(float distanceCm) {
  if (distanceCm < 0)            return 0.0;
  if (distanceCm <= DIST_MIN_CM) return 0.0;
  if (distanceCm >= DIST_MAX_CM) return 100.0;
  return ((distanceCm - DIST_MIN_CM) / (DIST_MAX_CM - DIST_MIN_CM)) * 100.0;
}

void printSensorLine(float distCm, float pct, int avgPct) {
  if (distCm < 0) {
    Serial.print("NO ECHO");
  } else {
    Serial.print(distCm, 1);
    Serial.print(" cm");
  }
  Serial.print(" -> ");
  Serial.print(pct, 1);
  Serial.print("% (avg: ");
  Serial.print(avgPct);
  Serial.print("%)");
}

float readDistanceCm(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(4);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  unsigned long waitStart = micros();
  while (digitalRead(echoPin) == LOW) {
    if (micros() - waitStart > 5000UL) return -1.0;
  }

  unsigned long echoStart = micros();
  while (digitalRead(echoPin) == HIGH) {
    if (micros() - echoStart > ECHO_TIMEOUT_US) return -1.0;
  }

  return ((micros() - echoStart) * SOUND_SPEED_CM_US) / 2.0;
}
