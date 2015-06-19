#include <time.h>
#include "cmsis_os.h"  
#include <cstring>
#include "dialog.h"
#include "AlarmClock.h"

/**
 ******************************************************************************
 * @file    GuiThread
 * @author  Jeroen Vennegoor op Nijhuis
 * @version V1.0
 * @brief   GUI thread: takes care of updating the LCD
 ******************************************************************************
 */
void GuiThread(void const *argument);              // thread function
osThreadId tid_GuiThread;                           // thread id
osThreadDef(GuiThread, osPriorityIdle, 1, 2048);   // thread object
WM_HWIN hWin;
int guiUpdate;
int previousAlarmtimeHours;
int previousAlarmtimeMinutes;
extern int alarmOn;
int textUpdated = 1;
char TimeString[5] = "";
const int blinkOnTime = 600;
const int blinkOffTime = 400;
int blinkCounter = 0;
int blinkMode = 0;
int show = 1;
int previousMinute = -1;
char previousTimeString[5] = "--:--";
int previousAlarmOn = -1;
enum DeviceState previousState = NORMAL;
int previousShow = -1;

//extern
extern enum DeviceState deviceState;
extern int alarmtimeHours;
extern int alarmtimeMinutes;
extern time_t clockTime;
extern int guiNeedsUpdate(void);
extern WM_HWIN CreateWindow(void);
extern int getUpButtonState(void);
extern int getDownButtonState(void);
extern osMutexId mutex;
extern void startTimeoutTimer(void);
extern time_t readRTC(void);
extern void writeRTC(time_t t);
extern time_t getAlarmTime(void);

int initGuiThread(void) {
	tid_GuiThread = osThreadCreate(osThread(GuiThread), NULL);
	if (!tid_GuiThread)
		return (-1);

	guiUpdate = 0;

	return (0);
}

void GuiThread(void const *argument) {

	GUI_Init(); /* Initialize the Graphics Component */
	hWin = CreateWindow();

	while (1) {

		if (deviceState == SETALARM || deviceState == ALARM) {
			blinkMode = 1;
		}
		if (deviceState == NORMAL || getUpButtonState() > 0
				|| getDownButtonState() > 0) {
			blinkMode = 0;
			show = 1;
		}

		if (blinkMode > 0) {
			if (show == 1) {
				if (blinkCounter >= blinkOnTime) {
					show = 0;
					blinkCounter = 0;
				}

			} else {
				if (blinkCounter >= blinkOffTime) {
					show = 1;
					blinkCounter = 0;
				}

			}
			blinkCounter++;
		} else {
			show = 1;
		}

		if (guiUpdate == 1) {
			WM_InvalidateWindow(hWin);
		}

		GUI_Exec();
		GUI_X_ExecIdle();

	}
}

int guiNeedsUpdate() {

	int value = 0;

	if (alarmOn != previousAlarmOn)
		value = 1;
	if (deviceState != previousState || show != previousShow)
		value = 1;
	if (textUpdated == 1)
		value = 1;

	return value;

}

void updateGui() {
	clockTime = readRTC();
	if (previousMinute != localtime(&clockTime)->tm_min
			&& deviceState == NORMAL) {

		strftime(TimeString, 5, "%H:%M", localtime(&clockTime));
		previousMinute = localtime(&clockTime)->tm_min;
		textUpdated = 1;
	}
	if (deviceState != NORMAL) {
		previousMinute = -1;
	}

	if ((previousAlarmtimeHours != alarmtimeHours
			|| previousAlarmtimeMinutes != alarmtimeMinutes)
			&& deviceState == SETALARM) {
		time_t time = getAlarmTime();
		strftime(TimeString, 5, "%H:%M", localtime(&time));
		previousAlarmtimeHours = alarmtimeHours;
		previousAlarmtimeMinutes = alarmtimeMinutes;
		textUpdated = 1;
	}
	if (deviceState != SETALARM) {
		previousAlarmtimeHours = -1;
		previousAlarmtimeMinutes = -1;
	}

	guiUpdate = guiNeedsUpdate();
}

void forceGuiUpdate() {
	previousAlarmtimeHours = -1;
	previousAlarmtimeMinutes = -1;
	updateGui();
}
