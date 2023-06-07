#include <WiFi.h>
#include <math.h>

// Wi-Fi related variables
const char* ssid = "KelinciPercobaanLite";
const char* password = "qwerty123";

// Log-normal shadowing model parameters
float referenceDistance = 1.0; // Referensi jarak dalam meters
float referenceRSSI = -40.0; // RSSI dengan referensi jarak
float pathLossExponent = 4.0; // Path loss exponent (shadowing urban celluar radio 3-5)
float shadowingStdDev = 3.0; // Standard deviation di log-normal shadowing dalam satuan dB

void setup() {
  Serial.begin(9600);
  
  // Connect to Wi-Fi network
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  
  Serial.println("Connected to WiFi");
}

void loop() {
  // Check Wi-Fi signal strength
  int rssi = WiFi.RSSI();
  
  // Calculate distance using log-normal shadowing model
  float shadowing = randomLogNormal(shadowingStdDev);
  float distance = pow(10, ((referenceRSSI - rssi - shadowing) / (10 * pathLossExponent))) * referenceDistance;
  
  // Print the calculated distance
  Serial.print("RSSI: ");
  Serial.print(rssi);
  Serial.print(" dBm\t");
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" meters");
  
  delay(1000); // Adjust delay as needed
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
