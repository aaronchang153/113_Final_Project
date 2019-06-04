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
