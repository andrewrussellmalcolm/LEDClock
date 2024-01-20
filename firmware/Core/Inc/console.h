/*
 * console.h
 *
 *  Created on: 25 Nov 2021
 *      Author: andrew
 */

#ifndef INC_CONSOLE_H_
#define INC_CONSOLE_H_

typedef void (*pf_command)(const char *command_string);

void console_process_key(void);
#endif /* INC_CONSOLE_H_ */
