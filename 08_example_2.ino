// Arduino pin assignment
#define PIN_LED   9
#define PIN_TRIG  12   // sonar TRIGGER
#define PIN_ECHO  13   // sonar ECHO

// configurable parameters
#define SND_VEL 346.0     // sound velocity at 24°C (m/s)
#define INTERVAL 25       // sampling interval (ms)
#define PULSE_DURATION 10 // trigger pulse width (us)

// duration(us) -> distance(mm)
#define SCALE_MM_PER_US (0.5f * SND_VEL / 1000.0f)  // 0.173 mm/us

#define TIMEOUT 30000UL   // pulseIn timeout (us) ≈ 5m

unsigned long last_sampling_time;

void setup() {
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  Serial.begin(57600);
}

void loop() {
  unsigned long now = millis();
  if (now - last_sampling_time < INTERVAL) return;
  last_sampling_time = now;

  // --- 초음파 트리거 ---
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(PIN_TRIG, LOW);

  // --- 에코 읽기 ---
  unsigned long duration = pulseIn(PIN_ECHO, HIGH, TIMEOUT);

  if (duration == 0) {
    Serial.println("No Echo");
    analogWrite(PIN_LED, 255); // 반대로: Echo 없으면 꺼짐
    return;
  }

  float distance = duration * SCALE_MM_PER_US;  // mm
  int duty = 255; // 기본 꺼짐 (active-low 논리)

  // --- 거리별 LED 밝기 제어 (∧ 모양) ---
  if (distance >= 100 && distance <= 200) {
    // 100mm → 200mm : 꺼짐(255) → 가장 밝음(0)
    duty = map((int)distance, 100, 200, 255, 0);
  } else if (distance > 200 && distance <= 300) {
    // 200mm → 300mm : 가장 밝음(0) → 꺼짐(255)
    duty = map((int)distance, 200, 300, 0, 255);
  } else {
    duty = 255; // 범위 밖: 꺼짐
  }

  analogWrite(PIN_LED, duty);

  // --- 디버깅 출력 ---
  Serial.print("Distance(mm): ");
  Serial.print(distance, 1);
  Serial.print(" | Duty: ");
  Serial.println(duty);
}
