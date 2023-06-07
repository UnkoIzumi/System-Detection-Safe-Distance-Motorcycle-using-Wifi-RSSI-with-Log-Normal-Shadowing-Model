#include <math.h>

float randomFloat() {
  return random(0, RAND_MAX) / static_cast<float>(RAND_MAX);
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

float randomLogNormal(float stdDev) {
  float mu = 0.0; // Mean of the log-normal distribution
  float sigma = log(1 + (stdDev * stdDev)); // Standard deviation of the log-normal distribution

  float x = randomGaussian();
  float y = exp(mu + (sigma * x));

  return y;
}
