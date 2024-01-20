/*
 * main_task.c
 *
 *  Created on: Oct 20, 2023
 *      Author: andrew
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include "main.h"
#include "display.h"
#include "console.h"

/** */
void main_task(void)
{
	printf(CLS "LED clock built at %s on %s\n\n", __TIME__, __DATE__);

	display_start();

	while(true)
	{
		console_process_key();
	}
}
