#include <WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

Adafruit_MPU6050 mpu;

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display = Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

double kecAwal = 0;
double velocity, accX, veloKm;
const long interval = 1000;
unsigned long prevMillis = 0;

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

float LNSM(float rssi){
  float get_rssi = v_Pt - v_Plo - 10 * v_Ple * 1.903089987 + v_Sd;
  Serial.print(", Estimasi Jarak Max RSSI : ");
  Serial.print(get_rssi, 0);
  float up_data = ((rssi) - v_Pt + v_Plo - v_Sd);
  float down_data = 10 * v_Ple;
  float rumus = -(up_data/down_data);
  float get_distance = pow(v_refd * 10, rumus);
  Serial.print(", Distance : ");
  Serial.print(get_distance, 1);

  return get_distance;
}

double kecepatan(double vo, double acX, int prevTimeX, int currentTimeX){
  int t = ((currentTimeX/1000) - (prevTimeX/1000)); 
  double v = (vo+(acX*t));
//  Serial.println(v);
  return v;
}

void setup()
{
    Serial.begin(115200);  /*Baud rate for serial communication*/
    WiFi.mode(WIFI_AP_STA);  /*ESP32 Access point configured*/
    WiFi.disconnect();
    Serial.println("\n[*] Creating ESP32 AP");
    WiFi.softAP(soft_ap_ssid, soft_ap_password);  /*Configuring ESP32 access point SSID and password*/
    WiFi.setTxPower(WIFI_POWER_2dBm);
    Serial.print("[+] AP Created with IP Gateway ");
    Serial.println(WiFi.softAPIP());     /*Printing the AP IP address*/
//    WiFi.begin(wifi_network_ssid, wifi_network_password);  /*Connecting to Defined Access point*/
//    Serial.println("\n[*] Connecting to WiFi Network");
//    while(WiFi.status() != WL_CONNECTED)
//    {
//        Serial.print(".");
//        delay(100);
//    }
//    Serial.print("\n[+] Connected to WiFi network with local IP : ");
//    Serial.println(WiFi.localIP());   /*Printing IP address of Connected network*/

  if (!mpu.begin()) {
    Serial.println("Sensor init failed");
    while (1)
      yield();
  }

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  } 

  display.display();
  delay(500);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setRotation(0);
  Serial.println("scan start");
}
void loop() {
  display.clearDisplay();
  unsigned long currentMillis = millis();
  if(currentMillis - prevMillis >= interval){

  //untuk pembacaan accelerometer
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

//    accX = a.acceleration.x;
    accX = 78.7;
    velocity = kecepatan(kecAwal, accX, prevMillis, currentMillis);
    prevMillis = currentMillis;

    //konversi m/s to km/h
    //double vKm = (velocity/1000)/(1/3600);
    double vKm = velocity*3.6;

    if(vKm >= 10){
      veloKm = vKm;
    }else{
      veloKm = 0;
    }
//    Serial.println(velo?Km);
    
    // WiFi.scanNetworks will return the number of networks found
    int n = WiFi.scanNetworks();
    //distance
    display.setTextSize(1);
    display.setCursor(0,0);
    display.print("Distance:");
    display.setCursor(0,15);
    display.print("Node1:");
    display.setCursor(0,25);
    display.print("Node2:");
    
    if (n == 0) {
        Serial.println("no networks found");
    } else {
        Serial.print(n);
        Serial.println(" networks found");
        for (int i = 0; i < n; ++i) {
            // Print SSID and RSSI for each network found
            ssid = WiFi.SSID(i);
            if(ssid == "Node-1 ESP"){
              Serial.print("Found Selected : ");
              Serial.print("SSID : ");
              Serial.print(ssid);
              rssi_1 = WiFi.RSSI(i);
              Serial.print(", RSSI : ");
              Serial.print(rssi_1, 0);
              distance_node1 = LNSM(rssi_1);
              Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
              //node 1 display
              display.setTextSize(1);
              display.setCursor(0,15);
              display.print("Node1:");
              display.print(distance_node1, 1);
              display.print("m");
            }else if(ssid == "Node-2 ESP"){
              Serial.print("Found Selected : ");
              Serial.print("SSID : ");
              Serial.print(ssid);
              rssi_2 = WiFi.RSSI(i);
              Serial.print(", RSSI : ");
              Serial.print(rssi_2, 0);
              distance_node2 = LNSM(rssi_2);
              Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
              //node 2 display
              display.setTextSize(1);
              display.setCursor(0,25);
              display.print("Node2:");
              display.print(distance_node2, 1);
              display.print("m");
            }
          }
     }
    //kecepatan
    display.setTextSize(1);
    display.setCursor(70,0);
    display.print("Speed:");
    display.setTextSize(2);
    display.setCursor(70, 15);
    display.print(veloKm,0);
    display.setTextSize(1);
    display.print("km");
    display.display(); 
  }
    Serial.println("");
}
