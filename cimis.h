#ifndef CIMIS_H
#define CIMIS_H

// stores the data we need from CIMS
struct CIMIS_data{
	int station;
	char date[15];
	int hour; // NOTE: this is in PST (not to be confused with PDT)
	int et0;
};

int get_latest_data(struct CIMIS_data *data);


#endif // CIMIS_H
