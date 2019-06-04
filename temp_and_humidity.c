#include "temp_and_humidity.h"

float getTemp()
{
	DHT sensor;	//create a sensor object
	int getSensor;	//read the value of the sensor

		/*if initial wiring failed, print the error*/
	if(wiringPiSetup() == -1)
	{
		printf("setup wiringPi failed\n");
		return 1;
	}	

	getSensor = sensor.readDHT11(DHT11_Pin);
	switch(getSensor)
	{
		case DHT_OK:		//everything is good
			printf("Correct Value\n");
			break;
		case DHT_ERROR_CHECK:	//data check has errors
			printf("DHT_ERROR_CHECK\n");
			break;
		case DHT_ERROR_TIMEOUT:	//reading DHT times out
			printf("DHT_ERROR_TIMEOUT\n");
			break;
		case DHT_INVALID_VALUE:	//other errors
			printf("DHT_INVALID_VALUE");
			break;
	}

	return sensor.temp;
}


float getHumidity()
{
	DHT sensor;     //create a sensor object
        int getSensor;  //read the value of the sensor

                /*if initial wiring failed, print the error*/
        if(wiringPiSetup() == -1)
        {
                printf("setup wiringPi failed\n");
                return 1;
        }

        getSensor = sensor.readDHT11(DHT11_Pin);
        switch(getSensor)
        {
                case DHT_OK:            //everything is good
                        printf("Correct Value\n");
                        break;
                case DHT_ERROR_CHECK:   //data check has errors
                        printf("DHT_ERROR_CHECK\n");
                        break;
                case DHT_ERROR_TIMEOUT: //reading DHT times out
                        printf("DHT_ERROR_TIMEOUT\n");
                        break;
		case DHT_INVALID_VALUE:  //other errors
                        printf("DHT_INVALID_VALUE");
                        break;
        }

        return sensor.humidity;
}
