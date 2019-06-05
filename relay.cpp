#include <wiringPi.h>
#include <stdio.h>

void relaySetup(void){
     pinmode(relayPin, OUTPUT);
}
void relayOff(void){
     digitalWrite(relayPin, LOW);
}
static void relayOn(void){
     digitalWrite(relayPin, HIGH);
}
int  relayCheckState(void){
     int reading;
     reading = digitalRead(relayPin);
     return reading;
}
int relayLoop(int detection, int time, int timeStalled){
     if ((detection == 0) || (timeStalled > longestWaitTime)){
          printf("Sprinkling...");
          time--;
          relayOn();
     }
     else if((detection == 1) && (timeStalled <= longestWaitTime)){
          printf("Movement Detected, Turning OFF");
          relayOff();
     }
     return time;
}

#ifdef DEBUG
int main()
{
  int time = 10;
  int count = 10;
	while(count>0)
	{
    count--;
		time = relayLoop(0,time,0);
		delay(2000);
	}
  printf("Test Program end.\n", );
  return 0;
}

#endif
