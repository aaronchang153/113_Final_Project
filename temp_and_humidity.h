#ifndef TEMP_AND_HUMIDITY
#define TEMP_AND_HUMIDITY

#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "DHT.hpp"

float getTemp();
float getHumidity();

#endif
