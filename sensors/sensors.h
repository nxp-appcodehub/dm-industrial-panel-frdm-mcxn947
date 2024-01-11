/*
 * Copyright 2024 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef SENSORS_H_
#define SENSORS_H_

#include <stdint.h>
#include <stdbool.h>
#include "fsl_lpadc.h"
#include "fsl_irtc.h"
#include "fsl_lptmr.h"
#include <fsl_p3t1755.h>

#define INT_TO_CHAR 48
#define CHAR_TO_INT -48

typedef enum _sign_of_string_t
{
	positive,
	negative
} sign_of_string_t;

typedef struct _performance_data_t
{
	uint8_t server_task_int;
	uint8_t lvgl_task_int;
	uint8_t other_task_int;
	uint8_t idle_task_int;
	uint8_t server_task_tens;
	uint8_t lvgl_task_tens;
	uint8_t other_task_tens;
	uint8_t idle_task_tens;
} performance_data_t;

typedef struct _touch_sensitive_data_t
{
	uint16_t touch_sensitive_data;
	uint8_t  touch_sensitive_percentaje;
} touch_sensitive_data_t;

typedef struct _rtc_data_t
{
	uint8_t  day;
	uint8_t  month;
	uint16_t year;
	uint8_t  hours;
	uint8_t  minutes;
	uint8_t  seconds;
} rtc_data_t;

typedef struct _temperature_data_t
{
	uint8_t internal_temperature;
	uint8_t external_temperature;
} temperature_data_t;

typedef struct _control_interface_values_t
{
	uint16_t rpm;
	uint16_t counter;
} control_interface_values_t;

typedef struct _sensor_updates_t
{
	bool update_ext_temperature;
	bool update_mcu_temperature;
	bool update_performance;
	bool update_rtc;
	bool update_touch;
} sensor_updates_t;

/********************************************************************************************
	@brief : This function convert integer to string
	@param : value   -> Integer value to convert
	@param : string  -> Buffer pointer to save string number
	@param : int_len -> String length, if is more than number need is filled with '0'
	@return: Return sign of number (Not put '-' in string)
*********************************************************************************************/
sign_of_string_t           int_to_string(int32_t value, uint8_t* string, uint8_t int_len);
// ------------------------------------------------------------------------------------------

/********************************************************************************************
	@brief : This function convert string to integer.
	@param : buffer -> String to convert, support negative number "-00050" or "-50" or "50"
	@param : size   -> Size of string
	@return: Integer number
*********************************************************************************************/
int32_t                    string_to_int(uint8_t buffer[], uint8_t size);
// ------------------------------------------------------------------------------------------

/********************************************************************************************
	@brief : This function is for lvgl, if some value is updated this returns true
	@return: True if some value is updated, false if not
*********************************************************************************************/
bool 					   sensor_updates_screen();
// ------------------------------------------------------------------------------------------

/********************************************************************************************
	@brief : This function is for server, if some value is updated this returns true
	@return: True if some value is updated, false if not
*********************************************************************************************/
bool 					   sensor_updates_server();
// ------------------------------------------------------------------------------------------

/********************************************************************************************
	@brief : This indicate values that are updated for lvgl (used for performance)
	@return: struct with flags that indicate updated values
*********************************************************************************************/
sensor_updates_t 		   get_values_state_screen();
// ------------------------------------------------------------------------------------------

/********************************************************************************************
	@brief : This indicate values that are updated for server (used for performance)
	@return: struct with flags that indicate updated values
*********************************************************************************************/
sensor_updates_t 		   get_values_state_server();
// ------------------------------------------------------------------------------------------

/********************************************************************************************
	@brief : Here get values of CPU performance
	@return: struct that have all CPU performance data, integer and tens are separated
*********************************************************************************************/
performance_data_t         get_performance_data();
// ------------------------------------------------------------------------------------------

/********************************************************************************************
	@brief : Here get values of touch sensitive sensor
	@return: struct that have touch sensitive data
*********************************************************************************************/
touch_sensitive_data_t     get_touch_sensitive_data();
// ------------------------------------------------------------------------------------------

/********************************************************************************************
	@brief : Here get values of real time clock (RTC)
	@return: struct with all RTC values
*********************************************************************************************/
rtc_data_t                 get_rtc_data();
// ------------------------------------------------------------------------------------------

/********************************************************************************************
	@brief : Here get temperature values (board and CPU)
	@return: struct with both temperature values
*********************************************************************************************/
temperature_data_t         get_temperature_data();
// ------------------------------------------------------------------------------------------

/********************************************************************************************
	@brief : Here get values of control interface panel (rpm and counter)
	@return: struct with rpm and counter
*********************************************************************************************/
control_interface_values_t get_control_interface_data();
// ------------------------------------------------------------------------------------------

/********************************************************************************************
	@brief : This function allows to set rpm between (0, 4000), if is less than 500 is set to 0
	@param : rpm -> Desired rpm
	@return: None
*********************************************************************************************/
void 					   set_rpm(uint16_t rpm);
// ------------------------------------------------------------------------------------------

/********************************************************************************************
	@brief : This function allows to get desired rpm
	@return: Actual desired rpm
*********************************************************************************************/
uint16_t                   get_rpm();
// ------------------------------------------------------------------------------------------

/********************************************************************************************
	@brief : Increment control interface counter
	@return: None
*********************************************************************************************/
void increment_counter();
// ------------------------------------------------------------------------------------------

/********************************************************************************************
	@brief : Decrement control interface counter
	@return: None
*********************************************************************************************/
void decrement_counter();
// ------------------------------------------------------------------------------------------

/********************************************************************************************
	@brief : Is used for calculate performance (Used in FreeRTOSConfig.h)
	@return: None
*********************************************************************************************/
void server_task_in();
// ------------------------------------------------------------------------------------------

/********************************************************************************************
	@brief : Is used for calculate performance (Used in FreeRTOSConfig.h)
	@return: None
*********************************************************************************************/
void lgvl_task_in();
// ------------------------------------------------------------------------------------------

/********************************************************************************************
	@brief : Is used for calculate performance (Used in FreeRTOSConfig.h)
	@return: None
*********************************************************************************************/
void sensor_task_in();
// ------------------------------------------------------------------------------------------

/********************************************************************************************
	@brief : Is used for calculate performance (Used in FreeRTOSConfig.h)
	@return: None
*********************************************************************************************/
void idle_task_in();
// ------------------------------------------------------------------------------------------

/********************************************************************************************
	@brief : Is used for calculate performance (Used in FreeRTOSConfig.h)
	@return: None
*********************************************************************************************/
void server_task_out();
// ------------------------------------------------------------------------------------------

/********************************************************************************************
	@brief : Is used for calculate performance (Used in FreeRTOSConfig.h)
	@return: None
*********************************************************************************************/
void lgvl_task_out();
// ------------------------------------------------------------------------------------------

/********************************************************************************************
	@brief : Is used for calculate performance (Used in FreeRTOSConfig.h)
	@return: None
*********************************************************************************************/
void sensor_task_out();
// ------------------------------------------------------------------------------------------

/********************************************************************************************
	@brief : Is used for calculate performance (Used in FreeRTOSConfig.h)
	@return: None
*********************************************************************************************/
void idle_task_out();
// ------------------------------------------------------------------------------------------

/********************************************************************************************
	@brief : Periodic function used for update all values, called for "other" task in main
	@return: None
*********************************************************************************************/
void update_values();
// ------------------------------------------------------------------------------------------

#endif /* SENSORS_H_ */
