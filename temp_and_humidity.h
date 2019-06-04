#ifndef TEMP_AND_HUMIDITY
#define TEMP_AND_HUMIDITY

#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "DHT.hpp"


#define DHT11_Pin 0
#define DHT_OK 0
#define DHT_ERROR_CHECK -1
#define DHT_ERROR_TIMEOUT -2
#define DHT_INVALID_VALUE -999
#define DHT_DHT11_WAKEUP 18
#define DHT_WAKEUP 1
#define DHT_TIMEOUT 100


/*
class DHT
{
	public:
		double humidity, temp;
		int readDHT11(int pin);
	private:
		int bits[5]; //Buffer to read data
		int readSensor(int pin, int delay);
};
*/
float getTemp();
float getHumidity();

#endif
