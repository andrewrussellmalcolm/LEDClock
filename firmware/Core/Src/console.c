/*
 * console.c
 *
 *  Created on: 25 Nov 2021
 *      Author: andrew
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include "main.h"
#include "console.h"
#include "time.h"
#include "display.h"

#define HISTORY_DEPTH 5
static char input[60];
static char input_history[HISTORY_DEPTH][60];
static int8_t history_index = 0;
static uint32_t inputIndex = 0;

typedef struct _command
{
	const char * const command;
	const char * const help_string;
	const pf_command command_interpreter;
}
command_t;

// forward references
static void console_help_command(const char *command_string);
static void console_test_command(const char *command_string);
static void console_time_command(const char *command_string);
static void console_alarm_command(const char *command_string);
static void console_history_command(const char *command_string);
static bool console_arg_is(const char *name, const char *arg, uint8_t length);
static const char* console_get_parameter( const char *command_string, uint8_t wanted_parameter, uint8_t *parameter_string_length);

/** list of commands */
static command_t commands[] =
{
    {"help", "help on commands", console_help_command},
    {"test", "test only for debugging", console_test_command},
	{"time", "time get | set HH:MM set or get the time", console_time_command},
	{"alarm", "alarm get | set HH:MM | enable | cancel set or get the alarm and enable or cancel", console_alarm_command},
	{"history", "linux-like command history", console_history_command},
};

/** */
static void console_help_command(const char *command_string)
{
	for(int idx = 0; idx < sizeof(commands)/sizeof(command_t); idx++)
	{
		printf("%s: %s\n", commands[idx].command, commands[idx].help_string);
	}
}

/** */
static void console_test_command(const char *command_string)
{
	printf("test command %s\n", command_string);
	fprintf(stderr, "test command %s\n", command_string);
}

/** */
void console_time_command(const char *command_string)
{
    uint8_t arg1Length;
    uint8_t arg2Length;
    const char *arg1 = console_get_parameter(command_string, 1, &arg1Length);
    const char *arg2 = console_get_parameter(command_string, 2, &arg2Length);

    if (console_arg_is("get", arg1, arg1Length))
    {
        uint8_t hours, minutes;
        get_time(&hours, &minutes);
        printf("The time is set to %2.2d:%2.2d\n", hours, minutes);
    }
    else if (console_arg_is("set", arg1, arg1Length))
    {
        if (arg2)
        {
            uint32_t hours, minutes;
            if (sscanf(arg2, "%d:%d", (unsigned int *)&hours, (unsigned int *)&minutes) == 2)
            {
                set_time(hours, minutes);
                display_update();
                printf("Time set to %2.2ld:%2.2ld\n", hours, minutes);
            }
            else
            {
                printf("Parse error. Enter date and time in the format\nHH:MM\n");
            }
        }
        else
        {
            printf("missing argument");
        }
    }
    else
    {
        printf("incorrect argument\n");
    }
}

/** */
void console_alarm_command(const char *command_string)
{
    uint8_t arg1Length;
    uint8_t arg2Length;
    const char *arg1 = console_get_parameter(command_string, 1, &arg1Length);
    const char *arg2 = console_get_parameter(command_string, 2, &arg2Length);

    if (console_arg_is("get", arg1, arg1Length))
    {
        uint8_t hours, minutes;
        get_alarm(&hours, &minutes);
        printf("The alarm is set to %2.2d:%2.2d\n", hours, minutes);
    }
    else if (console_arg_is("set", arg1, arg1Length))
    {
        if (arg2)
        {
            uint32_t hours, minutes;
            if (sscanf(arg2, "%d:%d", (unsigned int *)&hours, (unsigned int *)&minutes) == 2)
            {
                set_alarm(hours, minutes);
                printf("Alarm set to %2.2ld:%2.2ld\n", hours, minutes);
            }
            else
            {
                printf("Parse error. Enter date and time in the format\nHH:MM\n");
            }
        }
        else
        {
            printf("missing argument");
        }
    }
    else if (console_arg_is("enable", arg1, arg1Length))
    {
    	enable_alarm(true);
		display_alarm_enabled(true);
    	printf("Alarm enabled\n");
    }
    else if (console_arg_is("cancel", arg1, arg1Length))
    {
    	enable_alarm(false);
		display_alarm_enabled(false);
    	printf("Alarm cancelled\n");
    }
    else
    {
        printf("incorrect argument\n");
    }
}

/** */
static void put_history(char *input)
{
    strcpy(input_history[history_index], input);

    if (++history_index == HISTORY_DEPTH)
    {
        history_index = 0;
    }
}

/** */
static void get_history(char *input, bool reset)
{
    static int8_t index = 0;
    if (reset)
    {
        index = history_index -1;

        if (index < 0)
        {
            index = HISTORY_DEPTH;
        }
        return;
    }

    strcpy(input, input_history[index]);

    if (--index < 0)
    {
        index = HISTORY_DEPTH;
    }
}

/** */
static void console_history_command(const char *command_string)
{
    (void)command_string;

    for (uint8_t i = 0; i < HISTORY_DEPTH; i++)
    {
        printf("%d %s\n", i + 1, input_history[i]);
    }
}

///** */
//static void console_print_array(uint32_t addr, uint8_t data[], uint16_t len)
//{
//    uint8_t str[17];
//
//    for (int i = 0; i < len; i += 16)
//    {
//        uint16_t bytes_per_line = 16;
//
//        if ((len - i) < 16)
//        {
//            bytes_per_line = len - i;
//        }
//
//        for (int j = 0; j < bytes_per_line; j++)
//        {
//            uint8_t ch = data[i + j];
//            if (ch >= 0x20 && ch < 0x7f)
//            {
//                str[j] = ch;
//            }
//            else
//            {
//                str[j] = '.';
//            }
//        }
//
//        str[bytes_per_line] = '\0';
//
//        printf("%8.8lx  ", addr + i);
//        for (int j = 0; j < bytes_per_line; j++)
//        {
//            printf("%2.2x ", data[i + j]);
//        }
//
//        printf("|%s|\n", str);
//    }
//}

/** */
static bool console_arg_is(const char *name, const char *arg, uint8_t length)
{
    return arg != NULL && length == strlen(name) && strncmp(name, arg, length) == 0;
}

/** */
static const char* console_get_parameter( const char *command_string, uint8_t wanted_parameter, uint8_t *parameter_string_length)
{
	uint8_t parameters_found = 0;
	const char *return_parameter = NULL;

	*parameter_string_length = 0;

	while(parameters_found < wanted_parameter)
	{
		/* Index the character pointer past the current word.  If this is the start
		of the command string then the first word is the command itself. */
		while( ( ( *command_string ) != 0x00 ) && ( ( *command_string ) != ' ' ) )
		{
			command_string++;
		}

		/* Find the start of the next string. */
		while( ( ( *command_string ) != 0x00 ) && ( ( *command_string ) == ' ' ) )
		{
			command_string++;
		}

		/* Was a string found? */
		if( *command_string != 0x00 )
		{
			/* Is this the start of the required parameter? */
			parameters_found++;

			if( parameters_found == wanted_parameter )
			{
				/* How long is the parameter? */
				return_parameter = command_string;
				while( ( ( *command_string ) != 0x00 ) && ( ( *command_string ) != ' ' ) )
				{
					( *parameter_string_length )++;
					command_string++;
				}

				if( *parameter_string_length == 0 )
				{
					return_parameter = NULL;
				}

				break;
			}
		}
		else
		{
			break;
		}
	}

	return return_parameter;
}

/** */
static void process_command(const char * const input)
{
	for(int idx = 0; idx < sizeof(commands)/sizeof(command_t); idx++)
	{
		if(strncmp( commands[idx].command, input, strlen(commands[idx].command)) == 0)
		{
			commands[idx].command_interpreter(input);
			return;
		}
	}

	printf("unrecognised command %s\n", input);
}

/** */
void console_process_key(void)
{
    if(kbhit())
    {
    	int8_t ch = getchar();

        // look for up arrow key
        if ((ch == 0x1b) || (ch == 0x5b))
        {
           	ch = getchar();
        	HAL_Delay(1);
			ch = getchar();
        	HAL_Delay(1);

        	// clear the line to remove the uparrow
            printf(CLL);
            get_history(input,false);
            inputIndex = strlen(input);
            printf("\r$ %s", input);
            return;
        }
        else
		{
        	putchar(ch);
		}

        if (ch == '\r')
        {
            printf("\n");

            if (inputIndex == 0)
            {
                printf("$ ");
                return;
            }

			process_command(input);

            if(strcmp(input, "history")!=0)
            {
                put_history(input);
            }

            get_history(NULL, true);

            inputIndex = 0;
            memset(input, 0x00, sizeof(input));
            printf("\n$ ");
        }
        else
        {
            if (ch == '\n')
            {
                // ignore carriage returns
            }
            else if (ch == '\b')
            {
                // process backspace
                if (inputIndex > 0)
                {
                    inputIndex--;
                    input[inputIndex] = '\0';
                }

                printf(CLL "\r$ %s", input);
            }
            else if (ch >= ' ' && ch <= '~')
            {
                // save printable characters
                if (inputIndex < sizeof(input))
                {
                    input[inputIndex] = ch;
                    inputIndex++;
                }
            }
        }
    }
}
