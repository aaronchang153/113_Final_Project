#include "LCD.h"

#include <stdlib.h>
#include <stdio.h>
#include <wiringPi.h>
#include <pcf8574.h>
#include <lcd.h>
#include <time.h>
#include <string.h>
#include <pthread.h>

// the main loop condition is always true if compiled with DEBUG
// otherwise, it's a global variable set elsewhere
#ifdef DEBUG
int global_run_lcd = 1;
#else
extern int global_run_lcd;
#endif // DEBUG

#define DISPLAY_LEN 16

// the entire strings to be displayed on the LCD
char local_str[100];
char cimis_str[100];

// length of each string and the current position in each string to be displayed
int local_len;
int cimis_len;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

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

    local_len = 0;
    cimis_len = 0;
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

void lcdUpdateInfo(double local_temp, double local_humidity,
					double cimis_temp, double cimis_humidity,
					double local_et, double cimis_et,
					double water_savings){

	pthread_mutex_lock(&mutex);
	// copy formatted strings into each string variable
	sprintf(cimis_str,"CIMIS: Temp:%.1fF, Humidity:%.1f%%,ET:%.1f",cimis_temp, cimis_humidity, cimis_et);
	sprintf(local_str,"Local: Temp:%.1fF, Humidity:%.1f%%,ET:%.1f, Water Savings:%.1f",local_temp, local_humidity, local_et, water_savings);

	local_len = strlen(local_str);
	cimis_len = strlen(cimis_str);

	pthread_mutex_unlock(&mutex);
}

// thread function to constantly display info from lcdUpdateInfo
void *lcdDisplayInfo(void *args){

	// buffers for what should currently be displayed on the LCD
	char local_buffer[DISPLAY_LEN + 1];
	char cimis_buffer[DISPLAY_LEN + 1];

	int local_pos = 0;
	int cimis_pos = 0;

	// counter to know how many times the display has shifted
	// reset display once this reaches a certain number
	int counter = 0;

	while(global_run_lcd){
		pthread_mutex_lock(&mutex);

		// copy n characters from strings to buffers
		strncpy(cimis_buffer, cimis_str + cimis_pos, DISPLAY_LEN);
		strncpy(local_buffer, local_str + local_pos, DISPLAY_LEN);

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

		pthread_mutex_unlock(&mutex);

		displayString(0, 0, cimis_buffer);// set cursor position to (1,0) and display CIMIS info on LCD
		displayString(0, 1, local_buffer);// set cursor position to (0,0) and display local info on LCD

		delay(500);
	}

	return NULL;
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
    lcdUpdateInfo( local_temp, local_humidity, cimis_temp, cimis_humidity, local_et, cimis_et, water_savings);
    lcdDisplayInfo(NULL);
    
    return 0;
}

#endif // DEBUG

