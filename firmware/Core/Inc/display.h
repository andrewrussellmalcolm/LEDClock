/*
 * display.h
 *
 *  Created on: Oct 24, 2023
 *      Author: andrew
 */

#ifndef INC_DISPLAY_H_
#define INC_DISPLAY_H_

#include "stdint.h"
#include "stdbool.h"

void display_start(void);
void display_update(void);
void display_increase_brightness(void);
void display_decrease_brightness(void);
void display_show_alarm(bool show_alarm);
void display_alarm_enabled(bool enabled);
void display_alarm_time(bool time);

#endif /* INC_DISPLAY_H_ */
