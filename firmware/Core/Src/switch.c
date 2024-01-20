/*
 * switch.c
 *
 *  Created on: Oct 25, 2023
 *      Author: andrew
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include "main.h"
#include "time.h"
#include "buzzer.h"
#include "display.h"

extern TIM_HandleTypeDef htim6;
extern TIM_HandleTypeDef htim7;

static bool timer_running = false;
static bool last_switch1_state = 1;
static bool last_switch2_state = 1;
static bool last_switch3_state = 1;
static bool last_switch4_state = 1;
static bool last_switch5_state = 1;
static uint32_t active_GPIO_Pin = 0;

static bool alarm_enable_toggle = false;

/** if SW1 or SW2 still down after 500ms, repeat inc/dec action */
static void repeat_callback(TIM_HandleTypeDef *timer)
{
	bool switch5_state = HAL_GPIO_ReadPin(SW5_INT_GPIO_Port, SW5_INT_Pin);

	if(HAL_GPIO_ReadPin(SW1_INT_GPIO_Port, SW1_INT_Pin) == 0)
	{
		if(switch5_state) // alternate function (alarm, if alarm button is held down)
		{
			increment_time();
			printf("increment time repeat\n");
		}
		else
		{
			increment_alarm();
			printf("increment alarm repeat\n");
		}

		display_update();

		// restart the repeat timer
		HAL_TIM_RegisterCallback(&htim6, HAL_TIM_PERIOD_ELAPSED_CB_ID, repeat_callback);
		HAL_TIM_Base_Start_IT(&htim6);

	}
	else if(HAL_GPIO_ReadPin(SW2_INT_GPIO_Port, SW2_INT_Pin) == 0)
	{
		if(switch5_state) // alternate function (alarm, if alarm button is held down)
		{
			decrement_time();
			printf("decrement time repeat\n");
		}
		else
		{
			decrement_alarm();
			printf("decrement alarm repeat\n");
		}

		display_update();
		printf("decrement time repeat\n");

		// restart the repeat timer
		HAL_TIM_RegisterCallback(&htim6, HAL_TIM_PERIOD_ELAPSED_CB_ID, repeat_callback);
		HAL_TIM_Base_Start_IT(&htim6);
	}
	else if(HAL_GPIO_ReadPin(SW5_INT_GPIO_Port, SW5_INT_Pin) == 0)
	{
		if(alarm_enable_toggle)
		{
			printf("cancel alarm\n");
			enable_alarm(false);
			display_alarm_enabled(false);
			alarm_enable_toggle = false;
		}
		else
		{
			printf("enable alarm\n");
			enable_alarm(true);
			display_alarm_enabled(true);
			alarm_enable_toggle = true;
		}

		HAL_TIM_Base_Stop(&htim6);
	}
	else
	{
		// no button held down, cancel the repeat
		printf("no repeat\n");
		display_alarm_time(false);
		HAL_TIM_Base_Stop(&htim6);
	}
}

/** debounce switches: if state is still same as initial afer 20ms, confim action */
static void switch_callback(TIM_HandleTypeDef *timer)
{
	HAL_TIM_Base_Stop(&htim7);
	bool switch1_state = HAL_GPIO_ReadPin(SW1_INT_GPIO_Port, SW1_INT_Pin);
	bool switch2_state = HAL_GPIO_ReadPin(SW2_INT_GPIO_Port, SW2_INT_Pin);
	bool switch3_state = HAL_GPIO_ReadPin(SW3_INT_GPIO_Port, SW3_INT_Pin);
	bool switch4_state = HAL_GPIO_ReadPin(SW4_INT_GPIO_Port, SW4_INT_Pin);
	bool switch5_state = HAL_GPIO_ReadPin(SW5_INT_GPIO_Port, SW5_INT_Pin);

	switch(active_GPIO_Pin)
	{
		case SW1_INT_Pin:
		if(switch1_state == last_switch1_state)
		{
			if(!switch1_state)
			{
				if(switch5_state) // alternate function (increment alarm, if alarm button is held down)
				{
					increment_time();
					printf("increment time\n");
				}
				else
				{
					increment_alarm();
					printf("increment alarm\n");
				}

				display_update();
			}

			break;
		}

		case SW2_INT_Pin:
		if(switch2_state == last_switch2_state)
		{
			if(!switch2_state)
			{
				if(switch5_state) // alternate function (decrement alarm, if alarm button is held down)
				{
					decrement_time();
					printf("decrement time\n");
				}
				else
				{
					decrement_alarm();
					printf("decrement alarm\n");
				}

				display_update();
			}

			break;
		}

		case SW3_INT_Pin:
		if(switch3_state == last_switch3_state)
		{
			if(!switch3_state)
			{
				display_increase_brightness();
				printf("display increase brightness\n");
			}

			break;
		}

		case SW4_INT_Pin:
		if(switch4_state == last_switch4_state)
		{
			if(!switch4_state)
			{
				display_decrease_brightness();
				printf("display decrease brightness\n");
			}

			break;
		}

		case SW5_INT_Pin:
		if(switch5_state == last_switch5_state)
		{
			if(!switch5_state) // alarm button has action of falling and rising edges
			{
				display_alarm_time(true);
				printf("display show alarm\n");
			}
			else
			{
				display_alarm_time(false);
				printf("display show time\n");
			}
			buzzer_stop();

			break;
		}
	}

	active_GPIO_Pin = 0;
	timer_running = false;
}

/** interrupt callback called when one of the switches is pushed (pin goes low - and also high for switch 5) */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(!timer_running)
	{
		switch(GPIO_Pin)
		{
			case SW1_INT_Pin: last_switch1_state = HAL_GPIO_ReadPin(SW1_INT_GPIO_Port, SW1_INT_Pin); active_GPIO_Pin = SW1_INT_Pin; break;
			case SW2_INT_Pin: last_switch2_state = HAL_GPIO_ReadPin(SW2_INT_GPIO_Port, SW2_INT_Pin); active_GPIO_Pin = SW2_INT_Pin; break;
			case SW3_INT_Pin: last_switch3_state = HAL_GPIO_ReadPin(SW3_INT_GPIO_Port, SW3_INT_Pin); active_GPIO_Pin = SW3_INT_Pin; break;
			case SW4_INT_Pin: last_switch4_state = HAL_GPIO_ReadPin(SW4_INT_GPIO_Port, SW4_INT_Pin); active_GPIO_Pin = SW4_INT_Pin; break;
			case SW5_INT_Pin: last_switch5_state = HAL_GPIO_ReadPin(SW5_INT_GPIO_Port, SW5_INT_Pin); active_GPIO_Pin = SW5_INT_Pin; break;
		}

		// start a timer to wait for 20ms, then resample in ISR. only act if the two samples match
		timer_running = true;
		HAL_TIM_RegisterCallback(&htim7, HAL_TIM_PERIOD_ELAPSED_CB_ID, switch_callback);
		HAL_TIM_Base_Start_IT(&htim7);

		// start a timer to wait 500ms. If a button is still down when this timer files, the action is repeated
		__HAL_TIM_CLEAR_FLAG(&htim6, TIM_SR_UIF);
		HAL_TIM_RegisterCallback(&htim6, HAL_TIM_PERIOD_ELAPSED_CB_ID, repeat_callback);
		HAL_TIM_Base_Start_IT(&htim6);
	}
}
