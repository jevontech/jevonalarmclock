#include "RTC.h"

/**
 ******************************************************************************
 * @file    RTC
 * @author  Jeroen Vennegoor op Nijhuis
 * @version V1.0
 * @brief   Wrapper for RTC functionality in HAL
 ******************************************************************************
 */

static RTC_HandleTypeDef RtcHandle;
static int rtcInitComplete = 0;
static int error = 1;

extern int alarmState;

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc) {
	alarmState = 1;
}

void RTC_Alarm_IRQHandler(void) {
	HAL_RTC_AlarmIRQHandler(&RtcHandle);
}

void initRTC(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct;
	uint32_t rtc_freq = 0;
	if (rtcInitComplete)
		return;
	rtcInitComplete = 1;

	RtcHandle.Instance = RTC;

	__PWR_CLK_ENABLE();
	HAL_PWR_EnableBkUpAccess();

	// Reset Backup domain
	__HAL_RCC_BACKUPRESET_FORCE();
	__HAL_RCC_BACKUPRESET_RELEASE();

	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI
			| RCC_OSCILLATORTYPE_LSE;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE; // Mandatory, otherwise the PLL is reconfigured!
	RCC_OscInitStruct.LSEState = RCC_LSE_OFF;
	RCC_OscInitStruct.LSIState = RCC_LSI_ON;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		error = 1;
	}
	// Connect LSI to RTC
	__HAL_RCC_RTC_CONFIG(RCC_RTCCLKSOURCE_LSI);
	rtc_freq = (uint32_t) 31250;

	// Enable RTC
	__HAL_RCC_RTC_ENABLE();

	HAL_NVIC_SetPriority(RTC_Alarm_IRQn, 0x0F, 0);
	HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);

	RtcHandle.Init.HourFormat = RTC_HOURFORMAT_24;
	RtcHandle.Init.AsynchPrediv = 127;
	RtcHandle.Init.SynchPrediv = (rtc_freq / 128) - 1;

	if (HAL_RTC_Init(&RtcHandle) != HAL_OK) {
		error = 1;
	}
}

void FreeRTC(void) {
	// Enable Power clock
	__PWR_CLK_ENABLE();

	// Enable access to Backup domain
	HAL_PWR_EnableBkUpAccess();

	// Reset Backup domain
	__HAL_RCC_BACKUPRESET_FORCE();
	__HAL_RCC_BACKUPRESET_RELEASE();

	// Disable access to Backup domain
	HAL_PWR_DisableBkUpAccess();

	rtcInitComplete = 0;
}

time_t readRTC(void) {
	RTC_DateTypeDef dateStruct;
	RTC_TimeTypeDef timeStruct;
	struct tm timeinfo;
	time_t t;

	RtcHandle.Instance = RTC;
	HAL_RTC_GetTime(&RtcHandle, &timeStruct, FORMAT_BIN);
	HAL_RTC_GetDate(&RtcHandle, &dateStruct, FORMAT_BIN);
	timeinfo.tm_wday = dateStruct.WeekDay;
	timeinfo.tm_mon = dateStruct.Month - 1;
	timeinfo.tm_mday = dateStruct.Date;
	timeinfo.tm_year = dateStruct.Year + 100;
	timeinfo.tm_hour = timeStruct.Hours;
	timeinfo.tm_min = timeStruct.Minutes;
	timeinfo.tm_sec = timeStruct.Seconds;
	t = mktime(&timeinfo);

	return t;
}

void writeRTC(time_t t) {
	RTC_DateTypeDef dateStruct;
	RTC_TimeTypeDef timeStruct;
	struct tm *timeinfo;

	RtcHandle.Instance = RTC;
	timeinfo = localtime(&t);
	dateStruct.WeekDay = timeinfo->tm_wday;
	dateStruct.Month = timeinfo->tm_mon + 1;
	dateStruct.Date = timeinfo->tm_mday;
	dateStruct.Year = timeinfo->tm_year - 100;
	timeStruct.Hours = timeinfo->tm_hour;
	timeStruct.Minutes = timeinfo->tm_min;
	timeStruct.Seconds = timeinfo->tm_sec;
	timeStruct.TimeFormat = RTC_HOURFORMAT_24;
	timeStruct.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	timeStruct.StoreOperation = RTC_STOREOPERATION_RESET;
	// Change the RTC current date/time
	HAL_RTC_SetDate(&RtcHandle, &dateStruct, FORMAT_BIN);
	HAL_RTC_SetTime(&RtcHandle, &timeStruct, FORMAT_BIN);
}


void setAlarmRTC(int hours, int minutes) {

	RTC_AlarmTypeDef alarmStruct;
	RTC_TimeTypeDef timeStruct;

	timeStruct.TimeFormat = RTC_HOURFORMAT_24;
	timeStruct.Hours = hours;
	timeStruct.Minutes = minutes;
	timeStruct.Seconds = 0;
	timeStruct.SubSeconds = 0x56;

	alarmStruct.Alarm = RTC_ALARM_A;
	alarmStruct.AlarmDateWeekDay = RTC_WEEKDAY_MONDAY;
	alarmStruct.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
	alarmStruct.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY;
	alarmStruct.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_None;
	alarmStruct.AlarmTime = timeStruct;
	HAL_RTC_SetAlarm_IT(&RtcHandle, &alarmStruct, FORMAT_BIN);

	if (HAL_RTC_SetAlarm_IT(&RtcHandle, &alarmStruct, FORMAT_BIN) != HAL_OK) {
		error = 1;
	}

}

