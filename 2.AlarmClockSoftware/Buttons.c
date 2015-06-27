#include "stm32f429xx.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"

/**
 ******************************************************************************
 * @file    Buttons.c
 * @author  Jeroen Vennegoor op Nijhuis
 * @version V1.0
 * @brief   Button press handler
 ******************************************************************************
 */

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

int counter1 = 0;
int counter2 = 0;
int counter3 = 0;
int counter4 = 0;
int lightButtonState = 0;
int alarmButtonState = 0;
int upButtonState = 0;
int downButtonState = 0;
int lightButtonHandled = 1;
int alarmButtonHandled = 1;
int upButtonHandled = 1;
int downButtonHandled = 1;
int thresholdShort = 3;
int thresholdLong = 30;

void initButtons(void) {
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

void checkButtons() {
	//software debouncing
	if (HAL_GPIO_ReadPin(LIGHTBUTTON_PORT, LIGHTBUTTON_PIN) != GPIO_PIN_SET) {
		counter1++;
	} else {
		counter1 = 0;
	}

	if (HAL_GPIO_ReadPin(ALARMBUTTON_PORT, ALARMBUTTON_PIN) != GPIO_PIN_SET) {
		counter2++;
	} else {
		counter2 = 0;
		alarmButtonHandled = 0;
	}

	if (HAL_GPIO_ReadPin(UPBUTTON_PORT, UPBUTTON_PIN) != GPIO_PIN_SET) {
		counter3++;
	} else {
		counter3 = 0;
	}

	if (HAL_GPIO_ReadPin(DOWNBUTTON_PORT, DOWNBUTTON_PIN) != GPIO_PIN_SET) {
		counter4++;
	} else {
		counter4 = 0;
	}

	if (counter1 >= thresholdShort) {
		if (counter1 >= thresholdLong) {
			lightButtonState = 2;
		} else {
			lightButtonState = 1;
		}
	} else {
		lightButtonState = 0;
	}

	if (counter2 >= thresholdShort) {
		if (counter2 >= thresholdLong) {
			alarmButtonState = 2;
		} else {
			alarmButtonState = 1;
		}
	} else {
		alarmButtonState = 0;
	}

	if (counter3 >= thresholdShort) {
		if (counter3 >= thresholdLong) {
			upButtonState = 2;
		} else {
			upButtonState = 1;
		}
	} else {
		upButtonState = 0;
	}

	if (counter4 >= thresholdShort) {
		if (counter4 >= thresholdLong) {
			downButtonState = 2;
		} else {
			downButtonState = 1;
		}
	} else {
		downButtonState = 0;
	}

}

int getLightButtonState() {
	return lightButtonState;
}

int getAlarmButtonState() {
	return alarmButtonState;
}

int getUpButtonState() {
	return upButtonState;
}

int getDownButtonState() {
	return downButtonState;
}
