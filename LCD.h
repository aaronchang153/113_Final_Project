#ifndef LCD_H
#define LCD_H


// setup pins and I2C stuff
void setupLCD();

// temperature in Fahrenheit, humidity in %, water_savings in gallons
void lcdDisplayInfo(double local_temp, double local_humidity,
					double, cimis_temp, double, cimis_humidity,
					double local_et, double cimis_et,
					double water_savings);

#endif // LCD_H
