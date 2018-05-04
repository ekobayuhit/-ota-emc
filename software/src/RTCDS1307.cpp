// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
#include <Wire.h>
#include "RTClib.h"
#include "config.h"
#include "RTCDS1307.h"
#include "relay.h"

RTC_DS1307 rtc;
long year1,month1,date1,year2,month2,date2;
String RTCTime;
long yearRTC;
long monthRTC;
long dayRTC;
long AmountTimeRTC;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

void setup_rtc() {
  Wire.begin();
  rtc.begin();
}

void setup_date_on_rtc() {
  rtc.adjust(DateTime(__DATE__,__TIME__));
  rtc.adjust(DateTime(2017, 12, 25, 19, 0, 15));
  config_save_rtc("1");
}

void readtime() {
    DateTime now = rtc.now();
    RTCTime="";
    RTCTime=String(now.year())+'/'+String(now.month())+'/'+String(now.day())+(" (");
    RTCTime=RTCTime+String(daysOfTheWeek[now.dayOfTheWeek()])+") "+String(now.hour())+':'+String(now.minute())+':'+String(now.second());
    yearRTC=now.year();
    monthRTC=now.month();
    dayRTC=now.day();
    AmountTimeRTC=now.hour()*3600;
    AmountTimeRTC+= now.minute()*60;
    AmountTimeRTC+= now.second();
}

long calculateAmountTime(String value){
  long AmountTime;
  char aa=value[0];
  AmountTime=(aa - '0')*10*3600;
  aa=value[1];
  AmountTime+= (aa - '0')*3600;
  aa=value[2];
  AmountTime+= (aa - '0')*10*60;
  aa=value[3];
  AmountTime+= (aa - '0')*60;
  aa=value[4];
  AmountTime+= (aa - '0')*10;
  aa=value[5];
  AmountTime+= (aa - '0');
  return AmountTime;
}

void calculateAmountDate1(String value){
  date1=0;month1=0;year1=0;
  char aa=value[0];
  date1=(aa - '0')*10;
  aa=value[1];
  date1=date1+(aa - '0');
  aa=value[2];
  month1=(aa - '0')*10;
  aa=value[3];
  month1=month1+(aa - '0');
  aa=value[4];
  year1=(aa - '0')*1000;
  aa=value[5];
  year1=year1+(aa - '0')*100;
  aa=value[6];
  year1=year1+(aa - '0')*10;
  aa=value[7];
  year1=year1+(aa - '0');
}

void calculateAmountDate2(String value){
  date2=0;month2=0;year2=0;
  char aa=value[0];
  date2=(aa - '0')*10;
  aa=value[1];
  date2=date2+(aa - '0');
  aa=value[2];
  month2=(aa - '0')*10;
  aa=value[3];
  month2=month2+(aa - '0');
  aa=value[4];
  year2=(aa - '0')*1000;
  aa=value[5];
  year2=year2+(aa - '0')*100;
  aa=value[6];
  year2=year2+(aa - '0')*10;
  aa=value[7];
  year2=year2+(aa - '0');
}

void scheduleTime(){
  calculateAmountDate1(sch_datestart);
  long timestart=calculateAmountTime(sch_timestart);
  calculateAmountDate2(sch_datestop);
  long timestop=calculateAmountTime(sch_timestop);
  if(yearRTC>=year1 && yearRTC<=year2){
    if(year2-yearRTC>0){
      TurnONLoad();//turn on load
    }
    else if(monthRTC>=month1 && monthRTC<=month2){
      if(month2-monthRTC>0){
        TurnONLoad();//turn on load
      }
      else if(dayRTC>=date1 && dayRTC<=date2){
        if(date2-dayRTC>0){
          TurnONLoad();//turn on load
        }
        else if(AmountTimeRTC>=timestart && AmountTimeRTC<=timestop){
          TurnONLoad();//turn on load
        }
        else{
          TurnOFFLoad();//turn off load
        }
      }
      else{
        TurnOFFLoad();//turn off load
      }
    }
    else{
      TurnOFFLoad();//turn off load
    }
  }
  else{
    TurnOFFLoad();//turn off load
  }
}
