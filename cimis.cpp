#include "cimis.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <curl/curl.h>

#define OUTFILE "recv/cimis_data.csv"


struct FTPFile{
	const char *filename;
	FILE *stream;
};


static size_t fwrite_cb(void *buffer, size_t size, size_t n, void *stream){
	struct FTPFile *out = (struct FTPFile *) stream;
	if(out->stream == NULL){
		out->stream = fopen(out->filename, "wb");
		if(out->stream == NULL)
			return -1;
	}
	return fwrite(buffer, size, n, out->stream);
}

static void get_time_date(int *hour, char *date){
	time_t raw_time;
	struct tm *t;
	time(&raw_time);
	t = localtime(&raw_time);

	// convert the hour to a time (sort of)
	// like, hour 13 goes to 1300 hours
	t->tm_hour *= 100;
	// -100 to get the last hour that has completely passed
	t->tm_hour -= 100;
	
	*hour = (t->tm_isdst) ? t->tm_hour - 100 : t->tm_hour;
	sprintf(date, "%d/%d/%d", t->tm_mon, t->tm_mday, t->tm_year + 1900);
}

static int find_most_recent(struct CIMIS_data *data){
	FILE *fp = fopen(OUTFILE, "r");

	if(fp == NULL)
		return -1;

	int hour;
	char date[15];

	get_time_date(&hour, date);
	
	char buffer[100];   // this stores the entire line read from the file
	char info[25];   // this is used to hold the part we actually care about

	while(fgets(buffer, 100, fp) != NULL){
		strncpy(info, buffer, 25);

		char *tok = strtok(info, ",");
		data->station = atoi(tok);
		tok = strtok(NULL, ",");
		strncpy(data->date, tok, 15);
		if(strcmp(data->date, date) != 0){
			continue;
		}
		tok = strtok(NULL, ",");
		data->hour = atoi(tok);
		if(data->hour != hour){
			continue;
		}
		tok = strtok(NULL, ",");
		data->et0 = atoi(tok);
	}
}


int get_latest_data(struct CIMIS_data *data){
	CURL *curl;
	CURLcode res;
	struct FTPFile ftpfile = {OUTFILE, NULL};
	curl_global_init(CURL_GLOBAL_DEFAULT);

	curl = curl_easy_init();
	if(curl){
		curl_easy_setopt(curl, CURLOPT_URL, "ftp://ftpcimis.water.ca.gov/pub2/hourly/hourly075.csv");
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, fwrite_cb);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ftpfile);

		//curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

		res = curl_easy_perform(curl);

		curl_easy_cleanup(curl);

		if(CURLE_OK != res){
			fprintf(stderr, "Failed to get file. curl error code: %d\n", res);
			return -1;
		}
	}

	if(ftpfile.stream != NULL){
		fclose(ftpfile.stream);
	}

	curl_global_cleanup();

	if(find_most_recent(data) != 0)
		return -2;

	return 0;
}

int main(){
	struct CIMIS_data d;
	get_latest_data(&d);
	printf("%d,%s,%d,%d\n", d.station, d.date, d.hour, d.et0);
	return 0;
}
