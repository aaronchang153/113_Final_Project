#ifndef CIMIS_H
#define CIMIS_H

// stores the data we need from CIMIS
struct CIMIS_data{
	int station;
	char date[15];
	int hour; // NOTE: this is in PST (not to be confused with PDT)
	double et0;
};

// gets the lastest information from the CIMIS FTP server and puts necessary information into the data struct
// returns 0 on success and non-zero otherwise
int get_latest_data(struct CIMIS_data *data);


#endif // CIMIS_H
