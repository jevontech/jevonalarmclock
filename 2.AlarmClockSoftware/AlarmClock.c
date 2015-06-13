#include <stdio.h>
#include <time.h>
#include <cmsis_os.h>  
#include "AlarmClock.h"
#include "LightState.h"

/**
  ******************************************************************************
  * @file    AlarmClock.c
  * @author  Jeroen Vennegoor op Nijhuis
  * @version V1.0
  * @brief   AlarmClock functionality
  ******************************************************************************
*/

const int lightprogramduration = 3; //minutes
const int updownconstant = 5;
struct LightState wakelight;
osMutexDef( Mutex);
osMutexId mutex;
time_t clocktime;
int alarmtime_hours;
int alarmtime_minutes;
struct tm timeinfo;
int upButtonCounter;
int downButtonCounter;
enum DeviceState deviceState;
int alarmon;
int alarmstate;
int alarmsound;

//extern
extern void StartTimeoutTimer(void);
//extern time_t ReadRTC(void);
extern void WriteRTC(time_t t);
extern void SetAlarmRTC(int hours, int minutes);
//extern void PlayAlarm(void);
extern void SetLightState(struct LightState state);
extern void GetLightState(void);
extern struct LightState state;

void ToggleLight()
{
    GetLightState();
    if(state.on==1)
    {
        state.on=0;
    }
    else
    {
        state.on=1;
    }
    SetLightState(state);
}

void GetTime(time_t* time, int* hours, int* minutes, int* seconds) {
	*hours = localtime(time)->tm_hour;
	*minutes = localtime(time)->tm_min;
	*seconds = localtime(time)->tm_sec;
}

void SetClockTime(int hours, int minutes, int seconds) {
	timeinfo.tm_hour = hours;
	timeinfo.tm_min = minutes;
	timeinfo.tm_sec = seconds;
	clocktime = mktime(&timeinfo);

	osMutexWait(mutex, osWaitForever);
	WriteRTC(clocktime);
	osMutexRelease(mutex);
}

void SaveAlarmTime() {

	osMutexWait(mutex, osWaitForever);
	SetAlarmRTC(alarmtime_hours, alarmtime_minutes);
	osMutexRelease(mutex);
}

void SetAlarmTime(int hours, int minutes) {
	alarmtime_hours = hours;
	alarmtime_minutes = minutes;

}

void AddAlarmTime(int addminutes) {
	int hours = alarmtime_hours;
	int minutes = alarmtime_minutes;

	minutes = minutes + addminutes;

	if (minutes >= 60) {
		hours++;
		minutes = minutes - 60;
	}

	if (minutes < 0) {
		hours--;
		minutes = minutes + 60;
	}

	if (hours >= 24) {
		hours = hours - 24;
	}

	if (hours < 0) {
		hours = hours + 24;

	}

	SetAlarmTime(hours, minutes);
}

time_t GetAlarmTime() {
	//map alarm time on today's date
	//or tomorrow if today's alarm time has already passed

	time_t alarmtime;
	timeinfo.tm_year = localtime(&clocktime)->tm_year;
	timeinfo.tm_mon = localtime(&clocktime)->tm_mon;
	timeinfo.tm_mday = localtime(&clocktime)->tm_mday;
	timeinfo.tm_hour = alarmtime_hours;
	timeinfo.tm_min = alarmtime_minutes;
	timeinfo.tm_sec = 0;
	alarmtime = mktime(&timeinfo);

	if ((((int) alarmtime) - ((int) clocktime)) < 0) {
		time_t tomorrow = clocktime + 24 * 60 * 60;
		timeinfo.tm_year = localtime(&tomorrow)->tm_year;
		timeinfo.tm_mon = localtime(&tomorrow)->tm_mon;
		timeinfo.tm_mday = localtime(&tomorrow)->tm_mday;
		alarmtime = mktime(&timeinfo);
	}

	return alarmtime;

}

//time in seconds
int TimeUntilAlarm() {

	return GetAlarmTime() - clocktime;

}

void IncreaseAlarmTime(int minutes) {
	if (deviceState == SETALARM) {
		AddAlarmTime(minutes);
		StartTimeoutTimer();
	}
}
void DecreaseAlarmTime(int minutes) {
	if (deviceState == SETALARM) {
		AddAlarmTime(-minutes);
		StartTimeoutTimer();
	}

}

void CheckAlarm() {
	if (alarmon == 0) {
		alarmstate = 0;
		alarmsound = 0;
	}
	if (alarmstate == 1 && alarmsound != 1) {
		deviceState = ALARM;
		alarmsound = 1;
	}

}

void UpdateLightState() {

	int updatecycle = 10;
	int lightprogramdurationSeconds = lightprogramduration * 60;
	int t = lightprogramdurationSeconds - TimeUntilAlarm();
	int totalsteps = lightprogramdurationSeconds / updatecycle;

	if (t > 0 && alarmon == 1 && deviceState == NORMAL) {
		int bri = ((t / updatecycle) * 255) / totalsteps;
		wakelight.bri = bri;
		wakelight.on = 1;
		SetLightState(wakelight);
	}

}

void ToggleAlarm() {
	if (alarmon == 0) {
		alarmon = 1;
		deviceState = SETALARM;
		StartTimeoutTimer();
	} else {
		alarmon = 0;
		deviceState = NORMAL;
	}
	CheckAlarm();

}

void Init_AlarmClock() {
	wakelight.bri = 0;
	wakelight.sat = 255;
	wakelight.hue = 14910;
	wakelight.on = 0;
	
	timeinfo.tm_year = 2012 - 1900;
	timeinfo.tm_mon = 1;
	timeinfo.tm_mday = 1;
	timeinfo.tm_hour = 0;
	timeinfo.tm_min = 0;
	timeinfo.tm_sec = 0;
	timeinfo.tm_isdst = 0;
	mutex = osMutexCreate(osMutex(Mutex));

	SetClockTime(0, 0, 0);
	SetAlarmTime(0, 0);

	deviceState = NORMAL;
	alarmon = 0;
	alarmstate = 0;
	alarmsound = 0;

}
