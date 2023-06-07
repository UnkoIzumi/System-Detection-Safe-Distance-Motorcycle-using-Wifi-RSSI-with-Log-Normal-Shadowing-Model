#include <math.h>

float randomFloat() {
  return random(10000) / 10000.0;
}

float randomGaussian() {
  float u1 = randomFloat();
  float u2 = randomFloat();

//  Calculate two independent Gaussian random variables z1 and z2 using Box - Muller transform
  float z1 = sqrt(-2 * log(u1)) * cos(2 * PI * u2);

  return z1;
}

float lognormalShadowing(float shadowingStdDev) {
  float mean = -1.0; // Mean of the log-normal distribution (usually 0)
  float shadowing = 0.0;
  float sigma = shadowingStdDev;
  float z1 = randomGaussian();

  shadowing = exp(mean + sigma * z1);

  return shadowing;
}
