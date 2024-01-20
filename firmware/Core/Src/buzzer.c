/*
 * buzzer.c
 *
 *  Created on: Oct 24, 2023
 *      Author: andrew
 */

#include "main.h"

extern TIM_HandleTypeDef htim17;

static uint32_t on_off_count;

/** */
static void buzzer_timer_callback(TIM_HandleTypeDef *timer)
{
	// generate bursts of square wave (and flash debug LEDs)
	if(((on_off_count++ /500) % 2) == 0)
	{
		HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, 0);
		HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, 0);

		HAL_GPIO_TogglePin(BUZZER_GPIO_Port, BUZZER_Pin);
	}
	else
	{
		HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, 1);
		HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, 1);
	}
}

/** */
void buzzer_start(void)
{
	htim17.Instance = TIM17;
	htim17.Init.Prescaler = 48; // counts in microseconds
	htim17.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim17.Init.Period = 1000; // 1000us (~1kHz square wave)
	htim17.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim17.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	HAL_TIM_Base_Init(&htim17);
	HAL_TIM_RegisterCallback(&htim17, HAL_TIM_PERIOD_ELAPSED_CB_ID, buzzer_timer_callback);
	on_off_count = 0;
	HAL_TIM_Base_Start_IT(&htim17);
}

///** */
void buzzer_stop(void)
{
	HAL_TIM_Base_Stop(&htim17);
	HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, 0);
}
