#include "motion.h"

int getMotion()
{
	int motion;

	pinMode(sensorPin, INPUT);
	if(digitalRead(sensorPin) == HIGH)
	{
		motion = 1;
	} else {
		motion = 0;
	}

	return motion;
}

#ifdef DEBUG
int main()
{
	int motion;
	while(1)
	{
		motion = getMotion();
		printf("motion: %d\n", motion);
	}
}
#endif
