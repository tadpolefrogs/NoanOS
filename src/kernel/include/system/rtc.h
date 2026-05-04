#ifndef RTC_H
#define RTC_H

#include "core/types.h"

void rtc_get_time(u8* h, u8* m, u8* s);
void rtc_get_date(u8* d, u8* mo, u16* y);

#endif
