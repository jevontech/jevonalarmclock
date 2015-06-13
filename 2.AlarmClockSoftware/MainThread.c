#include <stdio.h>
#include <time.h>
#include <cmsis_os.h>                                           // CMSIS RTOS header file
#include "stm32f4xx_hal.h"
#include "LightState.h"
//#include "GUI.h"

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
GPIO_PinState PwmPin;

const int SmallTimeStep = 1;
const int BigTimeStep = 15;

//extern
extern void RequestTime(void);
extern void ToggleLight(void);
extern void ToggleAlarm(void);
extern void CheckButtons(void);
extern int GetLightButtonState(void);
extern int GetAlarmButtonState(void);
extern int GetUpButtonState(void);
extern int GetDownButtonState(void);
extern void IncreaseAlarmTime(int);
extern void DecreaseAlarmTime(int);
extern void CheckAlarm(void);
extern void UpdateGUI(void);
extern int AlarmButtonHandled;

/**
 * @brief  Initialization
 * @param  None
 * @retval int
 */
int Init_MainThread(void) {
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
	RequestTime();
	//ForceGUIupdate();

	while (1) {
		CheckButtons();
		CheckAlarm();

		if (GetLightButtonState() == 1) {
			ToggleLight();
		}

		if (GetAlarmButtonState() == 1) {
			if (AlarmButtonHandled == 0) {
				ToggleAlarm();
				AlarmButtonHandled = 1;
			}
		}
		if (GetUpButtonState() > 0) {
			if (GetUpButtonState() == 1) {
				IncreaseAlarmTime(SmallTimeStep);
			} else {
				IncreaseAlarmTime(BigTimeStep);
			}

		}
		if (GetDownButtonState() > 0) {
			if (GetDownButtonState() == 1) {
				DecreaseAlarmTime(SmallTimeStep);
			} else {
				DecreaseAlarmTime(BigTimeStep);
			}

		}
		UpdateGUI();

		osDelay(50);
		osThreadYield();                                       // suspend thread
	}
}

