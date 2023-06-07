#include <Wire.h>
#include <WiFi.h>
#include <math.h>
#include <MPU6050_tockn.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>

MPU6050 mpu(Wire);

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

#define ledRed 27
#define ledYellow 14
#define ledGreen 12

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display = Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Variables for storing acceleration and velocity values
int interval = 100;
int dataOutn1 = 0, dataOutn3 = 0;
float accelerationX, accelerationY, vKmX, vKmY, veloKm;
float velocityX = 0.0, velocityY = 0.0;
unsigned long prevTime, currentMillis = 0, prevMillis = 0;

// Wi-Fi related variables
float distance_node1, distance_node3;
int rssi_1, rssi_3;
String ssid;
//const char* ssid = "KelinciPercobaanLite";
//const char* password = "qwerty123";
const char *soft_ap_ssid = "Node-2 ESP";    /*Create a SSID for ESP32 Access Point*/
const char *soft_ap_password = "123456789"; /*Create Password for ESP32 AP*/

// Log-normal shadowing model parameters
float referenceDistance = 1.0; // Referensi jarak dalam meters
float referenceRSSI = -40.0; // RSSI dengan referensi jarak
float pathLossExponent = 4.0; // Path loss exponent (shadowing urban celluar radio 3-5)
float shadowingStdDev = 3.0; // Standard deviation di log-normal shadowing dalam satuan dB

float LNSM(int rssi) {
  // Calculate distance using log-normal shadowing model
  float shadowing = randomLogNormal(shadowingStdDev);
  float distance = pow(10, ((referenceRSSI - rssi - shadowing) / (10 * pathLossExponent))) * referenceDistance;

  return distance;
}

void ledNotif(void) {
  if (51 <= veloKm >=  60) {
    if ((distance_node1 < 41) || (distance_node3 < 41)) {
      digitalWrite(ledRed, HIGH);
      digitalWrite(ledYellow, LOW);
      digitalWrite(ledGreen, LOW);
    } else if ((41 < distance_node1 > 60) || (41 < distance_node3 > 60)) {
      digitalWrite(ledRed, LOW);
      digitalWrite(ledYellow, HIGH);
      digitalWrite(ledGreen, LOW);
    } else {
      digitalWrite(ledRed, LOW);
      digitalWrite(ledYellow, LOW);
      digitalWrite(ledGreen, HIGH);
    }
  } else if (41 <= veloKm >= 50) {
    if ((distance_node1 < 25) || (distance_node3 < 25)) {
      digitalWrite(ledRed, HIGH);
      digitalWrite(ledYellow, LOW);
      digitalWrite(ledGreen, LOW);
    } else if ((25 < distance_node1 > 50) || (25 < distance_node3 > 50)) {
      digitalWrite(ledRed, LOW);
      digitalWrite(ledYellow, HIGH);
      digitalWrite(ledGreen, LOW);
    } else {
      digitalWrite(ledRed, LOW);
      digitalWrite(ledYellow, LOW);
      digitalWrite(ledGreen, HIGH);
    }
  } else if (31 <= veloKm >= 40) {
    if ((distance_node1 < 20) || (distance_node3 < 20)) {
      digitalWrite(ledRed, HIGH);
      digitalWrite(ledYellow, LOW);
      digitalWrite(ledGreen, LOW);
    } else if ((20 < distance_node1 > 40) || (20 < distance_node3 > 40)) {
      digitalWrite(ledRed, LOW);
      digitalWrite(ledYellow, HIGH);
      digitalWrite(ledGreen, LOW);
    } else {
      digitalWrite(ledRed, LOW);
      digitalWrite(ledYellow, LOW);
      digitalWrite(ledGreen, HIGH);
    }
  } else if (21 <= veloKm >= 30) {
    if ((distance_node1 < 15) || (distance_node3 < 15)) {
      digitalWrite(ledRed, HIGH);
      digitalWrite(ledYellow, LOW);
      digitalWrite(ledGreen, LOW);
    } else if ((15 < distance_node1 > 30) || (15 < distance_node3 > 30)) {
      digitalWrite(ledRed, LOW);
      digitalWrite(ledYellow, HIGH);
      digitalWrite(ledGreen, LOW);
    } else {
      digitalWrite(ledRed, LOW);
      digitalWrite(ledYellow, LOW);
      digitalWrite(ledGreen, HIGH);
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
      if (ssid == "Node-1 ESP") {
        rssi_1 = WiFi.RSSI(i);
        Serial.print("RSSI Node 1 : ");
        Serial.println(rssi_1, 0);
        distance_node1 = LNSM(rssi_1);
        dataOutn1 = 1;
      }
      else if (ssid == "Node-3 ESP") {
        rssi_3 = WiFi.RSSI(i);
        Serial.print("RSSI Node 3 : ");
        Serial.println(rssi_3, 0);
        distance_node3 = LNSM(rssi_3);
        dataOutn3 = 3;
      }
    }
  }
}

void setWifi(void) {
  WiFi.mode(WIFI_AP_STA);  /*ESP32 Access point configured*/
  WiFi.disconnect();
  Serial.println("\n[*] Creating ESP32 AP");
  WiFi.softAP(soft_ap_ssid, soft_ap_password);  /*Configuring ESP32 access point SSID and password*/
  WiFi.setTxPower(WIFI_POWER_2dBm);
  Serial.print("[+] AP Created with IP Gateway ");
  Serial.println(WiFi.softAPIP());     /*Printing AP IP address*/
}

void mpu6050(void) {
  // Read accelerometer data
  mpu.update();
  accelerationX = mpu.getAccX();
  accelerationY = mpu.getAccY();

  // Calculate time difference
  unsigned long currentTime = micros();
  float timeDiff = (currentTime - prevTime) / 1000000.0; // Convert to seconds

  // Calculate velocity using the trapezoidal rule
  velocityX += (accelerationX * timeDiff);
  velocityY += (accelerationY * timeDiff);

  vKmX = velocityX * 3.6;
  vKmY = velocityY * 3.6;

  if (vKmX > 0) {
    veloKm = vKmX;
  } else {
    veloKm = 0;
  }

  // Update previous time
  prevTime = currentTime;
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  mpu.begin();

  pinMode(ledRed, OUTPUT);
  pinMode(ledYellow, OUTPUT);
  pinMode(ledGreen, OUTPUT);

  setWifi();
  // --- set display oled --- //
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  // --- End Oled --- //

  // --- set MPU --- //
  // Calibrate the MPU6050
  mpu.calcGyroOffsets(true);

  // Get initial time
  prevTime = micros();
  // --- End MPU --- //

  display.display();
  display.setTextColor(WHITE);
  display.setRotation(0);
  delay(500);
  testscan();
}

void loop() {
  mpu6050();
  findWifi();
  displayOled();
  ledNotif();
  // Print the horizontal velocity
  // Print the calculated distance
  Serial.print("RSSI Node 1: ");
  Serial.print(rssi_1);
  Serial.print(" dBm, ");
  Serial.print("Distance: ");
  Serial.print(distance_node1);
  Serial.print(" meters, ");
  Serial.print("Velocity X: ");
  Serial.print(velocityX);
  Serial.print(" m/s, ");
  Serial.print(veloKm);
  Serial.println(" km/h");
}

void displayOled(void) {
  display.clearDisplay();
  if (dataOutn1 == 1) {
    //node 1 display
    display.setTextSize(1);
    display.setCursor(0, 15);
    display.print("Node1:");
    display.print(distance_node1, 1);
    display.print("m");
    dataOutn1 = 0;
  } if (dataOutn3 == 3) {
    //node 3 display
    display.setTextSize(1);
    display.setCursor(0, 25);
    display.print("Node3:");
    display.print(distance_node3, 1);
    display.print("m");
    dataOutn3 = 0;
  }
  //distance
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Distance:");
  display.setCursor(0, 15);
  display.print("Node1:");
  display.setCursor(0, 25);
  display.print("Node3:");
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

float randomLogNormal(float stdDev) {
  float mu = 0.0; // Mean of the log-normal distribution
  float sigma = log(1 + (stdDev * stdDev)); // Standard deviation of the log-normal distribution

  float x = randomGaussian();
  float y = exp(mu + (sigma * x));

  return y;
}

float randomGaussian() {
  float v1, v2, s;

  do {
    v1 = 2.0 * randomFloat() - 1.0;
    v2 = 2.0 * randomFloat() - 1.0;
    s = v1 * v1 + v2 * v2;
  } while (s >= 1.0 || s == 0.0);

  float multiplier = sqrt(-2.0 * log(s) / s);

  return v1 * multiplier;
}

float randomFloat() {
  return random(0, RAND_MAX) / static_cast<float>(RAND_MAX);
}
