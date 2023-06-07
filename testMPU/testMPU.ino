#include "MPU6050_TRY.h"
#include <Adafruit_GFX.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <WiFi.h>
#include <PubSubClient.h>

#define ledRed 27
#define ledYellow 14
#define ledGreen 12

Adafruit_MPU6050 mpu;
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);

const char* ssid = "KelinciPercobaan";
const char* password = "qwerty123";
const char* mqttserver = "broker.mqtt-dashboard.com";
int mqttport = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

float velocity = 0;
float accelY = 0;
float currentAccel;
float vKm = 0;
float prevAccel = 0;
unsigned long prevMillis = millis();
unsigned long currentMillis;
float* getData;

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

void callback1(char* topic, byte* payload, unsigned int length) {
  for (int i = 0; 1 < length; i++) {
    Serial.print((char)payload[i]);
  }

  //aktif
  //    if ((char)payload[0] == '1') {
  //    motion_aktif = 1; // motion auto on *toilet lock
  //    Serial.println("Motion Auto Aktif");
  //  } else if((char)payload[0] == '0') {
  //    motion_aktif = 0; // motion auto off *toilet unlock
  //    Serial.println("Motion Auto Mati");
  //  }
}

void reconnected() {
  while (!client.connected()) {
    Serial.println("Menghubungkan kembali ke MQTT..");
    if (client.connect("client_kelinci")) {
      Serial.println("Sukses terhubung Kembali..");
    } else {
      Serial.print("Gagal,..");
      Serial.print(client.state());
      Serial.println("tunggu 3 detik..");
      delay(3000);
    }
  }
  client.subscribe("kontroller1/data");
}

void wifi_mode() {
  // Mengatur WiFi ----------------------------------------------------------
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // Attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
    yield();
  }
  client.setServer(mqttserver, mqttport);
  //  client.setCallback(callback1);
  Serial.println(" ");
  Serial.println("Wifi Connected");
  digitalWrite(LED_BUILTIN, HIGH);
  // Print the IP address ---------------------------------------------------
  Serial.print("IP address : ");
  Serial.println(WiFi.localIP());
  Serial.print(" ");
}

void setup() {
  Serial.begin(115200);
  pinMode(ledRed, OUTPUT);
  pinMode(ledYellow, OUTPUT);
  pinMode(ledGreen, OUTPUT);

  start_mpu();

  if (!mpu.begin()) {
    Serial.println("Sensor init failed");
    while (1)
      yield();
  }

  // set accelerometer range to +-8G
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);


  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }

//  wifi_mode();
  display.display();
  delay(500); // Pause for 2 seconds
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setRotation(0);
}

void loop() {
  char dataa[5];
  getData = run_mpu();

  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
    currentAccel = getData[0];
//  currentAccel = a.acceleration.x;

  currentMillis = millis();
  velocity += (currentAccel + prevAccel) / 2 * (currentMillis - prevMillis) / 1000;
  prevAccel = currentAccel;
  prevMillis = currentMillis;

  //  Serial.print("Time sec : ");
  //  Serial.println(currentMillis / 1000);
  vKm = velocity * 3.6;

  Serial.print("Velocity(M) : ");
  Serial.print(velocity);
  Serial.print(", Speed(KM) : ");
  Serial.print(vKm);
  Serial.println("");
  ledNotif(vKm);

//  dtostrf(vKm, 3, 0, dataa);
//  client.publish("kontroller1/data", dataa);
  display.clearDisplay();
  display.setCursor(0, 0);

  //    Serial.println("Accelerometer - m/s^2");
  //    Serial.print("Accel X : ");
  //    Serial.print(getData[0]);
  //    Serial.print(", Accel Y : ");
  //    Serial.print(getData[1]);
  //    Serial.print(", Accel Z : ");
  //    Serial.print(getData[2]);
  //    Serial.print(", Roll : ");
  //    Serial.print(getData[3]);
  //    Serial.print(", Pitch : ");
  //    Serial.print(getData[4]);
  //    Serial.println("");
  //    delay(1000);

  //  float velocity = kecepatan(accY, currentNode);
  display.setTextSize(2);
  display.println("Speed");
  display.print(vKm, 0);
  display.print("km");

  display.display();
//  if (!client.connected()) {
//    reconnected();
//  }
//  client.loop();
}
