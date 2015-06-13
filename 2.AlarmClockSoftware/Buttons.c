#include "stm32f429xx.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"

#define BUTTON1_PORT               GPIOA
#define BUTTON1_PIN               GPIO_PIN_0

#define LIGHTBUTTON_PORT               GPIOE
#define LIGHTBUTTON_PIN               GPIO_PIN_6

#define ALARMBUTTON_PORT               GPIOE
#define ALARMBUTTON_PIN               GPIO_PIN_3

#define UPBUTTON_PORT               GPIOE
#define UPBUTTON_PIN               GPIO_PIN_4

#define DOWNBUTTON_PORT               GPIOE
#define DOWNBUTTON_PIN               GPIO_PIN_2

int Counter1 = 0;
int Counter2 = 0;
int Counter3 = 0;
int Counter4 = 0;
int LightButtonState = 0;
int AlarmButtonState = 0;
int UpButtonState = 0;
int DownButtonState = 0;
int LightButtonHandled = 1;
int AlarmButtonHandled = 1;
int UpButtonHandled = 1;
int DownButtonHandled = 1;
int ThresholdShort = 3;
int ThresholdLong = 30;

void Init_Buttons(void) {
	GPIO_InitTypeDef GPIO_InitStruct;
	__GPIOE_CLK_ENABLE();
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Pin = LIGHTBUTTON_PIN;
	HAL_GPIO_Init(LIGHTBUTTON_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = ALARMBUTTON_PIN;
	HAL_GPIO_Init(ALARMBUTTON_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = UPBUTTON_PIN;
	HAL_GPIO_Init(UPBUTTON_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = DOWNBUTTON_PIN;
	HAL_GPIO_Init(DOWNBUTTON_PORT, &GPIO_InitStruct);
}

void CheckButtons() {
	//software debouncing
	if (HAL_GPIO_ReadPin(LIGHTBUTTON_PORT, LIGHTBUTTON_PIN) != GPIO_PIN_SET) {
		Counter1++;
	} else {
		Counter1 = 0;
	}

	if (HAL_GPIO_ReadPin(ALARMBUTTON_PORT, ALARMBUTTON_PIN) != GPIO_PIN_SET) {
		Counter2++;
	} else {
		Counter2 = 0;
		AlarmButtonHandled = 0;
	}

	if (HAL_GPIO_ReadPin(UPBUTTON_PORT, UPBUTTON_PIN) != GPIO_PIN_SET) {
		Counter3++;
	} else {
		Counter3 = 0;
	}

	if (HAL_GPIO_ReadPin(DOWNBUTTON_PORT, DOWNBUTTON_PIN) != GPIO_PIN_SET) {
		Counter4++;
	} else {
		Counter4 = 0;
	}

	if (Counter1 >= ThresholdShort) {
		if (Counter1 >= ThresholdLong) {
			LightButtonState = 2;
		} else {
			LightButtonState = 1;
		}
	} else {
		LightButtonState = 0;
	}

	if (Counter2 >= ThresholdShort) {
		if (Counter2 >= ThresholdLong) {
			AlarmButtonState = 2;
		} else {
			AlarmButtonState = 1;
		}
	} else {
		AlarmButtonState = 0;
	}

	if (Counter3 >= ThresholdShort) {
		if (Counter3 >= ThresholdLong) {
			UpButtonState = 2;
		} else {
			UpButtonState = 1;
		}
	} else {
		UpButtonState = 0;
	}

	if (Counter4 >= ThresholdShort) {
		if (Counter4 >= ThresholdLong) {
			DownButtonState = 2;
		} else {
			DownButtonState = 1;
		}
	} else {
		DownButtonState = 0;
	}

}

int GetLightButtonState() {
	return LightButtonState;
}

int GetAlarmButtonState() {
	return AlarmButtonState;
}

int GetUpButtonState() {
	return UpButtonState;
}

int GetDownButtonState() {
	return DownButtonState;
}
