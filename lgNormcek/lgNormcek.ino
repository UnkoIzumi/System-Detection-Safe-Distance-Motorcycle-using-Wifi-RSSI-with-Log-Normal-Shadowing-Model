#include <WiFi.h>

void setup() {
  Serial.begin(115200);

  // Connect to WiFi network
  WiFi.begin("Node-1 ESP", "123456789");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  // Get the RSSI at reference distance
  int referenceDistance = 1; // Reference distance in meters
  int referenceRSSI = WiFi.RSSI();

  // Calculate path loss exponent
  float pathLossExponent = calculatePathLossExponent(referenceDistance, referenceRSSI);

  // Calculate log-normal shadowing term
  float shadowingStdDev = 4.0; // Shadowing standard deviation in dB
  float shadowingTerm = generateLogNormalShadowing(shadowingStdDev);

  // Distance for signal strength estimation
  int distance = 10; // Distance in meters

  // Calculate path loss
  float pathLoss = calculatePathLoss(distance, referenceDistance, pathLossExponent);

  // Estimate received signal strength
  float receivedRSSI = referenceRSSI - pathLoss + shadowingTerm;

  // Print results
  Serial.print("Reference RSSI: ");
  Serial.println(referenceRSSI);
  Serial.print("Path Loss Exponent: ");
  Serial.println(pathLossExponent);
  Serial.print("Shadowing Term: ");
  Serial.println(shadowingTerm);
  Serial.print("Estimated RSSI at ");
  Serial.print(distance);
  Serial.print(" meters: ");
  Serial.println(receivedRSSI);
}

void loop() {
  // Your code here
}

// Function to calculate the path loss exponent
float calculatePathLossExponent(int referenceDistance, int referenceRSSI) {
  // Calculate path loss exponent based on empirical data or analysis
  // You can use measurements or models specific to your environment
  // and derive the path loss exponent from the reference distance and RSSI
  // Return the calculated path loss exponent
}

// Function to calculate the path loss
float calculatePathLoss(int distance, int referenceDistance, float pathLossExponent) {
  // Calculate the path loss using the path loss formula:
  // PL(d) = PL(d0) + 10 * n * log10(d / d0)
  float pathLoss = pathLossExponent * 10 * log10(distance / referenceDistance);
  return pathLoss;
}

// Function to generate the log-normal shadowing term
float generateLogNormalShadowing(float shadowingStdDev) {
  // Generate a random value from the log-normal distribution
  // with mean 0 and standard deviation given by shadowingStdDev
  // You can use statistical functions or libraries to generate the random value
  // Return the generated log-normal shadowing term
}
