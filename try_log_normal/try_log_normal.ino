#include "WiFi.h"

float v_Pt = 2; //transmitter power (dbm)
float v_Sd = 2; //standard deviation (random number)
int v_Ple = 3; //path loss exponent
int v_Plo = 32; //path loss distance do
int v_refd = 1; // rekomendasi 1 meter
int v_distance = 60; // jarak uji maksimal

float LNSM(float rssi){
  float get_rssi = v_Pt - v_Plo - 10 * v_Ple * 2 + v_Sd;
  Serial.printf("Estimasi Jarak Max RSSI : %f, ", get_rssi, 0);
  float up_data = ((rssi) - v_Pt + v_Plo - v_Sd);
  float down_data = 10 * v_Ple;
  float rumus = -(up_data/down_data);
  float get_distance = pow(v_refd * 10, rumus);
  Serial.printf("Distance : %f, ", get_distance, 0);
}

void setup()
{
    Serial.begin(115200);

    // Set WiFi to station mode and disconnect from an AP if it was previously connected
    WiFi.mode(WIFI_STA);
    WiFi.setTxPower(WIFI_POWER_2dBm);
    WiFi.disconnect();
    delay(100);

    Serial.println("Setup done");
}

void loop()
{
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
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.print(WiFi.SSID(i));
            float rssi_dat = WiFi.RSSI(i);
            Serial.print(" (");
            Serial.print(rssi_dat, 0);
            Serial.print("), ");
            LNSM(rssi_dat);
            Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
            delay(10);
        }
    }
    Serial.println("");

    // Wait a bit before scanning again
    delay(5000);
}
