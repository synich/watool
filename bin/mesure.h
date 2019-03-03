#ifndef __TIME_MESURE_H__
#define __TIME_MESURE_H__

#include <time.h>

#define MESURE_MS(x) do{\
	clock_t start, end;\
	start = clock();\
	(x);\
	end = clock();\
	printf("%s elapsed %ums\n", #x, end - start);\
}while(0)


#ifdef _WIN32
#include <windows.h>

#define MESURE_US(x) do{\
	LARGE_INTEGER freq, start, end;\
	double dfreq;\
	QueryPerformanceFrequency(&freq);\
	dfreq = (double)freq.QuadPart;\
	QueryPerformanceCounter(&start);\
	(x);\
	QueryPerformanceCounter(&end);\
	printf("%s elapsed %.1fus\n", #x,\
	  (double)(end.QuadPart-start.QuadPart)/dfreq*1e6);\
}while(0)

#else
#include <sys/time.h>

#define MESURE_US(x) do{\
  struct timeval bgt, edt;\
  struct timezone zone;\
  gettimeofday(&bgt, &zone);\
  (x);\
  gettimeofday(&edt, &zone);\
  printf("%s elapsed %d us\n", #x, edt.tv_usec-bgt.tv_usec);\
}while(0)

#endif

#endif //__TIME_MESURE_H__
