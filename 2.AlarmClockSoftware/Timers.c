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

extern void RequestTime(void);
extern enum DeviceState deviceState;
extern void SaveAlarmTime();
extern void UpdateLightState();
extern void StartAlarmSound(void);
extern void StopAlarmSound(void);

//Timer 1
static void TimeoutTimer_Callback(void const *arg); // prototype for timer callback function
static osTimerId id1;                                     // timer id
static uint32_t exec1;              // argument for the timer call back function
static osTimerDef (TimeoutTimer, TimeoutTimer_Callback);        // define timers
int Timeout = 6000;

static void TimeoutTimer_Callback(void const *arg) {
	if (deviceState != NORMAL) {
		SaveAlarmTime();
		deviceState = NORMAL;
	}
}

void StartTimeoutTimer() {
	osStatus status;
	if (id1 != NULL)
	{
		status = osTimerStart(id1, Timeout);
		if (status != osOK) {
			// Timer could not be started
		}
	}
}

//Timer 2
static void UpdateTimer_Callback(void const *arg);
static osTimerId id2;
static uint32_t exec2;
static osTimerDef (UpdateTimer, UpdateTimer_Callback);
int PeriodicUpdate = 5 * 60 * 1000;

static void UpdateTimer_Callback(void const *arg) {
	RequestTime();
}

//Timer 3
static void StartSoundTimer_Callback(void const *arg);
static osTimerId id3;
static uint32_t exec3;
static osTimerDef (StartSoundTimer, StartSoundTimer_Callback);
int StartSoundTimerPeriod = 300;

//Timer 4
static void StopSoundTimer_Callback(void const *arg);
static osTimerId id4;
static uint32_t exec4;
static osTimerDef (StopSoundTimer, StopSoundTimer_Callback);
int StopSoundTimerTime = 150;

void StartStopSoundTimer() {
	osStatus status;
	if (id4 != NULL)
	{
		status = osTimerStart(id4, StopSoundTimerTime);
		if (status != osOK) {
			// Timer could not be started
		}
	}
}
static void StartSoundTimer_Callback(void const *arg) {
	StartAlarmSound();
	StartStopSoundTimer();
}
static void StopSoundTimer_Callback(void const *arg) {
	StopAlarmSound();
}

//Timer 5
static void LightUpdateTimer_Callback(void const *arg);
static osTimerId id5;
static uint32_t exec5;
static osTimerDef (LightUpdateTimer, LightUpdateTimer_Callback);
int LightUpdateTimerPeriod = 10000;
static void LightUpdateTimer_Callback(void const *arg) {
	UpdateLightState();
}

void Init_Timers(void) {
	osStatus status;

	exec1 = 1;
	id1 = osTimerCreate(osTimer(TimeoutTimer), osTimerOnce, &exec1);
	StartTimeoutTimer();

	exec2 = 2;
	id2 = osTimerCreate(osTimer(UpdateTimer), osTimerPeriodic, &exec2);
	if (id2 != NULL)
	{
		status = osTimerStart(id2, PeriodicUpdate);
		if (status != osOK) {
			// Timer could not be started
		}
	}

	exec3 = 3;
	id3 = osTimerCreate(osTimer(StartSoundTimer), osTimerPeriodic, &exec3);
	if (id3 != NULL)
	{
		status = osTimerStart(id3, StartSoundTimerPeriod);
		if (status != osOK) {
			// Timer could not be started
		}
	}

	exec4 = 4;
	id4 = osTimerCreate(osTimer(StopSoundTimer), osTimerOnce, &exec4);

	exec5 = 5;
	id5 = osTimerCreate(osTimer(LightUpdateTimer), osTimerPeriodic, &exec5);
	if (id5 != NULL)
	{
		status = osTimerStart(id5, LightUpdateTimerPeriod);
		if (status != osOK) {
			// Timer could not be started
		}
	}

}

