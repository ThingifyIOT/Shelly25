#include <Arduino.h>


#define TO_KELVIN(x) ((x) + 273.15)
#define ANALOG_T0  TO_KELVIN(25.0)
#define TO_CELSIUS(x) ((x) - 273.15)
#define ANALOG_V33 3.3


double TaylorLog(double x);
float ReadTemperature(int pin);
uint16_t AdcRead(uint32_t pin, uint32_t factor);
