#include <stdio.h>
#include <wiringPi.h>
#include "cimis.h"
#include "temp_and_humidity.h"
#include "DHT.hpp"
#include "relay.h"
#include "motion.h"


#define AREA           200 // square feet
#define WATER_RATE    1020 // gallons per hour
#define IE            0.75
#define PF             0.5

// in gallons per day
#define WATER_AMT(et) ((et)*PF*AREA*0.62/IE)

// input seconds, gives milliseconds
#define SECONDS(x) ((x) * 1000)

static double et_local[3];
static int et_index;
static int et_count;


void hourlyCheck(double temp, double humidity);
void waterPlants();

static double avgET(){
	double avg = 0.0l;
	for(int i = 0; i < et_count; i++)
		avg += et_local[i];
	return avg / (double) et_count;
}

int main(){
	if(wiringPiSetup() == -1){
		fprintf(stderr, "wiringPi setup failed,\n");
		return -1;
	}

	/*** Call whatever additional setup is needed here ***/
	relaySetup();
	setupMotion();

	et_index = 0;
	et_count = 0;

	DHT dht;

	double avg_temp = 0.0l;
	double avg_humidity = 0.0l;

	// just a flag to know when the for loop is doing its first run
	// used to properly calculate average temp+humidity
	char first_run = 1;

	// counts between 0 and 59, gets incremented eveytime local temp + humidity is checked
	// used to track when an hour has passed
	int counter = 59; // starts at 59 so the first iteration of the loop sets it to 0

	for(;;){
		counter = (counter + 1) % 60;

		dht.readDHT11(DHT_PIN);

		avg_temp += dht.temperature;
		avg_humidity += dht.humidity;
		
		if(first_run){
			first_run = 0;
		}
		else{
			avg_temp /= 60.0l;
			avg_humidity /= 60.0l;
		}

		hourlyCheck(avg_temp, avg_humidity);

		waterPlants();

		avg_temp = 0.0l;
		avg_humidity = 0.0l;

		delay(SECONDS(60)); // 1 minute delay
	}

	return 0;
}

void hourlyCheck(double temp, double humidity){
	struct CIMIS_data cimis_data;

	// if we successfully get the latest data from CIMIS
	if(get_latest_data(&cimis_data) == 0){
		double et = cimis_data.et0 * (temp / cimis_data.air_temp) * (cimis_data.humidity / humidity);
		et_index = (et_index + 1) % 3;
		et_local[et_index] = et;
		if(et_count < 3)
			et_count++;
	}
}

void waterPlants(){
	double amount = WATER_AMT(avgET()); // gallons per day
	int water_time = SECONDS((3600 * amount) / (24 * WATER_RATE));
	int tmp;
	int detect;
	int timeStalled = 0;

	// water_time is the remaining number of milliseconds we need to water for
	while(water_time > 0){
		detect = getMotion();
		tmp = relayLoop(detect, water_time, timeStalled);
		if(water_time == tmp)
			timeStalled++;
		water_time = tmp;
		delay(1); // wait a millisecond
	}
}

