/*
 * Copyright 2024 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "web_uart_interface.h"
#include "mikroe_in_mcxn9.h"
#include "sensors.h"

#define BUFF_SIZE 10

#define INCREMENT 1
#define DECREMENT 2

extern lv_ui guider_ui;

static bool g_counter_ready = true;

typedef struct _send_data_t
{
	uint8_t secure_char_1;
	uint8_t server_task_int;
	uint8_t lvgl_task_int;
	uint8_t other_task_int;
	uint8_t idle_task_int;
	uint8_t server_task_tens;
	uint8_t lvgl_task_tens;
	uint8_t other_task_tens;
	uint8_t idle_task_tens;
	uint8_t touch_sensitive_value_L;
	uint8_t touch_sensitive_value_H;
	uint8_t touch_sensitive_percentaje;
	uint8_t day;
	uint8_t month;
	uint8_t year_L;
	uint8_t year_H;
	uint8_t hour;
	uint8_t minutes;
	uint8_t seconds;
	uint8_t internal_temperature;
	uint8_t external_temperature;
	uint8_t rpm_L;
	uint8_t rpm_H;
	uint8_t counter;
	uint8_t counter_ready;
	uint8_t window;
	uint8_t secure_char_2;
} send_data_t;

typedef struct _receive_data_t
{
	uint8_t secure_char_1;
	uint8_t increment_decrement;
	uint16_t rpm;
	uint8_t window;
	uint8_t window_update;
	uint8_t rpm_update;
	uint8_t secure_char_2;
} receive_data_t;

typedef enum _receive_state_machine_t
{
	k_Idle_State,
	k_Reading_State,
	k_Finish_State,
	k_Error_State
} receive_sate_machine_t;

void web_uart_interface_init()
{
	mikroe_uart_config_t uart_config =
	{
			false,
			115200u,
			k_mikroe_uart_pariry_disable,
			k_mikroe_uart_one_stop_bit
	};
	init_mikro_e_uart(&uart_config);
}

void web_uart_interface_send()
{
	static send_data_t send_data;
	performance_data_t 			performance_data 	= get_performance_data();
	touch_sensitive_data_t  	touch_data  		= get_touch_sensitive_data();
	rtc_data_t              	rtc_data			= get_rtc_data();
	temperature_data_t      	temp_data			= get_temperature_data();
	control_interface_values_t 	interface_data		= get_control_interface_data();

	send_data.secure_char_1		= '*';
	send_data.secure_char_2		= '#';
	// Performance -----------------------------------------------------------------------
	send_data.server_task_int 	= performance_data.server_task_int;
	send_data.lvgl_task_int 	= performance_data.lvgl_task_int;
	send_data.other_task_int 	= performance_data.other_task_int;
	send_data.idle_task_int 	= performance_data.idle_task_int;
	send_data.server_task_tens 	= performance_data.server_task_tens;
	send_data.lvgl_task_tens 	= performance_data.lvgl_task_tens;
	send_data.other_task_tens 	= performance_data.other_task_tens;
	send_data.idle_task_tens 	= performance_data.idle_task_tens;
	// -----------------------------------------------------------------------------------

	// Touch -----------------------------------------------------------------------------
	send_data.touch_sensitive_value_L	= (uint8_t)(touch_data.touch_sensitive_data & 0x00FF);
	send_data.touch_sensitive_value_H	= (uint8_t)((touch_data.touch_sensitive_data & 0xFF00)>>8);
	send_data.touch_sensitive_percentaje 	= touch_data.touch_sensitive_percentaje;
	// -----------------------------------------------------------------------------------

	// RTC -------------------------------------------------------------------------------
	send_data.day 		= rtc_data.day;
	send_data.month 	= rtc_data.month;
	send_data.year_L	= (uint8_t)(rtc_data.year & 0x00FF);
	send_data.year_H	= (uint8_t)((rtc_data.year & 0xFF00)>>8);
	send_data.hour		= rtc_data.hours;
	send_data.minutes	= rtc_data.minutes;
	send_data.seconds	= rtc_data.seconds;
	// -----------------------------------------------------------------------------------

	// Temperature -----------------------------------------------------------------------
	send_data.internal_temperature = temp_data.internal_temperature;
	send_data.external_temperature = temp_data.external_temperature;
	// -----------------------------------------------------------------------------------

	// Control Interface -----------------------------------------------------------------
	send_data.rpm_L			= (uint8_t)(interface_data.rpm & 0x00FF);
	send_data.rpm_H			= (uint8_t)((interface_data.rpm & 0xFF00)>>8);
	send_data.counter 		= interface_data.counter;
	send_data.counter_ready = g_counter_ready;
	// -----------------------------------------------------------------------------------

	send_data.window = get_actual_window();
	mikroe_uart_send(false, (uint8_t*)&send_data, sizeof(send_data_t));
}

bool web_uart_interface_receive()
{
	static uint16_t previus_rpm 	= 0;
	static uint8_t	previus_window 	= 0;
	static uint8_t 	counter			= 0;
	static uint8_t  previus_increment_decrement = 0;
	static receive_sate_machine_t  state = k_Idle_State;
	static uint8_t	buff[BUFF_SIZE];
	static uint8_t	ready_buff[BUFF_SIZE];

	bool retval = false;
	receive_data_t *receive_data = (receive_data_t*)ready_buff;
	int8_t index;
	int32_t size_received;

	mikroe_uart_recive(false, buff, BUFF_SIZE, &size_received);

	for(index = 0; index < size_received; index ++)
	{
		switch(state)
		{
			case k_Idle_State:
				if(buff[index] == '*')
				{
					state = k_Reading_State;
					ready_buff[0] = '*';
					counter ++;
				}
				else
				{
					counter = 0;
				}
			break;

			case k_Reading_State:
				ready_buff[counter] = buff[index];
				if(7 <= counter)
				{
					if(ready_buff[7] == '#')
					{
						state = k_Finish_State;
					}
					else
					{
						state = k_Idle_State;
					}
					counter = 0;
				}
				else
				{
					counter ++;
				}
			break;
		}
	}

	if(k_Finish_State == state)
	{
		state = k_Idle_State;
		retval = true;
		if(previus_increment_decrement != receive_data->increment_decrement)
		{
			previus_increment_decrement = receive_data->increment_decrement;
			if(INCREMENT == receive_data->increment_decrement)
			{
				increment_counter();
				g_counter_ready = false;
			}
			else if(DECREMENT == receive_data->increment_decrement)
			{
				decrement_counter();
				g_counter_ready = false;
			}
			else
			{
				g_counter_ready = true;
			}
		}

		if(receive_data->rpm_update == 1 && previus_rpm != receive_data->rpm)
		{
			previus_rpm = receive_data->rpm;
			set_rpm(receive_data->rpm);
		}

		if(receive_data->window_update == 1 && previus_window !=  receive_data->window)
		{
			previus_window = receive_data->window;
			set_actual_window(previus_window);
		}
	}
	return retval;
}
