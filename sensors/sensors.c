/*
 * Copyright 2024 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "sensors.h"

#define CELCIUS_TO_FAHRENHEIT(celcius) (((celcius)*1.8)+32)
#define TRESHOLD_TOUCH_PAD 11000

#define SCALE_ADJUST(value) (LCD_WIDTH*(value)/800)




volatile static p3t1755_handle_t p3t1755Handle;
volatile static i3c_master_handle_t g_i3c_m_handle;

volatile static uint8_t  g_control_interface_counter   = 0;
volatile static uint16_t g_rpm                        = 0;

// Temperature ------------------------------------------------------------
volatile static uint8_t g_temperature_c               = 0;
volatile static uint8_t g_ext_temperature             = 0;
volatile static bool    g_temperature_updated         = false;
// ------------------------------------------------------------------------

// Touch sensitive --------------------------------------------------------
volatile static uint16_t g_touch_sensitive            = 0;
volatile static uint8_t g_touch_sensitive_percentaje  = 0;
volatile static uint16_t g_valid_touch_sensitive_percentaje = 0;
volatile static uint16_t g_valid_touch_sensitive            = 0;
// ------------------------------------------------------------------------

// Performance ------------------------------------------------------------
volatile static uint32_t g_server_task_time           = 0;
volatile static uint32_t g_lgvl_task_time          	  = 0;
volatile static uint32_t g_other_task_time            = 0;
volatile static uint32_t g_idle_time          		  = 0;
volatile static uint8_t g_server_task_time_percent    = 0;
volatile static uint8_t g_lgvl_task_time_percent  	  = 0;
volatile static uint8_t g_other_task_time_percent  	  = 0;
volatile static uint8_t g_idle_time_percent   		  = 0;
volatile static uint8_t g_server_task_time_percent_dec;
volatile static uint8_t g_lgvl_task_time_percent_dec;
volatile static uint8_t g_other_task_time_percent_dec;
volatile static uint8_t g_idle_time_percent_dec;
// ------------------------------------------------------------------------

// ------------------------------------------------------------------------
volatile static uint8_t  day;
volatile static uint8_t  month;
volatile static uint16_t year;
volatile static uint8_t  hours;
volatile static uint8_t  minutes;
volatile static uint8_t  seconds;
// ------------------------------------------------------------------------

volatile static uint32_t g_previous_timer_value        = 0;
volatile static bool     g_performance_times_available = false;
volatile static bool     g_sensors_updated             = true;
volatile static bool     g_sensors_updated_server      = true;
volatile static sensor_updates_t g_sensors_state       = {true, true, true, true, true};
volatile static sensor_updates_t g_sensors_state_server= {true, true, true, true, true};

volatile static uint8_t	g_window = 3;


void ADC0_IRQHandler(void)
{
    lpadc_conv_result_t convResultStruct;
    uint16_t Vbe1            = 0U;
    uint16_t Vbe8            = 0U;
    uint32_t convResultShift = 3U;
    float parameterSlope     = FSL_FEATURE_LPADC_TEMP_PARAMETER_A;
    float parameterOffset    = FSL_FEATURE_LPADC_TEMP_PARAMETER_B;
    float parameterAlpha     = FSL_FEATURE_LPADC_TEMP_PARAMETER_ALPHA;
    float temperature        = -273.15f; /* Absolute zero degree as the incorrect return value. */

	/* Read the 2 temperature sensor result. */
	if (true == LPADC_GetConvResult(ADC0, &convResultStruct, 0))
	{
		if(1 == convResultStruct.commandIdSource)
		{
			Vbe1 = convResultStruct.convValue >> convResultShift;
			if (true == LPADC_GetConvResult(ADC0, &convResultStruct, 0))
			{
				Vbe8 = convResultStruct.convValue >> convResultShift;
				/* Final temperature = A*[alpha*(Vbe8-Vbe1)/(Vbe8 + alpha*(Vbe8-Vbe1))] - B. */
				temperature = parameterSlope * (parameterAlpha * ((float)Vbe8 - (float)Vbe1) /
												((float)Vbe8 + parameterAlpha * ((float)Vbe8 - (float)Vbe1))) -
							  parameterOffset;
			}
			g_temperature_c = (temperature + g_temperature_c)/2;
			g_temperature_updated = true;
		}
		else if(2 == convResultStruct.commandIdSource)
		{
			g_touch_sensitive = convResultStruct.convValue;
		}
	}
}
// ------------------------------------------------------------------------------------------

void LPTMR0_IRQHandler(void)
{
	g_performance_times_available = true;
	g_previous_timer_value = 0;
    LPTMR_ClearStatusFlags(LPTMR0, kLPTMR_TimerCompareFlag);
}
// ------------------------------------------------------------------------------------------


/********************************************************************************************
	@brief : This function convert integer to string
	@param : value   -> Integer value to convert
	@param : string  -> Buffer pointer to save string number
	@param : int_len -> String length, if is more than number need is filled with '0'
	@return: Return sign of number (Not put '-' in string)
*********************************************************************************************/
sign_of_string_t int_to_string(int32_t value, uint8_t* string, uint8_t int_len)
{
	uint8_t counter;
	uint8_t index;
	uint8_t pow_cnt;
	uint32_t divisor;
	uint32_t suma = 0;
	sign_of_string_t sign = positive;

	if(value < 0)
	{
		sign = negative;
		value *= -1;
	}
	for(index = 0, counter = int_len; index < int_len; index ++, counter --)
	{
		divisor = 1;
		for(pow_cnt = 1; pow_cnt < counter; pow_cnt ++)
		{
			divisor *= 10;
		}
		string[index] = (value-suma)/divisor;
		suma += string[index]*divisor;
		string[index] += INT_TO_CHAR;
	}
	return sign;
}
// ------------------------------------------------------------------------------------------

/********************************************************************************************
	@brief : This function convert string to integer.
	@param : buffer -> String to convert, support negative number "-00050" or "-50" or "50"
	@param : size   -> Size of string
	@return: Integer number
*********************************************************************************************/
int32_t string_to_int(uint8_t buffer[], uint8_t size) // Ends when character is not a number or reach size
{
	uint8_t index;
	int32_t retval = 0;
	sign_of_string_t sign = positive;
	if(NULL != buffer)
	{
		for(index = 0; index < size && index < 100; index ++)
		{
			if('-' == buffer[index])
			{
				sign = negative;
			}
			else if('0' <= buffer[index] && '9' >= buffer[index])
			{
				retval *= 10;
				retval += (int8_t)buffer[index] + CHAR_TO_INT;
			}
			else
			{
				break;
			}
		}
		if(negative == sign)
		{
			retval *= -1;
		}
	}
	return retval;
}
// ------------------------------------------------------------------------------------------

/********************************************************************************************
	@brief : This function is for lvgl, if some value is updated this returns true
	@return: True if some value is updated, false if not
*********************************************************************************************/
bool sensor_updates_screen()
{
	bool sensors_updated = g_sensors_updated;
	g_sensors_updated = false;
	return sensors_updated;
}
// ------------------------------------------------------------------------------------------

/********************************************************************************************
	@brief : This function is for server, if some value is updated this returns true
	@return: True if some value is updated, false if not
*********************************************************************************************/
bool sensor_updates_server()
{
	bool sensors_updated = g_sensors_updated_server;
	g_sensors_updated_server = false;
	return sensors_updated;
}
// ------------------------------------------------------------------------------------------

/********************************************************************************************
	@brief : This indicate values that are updated for lvgl (used for performance)
	@return: struct with flags that indicate updated values
*********************************************************************************************/
sensor_updates_t get_values_state_screen()
{
	sensor_updates_t sensor_updates = g_sensors_state;
	g_sensors_state.update_ext_temperature = false;
	g_sensors_state.update_mcu_temperature = false;
	g_sensors_state.update_performance     = false;
	g_sensors_state.update_rtc             = false;
	g_sensors_state.update_touch           = false;
	return sensor_updates;
}
// ------------------------------------------------------------------------------------------

/********************************************************************************************
	@brief : This indicate values that are updated for server (used for performance)
	@return: struct with flags that indicate updated values
*********************************************************************************************/
sensor_updates_t get_values_state_server()
{
	sensor_updates_t sensor_updates = g_sensors_state_server;
	g_sensors_state_server.update_ext_temperature = false;
	g_sensors_state_server.update_mcu_temperature = false;
	g_sensors_state_server.update_performance     = false;
	g_sensors_state_server.update_rtc             = false;
	g_sensors_state_server.update_touch           = false;
	return sensor_updates;
}
// ------------------------------------------------------------------------------------------

/********************************************************************************************
	@brief : Here get values of CPU performance
	@return: struct that have all CPU performance data, integer and tens are separated
*********************************************************************************************/
performance_data_t get_performance_data()
{
	performance_data_t performance_data;
	performance_data.server_task_int = g_server_task_time_percent;
	performance_data.server_task_tens = g_server_task_time_percent_dec;
	performance_data.lvgl_task_int = g_lgvl_task_time_percent;
	performance_data.lvgl_task_tens = g_lgvl_task_time_percent_dec;
	performance_data.idle_task_int = g_idle_time_percent;
	performance_data.idle_task_tens = g_idle_time_percent_dec;
	performance_data.other_task_int = g_other_task_time_percent;
	performance_data.other_task_tens = g_other_task_time_percent_dec;
	return performance_data;
}
// ------------------------------------------------------------------------------------------

/********************************************************************************************
	@brief : Here get values of touch sensitive sensor
	@return: struct that have touch sensitive data
*********************************************************************************************/
touch_sensitive_data_t get_touch_sensitive_data()
{
	touch_sensitive_data_t touch_sensitive_data;
	touch_sensitive_data.touch_sensitive_data = g_valid_touch_sensitive;
	touch_sensitive_data.touch_sensitive_percentaje = g_valid_touch_sensitive_percentaje;
	return touch_sensitive_data;
}
// ------------------------------------------------------------------------------------------

/********************************************************************************************
	@brief : Here get values of real time clock (RTC)
	@return: struct with all RTC values
*********************************************************************************************/
rtc_data_t get_rtc_data()
{
	rtc_data_t rtc_data;
	rtc_data.day     = day;
	rtc_data.month   = month;
	rtc_data.year    = year;
	rtc_data.hours   = hours;
	rtc_data.minutes = minutes;
	rtc_data.seconds = seconds;
	return rtc_data;
}
// ------------------------------------------------------------------------------------------

/********************************************************************************************
	@brief : Here get temperature values (board and CPU)
	@return: struct with both temperature values
*********************************************************************************************/
temperature_data_t get_temperature_data()
{
	temperature_data_t temperature_data;
	temperature_data.external_temperature = g_ext_temperature;
	temperature_data.internal_temperature = g_temperature_c;
	return temperature_data;
}
// ------------------------------------------------------------------------------------------

/********************************************************************************************
	@brief : Here get values of control interface panel (rpm and counter)
	@return: struct with rpm and counter
*********************************************************************************************/
control_interface_values_t get_control_interface_data()
{
	control_interface_values_t control_interface_data;
	control_interface_data.rpm = g_rpm;
	control_interface_data.counter = g_control_interface_counter;
	return control_interface_data;
}
// ------------------------------------------------------------------------------------------


/********************************************************************************************
	@brief : Increment control interface counter
	@return: None
*********************************************************************************************/
void increment_counter()
{
	if(100 > g_control_interface_counter)
	{
		g_control_interface_counter ++;
	}
	else
	{
		g_control_interface_counter = 0;
	}
}
// ------------------------------------------------------------------------------------------

/********************************************************************************************
	@brief : Decrement control interface counter
	@return: None
*********************************************************************************************/
void decrement_counter()
{
	if(0 < g_control_interface_counter)
	{
		g_control_interface_counter --;
	}
	else
	{
		g_control_interface_counter = 100;
	}
}
// ------------------------------------------------------------------------------------------

/********************************************************************************************
	@brief : Is used for calculate performance (Used in FreeRTOSConfig.h)
	@return: None
*********************************************************************************************/
void server_task_in()
{
	uint32_t timer_value =  LPTMR_GetCurrentTimerCount(LPTMR0);
	g_previous_timer_value = timer_value;
}
// ------------------------------------------------------------------------------------------

/********************************************************************************************
	@brief : Is used for calculate performance (Used in FreeRTOSConfig.h)
	@return: None
*********************************************************************************************/
void lgvl_task_in()
{
	uint32_t timer_value =  LPTMR_GetCurrentTimerCount(LPTMR0);
	g_previous_timer_value = timer_value;
}
// ------------------------------------------------------------------------------------------

/********************************************************************************************
	@brief : Is used for calculate performance (Used in FreeRTOSConfig.h)
	@return: None
*********************************************************************************************/
void sensor_task_in()
{
	uint32_t timer_value =  LPTMR_GetCurrentTimerCount(LPTMR0);
	g_previous_timer_value = timer_value;
}
// ------------------------------------------------------------------------------------------

/********************************************************************************************
	@brief : Is used for calculate performance (Used in FreeRTOSConfig.h)
	@return: None
*********************************************************************************************/
void idle_task_in()
{
	uint32_t timer_value =  LPTMR_GetCurrentTimerCount(LPTMR0);
	g_previous_timer_value = timer_value;
}
// ------------------------------------------------------------------------------------------

/********************************************************************************************
	@brief : Is used for calculate performance (Used in FreeRTOSConfig.h)
	@return: None
*********************************************************************************************/
void server_task_out()
{
	uint32_t timer_value = LPTMR_GetCurrentTimerCount(LPTMR0);
	if(false == g_performance_times_available)
	{
		g_server_task_time += timer_value - g_previous_timer_value;
	}
	g_previous_timer_value = timer_value;
}
// ------------------------------------------------------------------------------------------

/********************************************************************************************
	@brief : Is used for calculate performance (Used in FreeRTOSConfig.h)
	@return: None
*********************************************************************************************/
void lgvl_task_out()
{
	uint32_t timer_value = LPTMR_GetCurrentTimerCount(LPTMR0);
	if(false == g_performance_times_available)
	{
		g_lgvl_task_time += timer_value - g_previous_timer_value;
	}
	g_previous_timer_value = timer_value;
}
// ------------------------------------------------------------------------------------------

/********************************************************************************************
	@brief : Is used for calculate performance (Used in FreeRTOSConfig.h)
	@return: None
*********************************************************************************************/
void sensor_task_out()
{
	uint32_t timer_value = LPTMR_GetCurrentTimerCount(LPTMR0);
	if(false == g_performance_times_available)
	{
		g_other_task_time += timer_value - g_previous_timer_value;
	}
	g_previous_timer_value = timer_value;
}
// ------------------------------------------------------------------------------------------

/********************************************************************************************
	@brief : Is used for calculate performance (Used in FreeRTOSConfig.h)
	@return: None
*********************************************************************************************/
void idle_task_out()
{
	uint32_t timer_value = LPTMR_GetCurrentTimerCount(LPTMR0);
	if(false == g_performance_times_available)
	{
		g_idle_time += timer_value - g_previous_timer_value;
	}
	g_previous_timer_value = timer_value;
}
// ------------------------------------------------------------------------------------------

bool update_time()
{
	irtc_datetime_t datetime;
	bool            update = false;


	// Set time ---------------------------------
	IRTC_GetDatetime(RTC, &datetime);
	if(seconds != datetime.second)
	{
		update = true;
		day   = datetime.day;
		month = datetime.month;
		year  = datetime.year;
		hours = datetime.hour;
		minutes = datetime.minute;
		seconds = datetime.second;
	}

	if(true == update)
	{
		g_sensors_state.update_rtc = true;
		g_sensors_state_server.update_rtc = true;
	}
	return update;
	// ------------------------------------------
}
// ------------------------------------------------------------------------------------------

bool update_performance()
{
	bool     update     = false;
	uint32_t total_time = 0;
	float server_task_time_percent;
	float lgvl_task_time_percent;
	float other_task_time_percent;
	float idle_time_percent;

	if(true == g_performance_times_available)
	{
		// Set Performance --------------------------
		update      = true;
		total_time  = g_idle_time + g_server_task_time + g_lgvl_task_time + g_other_task_time;

		server_task_time_percent = g_server_task_time*100.0/total_time;
		lgvl_task_time_percent   = g_lgvl_task_time*100.0/total_time;
		other_task_time_percent  = g_other_task_time*100.0/total_time;
		idle_time_percent 		 = g_idle_time*100.0/total_time;


		g_server_task_time_percent = (uint8_t)server_task_time_percent;
		g_lgvl_task_time_percent   = (uint8_t)lgvl_task_time_percent;
		g_other_task_time_percent  = (uint8_t)other_task_time_percent;
		g_idle_time_percent 	   = (uint8_t)idle_time_percent;

		g_server_task_time_percent_dec = (uint8_t)((server_task_time_percent-(float)g_server_task_time_percent)*10.0);
		g_lgvl_task_time_percent_dec   = (uint8_t)((lgvl_task_time_percent-(float)g_lgvl_task_time_percent)*10.0);
		g_other_task_time_percent_dec  = (uint8_t)((other_task_time_percent-(float)g_other_task_time_percent)*10.0);
		g_idle_time_percent_dec 	   = (uint8_t)((idle_time_percent-(float)g_idle_time_percent)*10.0);
		// ------------------------------------------
		g_idle_time = 0;
		g_server_task_time = 0;
		g_lgvl_task_time = 0;
		g_other_task_time = 0;
		g_performance_times_available = false;
		g_sensors_state.update_performance = true;
		g_sensors_state_server.update_performance = true;
	}
	return update;
}
// ------------------------------------------------------------------------------------------

bool update_touch_pad()
{
	static uint16_t touch_sensitive                  = 0;
	static uint16_t previous_touch_sensitive         = 0;
	bool            update                           = false;

	if(touch_sensitive != g_touch_sensitive)
	{
		touch_sensitive = g_touch_sensitive;
		// Set touch --------------------------------
		if(g_touch_sensitive > previous_touch_sensitive && g_touch_sensitive > TRESHOLD_TOUCH_PAD)
		{
			previous_touch_sensitive = g_touch_sensitive;
			previous_touch_sensitive = (previous_touch_sensitive < 40000)? previous_touch_sensitive:40000;
		}
		else
		{
			previous_touch_sensitive -=  (previous_touch_sensitive/10);
		}
		// ------------------------------------------
		g_valid_touch_sensitive = previous_touch_sensitive;
		g_valid_touch_sensitive_percentaje = previous_touch_sensitive*100/40000;
		g_sensors_state.update_touch = true;
		g_sensors_state_server.update_touch = true;
		update = true;
	}
	return update;
}
// ------------------------------------------------------------------------------------------

bool update_temperature()
{
	static uint8_t temperature_c = 0;
	uint8_t temperature = (uint8_t)P3T1755_get_temperature();
	bool    update      = false;
	if((g_ext_temperature != temperature) || (temperature_c != g_temperature_c))
	{
		g_sensors_state.update_ext_temperature = (g_ext_temperature != temperature);
		g_sensors_state.update_mcu_temperature = (temperature_c != g_temperature_c);
		g_sensors_state_server.update_ext_temperature = (g_ext_temperature != temperature);
		g_sensors_state_server.update_mcu_temperature = (temperature_c != g_temperature_c);
		g_ext_temperature = temperature;
		temperature_c = g_temperature_c;
		update = true;
	}
	return update;
}
// ------------------------------------------------------------------------------------------

/********************************************************************************************
	@brief : Periodic function used for update all values, called for "other" task in main
	@return: None
*********************************************************************************************/
void update_values()
{
	bool updated = false;
	if(false == g_sensors_updated)
	{
		updated |= update_time();
		updated |= update_performance();
		updated |= update_touch_pad();
		if(true == g_temperature_updated)
		{
			updated |= update_temperature();
			g_temperature_updated = false;
			LPADC_DoSoftwareTrigger(ADC0, 3U);
		}
		g_sensors_updated |= updated;
	}
}
// ------------------------------------------------------------------------------------------


/********************************************************************************************
	@brief : This function allows to get desired rpm
	@return: Actual desired rpm
*********************************************************************************************/
uint16_t get_rpm()
{
	return g_rpm;
}
// ------------------------------------------------------------------------------------------

/********************************************************************************************
	@brief : This function allows to set rpm between (0, 4000), if is less than 500 is set to 0
	@param : rpm -> Desired rpm
	@return: None
*********************************************************************************************/
void set_rpm(uint16_t rpm)
{
	if(4000 >= rpm)
	{
		g_rpm = rpm;
	}
}
// ------------------------------------------------------------------------------------------

void set_actual_window(uint8_t window)
{
	g_window = window;
}

uint8_t get_actual_window()
{
	return g_window;
}
