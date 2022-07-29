#include "temperature_sensor.h"

double TaylorLog(double x)
{
  // https://stackoverflow.com/questions/46879166/finding-the-natural-logarithm-of-a-number-using-taylor-series-in-c

  if (x <= 0.0) { return NAN; }
  if (x == 1.0) { return 0; }
  double z = (x + 1) / (x - 1);                              // We start from power -1, to make sure we get the right power in each iteration;
  double step = ((x - 1) * (x - 1)) / ((x + 1) * (x + 1));   // Store step to not have to calculate it each time
  double totalValue = 0;
  double powe = 1;
  for (uint32_t count = 0; count < 10; count++) {            // Experimental number of 10 iterations
    z *= step;
    double y = (1 / powe) * z;
    totalValue = totalValue + y;
    powe = powe + 2;
  }
  totalValue *= 2;
  return totalValue;
}

float ReadTemperature(int pin)
{
    int adc = AdcRead(pin, 2);
    // Steinhart-Hart equation for thermistor as temperature sensor
    double Rt = (adc * 32000) / (1024.0*ANALOG_V33- (double)adc);
    double BC = (double)(3350 * 10000) / 10000;
    double T = BC / (BC / ANALOG_T0 + TaylorLog(Rt / (double)10000));
    return TO_CELSIUS(T);
}

uint16_t AdcRead(uint32_t pin, uint32_t factor) 
{
  // factor 1 = 2 samples
  // factor 2 = 4 samples
  // factor 3 = 8 samples
  // factor 4 = 16 samples
  // factor 5 = 32 samples
  uint32_t samples = 1 << factor;
  uint32_t analog = 0;
  for (uint32_t i = 0; i < samples; i++) {
    analog += analogRead(pin);
    delay(1);
  }
  analog >>= factor;
  return analog;
}
