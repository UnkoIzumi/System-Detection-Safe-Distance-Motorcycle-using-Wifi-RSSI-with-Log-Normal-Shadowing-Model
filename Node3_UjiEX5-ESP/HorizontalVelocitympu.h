#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

Adafruit_MPU6050 mpu;

// Variables for storing acceleration and velocity values
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
//int dataOutn2 = 0, dataOutn3 = 0;
float vKmY, veloKm;

void initMPU(void) {
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

void mpu6050(void) {
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

  vKmY = velocity * 3.6;

  if (vKmY > 0) {
    veloKm = vKmY;
  } else {
    veloKm = 0;
  }

  // Update previous time
  // Delay for the desired sampling rate
  delay(dt * 1000);
}

float* run_mpu() {
  mpu6050();
  static float Getdata[2];
  Getdata[0] = velocity;
  Getdata[1] = veloKm;

  return Getdata;
}
