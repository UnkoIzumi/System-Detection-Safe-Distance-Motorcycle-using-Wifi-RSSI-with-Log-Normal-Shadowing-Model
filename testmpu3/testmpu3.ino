#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

Adafruit_MPU6050 mpu;

const float dt = 0.01;  // Time interval between measurements (in seconds)
float velocity = 0.0;
float acceleration = 0.0;
float filteredAcceleration = 0.0;
float gyroX = 0.0;
float gyroY = 0.0;
float gyroDisplacementX = 0.0;
float gyroDisplacementY = 0.0;
float accelOffsetX = 0.0;
float gyroOffsetX = 0.0;
float gyroOffsetY = 0.0;

void setup() {
  Serial.begin(9600);

  Wire.begin();
  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }

  mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
  mpu.setGyroRange(MPU6050_RANGE_2000_DEG);

  // Perform sensor calibration
  int numCalibrationSamples = 100;
  for (int i = 0; i < numCalibrationSamples; i++) {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    acceleration = a.acceleration.y;
    gyroX = g.gyro.x;
    gyroY = g.gyro.y;
    
    accelOffsetX += acceleration;
    gyroOffsetX += gyroX;
    gyroOffsetY += gyroY;
    delay(10); // Delay between samples
  }
  accelOffsetX /= numCalibrationSamples;
  gyroOffsetX /= numCalibrationSamples;
  gyroOffsetY /= numCalibrationSamples;
}

void loop() {
  // Read accelerometer data
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  acceleration = a.acceleration.y;
  gyroX = g.gyro.x;
  gyroY = g.gyro.y;

// Apply calibration offsets
  acceleration -= accelOffsetX;
  gyroX -= gyroOffsetX;
  gyroY -= gyroOffsetY;
  
  // Implement noise filtering
  const float alpha = 0.2; // Smoothing factor (0 < alpha < 1)
  filteredAcceleration = alpha * acceleration + (1 - alpha) * filteredAcceleration;
  
  // Calculate angular displacement
  gyroDisplacementX += gyroX * dt;
  gyroDisplacementY += gyroY * dt;
  
  // Combine accelerometer and gyroscope data
  float horizontalAcceleration = filteredAcceleration + gyroDisplacementY * dt;
  velocity += horizontalAcceleration * dt;
  // Display velocity
  Serial.print("Horizontal Velocity: ");
  Serial.print(velocity);
  Serial.println(" m/s");

  // Delay for the desired sampling rate
  delay(dt * 1000);
}
