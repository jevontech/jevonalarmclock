#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio_ex.h"

/**
 ******************************************************************************
 * @file    PwmOutput.h
 * @author  Jeroen Vennegoor op Nijhuis
 * @version V1.0
 * @brief   defines timer and GPIO for PWM output
 ******************************************************************************
 */
#define TIMx                           TIM9
#define TIMx_CLK_ENABLE()              __TIM9_CLK_ENABLE()

/* Definition for TIMx Pins */
#define TIMx_CHANNEL_GPIO_PORT()       __GPIOE_CLK_ENABLE()
#define GPIO_PORT                      GPIOE
#define GPIO_PIN_CHANNEL1              GPIO_PIN_5
#define GPIO_AF_TIMx                   GPIO_AF3_TIM9
