#include <stdio.h>
#include <wiringPi.h>
#include "cimis.h"
#include "temp_and_humidity.h"


static double et_local[3];
static int et_index;
static int et_count;


void hourlyCheck(float temp, float humidity);

int main(){
	if(wiringPiSetup() == -1){
		fprintf(stderr, "wiringPi setup failed,\n");
		return -1;
	}

	/*** Call whatever additional setup is needed here ***/

	et_index = 0;
	et_count = 0;

	float avg_temp = 0.0f;
	float avg_humidity = 0.0f;

	// counts between 0 and 59, gets incremented eveytime local temp + humidity is checked
	// used to track when an hour has passed
	int counter = 59; // starts at 59 so the first iteration of the loop sets it to 0

	for(;;){
		counter = (counter + 1) % 60;

		avg_temp += getTemp();
		avg_humidity += getHumidity();
	}

	return 0;
}

void hourlyCheck(float temp, float humidity){
	struct CIMIS_data cimis_data;

	// if we successfully get the latest data from CIMIS
	if(get_latest_data(&cimis_data) == 0){
		double et = cimis_data.et0 * ((double) temp / cimis_data.air_temp) * (cimis_data.humidity / (double) humidity);
		et_index = (et_index + 1) % 3;
		et_local[et_index] = et;
	}
}