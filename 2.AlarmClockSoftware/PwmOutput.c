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

TIM_HandleTypeDef TimHandle;
uint32_t uwPrescalerValue = 0;
uint32_t TickFrequency = 0;
TIM_OC_InitTypeDef sConfigOC;

extern int alarmsound;

void Init_PwmOutput() {

	TIM_ClockConfigTypeDef sClockSourceConfig;
	TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig;
	TIM_MasterConfigTypeDef sMasterConfig;
	GPIO_InitTypeDef GPIO_InitStruct;

	/* TIMx Peripheral clock enable */
	TIMx_CLK_ENABLE();

	/* Enable GPIO channels Clock */
	TIMx_CHANNEL_GPIO_PORT();

	/* Configure  (TIMx_Channel) in Alternate function, push-pull and 100MHz speed */
	GPIO_InitStruct.Pin = GPIO_PIN_CHANNEL1;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	//GPIO_InitStruct.Pull = GPIO_NOPULL ;
	//GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Pull = GPIO_PULLUP;

	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF_TIMx;
	HAL_GPIO_Init(GPIO_PORT, &GPIO_InitStruct);

	TimHandle.Instance = TIM9;
	TimHandle.Init.Prescaler = 20; //(uint16_t)(SystemCoreClock / 10000000) - 1; // 1
	TickFrequency = 168000000 * 4 / (TimHandle.Init.Prescaler + 1);
	TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
	TimHandle.Init.Period = TickFrequency / 1000;
	TimHandle.Init.ClockDivision = 0;
	TimHandle.Init.RepetitionCounter = 0;

	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	HAL_TIM_ConfigClockSource(&TimHandle, &sClockSourceConfig);

	if (HAL_TIM_PWM_Init(&TimHandle) != HAL_OK) {
		__breakpoint(0);
	}

	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = 10000;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
	HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfigOC, TIM_CHANNEL_1);
}

void PlayAlarm(void) {
	int i = 0;
	int tone = 1000;

	TimHandle.Init.Period = TickFrequency / tone;
	//sConfigOC.Pulse = TimHandle.Init.Period/2;
	sConfigOC.Pulse = 20000;
	HAL_TIM_PWM_Init(&TimHandle);
	HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfigOC, TIM_CHANNEL_1);

	for (i = 0; i < 10; i++) {
		HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_1);
		osDelay(300);
		HAL_TIM_PWM_Stop(&TimHandle, TIM_CHANNEL_1);
		osDelay(200);
	}

}

void StartAlarmSound() {
	int i = 0;
	int tone = 1000;

	TimHandle.Init.Period = TickFrequency / tone;
	sConfigOC.Pulse = 20000;
	HAL_TIM_PWM_Init(&TimHandle);
	HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfigOC, TIM_CHANNEL_1);
	if (alarmsound == 1) {
		HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_1);
	}
}

void StopAlarmSound() {
	HAL_TIM_PWM_Stop(&TimHandle, TIM_CHANNEL_1);
}


