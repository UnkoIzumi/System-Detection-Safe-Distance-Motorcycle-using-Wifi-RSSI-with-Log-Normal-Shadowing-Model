                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         #include <WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

#define ledRed 27
#define ledYellow 14
#define ledGreen 12

Adafruit_MPU6050 mpu;

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display = Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

float currentAccel;
float prevAccel = 0;
float velocity, veloKm, vKm;
int interval = 10000;
int dataOutn1 = 0;
int dataOutn2 = 0;
unsigned long prevMillis = 0;
unsigned long currentMillis;

//const char* wifi_network_ssid     = "KelinciPercobaan";   /*Replace with Your own network SSID*/
//const char* wifi_network_password =  "qwerty123"; /*Replace with Your own network PASSWORD*/
const char *soft_ap_ssid          = "Node-3 ESP";    /*Create a SSID for ESP32 Access Point*/
const char *soft_ap_password      = "123456789"; /*Create Password for ESP32 AP*/
String ssid;

float v_Pt = 2; //transmitter power (dbm)
float v_Sd = 2; //standard deviation (random number)
int v_Ple = 4; //path loss exponent
int v_Plo = 24; //path loss distance do
int v_refd = 1; // rekomendasi 1 meter
int v_distance = 100; // jarak uji maksimal
float rssi_1, rssi_2, distance_node1, distance_node2;

float LNSM(float rssi) {
  float get_rssi = v_Pt - v_Plo - 10 * v_Ple * 1.903089987 + v_Sd;
  //  Serial.print(", Estimasi Jarak Max RSSI : ");
  //  Serial.print(get_rssi, 0);
  float up_data = ((rssi) - v_Pt + v_Plo - v_Sd);
  float down_data = 10 * v_Ple;
  float rumus = -(up_data / down_data);
  float get_distance = pow(v_refd * 10, rumus);
  //  Serial.print(", Distance : ");
  //  Serial.print(get_distance, 1);

  return get_distance;
}

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
  Serial.begin(115200);  /*Baud rate for serial communication*/
  pinMode(ledRed, OUTPUT);
  pinMode(ledYellow, OUTPUT);
  pinMode(ledGreen, OUTPUT);
  WiFi.mode(WIFI_AP_STA);  /*ESP32 Access point configured*/
  WiFi.disconnect();
  Serial.println("\n[*] Creating ESP32 AP");
  WiFi.softAP(soft_ap_ssid, soft_ap_password);  /*Configuring ESP32 access point SSID and password*/
  WiFi.setTxPower(WIFI_POWER_2dBm);
  Serial.print("[+] AP Created with IP Gateway ");
  Serial.println(WiFi.softAPIP());     /*Printing AP IP address*/

  if (!mpu.begin()) {
    Serial.println("Sensor init failed");
    while (1)
      yield();
  }

  // set accelerometer range to +-8G
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  display.display();
  display.setTextColor(WHITE);
  display.setRotation(0);
  delay(500);
  testscan();
}

void findWifi(void) {
  // find wifi rssi
  int n = WiFi.scanNetworks();

  if (n == 0) {
    Serial.println("no networks found");
  } else {
    Serial.print(n);
    Serial.println(" networks found");
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
      else if (ssid == "Node-2 ESP") {
        rssi_2 = WiFi.RSSI(i);
        Serial.print("RSSI Node 2 : ");
        Serial.println(rssi_2, 0);
        distance_node2 = LNSM(rssi_2);
        dataOutn2 = 2;
      }
    }
  }
}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  //currentAccel = getData[0];
  currentAccel = a.acceleration.x;

  currentMillis = millis();
  if (currentMillis >= prevMillis + interval) {
    Serial.println("Mulai");
    velocity = ((currentAccel + prevAccel) / 2) * (interval / 1000);
    prevAccel = currentAccel;
    prevMillis = interval;

    //  Serial.print("Time sec : ");
    //  Serial.println(currentMillis / 1000);
    vKm = velocity * 3.6;

    if (vKm > 0) {
      veloKm = vKm;
    } else {
      veloKm = 0;
    }
    ledNotif(veloKm);
    Serial.println("Selesai");
  }
  findWifi();
  displayOled();
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
  }
  if (dataOutn2 == 2) {
    //node 2 display
    display.setTextSize(1);
    display.setCursor(0, 25);
    display.print("Node2:");
    display.print(distance_node2, 1);
    display.print("m");
    dataOutn2 = 0;
  }
  //distance
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Distance:");
  display.setCursor(0, 15);
  display.print("Node1:");
  display.setCursor(0, 25);
  display.print("Node2:");
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
