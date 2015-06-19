#include <stdio.h>
#include <time.h>
#include <cmsis_os.h>                                           // CMSIS RTOS header file
#include "stm32f4xx_hal.h"
#include "LightState.h"

/**
 ******************************************************************************
 * @file    MainThread.c
 * @author  Jeroen Vennegoor op Nijhuis
 * @version V1.0
 * @brief   Main thread
 ******************************************************************************
 */

void MainThread(void const *argument);                        // thread function
osThreadId tid_MainThread;                                          // thread id
osThreadDef(MainThread, osPriorityNormal, 1, 0);                // thread object
GPIO_PinState pwmPin;

const int smallTimeStep = 1;
const int bigTimeStep = 15;

//extern
extern int alarmButtonHandled;
extern void requestTime(void);
extern void toggleLight(void);
extern void toggleAlarm(void);
extern void checkButtons(void);
extern int getLightButtonState(void);
extern int getAlarmButtonState(void);
extern int getUpButtonState(void);
extern int getDownButtonState(void);
extern void increaseAlarmTime(int);
extern void decreaseAlarmTime(int);
extern void checkAlarm(void);
extern void updateGui(void);


/**
 * @brief  Initialization
 * @param  None
 * @retval int
 */
int initMainThread(void) {
	tid_MainThread = osThreadCreate(osThread(MainThread), NULL);
	if (!tid_MainThread)
		return (-1);

	return (0);
}

/**
 * @brief  Main thread loop
 * @param  argument
 * @retval None
 */
void MainThread(void const *argument) {

	//update RTC with time from the Internet
	requestTime();

	while (1) {
		checkButtons();
		checkAlarm();

		if (getLightButtonState() == 1) {
			toggleLight();
		}

		if (getAlarmButtonState() == 1) {
			if (alarmButtonHandled == 0) {
				toggleAlarm();
				alarmButtonHandled = 1;
			}
		}
		if (getUpButtonState() > 0) {
			if (getUpButtonState() == 1) {
				increaseAlarmTime(smallTimeStep);
			} else {
				increaseAlarmTime(bigTimeStep);
			}

		}
		if (getDownButtonState() > 0) {
			if (getDownButtonState() == 1) {
				decreaseAlarmTime(smallTimeStep);
			} else {
				decreaseAlarmTime(bigTimeStep);
			}

		}
		updateGui();

		osDelay(50);
		osThreadYield();                                       // suspend thread
	}
}

