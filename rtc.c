#include <LPC21xx.H>

#include "rtc_defines.h"
#include "types.h"
#include "lcd.h"
//#include "lcd_defines.h"



// Array to hold names of days of the week
char week[][4] = {"SUN","MON","TUE","WED","THU","FRI","SAT"};
u32 hr,min,sec,date,year,day,month;


/*
Initialize the Real-Time Clock (RTC)
This function disables the RTC, sets the prescaler values, 
and then enables the RTC.
*/
void RTC_Init(void) 
{
  // Disable and reset the RTC
	CCR = RTC_RESET;
  
#ifdef _LPC2148
  // Enable the RTC & select the clock source
	CCR = RTC_ENABLE | RTC_CLKSRC;  
#else
  // Set prescaler integer and fractional parts
	PREINT = PREINT_VAL;
	PREFRAC= PREFRAC_VAL;
  
  // Enable the RTC
	CCR = RTC_ENABLE;  
	
#endif

}

/*
Get the current RTC time
hour Pointer to store the current hour
minute Pointer to store the current minute
second Pointer to store the current second
*/
void GetRTCTimeInfo(u32 *hour, u32 *minute, u32 *second)
{
	*hour = HOUR;
	*minute = MIN;
	*second = SEC;
}

/*
Display the RTC time on LCD
hour value (0 23)
minute value (0 59)
second value (0 59) seperated by ':'
*/
void DisplayRTCTime(u32 hour, u32 minute, u32 second)
{
		Write_CMD_LCD(0x80);
		Write_DAT_LCD((hour/10)+48);
		Write_DAT_LCD((hour%10)+48);
		Write_DAT_LCD(':');
		Write_DAT_LCD((minute/10)+48);
		Write_DAT_LCD((minute%10)+48);
		Write_DAT_LCD(':');
		Write_DAT_LCD((second/10)+48);
		Write_DAT_LCD((second%10)+48);

}

/*
Get the current RTC date
day Pointer to store the current date (1 31)
month Pointer to store the current month (1 12)
year Pointer to store the current year (four digits)
*/
void GetRTCDateInfo(u32 *date, u32 *month, u32 *year)
{
	*date = DOM;
	*month = MONTH;
	*year = YEAR;
}

/*
Display the RTC date on LCD
Day of month (1 31)
Month (1 12)
Year (four digits) and seperated by '/'
*/
void DisplayRTCDate(u32 date, u32 month, u32 year)
{
		Write_CMD_LCD(0xC0);
		Write_DAT_LCD((date/10)+48);
		Write_DAT_LCD((date%10)+48);
		Write_DAT_LCD('/');
		Write_DAT_LCD((month/10)+48);
		Write_DAT_LCD((month%10)+48);
		Write_DAT_LCD('/');
		Write_int_LCD(year);
}

/*
Set the RTC time
Hour to set (0 23)
Minute to set (0 59)
Second to set (0 59)
*/
void SetRTCTimeInfo(u32 hour, u32 minute, u32 second)
{
	HOUR = hour;
	MIN = minute;
	SEC = second;
}

/*
Set the RTC date
day of month to set (1 31)
month to set (1 12)
year to set (four digits)
*/
void SetRTCDateInfo(u32 date, u32 month, u32 year)
{
	DOM = date;
	MONTH = month;
	YEAR = year;	
}

/*
Get the current day of the week
dow Pointer to store Day of Week (0=Sunday, ..., 6=Saturday)
*/
void GetRTCDay(u32 *day)
{
	*day = DOW; 
}

/*
Display the current day of the week on LCD
dow (Day of Week) (0=Sunday, ..., 6=Saturday)
*/
void DisplayRTCDay(u32 dow)
{
	Write_CMD_LCD(0X80+10);
	Write_str_LCD(week[dow]);  
}

/*
Set the day of the week in RTC
Day of Week to set (0=Sunday, ..., 6=Saturday)
*/		
void SetRTCDay(u32 day)
{
	DOW = day;
}

void time_disp(void)
{
	GetRTCTimeInfo(&hr,&min,&sec);
	GetRTCDateInfo(&date,&month,&year);
	GetRTCDay(&day);
	DisplayRTCTime(hr,min,sec);
	DisplayRTCDate(date,month,year);
	DisplayRTCDay(day);
}

