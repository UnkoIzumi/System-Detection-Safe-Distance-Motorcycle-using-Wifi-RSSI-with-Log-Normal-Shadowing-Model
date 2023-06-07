#include <WiFi.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>

#define ledRed 27
#define ledYellow 14
#define ledGreen 12

Adafruit_MPU6050 mpu;
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);

float velocity = 0;
float accelY = 0;
float currentAccel;
float vKm = 0;
float prevAccel = 0;
unsigned long prevMillis = millis();
unsigned long currentMillis, timeY;

float ledNotif(float velocity) {
  if (velocity <= 10) {
    digitalWrite(ledRed, LOW);
    digitalWrite(ledYellow, LOW);
    digitalWrite(ledGreen, HIGH);
  } else if (velocity >= 10 && velocity <= 30) {
    digitalWrite(ledRed, LOW);
    digitalWrite(ledYellow, HIGH);
    digitalWrite(ledGreen, LOW);
  } else {
    digitalWrite(ledRed, HIGH);
    digitalWrite(ledYellow, LOW);
    digitalWrite(ledGreen, LOW);
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(ledRed, OUTPUT);
  pinMode(ledYellow, OUTPUT);
  pinMode(ledGreen, OUTPUT);
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }

  if (!mpu.begin()) {
    Serial.println("Sensor init failed");
    while (1)
      yield();
  }

  display.display();
  delay(500); // Pause for 2 seconds
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setRotation(0);
}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  currentAccel = (a.acceleration.x)*2;

  currentMillis = millis();
  if (currentMillis - prevMillis >= 1000) {
    velocity = ((currentAccel + prevAccel) / 2) * ((currentMillis - prevMillis) / 1000);
    prevAccel = currentAccel;
    prevMillis = currentMillis;
  }

  vKm = velocity * 3.6;

  Serial.print("Accel(X) : ");
  Serial.print(a.acceleration.x);
  Serial.print(", Velocity(M) : ");
  Serial.print(velocity);
  Serial.print(", Speed(KM) : ");
  Serial.print(vKm);
  Serial.println("");
  ledNotif(vKm);
}
