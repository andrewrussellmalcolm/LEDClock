/*
 * display.c
 *
 *  Created on: Oct 24, 2023
 *      Author: andrew
 */

#include <stdio.h>
#include <stdbool.h>

#include "main.h"
#include "time.h"
#include "display.h"

extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim2;

#define MAX_BRIGHTNESS 4750
#define MIN_BRIGHTNESS 1000
#define INC_BRIGHTNESS 250

// digit (0 to 9) to seven-segment pattern

static uint8_t  patterns[16] =
{
		//ABCDEFG
		0b1111110, // 0
		0b0110000, // 1
		0b1101101, // 2
		0b1111001, // 3
		0b0110011, // 4
		0b1011011, // 5
		0b1011111, // 6
		0b1110000, // 7
		0b1111111, // 8
		0b1111011, // 9
		0b0000000, // blank
		0b0000000, // blank
		0b0000000, // blank
		0b0000000, // blank
		0b0000000, // blank
		0b0000000, // blank
};

typedef struct _bcd_time
{
	uint8_t digit[4]; // hh:mm 1 byte per digit
}
bcd_time_t;

// time refreshed from timer tick
static bcd_time_t display_time;

// toggle for colon
static bool colon_on = false;

// toggle for alarm enabled indicator (DP)
static bool alarm_enabled = false;

// display alarm time instead if real time
static bool alarm_time = false;

// brightness (multiplexing pulse width from 1000 to 5000
static int32_t brightness = 5000;

/** turn off all digit selects */
static void deselect_digit(void)
{
	HAL_GPIO_WritePin(DIG_1_GPIO_Port, DIG_1_Pin, 1);
	HAL_GPIO_WritePin(DIG_2_GPIO_Port, DIG_2_Pin, 1);
	HAL_GPIO_WritePin(DIG_3_GPIO_Port, DIG_3_Pin, 1);
	HAL_GPIO_WritePin(DIG_4_GPIO_Port, DIG_4_Pin, 1);
	HAL_GPIO_WritePin(COLON_GPIO_Port, COLON_Pin, 0);
	HAL_GPIO_WritePin(DP_GPIO_Port, DP_Pin, 0);
}

/** select a specific digit, 0 to 3 */
static void select_digit(int digit)
{
	switch( digit & 0x03)
	{
		case 0:	HAL_GPIO_WritePin(DIG_1_GPIO_Port, DIG_1_Pin, 0); break;
		case 1:	HAL_GPIO_WritePin(DIG_2_GPIO_Port, DIG_2_Pin, 0); break;
		case 2:	HAL_GPIO_WritePin(DIG_3_GPIO_Port, DIG_3_Pin, 0); break;
		case 3:	HAL_GPIO_WritePin(DIG_4_GPIO_Port, DIG_4_Pin, 0); break;
	}

	// turn colon on when requested
	if(((digit & 0x03) == 0x03) && colon_on)
	{
		HAL_GPIO_WritePin(COLON_GPIO_Port, COLON_Pin, 1);
	}

	// display decimal point in rightmost digit if alarm is enabled
	if(((digit & 0x03) == 0) && alarm_enabled)
	{
		HAL_GPIO_WritePin(DP_GPIO_Port, DP_Pin, 1);
	}
}

/** write to selected digit */
static void write_digit(int digit)
{
	uint8_t pattern = patterns[digit & 0x0f];

	HAL_GPIO_WritePin(SEG_G_GPIO_Port, SEG_G_Pin, (pattern >> 0 & 1));
	HAL_GPIO_WritePin(SEG_F_GPIO_Port, SEG_F_Pin, (pattern >> 1 & 1));
	HAL_GPIO_WritePin(SEG_E_GPIO_Port, SEG_E_Pin, (pattern >> 2 & 1));
	HAL_GPIO_WritePin(SEG_D_GPIO_Port, SEG_D_Pin, (pattern >> 3 & 1));
	HAL_GPIO_WritePin(SEG_C_GPIO_Port, SEG_C_Pin, (pattern >> 4 & 1));
	HAL_GPIO_WritePin(SEG_B_GPIO_Port, SEG_B_Pin, (pattern >> 5 & 1));
	HAL_GPIO_WritePin(SEG_A_GPIO_Port, SEG_A_Pin, (pattern >> 6 & 1));
}

/** */
static void multiplexer_timer_callback(TIM_HandleTypeDef *htim)
{
	static uint8_t digit;

	if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
	{
		// leading zero suppression (on hours only)
		if(digit == 3 && display_time.digit[digit] == 0x00)
		{
			display_time.digit[digit] = 0x0a;
		}

		write_digit(display_time.digit[digit]);
		select_digit(digit);
	}

	if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
	{
		deselect_digit();

		if(++digit == 4)
		{
			digit = 0;
		}
	}
}

/** */
static void update_timer_callback(TIM_HandleTypeDef *timer)
{
	display_update();
}

/** */
static void set_brightness(uint32_t brightness)
{
	TIM_OC_InitTypeDef sConfigOC = {0};
	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = 0;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	sConfigOC.Pulse = brightness;
	HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2);
}

/** */
void display_start(void)
{
	// start the update timer (1s period)
	HAL_TIM_RegisterCallback(&htim2, HAL_TIM_PERIOD_ELAPSED_CB_ID, update_timer_callback);
	HAL_TIM_Base_Start_IT(&htim2);

	// start the multiplexer timer (100ms period)
	HAL_TIM_RegisterCallback(&htim3, HAL_TIM_PWM_PULSE_FINISHED_CB_ID, multiplexer_timer_callback);
	HAL_TIM_PWM_Start_IT(&htim3,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start_IT(&htim3,TIM_CHANNEL_2);

	set_brightness(MAX_BRIGHTNESS);
}

/** */
void display_update(void)
{
	uint8_t hours, minutes;

	if(alarm_time)
	{
		get_alarm(&hours, &minutes);
	    colon_on = false;
	}
	else
	{
		get_time(&hours, &minutes);
	    colon_on = !colon_on;
	}

	uint8_t bcd_hours = RTC_ByteToBcd2(hours);
	uint8_t bcd_minutes = RTC_ByteToBcd2(minutes);

	display_time.digit[3] = ( bcd_hours & 0xf0 ) >> 4;
	display_time.digit[2] = ( bcd_hours & 0x0f ) >> 0;
	display_time.digit[1] = ( bcd_minutes & 0xf0 ) >> 4;
	display_time.digit[0] = ( bcd_minutes & 0x0f ) >> 0;
}

/** */
void display_increase_brightness(void)
{
	if(brightness < MAX_BRIGHTNESS)
	{
		brightness+=INC_BRIGHTNESS;
	}
	set_brightness(brightness);
}

/** */
void display_decrease_brightness(void)
{
	if(brightness > MIN_BRIGHTNESS)
	{
		brightness-=INC_BRIGHTNESS;
	}
	set_brightness(brightness);
}

/** */
void display_alarm_enabled(bool enabled)
{
	alarm_enabled = enabled;
}

/** */
void  display_alarm_time(bool time)
{
	alarm_time = time;
}
