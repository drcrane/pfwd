#include "myconfig.h"
#include "timefn.h"
#include <malloc.h>
// #include "leakdetector.h"
#ifdef __COMPILE_FOR_WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#ifdef __COMPILE_FOR_LINUX
//#define _BSD_SOURCE
//#define _DEFAULT_SOURCE
#include <time.h>
#include <math.h>
#endif

#include <stdio.h>
#include <stdlib.h>

#ifdef __COMPILE_FOR_LINUX
#include <time.h>
#include <stdlib.h>
// can use this to avoid usage of timegm() see man page
time_t my_timegm(struct tm *tm) {
	time_t ret;
	char *tz;
	tz = getenv("TZ");
	setenv("TZ", "", 1);
	tzset();
	ret = mktime(tm);
	if (tz) {
		setenv("TZ", tz, 1);
	} else {
		unsetenv("TZ");
		tzset();
	}
	return ret;
}
#endif

uint64_t timefn_getunixmillis(int year, int month, int day, int hour, int minute, int second) {
#ifdef __COMPILE_FOR_WIN32
	FILETIME filetime;
	SYSTEMTIME systemtime;
	ULARGE_INTEGER resultingtime;

	systemtime.wDayOfWeek = 0;
	systemtime.wYear = year;
	systemtime.wMonth = month;
	systemtime.wDay = day;
	systemtime.wHour = hour;
	systemtime.wMinute = minute;
	systemtime.wSecond = second;
	systemtime.wMilliseconds = 0;
	SystemTimeToFileTime(&systemtime, &filetime);

	resultingtime.HighPart = filetime.dwHighDateTime;
	resultingtime.LowPart = filetime.dwLowDateTime;

	// remove the number of 100 nanosecond intervals between 1601 and 1970
	resultingtime.QuadPart = resultingtime.QuadPart - (11644473600000L * 10000L);
	// convert the 100 nanosecond intervals into millis
	resultingtime.QuadPart = resultingtime.QuadPart / 10000L;

	return (uint64_t)resultingtime.QuadPart;
#endif
#ifdef __COMPILE_FOR_LINUX
	struct tm tm;
	time_t unix_time;
	tm.tm_year = year;
	tm.tm_mon = month;
	tm.tm_mday = day;
	tm.tm_hour = hour;
	tm.tm_min = minute;
	tm.tm_sec = second;
	//unix_time = timegm(&tm);
	unix_time = my_timegm(&tm);
	return 0;
#endif
}

void timefn_gettimefromunixtimemillis(timefntime_t * timeoutput, int64_t unixtimemillis) {
#ifdef __COMPILE_FOR_WIN32
	FILETIME filetime;
	SYSTEMTIME systemtime;
	ULARGE_INTEGER inputtime;

	inputtime.QuadPart = unixtimemillis * 10000L;
	inputtime.QuadPart += (11644473600000L * 10000L);
	filetime.dwHighDateTime = inputtime.HighPart;
	filetime.dwLowDateTime = inputtime.LowPart;
	FileTimeToSystemTime(&filetime, &systemtime);

	timeoutput->year = systemtime.wYear;
	timeoutput->month = systemtime.wMonth;
	timeoutput->day = systemtime.wDay;
	timeoutput->hour = systemtime.wHour;
	timeoutput->minute = systemtime.wMinute;
	timeoutput->second = systemtime.wSecond;
#else
	struct tm tm;
	time_t unix_time;
	unix_time = unixtimemillis / 1000;
	gmtime_r(&unix_time, &tm);
	timeoutput->year = tm.tm_year;
	timeoutput->month = tm.tm_mon;
	timeoutput->day = tm.tm_mday;
	timeoutput->hour = tm.tm_hour;
	timeoutput->minute = tm.tm_min;
	timeoutput->second = tm.tm_sec;
#endif
}

void timefn_formattimefrommillis(char * dst, int64_t unixtimemillis) {
	timefntime_t timebuf;
	timefn_gettimefromunixtimemillis(&timebuf, unixtimemillis);
	timefn_formattimefromdatetimestruct_inplace(dst, &timebuf);
}

void timefn_formattimefromdatetimestruct_inplace(char * dst, timefntime_t * datetimestruct) {
	sprintf(dst, "%04d-%02d-%02dT%02d:%02d:%02d",
			datetimestruct->year, datetimestruct->month, datetimestruct->day,
			datetimestruct->hour, datetimestruct->minute, datetimestruct->second);
}

char * timefn_formattimefromdatetimestruct(timefntime_t * datetimestruct) {
	/* Simple helper, min size of dst is 19 bytes + 1 for null trm */
	char * dst = malloc(20);
	sprintf(dst, "%04d-%02d-%02dT%02d:%02d:%02d",
			datetimestruct->year, datetimestruct->month, datetimestruct->day,
			datetimestruct->hour, datetimestruct->minute, datetimestruct->second);
	return dst;
}

uint64_t timefn_getunixmillisfromtimestruct(timefntime_t * timestruct) {
	return timefn_getunixmillis(timestruct->year, timestruct->month, timestruct->day, timestruct->hour, timestruct->minute, timestruct->second);
}

uint64_t timefn_getcurrentunixtimemillis() {
#ifdef __COMPILE_FOR_WIN32
	SYSTEMTIME systemtime;
	GetSystemTime(&systemtime);
	return timefn_getunixmillis(systemtime.wYear, systemtime.wMonth, systemtime.wDay, systemtime.wHour, systemtime.wMinute, systemtime.wSecond) + systemtime.wMilliseconds;
#else
	long ms;
	time_t s;
	struct timespec spec;
	clock_gettime(CLOCK_REALTIME, &spec);
	ms = round(spec.tv_nsec / 1.0e6); // nano seconds to milliseconds
	if (ms > 999) {
		s ++;
		ms = 0;
	}
	return (s * 1000) + ms;
#endif
}

void timefn_getcurrenttimedatestruct(timefntime_t * timeoutput) {
#ifdef __COMPILE_FOR_WIN32
	SYSTEMTIME systemtime;
	GetSystemTime(&systemtime);
	timeoutput->year = systemtime.wYear;
	timeoutput->month = systemtime.wMonth;
	timeoutput->day = systemtime.wDay;
	timeoutput->hour = systemtime.wHour;
	timeoutput->minute = systemtime.wMinute;
	timeoutput->second = systemtime.wSecond;
#else
	//
#endif
}

uint64_t timefn_parsetimetomillis(char * src) {
	//timefntime_t timebuf;
	int year, month, day, hour, min, sec;
	sscanf(src, "%04d-%02d-%02dT%02d:%02d:%02d",
			&year, &month, &day, &hour, &min, &sec);
	return timefn_getunixmillis(year, month, day, hour, min, sec);
}

