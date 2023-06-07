/*
    This sketch demonstrates how to scan WiFi networks.
    The API is almost the same as with the WiFi Shield library,
    the most obvious difference being the different file you need to include:
*/
#include "WiFi.h"

void setup()
{
  Serial.begin(115200);

  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  Serial.println("Setup done");
}

void loop()
{
//  Serial.println("scan start");

  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
//  Serial.println("scan done");
  if (n == 0) {
  } else {
    for (int i = 0; i < n; ++i) {
      String ssid = WiFi.SSID(i);
      if (ssid == "Node-3 ESP") {
//        rssi_1 = WiFi.RSSI(i);
//        Serial.print("RSSI Node 1 : ");
        Serial.println(WiFi.RSSI(i));
      }
    }
  }
  Serial.println("");

  // Wait a bit before scanning again
  delay(5000);
}
