#ifdef ESP8266
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif
/*
 *  Available ESP32 RF power parameters:
    WIFI_POWER_19_5dBm    // 19.5dBm (For 19.5dBm of output, highest. Supply current ~150mA)
    WIFI_POWER_19dBm      // 19dBm
    WIFI_POWER_18_5dBm    // 18.5dBm
    WIFI_POWER_17dBm      // 17dBm
    WIFI_POWER_15dBm      // 15dBm
    WIFI_POWER_13dBm      // 13dBm
    WIFI_POWER_11dBm      // 11dBm
    WIFI_POWER_8_5dBm     //  8dBm
    WIFI_POWER_7dBm       //  7dBm
    WIFI_POWER_5dBm       //  5dBm
    WIFI_POWER_2dBm       //  2dBm
    WIFI_POWER_MINUS_1dBm // -1dBm( For -1dBm of output, lowest. Supply current ~120mA)
    Available ESP8266 RF power parameters:
    0    (for lowest RF power output, supply current ~ 70mA
    20.5 (for highest RF power output, supply current ~ 80mA
*/

const char *soft_ap_ssid          = "Node-2 ESP";    /*Create a SSID for ESP32 Access Point*/
const char *soft_ap_password      = "123456789"; /*Create Password for ESP32 AP*/

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(soft_ap_ssid, soft_ap_password);  /*Configuring ESP32 access point SSID and password*/
//  WiFi.begin("yourSSID", "yourPASSWORD");
//  while (WiFi.status() != WL_CONNECTED) {
//    Serial.print(".");
//    delay(100);
//  }
  Serial.println("\nWiFi connected at: " + WiFi.localIP().toString());
  
  #ifdef ESP32
    Serial.println("ESP 32");
    WiFi.setTxPower(WIFI_POWER_MINUS_1dBm); // Sets WiFi RF power output to lowest level, lowest RF power usage
    delay(60000);
    Serial.println("Now switching to HIGH power mode");
    WiFi.setTxPower(WIFI_POWER_19_5dBm);    // Sets WiFi RF power output to highest level, highest RF power usage
  #else
    WiFi.setOutputPower(0); // Sets WiFi RF power output to lowest level, lowest RF power usage
    delay(60000);
    Serial.println("Now switching to HIGH power mode");
    WiFi.setOutputPower(20.5);    // Sets WiFi RF power output to highest level, highest RF power usage
  #endif  
}

void loop() {
  // put your main code here, to run repeatedly:
}
