#include "LCD.h"

void lcdDisplayInfo(double local_temp, double local_humidity,
					double cimis_temp, double cimis_humidity,
					double local_et, double cimis_et,
					double water_savings){

    int i;
    pcf8574Setup(BASE,pcf8574_address);// initialize PCF8574
    for(i=0;i<8;i++){
        pinMode(BASE+i,OUTPUT);     // set PCF8574 port to output mode
    } 
    digitalWrite(LED,HIGH);     // turn on LCD backlight
    digitalWrite(RW,LOW);       // allow writing to LCD
    
    lcdhd = lcdInit(2,16,4,RS,EN,D4,D5,D6,D7,0,0,0,0);// initialize LCD and return “handle” used to handle LCD
    
    if(lcdhd == -1){
        printf("lcdInit failed !");
        return;
    }
    
    while(1){
        lcdPosition(lcdhd,0,0);     // set the LCD cursor position to (0,0) 
        lcdPrintf(lcdhd,"local_temp:%.2fF  local_humidity:%.2f  local_et:%.2f  water_savings:%.2f",local_temp, local_humidity, local_et, water_savings);// Display Local values on LCD
        
        lcdPosition(lcdhd,0,1);     // set the LCD cursor position to (0,1) 
        lcdPrintf(lcdhd,"cimis_temp:%.2fF  cimis_humidity:%.2f  cimis_et:%.2f  water_savings:%.2f",cimis_temp, cimis_humidity, cimis_et, water_savings);// Display CIMIS values on LCD
        
        delay(1000);
    }
}

/*

//main function for testing purpose

int main(void){
    wiringPiSetupGpio();
    double local_temp, local_humidity, cimis_temp, cimis_humidity, local_et, cimis_et, water_savings;
    
    local_temp = 25.20;
    local_humidity = 30.50;
    cimis_temp = 26.20;
    cimis_humidity = 31.50;
    local_et = 10.10;
    cimis_et = 11.10;
    water_savings = 40.45;

    lcdDisplayInfo( local_temp, local_humidity, cimis_temp, cimis_humidity, local_et, cimis_et, water_savings);
    
    return 0;
}
*/