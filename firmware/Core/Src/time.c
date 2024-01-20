/*
 * time.c
 *
 *  Created on: Oct 24, 2023
 *      Author: andrew
 */

#include <stdio.h>

#include "main.h"
#include "time.h"
#include "buzzer.h"

extern RTC_HandleTypeDef hrtc;

// n.b. time and date must be set together

/** */
void set_time(uint8_t hours, uint8_t minutes)
{
    RTC_TimeTypeDef sTime = {.Hours = hours, .Minutes = minutes};
    RTC_DateTypeDef sDate = {0};
    HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
}

/** */
void get_time(uint8_t *hours, uint8_t *minutes)
{
    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};
    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
    *hours = sTime.Hours;
    *minutes = sTime.Minutes;
}

/** */
void increment_time(void)
{
    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};
    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
    uint8_t hours = sTime.Hours;
    uint8_t minutes = sTime.Minutes;

    if(minutes<59)
    {
    	minutes++;
    }
    else
    {
        hours++;
        minutes = 0;
    }

    sTime.Hours = hours;
    sTime.Minutes = minutes;
    HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
}

/** */
void decrement_time(void)
{
    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};
    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
    uint8_t hours = sTime.Hours;
    uint8_t minutes = sTime.Minutes;

    if(minutes>0)
    {
    	minutes--;
    }
    else
    {
        hours--;
        minutes = 59;
    }

    sTime.Hours = hours;
    sTime.Minutes = minutes;
    HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
}

/** */
void alarm_event_callback(RTC_HandleTypeDef *hrtc)
{
	printf("Alarm time reached\n");
	buzzer_start();
}

/** */
void set_alarm(uint8_t hours, uint8_t minutes)
{
	RTC_AlarmTypeDef sAlarm = {0};

	sAlarm.AlarmTime.Hours = hours;
	sAlarm.AlarmTime.Minutes = minutes;
	sAlarm.AlarmTime.Seconds = 0x0;
	sAlarm.AlarmTime.SubSeconds = 0x0;
	sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_SET;
	sAlarm.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY;
	sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
	sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
	sAlarm.AlarmDateWeekDay = 0;
	sAlarm.Alarm = RTC_ALARM_A;
	HAL_RTC_SetAlarm(&hrtc, &sAlarm, RTC_FORMAT_BIN);
}

/** */
void get_alarm(uint8_t *hours, uint8_t *minutes)
{
	RTC_AlarmTypeDef sAlarm = {0};

	HAL_RTC_GetAlarm(&hrtc, &sAlarm, RTC_ALARM_A, RTC_FORMAT_BIN);
	*hours = sAlarm.AlarmTime.Hours;
	*minutes = sAlarm.AlarmTime.Minutes;
}

/** */
void enable_alarm(bool enable)
{
	RTC_AlarmTypeDef sAlarm = {0};

	if(enable)
	{
		HAL_RTC_GetAlarm(&hrtc, &sAlarm, RTC_ALARM_A, RTC_FORMAT_BIN);
		HAL_RTC_RegisterCallback(&hrtc, HAL_RTC_ALARM_A_EVENT_CB_ID, alarm_event_callback);
		HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BIN);
	}
	else
	{
		HAL_RTC_DeactivateAlarm(&hrtc, RTC_ALARM_A);
	}
}

/** */
void increment_alarm(void)
{
	RTC_AlarmTypeDef sAlarm = {0};

	HAL_RTC_GetAlarm(&hrtc, &sAlarm, RTC_ALARM_A, RTC_FORMAT_BIN);
    uint8_t hours = sAlarm.AlarmTime.Hours;
    uint8_t minutes = sAlarm.AlarmTime.Minutes;

    if(minutes<59)
    {
    	minutes++;
    }
    else
    {
        hours++;
        minutes = 0;
    }

	sAlarm.AlarmTime.Hours = hours;
	sAlarm.AlarmTime.Minutes = minutes;

	HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BIN);
}

/** */
void decrement_alarm(void)
{
	RTC_AlarmTypeDef sAlarm = {0};

	HAL_RTC_GetAlarm(&hrtc, &sAlarm, RTC_ALARM_A, RTC_FORMAT_BIN);
    uint8_t hours = sAlarm.AlarmTime.Hours;
    uint8_t minutes = sAlarm.AlarmTime.Minutes;

    if(minutes>0)
    {
    	minutes--;
    }
    else
    {
        hours--;
        minutes = 59;
    }

	sAlarm.AlarmTime.Hours = hours;
	sAlarm.AlarmTime.Minutes = minutes;
	HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BIN);
}

