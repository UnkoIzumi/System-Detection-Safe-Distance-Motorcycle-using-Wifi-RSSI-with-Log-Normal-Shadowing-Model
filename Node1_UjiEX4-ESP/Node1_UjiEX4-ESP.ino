#include <Wire.h>
#include <WiFi.h>
#include <math.h>
#include "LgNormShadow.h"
#include <Adafruit_MPU6050.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <Arduino.h>

Adafruit_MPU6050 mpu;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

#define ledRed 27
#define ledYellow 14
#define ledGreen 12
#define buzzer 19

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display = Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Variables for storing acceleration and velocity values
const float dt = 0.01;  // Time interval between measurements (in seconds)
float velocity = 0.0;
float acceleration = 0.0;
float filteredAcceleration = 0.0;
float gyroX = 0.0;
float gyroY = 0.0;
float gyroDisplacementX = 0.0;
float gyroDisplacementY = 0.0;
float accelOffsetX = 0.0;
float gyroOffsetX = 0.0;
float gyroOffsetY = 0.0;
//int dataOutn2 = 0, dataOutn3 = 0;
float vKmY, veloKm;
unsigned long currentMillis;
unsigned long prevMillis1 = 0, prevMillis2 = 0, prevTime = 0;
int buzstate = LOW;

// Wi-Fi related variables
float distance_node2, distance_node3;
float rssi_2, rssi_3;
String ssid;
//const char* ssid = "KelinciPercobaanLite";
//const char* password = "qwerty123";
const char *soft_ap_ssid = "Node-1 ESP";    /*Create a SSID for ESP32 Access Point*/
const char *soft_ap_password = "123456789"; /*Create Password for ESP32 AP*/

// Log-normal shadowing model parameters
float referenceDistance = 1.0; // Referensi jarak dalam meters
float referenceRSSI = -29.0; // RSSI dengan referensi jarak (do)
float pathLossExponent = 3.50; // Path loss exponent (shadowing urban celluar radio 3-5)
float shadowingStdDev = 3.0; // Standard deviation di log-normal shadowing dalam satuan dB

float LNSM(float rssi) {
  // Calculate distance using log-normal shadowing model
  float shadowing = randomLogNormal(shadowingStdDev);
  float distance = pow(10, ((referenceRSSI - rssi - shadowing) / (10 * pathLossExponent))) * referenceDistance;

  return distance;
}

void ledNotif(void) {
  distance_node2, distance_node3 = 45;
  if ((51 <= veloKm) && (veloKm <=  60)) {
    if ((20 < distance_node2) && (distance_node2 < 41) || (20 < distance_node3) && (distance_node3 < 41)) {
      digitalWrite(ledRed, HIGH);
      digitalWrite(buzzer, HIGH);
      digitalWrite(ledYellow, LOW);
      digitalWrite(ledGreen, LOW);
    } else if (((41 < distance_node2) && (distance_node2 < 60)) || ((41 < distance_node3) && (distance_node3 < 60))) {
      digitalWrite(ledRed, LOW);
      digitalWrite(ledYellow, HIGH);
      digitalWrite(ledGreen, LOW);
      if (millis() - prevTime >= 500) {
        prevTime = millis();
        if (buzstate == LOW) {
          buzstate = HIGH;
        } else {
          buzstate = LOW;
        }
        digitalWrite(buzzer, buzstate);
      }
    } else {
      digitalWrite(ledRed, LOW);
      digitalWrite(ledYellow, LOW);
      digitalWrite(ledGreen, HIGH);
      digitalWrite(buzzer, LOW);
    }
  } else if ((41 <= veloKm) && (veloKm <= 50)) {
    if ((15 < distance_node2) && (distance_node2 < 25) || (10 < distance_node3) && (distance_node3 < 25)) {
      digitalWrite(ledRed, HIGH);
      digitalWrite(buzzer, HIGH);
      digitalWrite(ledYellow, LOW);
      digitalWrite(ledGreen, LOW);
    } else if (((25 < distance_node2) && (distance_node2 < 50)) || ((25 < distance_node3) && (distance_node3 < 50))) {
      digitalWrite(ledRed, LOW);
      digitalWrite(ledYellow, HIGH);
      digitalWrite(ledGreen, LOW);
      if (millis() - prevTime >= 500) {
        prevTime = millis();
        if (buzstate == LOW) {
          buzstate = HIGH;
        } else {
          buzstate = LOW;
        }
        digitalWrite(buzzer, buzstate);
      }
    } else {
      digitalWrite(ledRed, LOW);
      digitalWrite(ledYellow, LOW);
      digitalWrite(ledGreen, HIGH);
      digitalWrite(buzzer, LOW);
    }
  } else if ((31 <= veloKm) && (veloKm <= 40)) {
    if ((10 < distance_node2) && (distance_node2 < 20) || (10 < distance_node3) && (distance_node3 < 20)) {
      digitalWrite(ledRed, HIGH);
      digitalWrite(buzzer, HIGH);
      digitalWrite(ledYellow, LOW);
      digitalWrite(ledGreen, LOW);
    } else if (((20 < distance_node2) && (distance_node2 < 40)) || ((20 < distance_node3) && (distance_node3 < 40))) {
      digitalWrite(ledRed, LOW);
      digitalWrite(ledYellow, HIGH);
      digitalWrite(ledGreen, LOW);
      if (millis() - prevTime >= 500) {
        prevTime = millis();
        if (buzstate == LOW) {
          buzstate = HIGH;
        } else {
          buzstate = LOW;
        }
        digitalWrite(buzzer, buzstate);
      }
    } else {
      digitalWrite(ledRed, LOW);
      digitalWrite(ledYellow, LOW);
      digitalWrite(ledGreen, HIGH);
      digitalWrite(buzzer, LOW);
    }
  } else if ((21 <= veloKm) && (veloKm <= 30)) {
    if ((5 < distance_node2) && (distance_node2 < 15) || (5 < distance_node3) && (distance_node3 < 15)) {
      digitalWrite(ledRed, HIGH);
      digitalWrite(buzzer, HIGH);
      digitalWrite(ledYellow, LOW);
      digitalWrite(ledGreen, LOW);
    } else if (((15 < distance_node2) && (distance_node2 < 30)) || ((15 < distance_node3) && (distance_node3 < 30))) {
      digitalWrite(ledRed, LOW);
      digitalWrite(ledYellow, HIGH);
      digitalWrite(ledGreen, LOW);
      if (millis() - prevTime >= 500) {
        prevTime = millis();
        if (buzstate == LOW) {
          buzstate = HIGH;
        } else {
          buzstate = LOW;
        }
        digitalWrite(buzzer, buzstate);
      }
    } else {
      digitalWrite(ledRed, LOW);
      digitalWrite(ledYellow, LOW);
      digitalWrite(ledGreen, HIGH);
      digitalWrite(buzzer, LOW);
    }
  }
}

void findWifi(void) {
  // find wifi rssi
  int n = WiFi.scanNetworks();

  if (n == 0) {
    Serial.println("no networks found");
  } else {
    //    Serial.print(n);
    //    Serial.println(" networks found");
    for (int i = 0; i < n; ++i) {
      // Print SSID and RSSI for each network found
      ssid = WiFi.SSID(i);
      if (ssid == "Node-2 ESP") {
        rssi_2 = WiFi.RSSI(i);
        Serial.print("RSSI Node 2 : ");
        Serial.println(rssi_2, 0);
        distance_node2 = LNSM(rssi_2);
        //        dataOutn2 = 2;
      }
      else if (ssid == "Node-3 ESP") {
        rssi_3 = WiFi.RSSI(i);
        Serial.print("RSSI Node 3 : ");
        Serial.println(rssi_3, 0);
        distance_node3 = LNSM(rssi_3);
        //        dataOutn3 = 3;
      }
    }
  }
}

void setWifi(void) {
  WiFi.mode(WIFI_AP_STA);  /*ESP32 Access point configured*/
  WiFi.disconnect();
  Serial.println("\n[*] Creating ESP32 AP");
  WiFi.softAP(soft_ap_ssid, soft_ap_password);  /*Configuring ESP32 access point SSID and password*/
  //  WiFi.setTxPower(WIFI_POWER_2dBm);
  Serial.print("[+] AP Created with IP Gateway ");
  Serial.println(WiFi.softAPIP());     /*Printing AP IP address*/
}

void mpu6050(void) {
  // Read accelerometer data
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  acceleration = a.acceleration.y;
  gyroX = g.gyro.x;
  gyroY = g.gyro.y;

  // Apply calibration offsets
  acceleration -= accelOffsetX;
  gyroX -= gyroOffsetX;
  gyroY -= gyroOffsetY;

  // Implement noise filtering
  const float alpha = 0.2; // Smoothing factor (0 < alpha < 1)
  filteredAcceleration = alpha * acceleration + (1 - alpha) * filteredAcceleration;

  // Calculate angular displacement
  gyroDisplacementX += gyroX * dt;
  gyroDisplacementY += gyroY * dt;

  // Combine accelerometer and gyroscope data
  float horizontalAcceleration = filteredAcceleration + gyroDisplacementY * dt;
  velocity += horizontalAcceleration * dt;

  vKmY = velocity * 3.6;

  if (vKmY > 0) {
    veloKm = vKmY;
  } else {
    veloKm = 0;
  }

  // Update previous time
  // Delay for the desired sampling rate
  delay(dt * 1000);
}

void initMPU(void) {
  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }

  mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
  mpu.setGyroRange(MPU6050_RANGE_2000_DEG);

  // Perform sensor calibration
  int numCalibrationSamples = 100;
  for (int i = 0; i < numCalibrationSamples; i++) {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    acceleration = a.acceleration.y;
    gyroX = g.gyro.x;
    gyroY = g.gyro.y;

    accelOffsetX += acceleration;
    gyroOffsetX += gyroX;
    gyroOffsetY += gyroY;
    delay(10); // Delay between samples
  }
  accelOffsetX /= numCalibrationSamples;
  gyroOffsetX /= numCalibrationSamples;
  gyroOffsetY /= numCalibrationSamples;
}

void task1(void * parameters) {
  for (;;) {
    findWifi();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void setup() {
  Serial.begin(115200);
  Wire.begin();

  pinMode(ledRed, OUTPUT);
  pinMode(ledYellow, OUTPUT);
  pinMode(ledGreen, OUTPUT);
  pinMode(buzzer, OUTPUT);

  setWifi();
  // --- set display oled --- //
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  // --- End Oled --- //

  // --- set MPU --- //
  // Calibrate the MPU6050
  initMPU();
  // --- End MPU --- //

  display.display();
  display.setTextColor(WHITE);
  display.setRotation(0);
  delay(500);
  testscan();
  delay(1000);

  xTaskCreate(
    task1, //function name
    "Task 1", // task name
    10000, // stack size
    NULL, // task parameters
    1, // task priority
    NULL // task handle
  );
}

void loop() {
  if (millis() - prevMillis1 >= 500) {
    prevMillis1 = millis();
    mpu6050();
  }
  if (millis() - prevMillis2 >= 1000) {
    prevMillis2 = millis();
    displayOled();
    ledNotif();
    //    Print the horizontal velocity
    Serial.print("Velocity: ");
    Serial.print(velocity);
    Serial.print(" m/s, ");
    Serial.print(veloKm);
    Serial.println(" km/h");
  }
}

void displayOled(void) {
  display.clearDisplay();
  //node 2 display
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Distance:");
  display.setCursor(0, 15);
  display.print("Node2:");
  display.print(distance_node2, 1);
  display.print("m");
  //node 3 display
  display.setTextSize(1);
  display.setCursor(0, 25);
  display.print("Node3:");
  display.print(distance_node3, 1);
  display.print("m");
  //kecepatan
  display.setTextSize(1);
  display.setCursor(70, 0);
  display.print("Speed:");
  display.setTextSize(2);
  display.setCursor(70, 15);
  display.print(veloKm, 0);
  display.setTextSize(1);
  display.print("km");
  display.display();
}

void testscan(void) {
  display.clearDisplay();
  //starting scan
  display.setTextSize(1);
  display.setCursor(32, 15);
  display.print("Scan Starting");
  display.display();
}
