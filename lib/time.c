#include <time.h>
#include <unistd.h>
#include "walib.h"

int wa_calendar(int* year, int* mon, int* day, int* hour, int* min, int* sec, int tz) {
	struct tm *cltm;
	time_t t = time(NULL) + tz;/*adjust time zone offset*/
	cltm = localtime(&t);
	year ? *year = cltm->tm_year+1900 : 0;
	mon ? *mon  = cltm->tm_mon+1 : 0;
	day ? *day  = cltm->tm_mday : 0;
	hour ? *hour = cltm->tm_hour : 0;
	min ? *min  = cltm->tm_min : 0;
	sec ? *sec  = cltm->tm_sec : 0;
	return (int)t;
}

void wa_msleep(int m) {
	usleep(m*1000);
}

int wa_rands(int from, int to){
#define SHUFFLE 0xF
	static int cnt = SHUFFLE;
	int ret, range=to-from;
	if ( (cnt++&SHUFFLE)==SHUFFLE ){ // several times reshuffle
	  srand(time(NULL)-cnt);
	}
	ret = from +( rand() % range );
	return ret;
}
