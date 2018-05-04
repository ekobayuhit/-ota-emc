// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
#ifndef RTCDS1307_H
#define RTCDS1307_H

extern String RTCTime;
extern long yearRTC;
extern long monthRTC;
extern long dayRTC;
extern long AmountTimeRTC;

extern long calculateAmountTime(String value);
extern void calculateAmountDate1(String value);
extern void calculateAmountDate2(String value);
extern void scheduleTime();
extern void setup_rtc();
extern void setup_date_on_rtc();
extern void readtime();
#endif
