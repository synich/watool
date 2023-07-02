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

int wa_datediff(const char* from, const char* to) {
    int fall, tall;
    time_t fep, tep;
    struct tm ftm={0}, ttm={0};
    fall = atoi(from);
    if (fall <1000000) {fall+=20000000;}
    ftm.tm_year = fall/10000 - 1900;
    ftm.tm_mon = (fall/100)%100 - 1;
    ftm.tm_mday = fall%100;
    fep = mktime(&ftm);
    tall = atoi(to);
    if (tall <1000000) {tall+=20000000;}
    ttm.tm_year = tall/10000 - 1900;
    ttm.tm_mon = (tall/100)%100 - 1;
    ttm.tm_mday = tall%100;
    tep = mktime(&ttm);
    return (fep-tep)/86400;
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

