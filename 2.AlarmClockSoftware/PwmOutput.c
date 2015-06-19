#include <cmsis_os.h>
#include "PwmOutput.h"

/**
 ******************************************************************************
 * @file    PwmOutput.c
 * @author  Jeroen Vennegoor op Nijhuis
 * @version V1.0
 * @brief   Wrapper for HAL PWM functionality
 ******************************************************************************
 */

TIM_HandleTypeDef timHandle;
//uint32_t prescalerValue = 0;
uint32_t tickFrequency = 0;
TIM_OC_InitTypeDef configOC;

extern int alarmSound;

void initPwmOutput() {

	TIM_ClockConfigTypeDef sClockSourceConfig;
	GPIO_InitTypeDef GPIO_InitStruct;
	/* TIMx Peripheral clock enable */
	TIMx_CLK_ENABLE();
	/* Enable GPIO channels Clock */
	TIMx_CHANNEL_GPIO_PORT();

	/* Configure  (TIMx_Channel) in Alternate function, push-pull and 100MHz speed */
	GPIO_InitStruct.Pin = GPIO_PIN_CHANNEL1;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF_TIMx;
	HAL_GPIO_Init(GPIO_PORT, &GPIO_InitStruct);

	timHandle.Instance = TIM9;
	timHandle.Init.Prescaler = 20; //(uint16_t)(SystemCoreClock / 10000000) - 1; // 1
	tickFrequency = 168000000 * 4 / (timHandle.Init.Prescaler + 1);
	timHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
	timHandle.Init.Period = tickFrequency / 1000;
	timHandle.Init.ClockDivision = 0;
	timHandle.Init.RepetitionCounter = 0;

	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	HAL_TIM_ConfigClockSource(&timHandle, &sClockSourceConfig);

	if (HAL_TIM_PWM_Init(&timHandle) != HAL_OK) {
		__breakpoint(0);
	}

	configOC.OCMode = TIM_OCMODE_PWM1;
	configOC.Pulse = 10000;
	configOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	configOC.OCFastMode = TIM_OCFAST_DISABLE;
	configOC.OCIdleState = TIM_OCIDLESTATE_RESET;
	HAL_TIM_PWM_ConfigChannel(&timHandle, &configOC, TIM_CHANNEL_1);
}

void startAlarmSound() {
	int tone = 1000;

	timHandle.Init.Period = tickFrequency / tone;
	configOC.Pulse = 20000;
	HAL_TIM_PWM_Init(&timHandle);
	HAL_TIM_PWM_ConfigChannel(&timHandle, &configOC, TIM_CHANNEL_1);
	if (alarmSound == 1) {
		HAL_TIM_PWM_Start(&timHandle, TIM_CHANNEL_1);
	}
}

void stopAlarmSound() {
	HAL_TIM_PWM_Stop(&timHandle, TIM_CHANNEL_1);
}

