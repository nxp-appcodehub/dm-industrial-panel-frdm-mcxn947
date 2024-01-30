/*
 * Copyright 2023-2024 NXP
 * SPDX-License-Identifier: MIT
 * The auto-generated can only be used on NXP devices
 */

#include "lvgl.h"
#include <stdio.h>
#include "gui_guider.h"
#include "lcd_drv.h"
#include "events_init.h"
#include "widgets_init.h"
#include "custom.h"
#include "motor_control.h"
#include "sensors.h"


volatile static uint8_t g_temperature_c               = 0;
volatile static uint8_t g_ext_temperature             = 0;
volatile static uint8_t g_lablel_celcius[]            = "00 °C";
volatile static uint8_t g_label_ext_temperature[]     = "00 °C";

volatile static uint8_t g_lablel_counter_ci[]         = "000";
volatile static uint8_t g_lablel_bar_indicator[]      = "0000 rpm";

volatile static uint16_t g_touch_sensitive            = 0;
volatile static uint8_t  g_touch_sensitive_percentaje  = 0;
volatile static uint16_t g_valid_touch_sensitive_percentaje = 0;
volatile static uint16_t g_valid_touch_sensitive            = 0;
volatile static uint8_t  g_label_touch[]              = "00000";
volatile static uint8_t  g_label_touch_percentaje[]   = "000%";

volatile static uint8_t g_server_task_time_percent    = 0;
volatile static uint8_t g_lgvl_task_time_percent  	  = 0;
volatile static uint8_t g_other_task_time_percent  	  = 0;
volatile static uint8_t g_idle_time_percent   		  = 0;
volatile static uint8_t g_server_task_time_percent_dec;
volatile static uint8_t g_lgvl_task_time_percent_dec;
volatile static uint8_t g_other_task_time_percent_dec;
volatile static uint8_t g_idle_time_percent_dec;
volatile static uint8_t g_label_server_task_percent[] = "00.0%";
volatile static uint8_t g_label_lgvl_task_percent[]   = "00.0%";
volatile static uint8_t g_label_other_task_percent[]  = "00.0%";
volatile static uint8_t g_label_idle_percent[]   	  = "00.0%";


volatile static uint8_t  day;
volatile static uint8_t  month;
volatile static uint16_t year;
volatile static uint8_t  hours;
volatile static uint8_t  minutes;
volatile static uint8_t  seconds;
volatile static uint8_t g_label_date[]                = "Sep 07, 2023";
volatile static uint8_t g_label_time[]                = "12:00:00 am";


volatile static sensor_updates_t g_sensors_state       = {0};

const uint8_t g_months[12][4] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};


void update_screen_values(lv_ui *ui)
{
	performance_data_t         performance_data;
	touch_sensitive_data_t     touch_sensitive_data;
	rtc_data_t                 rtc_data;
	temperature_data_t         temperature_data;
	control_interface_values_t control_interface_values;

	static uint8_t  slider_value              = 0;
	static uint16_t rpm                       = 0;
	uint16_t degrees_other                    = 0;
	uint16_t degrees_idle                     = 0;
	uint16_t degrees_lgvl                     = 0;
	uint16_t degrees_server                   = 0;
	if(true == sensor_updates_screen())
	{
		g_sensors_state = get_values_state_screen();
		// Set temperature --------------------------
		// Celcius --------
		if(true == g_sensors_state.update_mcu_temperature)
		{
			temperature_data = get_temperature_data();
			g_temperature_c = temperature_data.internal_temperature;
			int_to_string(g_temperature_c, (uint8_t*)g_lablel_celcius, 2);
			lv_meter_set_indicator_value(ui->screen_TempCelsius, ui->screen_TempCelsius_scale_1_ndline_0, g_temperature_c);
			lv_label_set_text(ui->screen_DCelsius, (uint8_t*)g_lablel_celcius);
			g_sensors_state.update_mcu_temperature = false;
		}
		// ----------------
		// Fahrenheit -----
		if(true == g_sensors_state.update_ext_temperature)
		{
			temperature_data = get_temperature_data();
			g_ext_temperature = temperature_data.external_temperature;
			int_to_string(g_ext_temperature, (uint8_t*)g_label_ext_temperature, 2);
			lv_meter_set_indicator_value(ui->screen_meter_1, ui->screen_meter_1_scale_1_ndline_0, g_ext_temperature);
			lv_label_set_text(ui->screen_DFahrenheit, (uint8_t*)g_label_ext_temperature);
			g_sensors_state.update_ext_temperature = false;
		}
		// ----------------
		// ------------------------------------------

		// Set time ---------------------------------
		if(true == g_sensors_state.update_rtc)
		{
			rtc_data = get_rtc_data();
			hours    = rtc_data.hours;
			minutes  = rtc_data.minutes;
			seconds  = rtc_data.seconds;
			day      = rtc_data.day;
			month    = rtc_data.month;
			year     = rtc_data.year;
			if(12 <= hours)
			{
				g_label_time[9] = 'p';
			}
			else
			{
				g_label_time[9] = 'a';
			}
			if(12 < hours)
			{
				hours-12;
			}
			g_label_date[0] = g_months[month-1][0];
			g_label_date[1] = g_months[month-1][1];
			g_label_date[2] = g_months[month-1][2];
			int_to_string(day, (uint8_t*)(g_label_date+4), 2);
			int_to_string(year, (uint8_t*)(g_label_date+8), 4);
			int_to_string(hours, (uint8_t*)(g_label_time), 2);
			int_to_string(minutes, (uint8_t*)(g_label_time+3), 2);
			int_to_string(seconds, (uint8_t*)(g_label_time+6), 2);
			lv_label_set_text(ui->screen_Date, (uint8_t*)g_label_date);
			lv_label_set_text(ui->screen_Clock, (uint8_t*)g_label_time);
			g_sensors_state.update_rtc = false;
		}
		// ------------------------------------------

		// Set Performance --------------------------
		if(true == g_sensors_state.update_performance)
		{
			performance_data = get_performance_data();
			g_server_task_time_percent     = performance_data.server_task_int;
			g_server_task_time_percent_dec = performance_data.server_task_tens;
			g_lgvl_task_time_percent       = performance_data.lvgl_task_int;
			g_lgvl_task_time_percent_dec   = performance_data.lvgl_task_tens;
			g_idle_time_percent            = performance_data.idle_task_int;
			g_idle_time_percent_dec        = performance_data.idle_task_tens;
			g_other_task_time_percent      = performance_data.other_task_int;
			g_other_task_time_percent_dec  = performance_data.other_task_tens;

			int_to_string(g_server_task_time_percent, (uint8_t*)(g_label_server_task_percent), 2);
			int_to_string(g_lgvl_task_time_percent, (uint8_t*)(g_label_lgvl_task_percent), 2);
			int_to_string(g_other_task_time_percent, (uint8_t*)(g_label_other_task_percent), 2);
			int_to_string(g_idle_time_percent, (uint8_t*)(g_label_idle_percent), 2);

			int_to_string(g_server_task_time_percent_dec, (uint8_t*)(g_label_server_task_percent+3), 1);
			int_to_string(g_lgvl_task_time_percent_dec, (uint8_t*)(g_label_lgvl_task_percent+3), 1);
			int_to_string(g_other_task_time_percent_dec, (uint8_t*)(g_label_other_task_percent+3), 1);
			int_to_string(g_idle_time_percent_dec, (uint8_t*)(g_label_idle_percent+3), 1);

			lv_label_set_text(ui->screen_LVGLPercentage, (uint8_t*)g_label_lgvl_task_percent);
			lv_label_set_text(ui->screen_ServerPercentage, (uint8_t*)g_label_server_task_percent);
			lv_label_set_text(ui->screen_OtherPercentage, (uint8_t*)g_label_other_task_percent);
			lv_label_set_text(ui->screen_IdlePercentage, (uint8_t*)g_label_idle_percent);

			degrees_other  = 360*g_other_task_time_percent/100;
			degrees_idle   = (360*g_idle_time_percent/100) + degrees_other;
			degrees_lgvl   = (360*g_lgvl_task_time_percent/100) + degrees_idle;
			degrees_server = (360*g_server_task_time_percent/100) + degrees_lgvl;

			lv_arc_set_angles(ui->screen_Other, 0, degrees_other);
			lv_arc_set_angles(ui->screen_Idle, degrees_other + 1, degrees_idle + 1);
			lv_arc_set_angles(ui->screen_LVGL, degrees_idle + 1, degrees_lgvl + 1);
			lv_arc_set_angles(ui->screen_HttpServer, degrees_lgvl + 1, degrees_server + 1);
			g_sensors_state.update_performance = false;
		}
		// ------------------------------------------

		// Modify control interface -----------------
		control_interface_values    = get_control_interface_data();
		if(slider_value != lv_slider_get_value(ui->screen_SliderControl))
		{
			slider_value = lv_slider_get_value(ui->screen_SliderControl);
			if(slider_value < 10)
			{
				lv_bar_set_value(ui->screen_BarIndicator, 0, LV_ANIM_OFF);
				set_rpm(0);
				int_to_string(0, (uint8_t*)g_lablel_bar_indicator, 4);
			}
			else
			{
				lv_bar_set_value(ui->screen_BarIndicator, slider_value, LV_ANIM_OFF);
				int_to_string(slider_value*40, (uint8_t*)g_lablel_bar_indicator, 4);
				set_rpm(slider_value*40);
			}
		}
		else if(rpm != get_rpm())
		{
			rpm = get_rpm();
			if(500 > rpm)
			{
				motor_control_stop();
				set_rpm(0);
				rpm = 0;
			}
			else
			{
				motor_control_set_speed(rpm);
				motor_control_run();
			}
			lv_bar_set_value(ui->screen_SliderControl, rpm/40, LV_ANIM_OFF);
			lv_bar_set_value(ui->screen_BarIndicator, rpm/40, LV_ANIM_OFF);
			int_to_string(rpm, (uint8_t*)g_lablel_bar_indicator, 4);
		}

		lv_label_set_text(ui->screen_SliderPosition, (uint8_t*)g_lablel_bar_indicator);

		lv_arc_set_value(ui->screen_CounterArc, control_interface_values.counter);
		int_to_string(control_interface_values.counter, (uint8_t*)g_lablel_counter_ci, 3);
		lv_label_set_text(ui->screen_CounterLabel, (uint8_t*)g_lablel_counter_ci);
		// ------------------------------------------

		// Set touch --------------------------------
		if(true == g_sensors_state.update_touch)
		{
			touch_sensitive_data               = get_touch_sensitive_data();
			g_valid_touch_sensitive            = touch_sensitive_data.touch_sensitive_data;
			g_valid_touch_sensitive_percentaje = touch_sensitive_data.touch_sensitive_percentaje;

			int_to_string(g_valid_touch_sensitive, (uint8_t*)g_label_touch, 5);
			int_to_string(g_valid_touch_sensitive_percentaje, (uint8_t*)g_label_touch_percentaje, 3);
			lv_label_set_text(ui->screen_E1Value, (uint8_t*)g_label_touch);
			lv_arc_set_value(ui->screen_TSISliderPos, g_valid_touch_sensitive_percentaje);
			lv_label_set_text(ui->screen_Position, (uint8_t*)g_label_touch_percentaje);
			g_sensors_state.update_touch = false;
		}
		// ------------------------------------------
	}
}

void setup_scr_screen(lv_ui *ui)
{
	//Write codes screen
	ui->screen = lv_obj_create(NULL);
	lv_obj_set_size(ui->screen, 480, 320);

	//Write style for screen, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->screen, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->screen, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_img_2
	ui->screen_img_2 = lv_img_create(ui->screen);
	lv_obj_add_flag(ui->screen_img_2, LV_OBJ_FLAG_CLICKABLE);
	lv_img_set_src(ui->screen_img_2, &_NXP_Logo_RGB_Colour_alpha_100x50);
	lv_img_set_pivot(ui->screen_img_2, 50,50);
	lv_img_set_angle(ui->screen_img_2, 0);
	lv_obj_set_pos(ui->screen_img_2, 366, 5);
	lv_obj_set_size(ui->screen_img_2, 100, 50);

	//Write style for screen_img_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_img_opa(ui->screen_img_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_Date
	ui->screen_Date = lv_label_create(ui->screen);
	lv_label_set_text(ui->screen_Date, "Sep 07, 2023");
	lv_label_set_long_mode(ui->screen_Date, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->screen_Date, 350, 287);
	lv_obj_set_size(ui->screen_Date, 120, 25);

	//Write style for screen_Date, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->screen_Date, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_Date, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->screen_Date, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->screen_Date, &lv_font_montserratMedium_14, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->screen_Date, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->screen_Date, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->screen_Date, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->screen_Date, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->screen_Date, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->screen_Date, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->screen_Date, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->screen_Date, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->screen_Date, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_Clock
	ui->screen_Clock = lv_label_create(ui->screen);
	lv_label_set_text(ui->screen_Clock, "12:00:00 am");
	lv_label_set_long_mode(ui->screen_Clock, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->screen_Clock, 10, 287);
	lv_obj_set_size(ui->screen_Clock, 120, 20);

	//Write style for screen_Clock, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->screen_Clock, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_Clock, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->screen_Clock, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->screen_Clock, &lv_font_montserratMedium_14, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->screen_Clock, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->screen_Clock, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->screen_Clock, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->screen_Clock, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->screen_Clock, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->screen_Clock, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->screen_Clock, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->screen_Clock, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->screen_Clock, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_StationsView
	ui->screen_StationsView = lv_tileview_create(ui->screen);
	ui->screen_StationsView_StationTileview1 = lv_tileview_add_tile(ui->screen_StationsView, 0, 0, LV_DIR_NONE);
	ui->screen_StationsView_StationTileview2 = lv_tileview_add_tile(ui->screen_StationsView, 1, 0, LV_DIR_NONE);
	ui->screen_StationsView_StationTileview3 = lv_tileview_add_tile(ui->screen_StationsView, 2, 0, LV_DIR_NONE);
	ui->screen_StationsView_StationTileview4 = lv_tileview_add_tile(ui->screen_StationsView, 3, 0, LV_DIR_NONE);
	lv_obj_set_pos(ui->screen_StationsView, 40, 10);
	lv_obj_set_size(ui->screen_StationsView, 400, 275);
	lv_obj_set_scrollbar_mode(ui->screen_StationsView, LV_SCROLLBAR_MODE_OFF);

	//Write style for screen_StationsView, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->screen_StationsView, 26, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->screen_StationsView, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_StationsView, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->screen_StationsView, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style for screen_StationsView, Part: LV_PART_SCROLLBAR, State: LV_STATE_DEFAULT.
//	lv_obj_set_style_bg_opa(ui->screen_StationsView, 255, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
//	lv_obj_set_style_bg_color(ui->screen_StationsView, lv_color_hex(0xeaeff3), LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
//	lv_obj_set_style_radius(ui->screen_StationsView, 0, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);



	//Write codes screen_TittleLabel1
	ui->screen_TittleLabel1 = lv_label_create(ui->screen_StationsView_StationTileview1);
	lv_label_set_text(ui->screen_TittleLabel1, "Touch Sensing Input");
	lv_label_set_long_mode(ui->screen_TittleLabel1, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->screen_TittleLabel1, 75, 10);
	lv_obj_set_size(ui->screen_TittleLabel1, 250, 30);

	//Write style for screen_TittleLabel1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->screen_TittleLabel1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_TittleLabel1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->screen_TittleLabel1, lv_color_hex(0xF9B500), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->screen_TittleLabel1, &lv_font_montserratMedium_20, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->screen_TittleLabel1, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->screen_TittleLabel1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->screen_TittleLabel1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->screen_TittleLabel1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->screen_TittleLabel1, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->screen_TittleLabel1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->screen_TittleLabel1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->screen_TittleLabel1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->screen_TittleLabel1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_TSISliderPos
	ui->screen_TSISliderPos = lv_arc_create(ui->screen_StationsView_StationTileview1);
	lv_arc_set_mode(ui->screen_TSISliderPos, LV_ARC_MODE_NORMAL);
	lv_arc_set_range(ui->screen_TSISliderPos, 0, 100);
	lv_arc_set_bg_angles(ui->screen_TSISliderPos, 180, 360);
	lv_arc_set_angles(ui->screen_TSISliderPos, 180, 270);
	lv_arc_set_rotation(ui->screen_TSISliderPos, 0);
	lv_obj_set_pos(ui->screen_TSISliderPos, 90, 34);
	lv_obj_set_size(ui->screen_TSISliderPos, 220, 220);

	//Write style for screen_TSISliderPos, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->screen_TSISliderPos, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(ui->screen_TSISliderPos, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_arc_width(ui->screen_TSISliderPos, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_arc_opa(ui->screen_TSISliderPos, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_arc_color(ui->screen_TSISliderPos, lv_color_hex(0x5c5c5c), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_TSISliderPos, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->screen_TSISliderPos, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->screen_TSISliderPos, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->screen_TSISliderPos, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->screen_TSISliderPos, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->screen_TSISliderPos, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style for screen_TSISliderPos, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
	lv_obj_set_style_arc_width(ui->screen_TSISliderPos, 20, LV_PART_INDICATOR|LV_STATE_DEFAULT);
	lv_obj_set_style_arc_opa(ui->screen_TSISliderPos, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
	lv_obj_set_style_arc_color(ui->screen_TSISliderPos, lv_color_hex(0x0EAFE0), LV_PART_INDICATOR|LV_STATE_DEFAULT);

	//Write style for screen_TSISliderPos, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->screen_TSISliderPos, 0, LV_PART_KNOB|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_all(ui->screen_TSISliderPos, 20, LV_PART_KNOB|LV_STATE_DEFAULT);

	//Write codes screen_Position
	ui->screen_Position = lv_label_create(ui->screen_StationsView_StationTileview1);
	lv_label_set_text(ui->screen_Position, "50%");
	lv_label_set_long_mode(ui->screen_Position, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->screen_Position, 160, 110);
	lv_obj_set_size(ui->screen_Position, 80, 30);

	//Write style for screen_Position, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->screen_Position, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_Position, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->screen_Position, lv_color_hex(0x0EAFE0), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->screen_Position, &lv_font_montserratMedium_20, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->screen_Position, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->screen_Position, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->screen_Position, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->screen_Position, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->screen_Position, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->screen_Position, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->screen_Position, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->screen_Position, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->screen_Position, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_Elect1
	ui->screen_Elect1 = lv_label_create(ui->screen_StationsView_StationTileview1);
	lv_label_set_text(ui->screen_Elect1, "Electrode");
	lv_label_set_long_mode(ui->screen_Elect1, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->screen_Elect1, 150, 170);
	lv_obj_set_size(ui->screen_Elect1, 100, 25);

	//Write style for screen_Elect1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->screen_Elect1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_Elect1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->screen_Elect1, lv_color_hex(0xFFFFFF), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->screen_Elect1, &lv_font_montserratMedium_14, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->screen_Elect1, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->screen_Elect1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->screen_Elect1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->screen_Elect1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->screen_Elect1, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->screen_Elect1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->screen_Elect1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->screen_Elect1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->screen_Elect1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_E1Value
	ui->screen_E1Value = lv_label_create(ui->screen_StationsView_StationTileview1);
	lv_label_set_text(ui->screen_E1Value, "0");
	lv_label_set_long_mode(ui->screen_E1Value, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->screen_E1Value, 150, 200);
	lv_obj_set_size(ui->screen_E1Value, 100, 25);

	//Write style for screen_E1Value, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->screen_E1Value, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_E1Value, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->screen_E1Value, lv_color_hex(0xFFFFFF), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->screen_E1Value, &lv_font_montserratMedium_14, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->screen_E1Value, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->screen_E1Value, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->screen_E1Value, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->screen_E1Value, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->screen_E1Value, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->screen_E1Value, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->screen_E1Value, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->screen_E1Value, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->screen_E1Value, 0, LV_PART_MAIN|LV_STATE_DEFAULT);



	//Write codes screen_TempCelsius
	ui->screen_TempCelsius = lv_meter_create(ui->screen_StationsView_StationTileview2);
	// add scale screen_TempCelsius_scale_1
	lv_meter_scale_t *screen_TempCelsius_scale_1 = lv_meter_add_scale(ui->screen_TempCelsius);
	lv_meter_set_scale_ticks(ui->screen_TempCelsius, screen_TempCelsius_scale_1, 41, 10, 10, lv_color_hex(0x4a4436));
	lv_meter_set_scale_major_ticks(ui->screen_TempCelsius, screen_TempCelsius_scale_1, 4, 3, 11, lv_color_hex(0xe4bd49), 11);
	lv_meter_set_scale_range(ui->screen_TempCelsius, screen_TempCelsius_scale_1, 0, 100, 300, 90);

	// add needle line for screen_TempCelsius_scale_1.
	ui->screen_TempCelsius_scale_1_ndline_0 = lv_meter_add_needle_line(ui->screen_TempCelsius, screen_TempCelsius_scale_1, 5, lv_color_hex(0xE0B74F), -10);
	lv_meter_set_indicator_value(ui->screen_TempCelsius, ui->screen_TempCelsius_scale_1_ndline_0, 10);
	lv_obj_set_pos(ui->screen_TempCelsius, 13, 50);
	lv_obj_set_size(ui->screen_TempCelsius, 180, 180);

	//Write style for screen_TempCelsius, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->screen_TempCelsius, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_TempCelsius, 133, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(ui->screen_TempCelsius, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_opa(ui->screen_TempCelsius, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_color(ui->screen_TempCelsius, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_side(ui->screen_TempCelsius, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->screen_TempCelsius, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style for screen_TempCelsius, Part: LV_PART_TICKS, State: LV_STATE_DEFAULT.
	lv_obj_set_style_text_color(ui->screen_TempCelsius, lv_color_hex(0xF9B500), LV_PART_TICKS|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->screen_TempCelsius, &lv_font_montserratMedium_14, LV_PART_TICKS|LV_STATE_DEFAULT);

	//Write style for screen_TempCelsius, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->screen_TempCelsius, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->screen_TempCelsius, lv_color_hex(0x000000), LV_PART_INDICATOR|LV_STATE_DEFAULT);

	//Write codes screen_TittleLabel2
	ui->screen_TittleLabel2 = lv_label_create(ui->screen_StationsView_StationTileview2);
	lv_label_set_text(ui->screen_TittleLabel2, "Temperature Sensors");
	lv_label_set_long_mode(ui->screen_TittleLabel2, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->screen_TittleLabel2, 50, 10);
	lv_obj_set_size(ui->screen_TittleLabel2, 300, 30);

	//Write style for screen_TittleLabel2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->screen_TittleLabel2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_TittleLabel2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->screen_TittleLabel2, lv_color_hex(0xF9B500), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->screen_TittleLabel2, &lv_font_montserratMedium_20, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->screen_TittleLabel2, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->screen_TittleLabel2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->screen_TittleLabel2, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->screen_TittleLabel2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->screen_TittleLabel2, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->screen_TittleLabel2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->screen_TittleLabel2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->screen_TittleLabel2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->screen_TittleLabel2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_DCelsius
	ui->screen_DCelsius = lv_label_create(ui->screen_StationsView_StationTileview2);
	lv_label_set_text(ui->screen_DCelsius, "30 °C");
	lv_label_set_long_mode(ui->screen_DCelsius, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->screen_DCelsius, 105, 180);
	lv_obj_set_size(ui->screen_DCelsius, 60, 22);

	//Write style for screen_DCelsius, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->screen_DCelsius, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_DCelsius, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->screen_DCelsius, lv_color_hex(0x0EAFE0), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->screen_DCelsius, &lv_font_montserratMedium_14, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->screen_DCelsius, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->screen_DCelsius, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->screen_DCelsius, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->screen_DCelsius, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->screen_DCelsius, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->screen_DCelsius, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->screen_DCelsius, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->screen_DCelsius, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->screen_DCelsius, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_meter_1
	ui->screen_meter_1 = lv_meter_create(ui->screen_StationsView_StationTileview2);
	// add scale screen_meter_1_scale_1
	lv_meter_scale_t *screen_meter_1_scale_1 = lv_meter_add_scale(ui->screen_meter_1);
	lv_meter_set_scale_ticks(ui->screen_meter_1, screen_meter_1_scale_1, 41, 10, 10, lv_color_hex(0x4a4436));
	lv_meter_set_scale_major_ticks(ui->screen_meter_1, screen_meter_1_scale_1, 4, 3, 11, lv_color_hex(0xe4bd49), 11);
	lv_meter_set_scale_range(ui->screen_meter_1, screen_meter_1_scale_1, 0, 200, 300, 90);

	// add needle line for screen_meter_1_scale_1.
	ui->screen_meter_1_scale_1_ndline_0 = lv_meter_add_needle_line(ui->screen_meter_1, screen_meter_1_scale_1, 5, lv_color_hex(0xE0B74F), -10);
	lv_meter_set_indicator_value(ui->screen_meter_1, ui->screen_meter_1_scale_1_ndline_0, 50);
	lv_obj_set_pos(ui->screen_meter_1, 207, 50);
	lv_obj_set_size(ui->screen_meter_1, 180, 180);

	//Write style for screen_meter_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->screen_meter_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_meter_1, 133, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(ui->screen_meter_1, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_opa(ui->screen_meter_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_color(ui->screen_meter_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_side(ui->screen_meter_1, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->screen_meter_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style for screen_meter_1, Part: LV_PART_TICKS, State: LV_STATE_DEFAULT.
	lv_obj_set_style_text_color(ui->screen_meter_1, lv_color_hex(0xE0B74F), LV_PART_TICKS|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->screen_meter_1, &lv_font_montserratMedium_14, LV_PART_TICKS|LV_STATE_DEFAULT);

	//Write style for screen_meter_1, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->screen_meter_1, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->screen_meter_1, lv_color_hex(0x000000), LV_PART_INDICATOR|LV_STATE_DEFAULT);

	//Write codes screen_DFahrenheit
	ui->screen_DFahrenheit = lv_label_create(ui->screen_StationsView_StationTileview2);
	lv_label_set_text(ui->screen_DFahrenheit, "25°C");
	lv_label_set_long_mode(ui->screen_DFahrenheit, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->screen_DFahrenheit, 300, 180);
	lv_obj_set_size(ui->screen_DFahrenheit, 60, 21);

	//Write style for screen_DFahrenheit, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->screen_DFahrenheit, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_DFahrenheit, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->screen_DFahrenheit, lv_color_hex(0x0EAFE0), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->screen_DFahrenheit, &lv_font_montserratMedium_14, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->screen_DFahrenheit, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->screen_DFahrenheit, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->screen_DFahrenheit, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->screen_DFahrenheit, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->screen_DFahrenheit, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->screen_DFahrenheit, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->screen_DFahrenheit, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->screen_DFahrenheit, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->screen_DFahrenheit, 0, LV_PART_MAIN|LV_STATE_DEFAULT);



	//Write codes screen_TittleLabel3
	ui->screen_TittleLabel3 = lv_label_create(ui->screen_StationsView_StationTileview3);
	lv_label_set_text(ui->screen_TittleLabel3, "CPU Performance");
	lv_label_set_long_mode(ui->screen_TittleLabel3, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->screen_TittleLabel3, 75, 10);
	lv_obj_set_size(ui->screen_TittleLabel3, 250, 30);

	//Write style for screen_TittleLabel3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->screen_TittleLabel3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_TittleLabel3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->screen_TittleLabel3, lv_color_hex(0xF9B500), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->screen_TittleLabel3, &lv_font_montserratMedium_20, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->screen_TittleLabel3, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->screen_TittleLabel3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->screen_TittleLabel3, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->screen_TittleLabel3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->screen_TittleLabel3, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->screen_TittleLabel3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->screen_TittleLabel3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->screen_TittleLabel3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->screen_TittleLabel3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_LVGL
	ui->screen_LVGL = lv_arc_create(ui->screen_StationsView_StationTileview3);
	lv_arc_set_mode(ui->screen_LVGL, LV_ARC_MODE_NORMAL);
	lv_arc_set_range(ui->screen_LVGL, 0, 100);
	lv_arc_set_bg_angles(ui->screen_LVGL, 0, 0);
	lv_arc_set_angles(ui->screen_LVGL, 180, 268);
	lv_arc_set_rotation(ui->screen_LVGL, 0);
	lv_obj_set_style_arc_rounded(ui->screen_LVGL, 0,  LV_PART_INDICATOR|LV_STATE_DEFAULT);
	lv_obj_set_style_arc_rounded(ui->screen_LVGL, 0, LV_STATE_DEFAULT);
	lv_obj_set_pos(ui->screen_LVGL, 20, 40);
	lv_obj_set_size(ui->screen_LVGL, 180, 180);

	//Write style for screen_LVGL, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->screen_LVGL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(ui->screen_LVGL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_arc_width(ui->screen_LVGL, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_arc_opa(ui->screen_LVGL, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_arc_color(ui->screen_LVGL, lv_color_hex(0x4d4d4d), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_LVGL, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->screen_LVGL, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->screen_LVGL, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->screen_LVGL, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->screen_LVGL, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->screen_LVGL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style for screen_LVGL, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
	lv_obj_set_style_arc_width(ui->screen_LVGL, 10, LV_PART_INDICATOR|LV_STATE_DEFAULT);
	lv_obj_set_style_arc_opa(ui->screen_LVGL, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
	lv_obj_set_style_arc_color(ui->screen_LVGL, lv_color_hex(0x0EAFE0), LV_PART_INDICATOR|LV_STATE_DEFAULT);

	//Write style for screen_LVGL, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->screen_LVGL, 0, LV_PART_KNOB|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_all(ui->screen_LVGL, 20, LV_PART_KNOB|LV_STATE_DEFAULT);

	//Write codes screen_HttpServer
	ui->screen_HttpServer = lv_arc_create(ui->screen_StationsView_StationTileview3);
	lv_arc_set_mode(ui->screen_HttpServer, LV_ARC_MODE_NORMAL);
	lv_arc_set_range(ui->screen_HttpServer, 0, 100);
	lv_arc_set_bg_angles(ui->screen_HttpServer, 0, 0);
	lv_arc_set_angles(ui->screen_HttpServer, 270, 358);
	lv_arc_set_rotation(ui->screen_HttpServer, 0);
	lv_obj_set_style_arc_rounded(ui->screen_HttpServer, 0,  LV_PART_INDICATOR|LV_STATE_DEFAULT);
	lv_obj_set_style_arc_rounded(ui->screen_HttpServer, 0, LV_STATE_DEFAULT);
	lv_obj_set_pos(ui->screen_HttpServer, 20, 40);
	lv_obj_set_size(ui->screen_HttpServer, 180, 180);

	//Write style for screen_HttpServer, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->screen_HttpServer, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(ui->screen_HttpServer, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_arc_width(ui->screen_HttpServer, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_arc_opa(ui->screen_HttpServer, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_arc_color(ui->screen_HttpServer, lv_color_hex(0x4d4d4d), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_HttpServer, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->screen_HttpServer, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->screen_HttpServer, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->screen_HttpServer, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->screen_HttpServer, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->screen_HttpServer, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style for screen_HttpServer, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
	lv_obj_set_style_arc_width(ui->screen_HttpServer, 15, LV_PART_INDICATOR|LV_STATE_DEFAULT);
	lv_obj_set_style_arc_opa(ui->screen_HttpServer, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
	lv_obj_set_style_arc_color(ui->screen_HttpServer, lv_color_hex(0x69CA00), LV_PART_INDICATOR|LV_STATE_DEFAULT);

	//Write style for screen_HttpServer, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->screen_HttpServer, 0, LV_PART_KNOB|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_all(ui->screen_HttpServer, 20, LV_PART_KNOB|LV_STATE_DEFAULT);

	//Write codes screen_Idle
	ui->screen_Idle = lv_arc_create(ui->screen_StationsView_StationTileview3);
	lv_arc_set_mode(ui->screen_Idle, LV_ARC_MODE_NORMAL);
	lv_arc_set_range(ui->screen_Idle, 0, 100);
	lv_arc_set_bg_angles(ui->screen_Idle, 0, 0);
	lv_arc_set_angles(ui->screen_Idle, 90, 178);
	lv_arc_set_rotation(ui->screen_Idle, 0);
	lv_obj_set_style_arc_rounded(ui->screen_Idle, 0,  LV_PART_INDICATOR|LV_STATE_DEFAULT);
	lv_obj_set_style_arc_rounded(ui->screen_Idle, 0, LV_STATE_DEFAULT);
	lv_obj_set_pos(ui->screen_Idle, 20, 40);
	lv_obj_set_size(ui->screen_Idle, 180, 180);

	//Write style for screen_Idle, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->screen_Idle, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(ui->screen_Idle, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_arc_width(ui->screen_Idle, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_arc_opa(ui->screen_Idle, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_arc_color(ui->screen_Idle, lv_color_hex(0x4d4d4d), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_Idle, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->screen_Idle, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->screen_Idle, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->screen_Idle, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->screen_Idle, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->screen_Idle, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style for screen_Idle, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
	lv_obj_set_style_arc_width(ui->screen_Idle, 20, LV_PART_INDICATOR|LV_STATE_DEFAULT);
	lv_obj_set_style_arc_opa(ui->screen_Idle, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
	lv_obj_set_style_arc_color(ui->screen_Idle, lv_color_hex(0xF9B500), LV_PART_INDICATOR|LV_STATE_DEFAULT);

	//Write style for screen_Idle, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->screen_Idle, 0, LV_PART_KNOB|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_all(ui->screen_Idle, 20, LV_PART_KNOB|LV_STATE_DEFAULT);

	//Write codes screen_Other
	ui->screen_Other = lv_arc_create(ui->screen_StationsView_StationTileview3);
	lv_arc_set_mode(ui->screen_Other, LV_ARC_MODE_NORMAL);
	lv_arc_set_range(ui->screen_Other, 0, 100);
	lv_arc_set_bg_angles(ui->screen_Other, 0, 0);
	lv_arc_set_angles(ui->screen_Other, 0, 88);
	lv_arc_set_rotation(ui->screen_Other, 0);
	lv_obj_set_style_arc_rounded(ui->screen_Other, 0,  LV_PART_INDICATOR|LV_STATE_DEFAULT);
	lv_obj_set_style_arc_rounded(ui->screen_Other, 0, LV_STATE_DEFAULT);
	lv_obj_set_pos(ui->screen_Other, 20, 40);
	lv_obj_set_size(ui->screen_Other, 180, 180);

	//Write style for screen_Other, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->screen_Other, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(ui->screen_Other, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_arc_width(ui->screen_Other, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_arc_opa(ui->screen_Other, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_arc_color(ui->screen_Other, lv_color_hex(0x4d4d4d), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_Other, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->screen_Other, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->screen_Other, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->screen_Other, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->screen_Other, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->screen_Other, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style for screen_Other, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
	lv_obj_set_style_arc_width(ui->screen_Other, 5, LV_PART_INDICATOR|LV_STATE_DEFAULT);
	lv_obj_set_style_arc_opa(ui->screen_Other, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
	lv_obj_set_style_arc_color(ui->screen_Other, lv_color_hex(0x646464), LV_PART_INDICATOR|LV_STATE_DEFAULT);

	//Write style for screen_Other, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->screen_Other, 0, LV_PART_KNOB|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_all(ui->screen_Other, 20, LV_PART_KNOB|LV_STATE_DEFAULT);

	//Write codes screen_LVGLBullet
	ui->screen_LVGLBullet = lv_img_create(ui->screen_StationsView_StationTileview3);
	lv_obj_add_flag(ui->screen_LVGLBullet, LV_OBJ_FLAG_CLICKABLE);
	lv_img_set_src(ui->screen_LVGLBullet, &_Bullet3_alpha_10x10);
	lv_img_set_pivot(ui->screen_LVGLBullet, 50,50);
	lv_img_set_angle(ui->screen_LVGLBullet, 0);
	lv_obj_set_pos(ui->screen_LVGLBullet, 220, 82);
	lv_obj_set_size(ui->screen_LVGLBullet, 10, 10);

	//Write style for screen_LVGLBullet, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_img_opa(ui->screen_LVGLBullet, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_IdleBullet
	ui->screen_IdleBullet = lv_img_create(ui->screen_StationsView_StationTileview3);
	lv_obj_add_flag(ui->screen_IdleBullet, LV_OBJ_FLAG_CLICKABLE);
	lv_img_set_src(ui->screen_IdleBullet, &_Bullet1_alpha_10x10);
	lv_img_set_pivot(ui->screen_IdleBullet, 50,50);
	lv_img_set_angle(ui->screen_IdleBullet, 0);
	lv_obj_set_pos(ui->screen_IdleBullet, 220, 178);
	lv_obj_set_size(ui->screen_IdleBullet, 10, 10);

	//Write style for screen_IdleBullet, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_img_opa(ui->screen_IdleBullet, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_OtherBullet
	ui->screen_OtherBullet = lv_img_create(ui->screen_StationsView_StationTileview3);
	lv_obj_add_flag(ui->screen_OtherBullet, LV_OBJ_FLAG_CLICKABLE);
	lv_img_set_src(ui->screen_OtherBullet, &_Bullet4_alpha_10x10);
	lv_img_set_pivot(ui->screen_OtherBullet, 50,50);
	lv_img_set_angle(ui->screen_OtherBullet, 0);
	lv_obj_set_pos(ui->screen_OtherBullet, 220, 146);
	lv_obj_set_size(ui->screen_OtherBullet, 10, 10);

	//Write style for screen_OtherBullet, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_img_opa(ui->screen_OtherBullet, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_HttpServerBullet
	ui->screen_HttpServerBullet = lv_img_create(ui->screen_StationsView_StationTileview3);
	lv_obj_add_flag(ui->screen_HttpServerBullet, LV_OBJ_FLAG_CLICKABLE);
	lv_img_set_src(ui->screen_HttpServerBullet, &_Bullet2_alpha_10x10);
	lv_img_set_pivot(ui->screen_HttpServerBullet, 50,50);
	lv_img_set_angle(ui->screen_HttpServerBullet, 0);
	lv_obj_set_pos(ui->screen_HttpServerBullet, 220, 114);
	lv_obj_set_size(ui->screen_HttpServerBullet, 10, 10);

	//Write style for screen_HttpServerBullet, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_img_opa(ui->screen_HttpServerBullet, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_LVGLLabel
	ui->screen_LVGLLabel = lv_label_create(ui->screen_StationsView_StationTileview3);
	lv_label_set_text(ui->screen_LVGLLabel, "LVGL:");
	lv_label_set_long_mode(ui->screen_LVGLLabel, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->screen_LVGLLabel, 233, 72);
	lv_obj_set_size(ui->screen_LVGLLabel, 70, 25);

	//Write style for screen_LVGLLabel, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->screen_LVGLLabel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_LVGLLabel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->screen_LVGLLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->screen_LVGLLabel, &lv_font_montserratMedium_14, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->screen_LVGLLabel, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->screen_LVGLLabel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->screen_LVGLLabel, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->screen_LVGLLabel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->screen_LVGLLabel, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->screen_LVGLLabel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->screen_LVGLLabel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->screen_LVGLLabel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->screen_LVGLLabel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_HttpLabel
	ui->screen_HttpLabel = lv_label_create(ui->screen_StationsView_StationTileview3);
	lv_label_set_text(ui->screen_HttpLabel, "Server:");
	lv_label_set_long_mode(ui->screen_HttpLabel, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->screen_HttpLabel, 240, 104);
	lv_obj_set_size(ui->screen_HttpLabel, 70, 25);

	//Write style for screen_HttpLabel, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->screen_HttpLabel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_HttpLabel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->screen_HttpLabel, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->screen_HttpLabel, &lv_font_montserratMedium_14, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->screen_HttpLabel, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->screen_HttpLabel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->screen_HttpLabel, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->screen_HttpLabel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->screen_HttpLabel, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->screen_HttpLabel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->screen_HttpLabel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->screen_HttpLabel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->screen_HttpLabel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_OtherLabel
	ui->screen_OtherLabel = lv_label_create(ui->screen_StationsView_StationTileview3);
	lv_label_set_text(ui->screen_OtherLabel, "Other:");
	lv_label_set_long_mode(ui->screen_OtherLabel, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->screen_OtherLabel, 236, 136);
	lv_obj_set_size(ui->screen_OtherLabel, 70, 25);

	//Write style for screen_OtherLabel, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->screen_OtherLabel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_OtherLabel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->screen_OtherLabel, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->screen_OtherLabel, &lv_font_montserratMedium_14, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->screen_OtherLabel, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->screen_OtherLabel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->screen_OtherLabel, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->screen_OtherLabel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->screen_OtherLabel, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->screen_OtherLabel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->screen_OtherLabel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->screen_OtherLabel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->screen_OtherLabel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_IdleLabel
	ui->screen_IdleLabel = lv_label_create(ui->screen_StationsView_StationTileview3);
	lv_label_set_text(ui->screen_IdleLabel, "Idle:");
	lv_label_set_long_mode(ui->screen_IdleLabel, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->screen_IdleLabel, 229, 168);
	lv_obj_set_size(ui->screen_IdleLabel, 70, 25);

	//Write style for screen_IdleLabel, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->screen_IdleLabel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_IdleLabel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->screen_IdleLabel, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->screen_IdleLabel, &lv_font_montserratMedium_14, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->screen_IdleLabel, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->screen_IdleLabel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->screen_IdleLabel, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->screen_IdleLabel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->screen_IdleLabel, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->screen_IdleLabel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->screen_IdleLabel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->screen_IdleLabel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->screen_IdleLabel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_LVGLPercentage
	ui->screen_LVGLPercentage = lv_label_create(ui->screen_StationsView_StationTileview3);
	lv_label_set_text(ui->screen_LVGLPercentage, "27%");
	lv_label_set_long_mode(ui->screen_LVGLPercentage, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->screen_LVGLPercentage, 300, 72);
	lv_obj_set_size(ui->screen_LVGLPercentage, 70, 25);

	//Write style for screen_LVGLPercentage, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->screen_LVGLPercentage, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_LVGLPercentage, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->screen_LVGLPercentage, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->screen_LVGLPercentage, &lv_font_montserratMedium_14, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->screen_LVGLPercentage, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->screen_LVGLPercentage, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->screen_LVGLPercentage, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->screen_LVGLPercentage, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->screen_LVGLPercentage, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->screen_LVGLPercentage, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->screen_LVGLPercentage, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->screen_LVGLPercentage, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->screen_LVGLPercentage, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_ServerPercentage
	ui->screen_ServerPercentage = lv_label_create(ui->screen_StationsView_StationTileview3);
	lv_label_set_text(ui->screen_ServerPercentage, "35%");
	lv_label_set_long_mode(ui->screen_ServerPercentage, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->screen_ServerPercentage, 300, 104);
	lv_obj_set_size(ui->screen_ServerPercentage, 70, 25);

	//Write style for screen_ServerPercentage, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->screen_ServerPercentage, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_ServerPercentage, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->screen_ServerPercentage, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->screen_ServerPercentage, &lv_font_montserratMedium_14, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->screen_ServerPercentage, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->screen_ServerPercentage, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->screen_ServerPercentage, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->screen_ServerPercentage, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->screen_ServerPercentage, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->screen_ServerPercentage, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->screen_ServerPercentage, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->screen_ServerPercentage, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->screen_ServerPercentage, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_OtherPercentage
	ui->screen_OtherPercentage = lv_label_create(ui->screen_StationsView_StationTileview3);
	lv_label_set_text(ui->screen_OtherPercentage, "16%");
	lv_label_set_long_mode(ui->screen_OtherPercentage, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->screen_OtherPercentage, 300, 136);
	lv_obj_set_size(ui->screen_OtherPercentage, 70, 25);

	//Write style for screen_OtherPercentage, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->screen_OtherPercentage, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_OtherPercentage, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->screen_OtherPercentage, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->screen_OtherPercentage, &lv_font_montserratMedium_14, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->screen_OtherPercentage, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->screen_OtherPercentage, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->screen_OtherPercentage, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->screen_OtherPercentage, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->screen_OtherPercentage, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->screen_OtherPercentage, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->screen_OtherPercentage, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->screen_OtherPercentage, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->screen_OtherPercentage, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_IdlePercentage
	ui->screen_IdlePercentage = lv_label_create(ui->screen_StationsView_StationTileview3);
	lv_label_set_text(ui->screen_IdlePercentage, "22%");
	lv_label_set_long_mode(ui->screen_IdlePercentage, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->screen_IdlePercentage, 300, 168);
	lv_obj_set_size(ui->screen_IdlePercentage, 70, 25);

	//Write style for screen_IdlePercentage, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->screen_IdlePercentage, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_IdlePercentage, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->screen_IdlePercentage, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->screen_IdlePercentage, &lv_font_montserratMedium_14, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->screen_IdlePercentage, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->screen_IdlePercentage, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->screen_IdlePercentage, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->screen_IdlePercentage, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->screen_IdlePercentage, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->screen_IdlePercentage, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->screen_IdlePercentage, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->screen_IdlePercentage, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->screen_IdlePercentage, 0, LV_PART_MAIN|LV_STATE_DEFAULT);



	//Write codes screen_tittleLabel4
	ui->screen_tittleLabel4 = lv_label_create(ui->screen_StationsView_StationTileview4);
	lv_label_set_text(ui->screen_tittleLabel4, "Control Interface");
	lv_label_set_long_mode(ui->screen_tittleLabel4, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->screen_tittleLabel4, 75, 10);
	lv_obj_set_size(ui->screen_tittleLabel4, 250, 30);

	//Write style for screen_tittleLabel4, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->screen_tittleLabel4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_tittleLabel4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->screen_tittleLabel4, lv_color_hex(0xF9B500), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->screen_tittleLabel4, &lv_font_montserratMedium_20, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->screen_tittleLabel4, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->screen_tittleLabel4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->screen_tittleLabel4, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->screen_tittleLabel4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->screen_tittleLabel4, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->screen_tittleLabel4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->screen_tittleLabel4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->screen_tittleLabel4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->screen_tittleLabel4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_CounterArc
	ui->screen_CounterArc = lv_arc_create(ui->screen_StationsView_StationTileview4);
	lv_arc_set_mode(ui->screen_CounterArc, LV_ARC_MODE_NORMAL);
	lv_arc_set_range(ui->screen_CounterArc, 0, 100);
	lv_arc_set_bg_angles(ui->screen_CounterArc, 0, 360);
	lv_arc_set_angles(ui->screen_CounterArc, 0, 200);
	lv_arc_set_rotation(ui->screen_CounterArc, 90);
	lv_obj_set_pos(ui->screen_CounterArc, 220, 40);
	lv_obj_set_size(ui->screen_CounterArc, 160, 160);

	//Write style for screen_CounterArc, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->screen_CounterArc, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(ui->screen_CounterArc, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_arc_width(ui->screen_CounterArc, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_arc_opa(ui->screen_CounterArc, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_arc_color(ui->screen_CounterArc, lv_color_hex(0x5c5c5c), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_CounterArc, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->screen_CounterArc, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->screen_CounterArc, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->screen_CounterArc, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->screen_CounterArc, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->screen_CounterArc, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style for screen_CounterArc, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
	lv_obj_set_style_arc_width(ui->screen_CounterArc, 20, LV_PART_INDICATOR|LV_STATE_DEFAULT);
	lv_obj_set_style_arc_opa(ui->screen_CounterArc, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
	lv_obj_set_style_arc_color(ui->screen_CounterArc, lv_color_hex(0x69CA00), LV_PART_INDICATOR|LV_STATE_DEFAULT);

	//Write style for screen_CounterArc, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->screen_CounterArc, 0, LV_PART_KNOB|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_all(ui->screen_CounterArc, 20, LV_PART_KNOB|LV_STATE_DEFAULT);

	//Write codes screen_MinusButton
	ui->screen_MinusButton = lv_btn_create(ui->screen_StationsView_StationTileview4);
	ui->screen_MinusButton_label = lv_label_create(ui->screen_MinusButton);
	lv_label_set_text(ui->screen_MinusButton_label, "-");
	lv_label_set_long_mode(ui->screen_MinusButton_label, LV_LABEL_LONG_WRAP);
	lv_obj_align(ui->screen_MinusButton_label, LV_ALIGN_CENTER, 0, 0);
	lv_obj_set_style_pad_all(ui->screen_MinusButton, 0, LV_STATE_DEFAULT);
	lv_obj_set_pos(ui->screen_MinusButton, 240, 190);
	lv_obj_set_size(ui->screen_MinusButton, 40, 30);

	//Write style for screen_MinusButton, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->screen_MinusButton, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->screen_MinusButton, lv_color_hex(0x5C5C5C), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(ui->screen_MinusButton, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_MinusButton, 3, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->screen_MinusButton, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->screen_MinusButton, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->screen_MinusButton, &lv_font_montserratMedium_20, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->screen_MinusButton, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style for screen_MinusButton, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
	lv_obj_set_style_bg_opa(ui->screen_MinusButton, 255, LV_PART_MAIN|LV_STATE_PRESSED);
	lv_obj_set_style_bg_color(ui->screen_MinusButton, lv_color_hex(0x858585), LV_PART_MAIN|LV_STATE_PRESSED);
	lv_obj_set_style_border_width(ui->screen_MinusButton, 0, LV_PART_MAIN|LV_STATE_PRESSED);
	lv_obj_set_style_radius(ui->screen_MinusButton, 3, LV_PART_MAIN|LV_STATE_PRESSED);
	lv_obj_set_style_shadow_width(ui->screen_MinusButton, 0, LV_PART_MAIN|LV_STATE_PRESSED);
	lv_obj_set_style_text_color(ui->screen_MinusButton, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_PRESSED);
	lv_obj_set_style_text_font(ui->screen_MinusButton, &lv_font_montserratMedium_20, LV_PART_MAIN|LV_STATE_PRESSED);

	//Write codes screen_PlusButton
	ui->screen_PlusButton = lv_btn_create(ui->screen_StationsView_StationTileview4);
	ui->screen_PlusButton_label = lv_label_create(ui->screen_PlusButton);
	lv_label_set_text(ui->screen_PlusButton_label, "+");
	lv_label_set_long_mode(ui->screen_PlusButton_label, LV_LABEL_LONG_WRAP);
	lv_obj_align(ui->screen_PlusButton_label, LV_ALIGN_CENTER, 0, 0);
	lv_obj_set_style_pad_all(ui->screen_PlusButton, 0, LV_STATE_DEFAULT);
	lv_obj_set_pos(ui->screen_PlusButton, 320, 190);
	lv_obj_set_size(ui->screen_PlusButton, 40, 30);

	//Write style for screen_PlusButton, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->screen_PlusButton, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->screen_PlusButton, lv_color_hex(0x5C5C5C), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(ui->screen_PlusButton, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_PlusButton, 3, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->screen_PlusButton, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->screen_PlusButton, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->screen_PlusButton, &lv_font_montserratMedium_20, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->screen_PlusButton, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style for screen_PlusButton, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
	lv_obj_set_style_bg_opa(ui->screen_PlusButton, 255, LV_PART_MAIN|LV_STATE_PRESSED);
	lv_obj_set_style_bg_color(ui->screen_PlusButton, lv_color_hex(0x858585), LV_PART_MAIN|LV_STATE_PRESSED);
	lv_obj_set_style_border_width(ui->screen_PlusButton, 0, LV_PART_MAIN|LV_STATE_PRESSED);
	lv_obj_set_style_radius(ui->screen_PlusButton, 3, LV_PART_MAIN|LV_STATE_PRESSED);
	lv_obj_set_style_shadow_width(ui->screen_PlusButton, 0, LV_PART_MAIN|LV_STATE_PRESSED);
	lv_obj_set_style_text_color(ui->screen_PlusButton, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_PRESSED);
	lv_obj_set_style_text_font(ui->screen_PlusButton, &lv_font_montserratMedium_20, LV_PART_MAIN|LV_STATE_PRESSED);

	//Write codes screen_CounterLabel
	ui->screen_CounterLabel = lv_label_create(ui->screen_StationsView_StationTileview4);
	lv_label_set_text(ui->screen_CounterLabel, "66");
	lv_label_set_long_mode(ui->screen_CounterLabel, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->screen_CounterLabel, 283, 106);
	lv_obj_set_size(ui->screen_CounterLabel, 35, 25);

	//Write style for screen_CounterLabel, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->screen_CounterLabel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_CounterLabel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->screen_CounterLabel, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->screen_CounterLabel, &lv_font_montserratMedium_14, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->screen_CounterLabel, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->screen_CounterLabel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->screen_CounterLabel, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->screen_CounterLabel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->screen_CounterLabel, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->screen_CounterLabel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->screen_CounterLabel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->screen_CounterLabel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->screen_CounterLabel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_BarIndicator
	ui->screen_BarIndicator = lv_bar_create(ui->screen_StationsView_StationTileview4);
	lv_obj_set_style_anim_time(ui->screen_BarIndicator, 1000, 0);
	lv_bar_set_mode(ui->screen_BarIndicator, LV_BAR_MODE_NORMAL);
	lv_bar_set_value(ui->screen_BarIndicator, 70, LV_ANIM_OFF);
	lv_obj_set_pos(ui->screen_BarIndicator, 85, 84);
	lv_obj_set_size(ui->screen_BarIndicator, 30, 100);

	//Write style for screen_BarIndicator, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->screen_BarIndicator, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->screen_BarIndicator, lv_color_hex(0x5C5C5C), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_BarIndicator, 6, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->screen_BarIndicator, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style for screen_BarIndicator, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->screen_BarIndicator, 129, LV_PART_INDICATOR|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->screen_BarIndicator, lv_color_hex(0x0EAFE0), LV_PART_INDICATOR|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->screen_BarIndicator, LV_GRAD_DIR_VER, LV_PART_INDICATOR|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_color(ui->screen_BarIndicator, lv_color_hex(0x000000), LV_PART_INDICATOR|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_BarIndicator, 6, LV_PART_INDICATOR|LV_STATE_DEFAULT);

	//Write codes screen_SliderControl
	ui->screen_SliderControl = lv_slider_create(ui->screen_StationsView_StationTileview4);
	lv_slider_set_range(ui->screen_SliderControl, 0, 100);
	lv_slider_set_mode(ui->screen_SliderControl, LV_SLIDER_MODE_NORMAL);
	lv_slider_set_value(ui->screen_SliderControl, 50, LV_ANIM_OFF);
	lv_obj_set_pos(ui->screen_SliderControl, 25, 200);
	lv_obj_set_size(ui->screen_SliderControl, 150, 10);

	//Write style for screen_SliderControl, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->screen_SliderControl, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->screen_SliderControl, lv_color_hex(0x5C5C5C), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_SliderControl, 33, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_outline_width(ui->screen_SliderControl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->screen_SliderControl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style for screen_SliderControl, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->screen_SliderControl, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->screen_SliderControl, lv_color_hex(0xF9B500), LV_PART_INDICATOR|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_SliderControl, 33, LV_PART_INDICATOR|LV_STATE_DEFAULT);

	//Write style for screen_SliderControl, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->screen_SliderControl, 144, LV_PART_KNOB|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->screen_SliderControl, lv_color_hex(0xE0B74F), LV_PART_KNOB|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_SliderControl, 33, LV_PART_KNOB|LV_STATE_DEFAULT);

	//Write codes screen_SliderPosition
	ui->screen_SliderPosition = lv_label_create(ui->screen_StationsView_StationTileview4);
	lv_label_set_text(ui->screen_SliderPosition, "3000 rpm");
	lv_label_set_long_mode(ui->screen_SliderPosition, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->screen_SliderPosition, 50, 50);
	lv_obj_set_size(ui->screen_SliderPosition, 100, 25);

	//Write style for screen_SliderPosition, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->screen_SliderPosition, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_SliderPosition, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->screen_SliderPosition, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->screen_SliderPosition, &lv_font_montserratMedium_14, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->screen_SliderPosition, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->screen_SliderPosition, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->screen_SliderPosition, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->screen_SliderPosition, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->screen_SliderPosition, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->screen_SliderPosition, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->screen_SliderPosition, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->screen_SliderPosition, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->screen_SliderPosition, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_StationButton1
	ui->screen_StationButton1 = lv_imgbtn_create(ui->screen);
	lv_obj_add_flag(ui->screen_StationButton1, LV_OBJ_FLAG_CHECKABLE);
	lv_imgbtn_set_src(ui->screen_StationButton1, LV_IMGBTN_STATE_RELEASED, NULL, &_TouchC_alpha_50x50, NULL);
	lv_imgbtn_set_src(ui->screen_StationButton1, LV_IMGBTN_STATE_PRESSED, NULL, &_TouchB_alpha_50x50, NULL);
	lv_imgbtn_set_src(ui->screen_StationButton1, LV_IMGBTN_STATE_CHECKED_RELEASED, NULL, &_TouchC_alpha_50x50, NULL);
	lv_imgbtn_set_src(ui->screen_StationButton1, LV_IMGBTN_STATE_CHECKED_PRESSED, NULL, &_TouchB_alpha_50x50, NULL);
	ui->screen_StationButton1_label = lv_label_create(ui->screen_StationButton1);
	lv_label_set_text(ui->screen_StationButton1_label, "");
	lv_label_set_long_mode(ui->screen_StationButton1_label, LV_LABEL_LONG_WRAP);
	lv_obj_align(ui->screen_StationButton1_label, LV_ALIGN_CENTER, 0, 0);
	lv_obj_set_style_pad_all(ui->screen_StationButton1, 0, LV_STATE_DEFAULT);
	lv_obj_set_pos(ui->screen_StationButton1, 260, 240);
	lv_obj_set_size(ui->screen_StationButton1, 50, 50);

	//Write style for screen_StationButton1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_text_color(ui->screen_StationButton1, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->screen_StationButton1, &lv_font_montserratMedium_14, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->screen_StationButton1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->screen_StationButton1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style for screen_StationButton1, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
	lv_obj_set_style_img_opa(ui->screen_StationButton1, 255, LV_PART_MAIN|LV_STATE_PRESSED);
	lv_obj_set_style_text_color(ui->screen_StationButton1, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_PRESSED);
	lv_obj_set_style_text_font(ui->screen_StationButton1, &lv_font_montserratMedium_14, LV_PART_MAIN|LV_STATE_PRESSED);
	lv_obj_set_style_shadow_width(ui->screen_StationButton1, 0, LV_PART_MAIN|LV_STATE_PRESSED);

	//Write style for screen_StationButton1, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
	lv_obj_set_style_img_opa(ui->screen_StationButton1, 255, LV_PART_MAIN|LV_STATE_CHECKED);
	lv_obj_set_style_text_color(ui->screen_StationButton1, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_CHECKED);
	lv_obj_set_style_text_font(ui->screen_StationButton1, &lv_font_montserratMedium_14, LV_PART_MAIN|LV_STATE_CHECKED);
	lv_obj_set_style_shadow_width(ui->screen_StationButton1, 0, LV_PART_MAIN|LV_STATE_CHECKED);

	//Write style for screen_StationButton1, Part: LV_PART_MAIN, State: LV_IMGBTN_STATE_RELEASED.
	lv_obj_set_style_img_opa(ui->screen_StationButton1, 255, LV_PART_MAIN|LV_IMGBTN_STATE_RELEASED);

	//Write codes screen_StationButton2
	ui->screen_StationButton2 = lv_imgbtn_create(ui->screen);
	lv_obj_add_flag(ui->screen_StationButton2, LV_OBJ_FLAG_CHECKABLE);
	lv_imgbtn_set_src(ui->screen_StationButton2, LV_IMGBTN_STATE_RELEASED, NULL, &_TempC_alpha_50x50, NULL);
	lv_imgbtn_set_src(ui->screen_StationButton2, LV_IMGBTN_STATE_PRESSED, NULL, &_TempB_alpha_50x50, NULL);
	lv_imgbtn_set_src(ui->screen_StationButton2, LV_IMGBTN_STATE_CHECKED_RELEASED, NULL, &_TempC_alpha_50x50, NULL);
	lv_imgbtn_set_src(ui->screen_StationButton2, LV_IMGBTN_STATE_CHECKED_PRESSED, NULL, &_TempB_alpha_50x50, NULL);
	ui->screen_StationButton2_label = lv_label_create(ui->screen_StationButton2);
	lv_label_set_text(ui->screen_StationButton2_label, "");
	lv_label_set_long_mode(ui->screen_StationButton2_label, LV_LABEL_LONG_WRAP);
	lv_obj_align(ui->screen_StationButton2_label, LV_ALIGN_CENTER, 0, 0);
	lv_obj_set_style_pad_all(ui->screen_StationButton2, 0, LV_STATE_DEFAULT);
	lv_obj_set_pos(ui->screen_StationButton2, 170, 240);
	lv_obj_set_size(ui->screen_StationButton2, 50, 50);

	//Write style for screen_StationButton2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_text_color(ui->screen_StationButton2, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->screen_StationButton2, &lv_font_montserratMedium_14, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->screen_StationButton2, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->screen_StationButton2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style for screen_StationButton2, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
	lv_obj_set_style_img_opa(ui->screen_StationButton2, 255, LV_PART_MAIN|LV_STATE_PRESSED);
	lv_obj_set_style_text_color(ui->screen_StationButton2, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_PRESSED);
	lv_obj_set_style_text_font(ui->screen_StationButton2, &lv_font_montserratMedium_14, LV_PART_MAIN|LV_STATE_PRESSED);
	lv_obj_set_style_shadow_width(ui->screen_StationButton2, 0, LV_PART_MAIN|LV_STATE_PRESSED);

	//Write style for screen_StationButton2, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
	lv_obj_set_style_img_opa(ui->screen_StationButton2, 255, LV_PART_MAIN|LV_STATE_CHECKED);
	lv_obj_set_style_text_color(ui->screen_StationButton2, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_CHECKED);
	lv_obj_set_style_text_font(ui->screen_StationButton2, &lv_font_montserratMedium_14, LV_PART_MAIN|LV_STATE_CHECKED);
	lv_obj_set_style_shadow_width(ui->screen_StationButton2, 0, LV_PART_MAIN|LV_STATE_CHECKED);

	//Write style for screen_StationButton2, Part: LV_PART_MAIN, State: LV_IMGBTN_STATE_RELEASED.
	lv_obj_set_style_img_opa(ui->screen_StationButton2, 255, LV_PART_MAIN|LV_IMGBTN_STATE_RELEASED);

	//Write codes screen_StationButton3
	ui->screen_StationButton3 = lv_imgbtn_create(ui->screen);
	lv_obj_add_flag(ui->screen_StationButton3, LV_OBJ_FLAG_CHECKABLE);
	lv_imgbtn_set_src(ui->screen_StationButton3, LV_IMGBTN_STATE_RELEASED, NULL, &_Station3c_alpha_50x50, NULL);
	lv_imgbtn_set_src(ui->screen_StationButton3, LV_IMGBTN_STATE_PRESSED, NULL, &_Station3b_alpha_50x50, NULL);
	lv_imgbtn_set_src(ui->screen_StationButton3, LV_IMGBTN_STATE_CHECKED_RELEASED, NULL, &_Station3c_alpha_50x50, NULL);
	lv_imgbtn_set_src(ui->screen_StationButton3, LV_IMGBTN_STATE_CHECKED_PRESSED, NULL, &_Station3b_alpha_50x50, NULL);
	ui->screen_StationButton3_label = lv_label_create(ui->screen_StationButton3);
	lv_label_set_text(ui->screen_StationButton3_label, "");
	lv_label_set_long_mode(ui->screen_StationButton3_label, LV_LABEL_LONG_WRAP);
	lv_obj_align(ui->screen_StationButton3_label, LV_ALIGN_CENTER, 0, 0);
	lv_obj_set_style_pad_all(ui->screen_StationButton3, 0, LV_STATE_DEFAULT);
	lv_obj_set_pos(ui->screen_StationButton3, 80, 240);
	lv_obj_set_size(ui->screen_StationButton3, 50, 50);

	//Write style for screen_StationButton3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_text_color(ui->screen_StationButton3, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->screen_StationButton3, &lv_font_montserratMedium_14, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->screen_StationButton3, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->screen_StationButton3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style for screen_StationButton3, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
	lv_obj_set_style_img_opa(ui->screen_StationButton3, 255, LV_PART_MAIN|LV_STATE_PRESSED);
	lv_obj_set_style_text_color(ui->screen_StationButton3, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_PRESSED);
	lv_obj_set_style_text_font(ui->screen_StationButton3, &lv_font_montserratMedium_14, LV_PART_MAIN|LV_STATE_PRESSED);
	lv_obj_set_style_shadow_width(ui->screen_StationButton3, 0, LV_PART_MAIN|LV_STATE_PRESSED);

	//Write style for screen_StationButton3, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
	lv_obj_set_style_img_opa(ui->screen_StationButton3, 255, LV_PART_MAIN|LV_STATE_CHECKED);
	lv_obj_set_style_text_color(ui->screen_StationButton3, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_CHECKED);
	lv_obj_set_style_text_font(ui->screen_StationButton3, &lv_font_montserratMedium_14, LV_PART_MAIN|LV_STATE_CHECKED);
	lv_obj_set_style_shadow_width(ui->screen_StationButton3, 0, LV_PART_MAIN|LV_STATE_CHECKED);

	//Write style for screen_StationButton3, Part: LV_PART_MAIN, State: LV_IMGBTN_STATE_RELEASED.
	lv_obj_set_style_img_opa(ui->screen_StationButton3, 255, LV_PART_MAIN|LV_IMGBTN_STATE_RELEASED);

	//Write codes screen_StationButton4
	ui->screen_StationButton4 = lv_imgbtn_create(ui->screen);
	lv_obj_add_flag(ui->screen_StationButton4, LV_OBJ_FLAG_CHECKABLE);
	lv_imgbtn_set_src(ui->screen_StationButton4, LV_IMGBTN_STATE_RELEASED, NULL, &_ControlC_alpha_50x50, NULL);
	lv_imgbtn_set_src(ui->screen_StationButton4, LV_IMGBTN_STATE_PRESSED, NULL, &_ControlB_alpha_50x50, NULL);
	lv_imgbtn_set_src(ui->screen_StationButton4, LV_IMGBTN_STATE_CHECKED_RELEASED, NULL, &_ControlC_alpha_50x50, NULL);
	lv_imgbtn_set_src(ui->screen_StationButton4, LV_IMGBTN_STATE_CHECKED_PRESSED, NULL, &_ControlB_alpha_50x50, NULL);
	ui->screen_StationButton4_label = lv_label_create(ui->screen_StationButton4);
	lv_label_set_text(ui->screen_StationButton4_label, "");
	lv_label_set_long_mode(ui->screen_StationButton4_label, LV_LABEL_LONG_WRAP);
	lv_obj_align(ui->screen_StationButton4_label, LV_ALIGN_CENTER, 0, 0);
	lv_obj_set_style_pad_all(ui->screen_StationButton4, 0, LV_STATE_DEFAULT);
	lv_obj_set_pos(ui->screen_StationButton4, 350, 240);
	lv_obj_set_size(ui->screen_StationButton4, 50, 50);

	//Write style for screen_StationButton4, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_text_color(ui->screen_StationButton4, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->screen_StationButton4, &lv_font_montserratMedium_14, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->screen_StationButton4, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->screen_StationButton4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style for screen_StationButton4, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
	lv_obj_set_style_img_opa(ui->screen_StationButton4, 255, LV_PART_MAIN|LV_STATE_PRESSED);
	lv_obj_set_style_text_color(ui->screen_StationButton4, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_PRESSED);
	lv_obj_set_style_text_font(ui->screen_StationButton4, &lv_font_montserratMedium_14, LV_PART_MAIN|LV_STATE_PRESSED);
	lv_obj_set_style_shadow_width(ui->screen_StationButton4, 0, LV_PART_MAIN|LV_STATE_PRESSED);

	//Write style for screen_StationButton4, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
	lv_obj_set_style_img_opa(ui->screen_StationButton4, 255, LV_PART_MAIN|LV_STATE_CHECKED);
	lv_obj_set_style_text_color(ui->screen_StationButton4, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_CHECKED);
	lv_obj_set_style_text_font(ui->screen_StationButton4, &lv_font_montserratMedium_14, LV_PART_MAIN|LV_STATE_CHECKED);
	lv_obj_set_style_shadow_width(ui->screen_StationButton4, 0, LV_PART_MAIN|LV_STATE_CHECKED);

	//Write style for screen_StationButton4, Part: LV_PART_MAIN, State: LV_IMGBTN_STATE_RELEASED.
	lv_obj_set_style_img_opa(ui->screen_StationButton4, 255, LV_PART_MAIN|LV_IMGBTN_STATE_RELEASED);

	//Update current screen layout.
	lv_obj_update_layout(ui->screen);


	//Init events for screen.
	events_init_screen(ui);
}
