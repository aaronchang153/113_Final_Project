#ifndef LCD_H
#define LCD_H

#include <stdlib.h>
#include <stdio.h>
#include <wiringPi.h>
#include <pcf8574.h>
#include <lcd.h>
#include <time.h>

// setup pins and I2C stuff
void setupLCD();

// temperature in Fahrenheit, humidity in %, water_savings in gallons
void lcdDisplayInfo(double local_temp, double local_humidity,
					double, cimis_temp, double, cimis_humidity,
					double local_et, double cimis_et,
					double water_savings);

#endif // LCD_H
