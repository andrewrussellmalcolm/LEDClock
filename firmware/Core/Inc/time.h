/*
 * time.h
 *
 *  Created on: Oct 24, 2023
 *      Author: andrew
 */

#ifndef INC_TIME_H_
#define INC_TIME_H_

#include "stdint.h"
#include "stdbool.h"

extern void set_time(uint8_t hours, uint8_t minutes);
extern void get_time(uint8_t *hours, uint8_t *minutes);
extern void set_alarm(uint8_t hours, uint8_t minutes);
extern void get_alarm(uint8_t *hours, uint8_t *minutes);
extern void enable_alarm(bool enable);
extern void increment_time(void);
extern void decrement_time(void);
extern void increment_alarm(void);
extern void decrement_alarm(void);

#endif /* INC_TIME_H_ */
