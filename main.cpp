#include <stdio.h>
#include <wiringPi.h>
#include <pthread.h>
#include <signal.h>
#include "cimis.h"
#include "temp_and_humidity.h"
#include "DHT.hpp"
#include "relay.h"
#include "motion.h"
#include "LCD.h"


#define AREA           200 // square feet
#define WATER_RATE    1020 // gallons per hour
#define IE            0.75
#define PF             0.5

// in gallons per day
#define WATER_AMT(et) ((et)*PF*AREA*0.62/IE)

// input seconds, gives milliseconds
#define SECONDS(x) ((x) * 1000)

#define WAIT_TIME 60000 //milliseconds

// global flag for when the lcd display thread should stop
int global_run_lcd;

// the most recent local ET and cimis data
double et_local;
struct CIMIS_data cimis_data;

double water_saved;

// gets set to 0 on Ctrl-C
int running;

// signal handler for SIGINT (i.e. Ctrl-C)
void handle_sigint(int sig) { running = 0; }

// gets data from CIMIS and updates the most recent data (from above)
void hourlyCheck(double temp, double humidity);
void waterPlants();

static inline double C_to_F(double temp_C) { return (temp_C * (9.0 / 5.0)) + 32.0; }

int main(){
	if(wiringPiSetup() == -1){
		fprintf(stderr, "wiringPi setup failed,\n");
		return -1;
	}

	/*** Call whatever additional setup is needed here ***/
	relaySetup();
	setupMotion();
	setupLCD();

	water_saved = 0.0l;

	DHT dht;

	double avg_temp = 0.0l;
	double avg_humidity = 0.0l;

	// just a flag to know when the for loop is doing its first run
	// used to properly calculate average temp+humidity
	char first_run = 1;

	// counts between 0 and 59, gets incremented eveytime local temp + humidity is checked
	// used to track when an hour has passed
	int counter = 59; // starts at 59 so the first iteration of the loop sets it to 0

	// so that the lcd display thread can actually run
	global_run_lcd = 1;

	// start lcd display thread
	pthread_t lcd_tid;
	pthread_create(&lcd_tid, NULL, lcdDisplayInfo, NULL);

	lcdUpdateStatus(LCD_STATUS_IDLE);

	// call handle_sigint on SIGINT signal
	signal(SIGINT, handle_sigint);

	running = 1;
	while(running){
		counter = (counter + 1) % 60;

		// read temp+humidity every 2 seconds until we get a good reading
		while(dht.readDHT11(DHT_PIN) != DHTLIB_OK)
			delay(2000);

		// accumulate temp and humidity readings over the hour
		avg_temp += dht.temperature;
		avg_humidity += dht.humidity;
		
		if(first_run){
			first_run = 0;
		}
		else{
			// divide total temp and humidity at the end of the hour to get average
			avg_temp /= 60.0l;
			avg_humidity /= 60.0l;
		}

		// after one hour, do this
		if(counter == 0){
			// update cimis data and local ET
			hourlyCheck(C_to_F(avg_temp), avg_humidity);

			// give the LCD all the new data
			lcdUpdateInfo(C_to_F(dht.temperature), dht.humidity,
					cimis_data.air_temp, cimis_data.humidity,
					et_local, cimis_data.et0, water_saved);

			waterPlants();

			// reset temp and humidity accumulators
			avg_temp = 0.0l;
			avg_humidity = 0.0l;
		}
		else{
			// give LCD new data (namely, local temp and humidity)
			lcdUpdateInfo(C_to_F(dht.temperature), dht.humidity,
					cimis_data.air_temp, cimis_data.humidity,
					et_local, cimis_data.et0, water_saved);
		}

		delay(WAIT_TIME); // 1 minute delay
	}

	printf("\nExiting...\n");

	global_run_lcd = 0;
	pthread_join(lcd_tid, NULL);
	cleanupLCD();
	relayOff();

	return 0;
}

void hourlyCheck(double temp, double humidity){
	// if we successfully get the latest data from CIMIS
	if(get_latest_data(&cimis_data) == 0){
		double et = cimis_data.et0 * (temp / cimis_data.air_temp) * (cimis_data.humidity / humidity);
		et_local = et;
	}
}

void waterPlants(){
	double amount = WATER_AMT(et_local); // gallons per day
	int water_time = SECONDS((3600 * amount) / (24 * WATER_RATE));
	int tmp; // used to store return value from relayLoop
	int detect;
	int timeStalled = 0;

	lcdUpdateStatus(LCD_STATUS_WATERING);

	// water_time is the remaining number of milliseconds we need to water for
	while(water_time > 0){
		detect = getMotion();
		// relayLoop returns the remaining time after it's done with what it's doing
		tmp = relayLoop(detect, water_time, timeStalled);
		if(water_time == tmp){
			lcdUpdateStatus(LCD_STATUS_MOTION);
			timeStalled++;
		}
		else{
			lcdUpdateStatus(LCD_STATUS_WATERING);
		}
		water_time = tmp;
		delay(1); // wait a millisecond
	}
	relayOff();

	lcdUpdateStatus(LCD_STATUS_IDLE);

	// calculation is (gallons per day) / (24 hours) to get gallons of water used for this hour
	water_saved += ((WATER_AMT(cimis_data.et0) - amount) / 24);
}

