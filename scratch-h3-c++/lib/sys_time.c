/**
 * @file sys_time.c
 *
 */
/* Copyright (C) 2020 by Arjan van Vught mailto:info@orangepi-dmx.nl
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/*
 * PoC Code -> Do not use in production
 */

#include <time.h>
#include <sys/time.h>

#include "../rtc/rtc.h"

#include "debug.h"

void __attribute__((cold)) sys_time_init(void) {
	struct tm tmbuf;
	struct tm tm_rtc;

	/*
	 * The mktime function ignores the specified contents of the tm_wday and tm_yday members of the broken- down time structure.
	 */

	if (!rtc_start(RTC_PROBE)) {
		tmbuf.tm_hour = 0;
		tmbuf.tm_min = 0;
		tmbuf.tm_sec = 0;
		tmbuf.tm_mday = _TIME_STAMP_DAY_;			// The day of the month, in the range 1 to 31.
		tmbuf.tm_mon = _TIME_STAMP_MONTH_ - 1;		// The number of months since January, in the range 0 to 11.
		tmbuf.tm_year = _TIME_STAMP_YEAR_ - 1900;	// The number of years since 1900.
		tmbuf.tm_isdst = 0; 						// 0 (DST not in effect, just take RTC time)

		const time_t seconds = mktime(&tmbuf);
		const struct timeval tv = { .tv_sec = seconds, .tv_usec = 0};

		settimeofday(&tv, NULL);

		DEBUG_PRINTF("%.4d/%.2d/%.2d %.2d:%.2d:%.2d", tmbuf.tm_year, tmbuf.tm_mon, tmbuf.tm_mday, tmbuf.tm_hour, tmbuf.tm_min, tmbuf.tm_sec);
		DEBUG_PRINTF("%s", asctime(localtime((const time_t *) &seconds)));

		return;
	}

	rtc_get_date_time(&tm_rtc);

	const time_t rtc_seconds = mktime(&tm_rtc);
	const struct timeval tv = { .tv_sec = rtc_seconds, .tv_usec = 0 };

	settimeofday(&tv, NULL);

	DEBUG_PUTS("RTC found");
	DEBUG_PRINTF("%.4d/%.2d/%.2d %.2d:%.2d:%.2d", tm_rtc.tm_year, tm_rtc.tm_mon, tm_rtc.tm_mday, tm_rtc.tm_hour, tm_rtc.tm_min, tm_rtc.tm_sec);
	DEBUG_PRINTF("rtc_seconds=%u", rtc_seconds);
	DEBUG_PRINTF("%s", asctime(localtime((const time_t *) &rtc_seconds)));
}
