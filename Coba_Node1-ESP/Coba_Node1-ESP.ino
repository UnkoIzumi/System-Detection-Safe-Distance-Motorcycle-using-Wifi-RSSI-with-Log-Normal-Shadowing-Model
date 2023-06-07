#include <WiFi.h>

//const char* wifi_network_ssid     = "KelinciPercobaan";   /*Replace with Your own network SSID*/
//const char* wifi_network_password =  "qwerty123"; /*Replace with Your own network PASSWORD*/
const char *soft_ap_ssid          = "Node-1 ESP";    /*Create a SSID for ESP32 Access Point*/
const char *soft_ap_password      = "123456789"; /*Create Password for ESP32 AP*/
String ssid;

float v_Pt = 2; //transmitter power (dbm)
float v_Sd = 2; //standard deviation (random number)
int v_Ple = 4; //path loss exponent
int v_Plo = 24; //path loss distance do
int v_refd = 1; // rekomendasi 1 meter
int v_distance = 100; // jarak uji maksimal

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
}
void loop() {
  Serial.println("scan start");

    // WiFi.scanNetworks will return the number of networks found
    int n = WiFi.scanNetworks();
    Serial.println("scan done");
    if (n == 0) {
        Serial.println("no networks found");
    } else {
        Serial.print(n);
        Serial.println(" networks found");
        for (int i = 0; i < n; ++i) {
            // Print SSID and RSSI for each network found
            ssid = WiFi.SSID(i);
            if(ssid == "Node-2 ESP"){
              Serial.print("Found Selected : ");
              Serial.print("SSID : ");
              Serial.print(ssid);
              float rssi_2 = WiFi.RSSI(i);
              Serial.print(", RSSI : ");
              Serial.print(rssi_2, 0);
              LNSM(rssi_2);
              Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
              delay(10);
            }else if(ssid == "Node-3 ESP"){
              Serial.print("Found Selected : ");
              Serial.print("SSID : ");
              Serial.print(ssid);
              float rssi_3 = WiFi.RSSI(i);
              Serial.print(", RSSI : ");
              Serial.print(rssi_3, 0);
              LNSM(rssi_3);
              Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
              delay(10);
            }
          }
     }
    Serial.println("");
    // Wait a bit before scanning again
    delay(1000);
}
