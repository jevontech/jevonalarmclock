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

const int lightProgramDuration = 1; //just 1 minute for demo & testing purposes
const int upDownConstant = 5;
struct LightState wakeLight;
osMutexDef( Mutex);
osMutexId mutex;
time_t clockTime;
int alarmtimeHours;
int alarmtimeMinutes;
struct tm timeInfo;
int upButtonCounter;
int downButtonCounter;
enum DeviceState deviceState;
int alarmOn;
int alarmState;
int alarmSound;

//extern
extern void startTimeoutTimer(void);
extern void writeRTC(time_t t);
extern void setAlarmRTC(int hours, int minutes);
extern void setLightState(struct LightState state);
extern void getLightState(void);
extern struct LightState state;

void toggleLight()
{
    getLightState();
    if(state.on==1)
    {
        state.on=0;
    }
    else
    {
        state.on=1;
    }
    setLightState(state);
}

void getTime(time_t* time, int* hours, int* minutes, int* seconds) {
	*hours = localtime(time)->tm_hour;
	*minutes = localtime(time)->tm_min;
	*seconds = localtime(time)->tm_sec;
}

void setClockTime(int hours, int minutes, int seconds) {
	timeInfo.tm_hour = hours;
	timeInfo.tm_min = minutes;
	timeInfo.tm_sec = seconds;
	clockTime = mktime(&timeInfo);

	osMutexWait(mutex, osWaitForever);
	writeRTC(clockTime);
	osMutexRelease(mutex);
}

void saveAlarmTime() {
	osMutexWait(mutex, osWaitForever);
	setAlarmRTC(alarmtimeHours, alarmtimeMinutes);
	osMutexRelease(mutex);
}

void setAlarmTime(int hours, int minutes) {
	alarmtimeHours = hours;
	alarmtimeMinutes = minutes;
}

void addAlarmTime(int addminutes) {
	int hours = alarmtimeHours;
	int minutes = alarmtimeMinutes;

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

	setAlarmTime(hours, minutes);
}

time_t getAlarmTime() {
	//map alarm time on today's date
	//or tomorrow if today's alarm time has already passed

	time_t alarmtime;
	timeInfo.tm_year = localtime(&clockTime)->tm_year;
	timeInfo.tm_mon = localtime(&clockTime)->tm_mon;
	timeInfo.tm_mday = localtime(&clockTime)->tm_mday;
	timeInfo.tm_hour = alarmtimeHours;
	timeInfo.tm_min = alarmtimeMinutes;
	timeInfo.tm_sec = 0;
	alarmtime = mktime(&timeInfo);

	if ((((int) alarmtime) - ((int) clockTime)) < 0) {
		time_t tomorrow = clockTime + 24 * 60 * 60;
		timeInfo.tm_year = localtime(&tomorrow)->tm_year;
		timeInfo.tm_mon = localtime(&tomorrow)->tm_mon;
		timeInfo.tm_mday = localtime(&tomorrow)->tm_mday;
		alarmtime = mktime(&timeInfo);
	}

	return alarmtime;

}

//time in seconds
int getTimeUntilAlarm() {

	return getAlarmTime() - clockTime;

}

void increaseAlarmTime(int minutes) {
	if (deviceState == SETALARM) {
		addAlarmTime(minutes);
		startTimeoutTimer();
	}
}
void decreaseAlarmTime(int minutes) {
	if (deviceState == SETALARM) {
		addAlarmTime(-minutes);
		startTimeoutTimer();
	}

}

void checkAlarm() {
	if (alarmOn == 0) {
		alarmState = 0;
		alarmSound = 0;
	}
	if (alarmState == 1 && alarmSound != 1) {
		deviceState = ALARM;
		alarmSound = 1;
	}

}

void updateLightState() {

	int updateCycle = 10;
	int lightProgramDurationInSeconds = lightProgramDuration * 60;
	int time = lightProgramDurationInSeconds - getTimeUntilAlarm();
	int totalSteps = lightProgramDurationInSeconds / updateCycle;

	if (time > 0 && alarmOn == 1 && deviceState == NORMAL) {
		int brightness = ((time / updateCycle) * 255) / totalSteps;
		wakeLight.bri = brightness;
		wakeLight.on = 1;
		setLightState(wakeLight);
	}

}

void toggleAlarm() {
	if (alarmOn == 0) {
		alarmOn = 1;
		deviceState = SETALARM;
		startTimeoutTimer();
	} else {
		alarmOn = 0;
		deviceState = NORMAL;
	}
	checkAlarm();

}

void initAlarmClock() {
	wakeLight.bri = 0;
	wakeLight.sat = 255;
	wakeLight.hue = 13910;
	wakeLight.on = 0;
	
	timeInfo.tm_year = 2012 - 1900;
	timeInfo.tm_mon = 1;
	timeInfo.tm_mday = 1;
	timeInfo.tm_hour = 0;
	timeInfo.tm_min = 0;
	timeInfo.tm_sec = 0;
	timeInfo.tm_isdst = 0;
	mutex = osMutexCreate(osMutex(Mutex));

	setClockTime(0, 0, 0);
	setAlarmTime(0, 0);

	deviceState = NORMAL;
	alarmOn = 0;
	alarmState = 0;
	alarmSound = 0;

}
