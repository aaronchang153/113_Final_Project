#include <stdio.h>
#include "cimis.h"


// just a placeholder for now
int main(){
	struct CIMIS_data d;
	get_latest_data(&d);
	printf("%d,%s,%d,%f\n", d.station, d.date, d.hour, d.et0);
	return 0;
}
