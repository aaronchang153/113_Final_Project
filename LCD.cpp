#include "LCD.h"

#include <stdlib.h>
#include <stdio.h>
#include <wiringPi.h>
#include <pcf8574.h>
#include <lcd.h>
#include <time.h>
#include <string.h>

// the main loop condition is always true if compiled with DEBUG
// otherwise, it's a global variable set elsewhere
#ifdef DEBUG
int global_run_lcd = 1;
#else
extern int global_run_lcd;
#endif // DEBUG


#define DISPLAY_LEN 16

static int lcdhd;// used to handle LCD

void setupLCD(){
    int i;
    pcf8574Setup(BASE,pcf8574_address);// initialize PCF8574
    for(i=0;i<8;i++){
        pinMode(BASE+i,OUTPUT);     // set PCF8574 port to output mode
    } 
    digitalWrite(LED,HIGH);     // turn on LCD backlight
    digitalWrite(RW,LOW);       // allow writing to LCD
    
    lcdhd = lcdInit(2,16,4,RS,EN,D4,D5,D6,D7,0,0,0,0);// initialize LCD and return handle used to handle LCD
    
    if(lcdhd == -1){
        printf("lcdInit failed !");
        return;
    }
}

void cleanupLCD(){
	lcdClear(lcdhd);
}

// lcdPrintf and lcdPuts don't wan't to cooperate with long strings so here's a small solution
static void displayString(int x, int y, char *str){
	lcdPosition(lcdhd, x, y);
	for(int i = 0; i < DISPLAY_LEN; i++){
		lcdPutchar(lcdhd, str[i]);
	}
}

void lcdDisplayInfo(double local_temp, double local_humidity,
					double cimis_temp, double cimis_humidity,
					double local_et, double cimis_et,
					double water_savings){

	// the entire strings to be displayed on the LCD
	char local_str[100];
	char cimis_str[100];

	// buffers for what should currently be displayed on the LCD
	char local_buffer[DISPLAY_LEN + 1];
	char cimis_buffer[DISPLAY_LEN + 1];

	// length of each string and the current position in each string to be displayed
	int local_len, local_pos;
	int cimis_len, cimis_pos;

	// copy formatted strings into each string variable
	sprintf(local_str,"Local: Temp:%.1fF, Humidity:%.1f%%,ET: %.1f, Water Savings:%.1f",local_temp, local_humidity, local_et, water_savings);
	sprintf(cimis_str,"CIMIS: Temp:%.1fF, Humidity:%.1f%%,ET:%.1f",cimis_temp, cimis_humidity, cimis_et);

	local_len = strlen(local_str);
	cimis_len = strlen(cimis_str);

	local_pos = 0;
	cimis_pos = 0;

	// counter to know how many times the display has shifted
	// reset display once this reaches a certain number
	int counter = 0;

	while(global_run_lcd){
		// copy n characters from string to buffer and display buffer
		strncpy(cimis_buffer, cimis_str + cimis_pos, DISPLAY_LEN);
		displayString(0, 0, cimis_buffer);// set cursor position to (1,0) and display CIMIS info on LCD

		// copy n characters from string to buffer and display buffer
		strncpy(local_buffer, local_str + local_pos, DISPLAY_LEN);
		displayString(0, 1, local_buffer);// set cursor position to (0,0) and display local info on LCD

		// if we're not at the end of either string, increment current position
		if(local_pos < local_len - DISPLAY_LEN)
			local_pos++;
		if(cimis_pos < cimis_len - DISPLAY_LEN)
			cimis_pos++;

		// once we've scrolled long enough, reset the positions to 0
		if(++counter >= local_len - DISPLAY_LEN + 2){
			local_pos = 0;
			cimis_pos = 0;
			counter = 0;
		}

		delay(500);
	}
}

#ifdef DEBUG

//main function for testing purpose

int main(void){
    wiringPiSetup();
    double local_temp, local_humidity, cimis_temp, cimis_humidity, local_et, cimis_et, water_savings;
    
    local_temp = 25.20;
    local_humidity = 30.50;
    cimis_temp = 26.20;
    cimis_humidity = 31.50;
    local_et = 10.10;
    cimis_et = 11.10;
    water_savings = 40.45;

	setupLCD();
    lcdDisplayInfo( local_temp, local_humidity, cimis_temp, cimis_humidity, local_et, cimis_et, water_savings);
    
    return 0;
}

#endif // DEBUG

