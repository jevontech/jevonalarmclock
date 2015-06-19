#include <cmsis_os.h>                                           // CMSIS RTOS header file
#include "AlarmClock.h"

/**
 ******************************************************************************
 * @file    Timers.c
 * @author  Jeroen Vennegoor op Nijhuis
 * @version V1.0
 * @brief   Various timers that are used in this application
 ******************************************************************************
 */
extern enum DeviceState deviceState;
extern void requestTime(void);
extern void saveAlarmTime(void);
extern void updateLightState(void);
extern void startAlarmSound(void);
extern void stopAlarmSound(void);

//Timer 1
static void timeoutTimerCallback(void const *arg); // prototype for timer callback function
static osTimerId id1;                                     // timer id
static uint32_t exec1;              // argument for the timer call back function
static osTimerDef (TimeoutTimer, timeoutTimerCallback);        // define timers
int Timeout = 6000;

static void timeoutTimerCallback(void const *arg) {
	if (deviceState != NORMAL) {
		saveAlarmTime();
		deviceState = NORMAL;
	}
}

void startTimeoutTimer() {
	osStatus status;
	if (id1 != NULL) {
		status = osTimerStart(id1, Timeout);
	}
}

//Timer 2
static void updateTimerCallback(void const *arg);
static osTimerId id2;
static uint32_t exec2;
static osTimerDef (UpdateTimer, updateTimerCallback);
int PeriodicUpdate = 5 * 60 * 1000;

static void updateTimerCallback(void const *arg) {
	requestTime();
}

//Timer 3
static void startSoundTimerCallback(void const *arg);
static osTimerId id3;
static uint32_t exec3;
static osTimerDef (StartSoundTimer, startSoundTimerCallback);
int startSoundTimerPeriod = 300;

//Timer 4
static void stopSoundTimerCallback(void const *arg);
static osTimerId id4;
static uint32_t exec4;
static osTimerDef (StopSoundTimer, stopSoundTimerCallback);
int stopSoundTimerTime = 150;

void StartStopSoundTimer() {
	osStatus status;
	if (id4 != NULL) {
		status = osTimerStart(id4, stopSoundTimerTime);
		if (status != osOK) {
			// Timer could not be started
		}
	}
}
static void startSoundTimerCallback(void const *arg) {
	startAlarmSound();
	StartStopSoundTimer();
}
static void stopSoundTimerCallback(void const *arg) {
	stopAlarmSound();
}

//Timer 5
static void lightUpdateTimerCallback(void const *arg);
static osTimerId id5;
static uint32_t exec5;
static osTimerDef (LightUpdateTimer, lightUpdateTimerCallback);
int lightUpdateTimerPeriod = 10000;
static void lightUpdateTimerCallback(void const *arg) {
	updateLightState();
}

void initTimers(void) {
	osStatus status;

	exec1 = 1;
	id1 = osTimerCreate(osTimer(TimeoutTimer), osTimerOnce, &exec1);
	startTimeoutTimer();
	exec2 = 2;
	id2 = osTimerCreate(osTimer(UpdateTimer), osTimerPeriodic, &exec2);
	if (id2 != NULL) {
		status = osTimerStart(id2, PeriodicUpdate);
	}
	exec3 = 3;
	id3 = osTimerCreate(osTimer(StartSoundTimer), osTimerPeriodic, &exec3);
	if (id3 != NULL) {
		status = osTimerStart(id3, startSoundTimerPeriod);
	}
	exec4 = 4;
	id4 = osTimerCreate(osTimer(StopSoundTimer), osTimerOnce, &exec4);
	exec5 = 5;
	id5 = osTimerCreate(osTimer(LightUpdateTimer), osTimerPeriodic, &exec5);
	if (id5 != NULL) {
		status = osTimerStart(id5, lightUpdateTimerPeriod);
	}

}

