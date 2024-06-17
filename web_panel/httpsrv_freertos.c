/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2022, 2024 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "lwip/opt.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>

#include "lwip/netif.h"
#include "lwip/sys.h"
#include "lwip/arch.h"
#include "lwip/api.h"
#include "lwip/tcpip.h"
#include "lwip/ip.h"
#include "lwip/netifapi.h"
#include "lwip/sockets.h"
#include "lv_tileview.h"
#include "netif/etharp.h"

#include "httpsrv.h"
#include "lwip/apps/mdns.h"
#include "sensors.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#ifndef HTTPD_DEBUG
#define HTTPD_DEBUG LWIP_DBG_ON
#endif

#ifndef DEBUG_WS
#define DEBUG_WS 0
#endif

#define CGI_DATA_LENGTH_MAX (96)
#define ARC_SERVER_PERIMETER 440
#define ARC_TOUCH_PERIMETER  471
#define ARC_CNT_PERIMETER    314

#define SERVER_PERF_LEN				3
#define LVGL_PERF_LEN   			3
#define OTHER_PERF_LEN  			3
#define IDLE_PERF_LEN				3
#define SERVER_PERF_TENS_LEN 		2
#define LVGL_PERF_TENS_LEN			2
#define OTHER_PERF_TENS_LEN			2
#define IDLE_PERF_TENS_LEN			2
#define EXT_TEMPERATURE_LEN			3
#define INT_TEMPERATURE_LEN			3
#define ELECTRODE_LEN				4
#define RPM_LEN						5
#define HOURS_LEN					3
#define MINUTES_LEN					3
#define SECONDS_LEN					3
#define MERIDIAN_LEN				3
#define DAY_LEN						3
#define MONTH_LEN					3
#define YEAR_LEN					5
#define EXT_TEMP_IND_LEN			5
#define INT_TEPM_IND_LEN			5
#define ON_LVGL_INDICATOR_LEN		4
#define OFF_LVGL_INDICATOR_LEN		4
#define OFFSET_LVGL_INDICATOR_LEN	4
#define ON_SERVER_INDICATOR_LEN		4
#define OFF_SERVER_INDICATOR_LEN	4
#define OFFSET_SERVER_INDICATOR_LEN	4
#define ON_OTHER_INDICATOR_LEN		4
#define OFF_OTHER_INDICATOR_LEN		4
#define ON_IDLE_INDICATOR_LEN		4
#define OFF_IDLE_INDICATOR_LEN		4
#define OFFSET_IDLE_INDICATOR_LEN	4
#define ON_TOUCH_INDICATOR_LEN		4
#define OFF_TOUCH_INDICATOR_LEN		4
#define RPM_PERCENT_LEN				4
#define RPM_SLIDER_LEN				4
#define ELECTRODE_VALUE_LEN			6
#define COUNTER_VALUE_LEN			4
#define ON_COUNTER_INDICATOR_LEN	4
#define OFF_COUNTER_INDICATOR_LEN	4
#define ACTUAL_WINDOW_LEN			2

const uint8_t offset_SERVER_PERF				= 0;
const uint8_t offset_LVGL_PERF   			    = offset_SERVER_PERF			    + SERVER_PERF_LEN;
const uint8_t offset_OTHER_PERF  			    = offset_LVGL_PERF   			    + LVGL_PERF_LEN;
const uint8_t offset_IDLE_PERF				    = offset_OTHER_PERF  			    + OTHER_PERF_LEN;
const uint8_t offset_SERVER_PERF_TENS 		    = offset_IDLE_PERF				    + IDLE_PERF_LEN;
const uint8_t offset_LVGL_PERF_TENS			    = offset_SERVER_PERF_TENS 			+ SERVER_PERF_TENS_LEN;
const uint8_t offset_OTHER_PERF_TENS			= offset_LVGL_PERF_TENS				+ LVGL_PERF_TENS_LEN;
const uint8_t offset_IDLE_PERF_TENS			    = offset_OTHER_PERF_TENS			+ OTHER_PERF_TENS_LEN;
const uint8_t offset_EXT_TEMPERATURE			= offset_IDLE_PERF_TENS				+ IDLE_PERF_TENS_LEN;
const uint8_t offset_INT_TEMPERATURE			= offset_EXT_TEMPERATURE			+ EXT_TEMPERATURE_LEN;
const uint8_t offset_ELECTRODE				    = offset_INT_TEMPERATURE		    + INT_TEMPERATURE_LEN;
const uint8_t offset_RPM						= offset_ELECTRODE					+ ELECTRODE_LEN;
const uint8_t offset_HOURS					    = offset_RPM					    + RPM_LEN;
const uint8_t offset_MINUTES					= offset_HOURS						+ HOURS_LEN;
const uint8_t offset_SECONDS					= offset_MINUTES					+ MINUTES_LEN;
const uint8_t offset_MERIDIAN				    = offset_SECONDS				    + SECONDS_LEN;
const uint8_t offset_DAY						= offset_MERIDIAN					+ MERIDIAN_LEN;
const uint8_t offset_MONTH					    = offset_DAY					    + DAY_LEN;
const uint8_t offset_YEAR					    = offset_MONTH					    + MONTH_LEN;
const uint8_t offset_EXT_TEMP_IND			    = offset_YEAR					    + YEAR_LEN;
const uint8_t offset_INT_TEPM_IND			    = offset_EXT_TEMP_IND			    + EXT_TEMP_IND_LEN;
const uint8_t offset_ON_LVGL_INDICATOR		    = offset_INT_TEPM_IND			    + INT_TEPM_IND_LEN;
const uint8_t offset_OFF_LVGL_INDICATOR		    = offset_ON_LVGL_INDICATOR			+ ON_LVGL_INDICATOR_LEN;
const uint8_t offset_OFFSET_LVGL_INDICATOR	    = offset_OFF_LVGL_INDICATOR		    + OFF_LVGL_INDICATOR_LEN;
const uint8_t offset_ON_SERVER_INDICATOR		= offset_OFFSET_LVGL_INDICATOR		+ OFFSET_LVGL_INDICATOR_LEN;
const uint8_t offset_OFF_SERVER_INDICATOR	    = offset_ON_SERVER_INDICATOR	    + ON_SERVER_INDICATOR_LEN;
const uint8_t offset_OFFSET_SERVER_INDICATOR	= offset_OFF_SERVER_INDICATOR		+ OFF_SERVER_INDICATOR_LEN;
const uint8_t offset_ON_OTHER_INDICATOR		    = offset_OFFSET_SERVER_INDICATOR	+ OFFSET_SERVER_INDICATOR_LEN;
const uint8_t offset_OFF_OTHER_INDICATOR		= offset_ON_OTHER_INDICATOR			+ ON_OTHER_INDICATOR_LEN;
const uint8_t offset_ON_IDLE_INDICATOR		    = offset_OFF_OTHER_INDICATOR	    + OFF_OTHER_INDICATOR_LEN;
const uint8_t offset_OFF_IDLE_INDICATOR		    = offset_ON_IDLE_INDICATOR			+ ON_IDLE_INDICATOR_LEN	;
const uint8_t offset_OFFSET_IDLE_INDICATOR	    = offset_OFF_IDLE_INDICATOR		    + OFF_IDLE_INDICATOR_LEN;
const uint8_t offset_ON_TOUCH_INDICATOR		    = offset_OFFSET_IDLE_INDICATOR      + OFFSET_IDLE_INDICATOR_LEN;
const uint8_t offset_OFF_TOUCH_INDICATOR		= offset_ON_TOUCH_INDICATOR	        + ON_TOUCH_INDICATOR_LEN;
const uint8_t offset_RPM_PERCENT				= offset_OFF_TOUCH_INDICATOR        + OFF_TOUCH_INDICATOR_LEN;
const uint8_t offset_RPM_SLIDER				    = offset_RPM_PERCENT		        + RPM_PERCENT_LEN;
const uint8_t offset_ELECTRODE_VALUE			= offset_RPM_SLIDER			        + RPM_SLIDER_LEN;
const uint8_t offset_COUNTER_VALUE			    = offset_ELECTRODE_VALUE		    + ELECTRODE_VALUE_LEN;
const uint8_t offset_ON_COUNTER_INDICATOR	    = offset_COUNTER_VALUE			    + COUNTER_VALUE_LEN;
const uint8_t offset_OFF_COUNTER_INDICATOR	    = offset_ON_COUNTER_INDICATOR	    + ON_COUNTER_INDICATOR_LEN;
const uint8_t offset_ACTUAL_WINDOW			    = offset_OFF_COUNTER_INDICATOR	    + OFF_COUNTER_INDICATOR_LEN;

#define BUFF_SIZE 152

/*******************************************************************************
 * Prototypes
 ******************************************************************************/


static void cgi_urldecode(char *url);
static int set_control_interface_window(HTTPSRV_CGI_REQ_STRUCT *param);
static int set_performance_window(HTTPSRV_CGI_REQ_STRUCT *param);
static int set_temperature_window(HTTPSRV_CGI_REQ_STRUCT *param);
static int set_touch_window(HTTPSRV_CGI_REQ_STRUCT *param);
static int increment_server_counter(HTTPSRV_CGI_REQ_STRUCT *param);
static int decrement_server_counter(HTTPSRV_CGI_REQ_STRUCT *param);
static int cgi_rtc_data(HTTPSRV_CGI_REQ_STRUCT *param);
static int cgi_example(HTTPSRV_CGI_REQ_STRUCT *param);
static int ssi_config(HTTPSRV_SSI_PARAM_STRUCT *param);
static bool cgi_get_varval(char *var_str, char *var_name, char *var_val, uint32_t length);

/*******************************************************************************
 * Variables
 ******************************************************************************/

static char s_mdns_hostname[65] = "";

/* FS data.*/
extern const HTTPSRV_FS_DIR_ENTRY httpsrv_fs_data[];
volatile static uint8_t g_buff[BUFF_SIZE] = {"00\n00\n00\n00\n0\n0\n0\n0\n00\n00\n000\n0000\n00\n00\n00\nam\n00\n00\n0000\n0000\n0000\n000\n000\n000\n000\n000\n000\n000\n000\n000\n000\n000\n000\n000\n000\n000\n00000\n000\n000\n000\n0\n"};

/*
 * Authentication users
 */
static const HTTPSRV_AUTH_USER_STRUCT users[] = {
    {"admin", "admin"}, {NULL, NULL} /* Array terminator */
};

/*
 * Authentication information.
 */
static const HTTPSRV_AUTH_REALM_STRUCT auth_realms[] = {
    {"Please use uid:admin pass:admin to login", "/auth.html", HTTPSRV_AUTH_BASIC, users},
    {NULL, NULL, HTTPSRV_AUTH_INVALID, NULL} /* Array terminator */
};

char cgi_data[CGI_DATA_LENGTH_MAX + 1];

const HTTPSRV_CGI_LINK_STRUCT cgi_lnk_tbl[] = {
    {"rtcdata", cgi_rtc_data},
    {"get", cgi_example},
    {"post", cgi_example},
    {"increment", increment_server_counter},
    {"decrement", decrement_server_counter},
    {"ctrl_interface", set_control_interface_window},
    {"perf", set_performance_window},
    {"temp", set_temperature_window},
    {"touch", set_touch_window},
    {0, 0} // DO NOT REMOVE - last item - end of table
};

const HTTPSRV_SSI_LINK_STRUCT ssi_lnk_tbl[] = {{"config", ssi_config}, {0, 0}};

/*******************************************************************************
 * Code
 ******************************************************************************/
void update_server_sensors()
{
	sensor_updates_t 		   sensors_state;
	performance_data_t         performance_data;
	touch_sensitive_data_t     touch_sensitive_data;
	rtc_data_t                 rtc_data;
	temperature_data_t         temperature_data;
	control_interface_values_t control_interface_values;
	uint16_t                   lvgl_server_arc_on;
	uint16_t                   lvgl_server_arc_off;
	uint16_t                   lvgl_server_arc_offset;
	uint16_t                   http_server_arc_on;
	uint16_t                   http_server_arc_off;
	uint16_t                   http_server_arc_offset;
	uint16_t                   idle_server_arc_on;
	uint16_t                   idle_server_arc_off;
	uint16_t                   idle_server_arc_offset;
	uint16_t                   other_server_arc_on;
	uint16_t                   other_server_arc_off;
	uint16_t                   touch_arc_on;
	uint16_t                   touch_arc_off;
	uint16_t                   counter_arc_on;
	uint16_t                   counter_arc_off;
	uint8_t                    rpm_percentaje;
	uint8_t                    rpm_slider;
	uint8_t                    counter;

	if(true == sensor_updates_server())
	{
		sensors_state = get_values_state_server();
		// Set temperature --------------------------
		if(true == sensors_state.update_mcu_temperature || true == sensors_state.update_ext_temperature)
		{
			temperature_data = get_temperature_data();
		    int_to_string(temperature_data.external_temperature, (uint8_t*)g_buff+offset_EXT_TEMPERATURE,  2);
		    int_to_string(temperature_data.internal_temperature, (uint8_t*)g_buff+offset_INT_TEMPERATURE, 2);
		    if(positive == int_to_string(1.5*temperature_data.external_temperature-90, (uint8_t*)g_buff+offset_EXT_TEMP_IND+1, 3))
		    {
		    	g_buff[offset_EXT_TEMP_IND] = '0';
		    }
		    else
		    {
		    	g_buff[offset_EXT_TEMP_IND] = '-';
		    }
		    if(positive == int_to_string(1.5*temperature_data.internal_temperature-90, (uint8_t*)g_buff+offset_INT_TEPM_IND+1, 3))
			{
				g_buff[offset_INT_TEPM_IND] = '0';
			}
			else
			{
				g_buff[offset_INT_TEPM_IND] = '-';
			}
		}
		// ------------------------------------------

		// Set time ---------------------------------
		if(true == sensors_state.update_rtc)
		{
			rtc_data = get_rtc_data();
		    if(12 < rtc_data.hours)
		    {
		    	rtc_data.hours -= 12;
		    }
		    if(12 <= rtc_data.hours)
		    {
		    	g_buff[offset_MERIDIAN] = 'p';
		    }
		    else
		    {
		    	g_buff[offset_MERIDIAN] = 'a';
		    }
		    int_to_string(rtc_data.hours, (uint8_t*)g_buff+offset_HOURS, 2);
		    int_to_string(rtc_data.minutes, (uint8_t*)g_buff+offset_MINUTES, 2);
		    int_to_string(rtc_data.seconds, (uint8_t*)g_buff+offset_SECONDS, 2);

		    int_to_string(rtc_data.day, (uint8_t*)g_buff+offset_DAY, 2);
		    int_to_string(rtc_data.month, (uint8_t*)g_buff+offset_MONTH, 2);
		    int_to_string(rtc_data.year, (uint8_t*)g_buff+offset_YEAR, 4);
		}
		// ------------------------------------------

		// Set Performance --------------------------
		if(true == sensors_state.update_performance)
		{
			performance_data = get_performance_data();
		    int_to_string(performance_data.server_task_int,   (uint8_t*) g_buff,    2);
		    int_to_string(performance_data.lvgl_task_int, (uint8_t*) g_buff+offset_LVGL_PERF,  2);
		    int_to_string(performance_data.other_task_int,   (uint8_t*)g_buff+offset_OTHER_PERF,  2);
		    int_to_string(performance_data.idle_task_int,    (uint8_t*)g_buff+offset_IDLE_PERF,  2);
		    int_to_string(performance_data.server_task_tens,   (uint8_t*)g_buff+offset_SERVER_PERF_TENS, 1);
		    int_to_string(performance_data.lvgl_task_tens, (uint8_t*)g_buff+offset_LVGL_PERF_TENS, 1);
		    int_to_string(performance_data.other_task_tens,  (uint8_t*)g_buff+offset_OTHER_PERF_TENS, 1);
		    int_to_string(performance_data.idle_task_tens,   (uint8_t*)g_buff+offset_IDLE_PERF_TENS, 1);

		    other_server_arc_on = ARC_SERVER_PERIMETER*performance_data.other_task_int/100;
		    other_server_arc_off = ARC_SERVER_PERIMETER - other_server_arc_on;

		    idle_server_arc_on     = ARC_SERVER_PERIMETER*performance_data.idle_task_int/100;
		    idle_server_arc_off    = ARC_SERVER_PERIMETER - idle_server_arc_on;
		    idle_server_arc_offset = other_server_arc_on;

		    lvgl_server_arc_on = ARC_SERVER_PERIMETER*performance_data.lvgl_task_int/100;
		    lvgl_server_arc_off = ARC_SERVER_PERIMETER - lvgl_server_arc_on;
		    lvgl_server_arc_offset = idle_server_arc_on + idle_server_arc_offset;

		    http_server_arc_on = ARC_SERVER_PERIMETER*performance_data.server_task_int/100;
		    http_server_arc_off = ARC_SERVER_PERIMETER - http_server_arc_on;
		    http_server_arc_offset = lvgl_server_arc_on + lvgl_server_arc_offset;

		    int_to_string(lvgl_server_arc_on,   (uint8_t*)g_buff+offset_ON_LVGL_INDICATOR, 3);
		    int_to_string(lvgl_server_arc_off,   (uint8_t*)g_buff+offset_OFF_LVGL_INDICATOR, 3);
		    int_to_string(lvgl_server_arc_offset,   (uint8_t*)g_buff+offset_OFFSET_LVGL_INDICATOR, 3);

		    int_to_string(http_server_arc_on,   (uint8_t*)g_buff+offset_ON_SERVER_INDICATOR, 3);
		    int_to_string(http_server_arc_off,   (uint8_t*)g_buff+offset_OFF_SERVER_INDICATOR, 3);
		    int_to_string(http_server_arc_offset,   (uint8_t*)g_buff+offset_OFFSET_SERVER_INDICATOR, 3);

		    int_to_string(other_server_arc_on,   (uint8_t*)g_buff+offset_ON_OTHER_INDICATOR, 3);
		    int_to_string(other_server_arc_off,   (uint8_t*)g_buff+offset_OFF_OTHER_INDICATOR, 3);

		    int_to_string(idle_server_arc_on,   (uint8_t*)g_buff+offset_ON_IDLE_INDICATOR, 3);
		    int_to_string(idle_server_arc_off,   (uint8_t*)g_buff+offset_OFF_IDLE_INDICATOR, 3);
		    int_to_string(idle_server_arc_offset,   (uint8_t*)g_buff+offset_OFFSET_IDLE_INDICATOR, 3);
		}
		// ------------------------------------------

	    // Rpm -------------------------------------------------
		control_interface_values    = get_control_interface_data();
	    int_to_string(control_interface_values.rpm, (uint8_t*)g_buff+offset_RPM, 4);
	    rpm_percentaje = control_interface_values.rpm/40;
	    rpm_slider     = control_interface_values.rpm/20;
	    counter        = control_interface_values.counter;
	    counter_arc_on = ARC_CNT_PERIMETER * (uint16_t)counter / 100;
	    counter_arc_off = ARC_CNT_PERIMETER - counter_arc_on;

	    int_to_string(rpm_percentaje, (uint8_t*)g_buff+offset_RPM_PERCENT, 3);
	    int_to_string(rpm_slider, (uint8_t*)g_buff+offset_RPM_SLIDER, 3);
	    int_to_string(counter, (uint8_t*)g_buff+offset_COUNTER_VALUE, 3);
	    int_to_string(counter_arc_on, (uint8_t*)g_buff+offset_ON_COUNTER_INDICATOR, 3);
	    int_to_string(counter_arc_off, (uint8_t*)g_buff+offset_OFF_COUNTER_INDICATOR, 3);
	    // -----------------------------------------------------

	    // Window ----------------------------------------------
	    int_to_string(get_actual_window(), (uint8_t*)g_buff+offset_ACTUAL_WINDOW, 1);
	    // -----------------------------------------------------

		// Set touch --------------------------------
		if(true == sensors_state.update_touch)
		{
			touch_sensitive_data = get_touch_sensitive_data();
		    int_to_string(touch_sensitive_data.touch_sensitive_percentaje, (uint8_t*)g_buff+offset_ELECTRODE, 3);
		    touch_arc_on = ARC_TOUCH_PERIMETER*touch_sensitive_data.touch_sensitive_percentaje/200;
		    touch_arc_off = ARC_TOUCH_PERIMETER - touch_arc_on;
		    int_to_string(touch_arc_on, (uint8_t*)g_buff+offset_ON_TOUCH_INDICATOR, 3);
		    int_to_string(touch_arc_off, (uint8_t*)g_buff+offset_OFF_TOUCH_INDICATOR, 3);
		    int_to_string(touch_sensitive_data.touch_sensitive_data, (uint8_t*)g_buff+offset_ELECTRODE_VALUE, 5);
		}
		// ------------------------------------------
	}
}

int return_index_page(HTTPSRV_CGI_REQ_STRUCT *param)
{
	HTTPSRV_CGI_RES_STRUCT response = {0};

	response.ses_handle  = param->ses_handle;
	response.status_code = HTTPSRV_CODE_OK;

	/* Write the response using chunked transmission coding. */
	response.content_type = HTTPSRV_CONTENT_TYPE_HTML;
	/* Set content length to -1 to indicate unknown content length. */
	response.content_length = -1;
	response.data           = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\">";
	response.data_length    = strlen(response.data);
	HTTPSRV_cgi_write(&response);
	response.data        = "<html><head><title>POST successfull!</title>";
	response.data_length = strlen(response.data);
	HTTPSRV_cgi_write(&response);
	response.data        = "<meta http-equiv=\"refresh\" content=\"0; url=index.html\"></head><body></body></html>";
	response.data_length = strlen(response.data);
	HTTPSRV_cgi_write(&response);
	response.data_length = 0;
	HTTPSRV_cgi_write(&response);
    return (response.content_length);
}

static int set_control_interface_window(HTTPSRV_CGI_REQ_STRUCT *param)
{
	set_actual_window(4);
	lv_obj_set_tile(guider_ui.screen_StationsView, guider_ui.screen_StationsView_StationTileview4, LV_ANIM_OFF);
	return 0;
}

static int set_performance_window(HTTPSRV_CGI_REQ_STRUCT *param)
{
	set_actual_window(3);
	lv_obj_set_tile(guider_ui.screen_StationsView, guider_ui.screen_StationsView_StationTileview3, LV_ANIM_OFF);
	return 0;
}

static int set_temperature_window(HTTPSRV_CGI_REQ_STRUCT *param)
{
	set_actual_window(2);
	lv_obj_set_tile(guider_ui.screen_StationsView, guider_ui.screen_StationsView_StationTileview2, LV_ANIM_OFF);
	return 0;
}

static int set_touch_window(HTTPSRV_CGI_REQ_STRUCT *param)
{
	set_actual_window(1);
	lv_obj_set_tile(guider_ui.screen_StationsView, guider_ui.screen_StationsView_StationTileview1, LV_ANIM_OFF);
	return 0;
}

static int increment_server_counter(HTTPSRV_CGI_REQ_STRUCT *param)
{
	increment_counter();
	return 0;
}

static int decrement_server_counter(HTTPSRV_CGI_REQ_STRUCT *param)
{
	decrement_counter();
	return 0;
}

static int cgi_rtc_data(HTTPSRV_CGI_REQ_STRUCT *param)
{
    HTTPSRV_CGI_RES_STRUCT response;
    static bool    init_buff = true;

    uint32_t length = 0;
    uint8_t  buff_cnt = 0;
    // Sensor values --------------------------------------
    performance_data_t         performance_data         = get_performance_data();
    touch_sensitive_data_t     touch_sensitive_data     = get_touch_sensitive_data();
    rtc_data_t                 rtc_data                 = get_rtc_data();
    bool                       meridian                 = false; // false is am
    temperature_data_t         temperature_data         = get_temperature_data();
    control_interface_values_t control_interface_values = get_control_interface_data();

    // ----------------------------------------------------


    if (param->request_method != HTTPSRV_REQ_GET)
    {
        return (0);
    }


    response.ses_handle   = param->ses_handle;
    response.content_type = HTTPSRV_CONTENT_TYPE_PLAIN;
    response.status_code  = HTTPSRV_CODE_OK;
    /*
    ** When the keep-alive is used we have to calculate a correct content length
    ** so the receiver knows when to ACK the data and continue with a next request.
    ** Please see RFC2616 section 4.4 for further details.
    */
    /* Send response */

    response.data           = (uint8_t*)g_buff;
    response.data_length    = BUFF_SIZE;
    response.content_length = response.data_length;
    HTTPSRV_cgi_write(&response);
    return (response.content_length);
}

/* Example Common Gateway Interface callback. */
static int cgi_example(HTTPSRV_CGI_REQ_STRUCT *param)
{
    HTTPSRV_CGI_RES_STRUCT response = {0};

    response.ses_handle  = param->ses_handle;
    response.status_code = HTTPSRV_CODE_OK;

	uint32_t length = 0;
	uint32_t read;
	uint8_t  val;
	char buffer[sizeof("post_input = ") + CGI_DATA_LENGTH_MAX] = {0};

	length = param->content_length;
	read   = HTTPSRV_cgi_read(param->ses_handle, buffer, (length > sizeof(buffer)) ? sizeof(buffer) : length);

	if (read > 0)
	{
		cgi_get_varval(buffer, "slider", cgi_data, sizeof(cgi_data));
		if(9 < read)
		{
			val = string_to_int(cgi_data, 3);
		}
		else
		{
			val = string_to_int(cgi_data, 2);
		}
		set_rpm((uint16_t)val*40);
		//cgi_urldecode(cgi_data);
	}

	/* Write the response using chunked transmission coding. */
	response.content_type = HTTPSRV_CONTENT_TYPE_HTML;
	/* Set content length to -1 to indicate unknown content length. */
	response.content_length = -1;
	response.data           = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\">";
	response.data_length    = strlen(response.data);
	HTTPSRV_cgi_write(&response);
	response.data        = "<html><head><title>POST successfull!</title>";
	response.data_length = strlen(response.data);
	HTTPSRV_cgi_write(&response);
	response.data        = "<meta http-equiv=\"refresh\" content=\"0; url=index.html\"></head><body></body></html>";
	response.data_length = strlen(response.data);
	HTTPSRV_cgi_write(&response);
	response.data_length = 0;
	HTTPSRV_cgi_write(&response);
    return (response.content_length);
}

static bool cgi_get_varval(char *src, char *var_name, char *dst, uint32_t length)
{
    char *name;
    bool result;
    uint32_t index;
    uint32_t n_length;

    result = false;
    dst[0] = 0;
    name   = src;

    n_length = strlen(var_name);

    while ((name != NULL) && ((name = strstr(name, var_name)) != NULL))
    {
        if (name[n_length] == '=')
        {
            name += n_length + 1;

            index = strcspn(name, "&");
            if (index >= length)
            {
                index = length - 1;
            }
            strncpy(dst, name, index);
            dst[index] = '\0';
            result     = true;
            break;
        }
        else
        {
            name = strchr(name, '&');
        }
    }

    return (result);
}

/* Example Server Side Include callback. */
static int ssi_config(HTTPSRV_SSI_PARAM_STRUCT *param)
{
    char *string_value = NULL;
    int int_value      = -1;
    char str[16];

    if (strcmp(param->com_param, "SERVER_STACK_SIZE") == 0)
    {
        int_value = HTTPSRV_CFG_SERVER_STACK_SIZE;
    }
    else if (strcmp(param->com_param, "HTTP_SESSION_STACK_SIZE") == 0)
    {
        int_value = HTTPSRV_CFG_HTTP_SESSION_STACK_SIZE;
    }
    else if (strcmp(param->com_param, "HTTPS_SESSION_STACK_SIZE") == 0)
    {
        int_value = HTTPSRV_CFG_HTTPS_SESSION_STACK_SIZE;
    }
    else if (strcmp(param->com_param, "DEFAULT_PRIORITY") == 0)
    {
        int_value = HTTPSRV_CFG_DEFAULT_PRIORITY;
    }
    else if (strcmp(param->com_param, "DEFAULT_HTTP_PORT") == 0)
    {
        int_value = HTTPSRV_CFG_DEFAULT_HTTP_PORT;
    }
    else if (strcmp(param->com_param, "DEFAULT_HTTPS_PORT") == 0)
    {
        int_value = HTTPSRV_CFG_DEFAULT_HTTPS_PORT;
    }
    else if (strcmp(param->com_param, "DEFAULT_INDEX_PAGE") == 0)
    {
        string_value = HTTPSRV_CFG_DEFAULT_INDEX_PAGE;
    }
    else if (strcmp(param->com_param, "CACHE_MAXAGE") == 0)
    {
        int_value = HTTPSRV_CFG_CACHE_MAXAGE;
    }
    else if (strcmp(param->com_param, "DEFAULT_SES_CNT") == 0)
    {
        int_value = HTTPSRV_CFG_DEFAULT_SES_CNT;
    }
    else if (strcmp(param->com_param, "SES_BUFFER_SIZE") == 0)
    {
        int_value = HTTPSRV_CFG_SES_BUFFER_SIZE;
    }
    else if (strcmp(param->com_param, "DEFAULT_URL_LEN") == 0)
    {
        int_value = HTTPSRV_CFG_DEFAULT_URL_LEN;
    }
    else if (strcmp(param->com_param, "MAX_SCRIPT_LN") == 0)
    {
        int_value = HTTPSRV_CFG_MAX_SCRIPT_LN;
    }
    else if (strcmp(param->com_param, "KEEPALIVE_ENABLED") == 0)
    {
        int_value = HTTPSRV_CFG_KEEPALIVE_ENABLED;
    }
    else if (strcmp(param->com_param, "KEEPALIVE_TIMEOUT") == 0)
    {
        int_value = HTTPSRV_CFG_KEEPALIVE_TIMEOUT;
    }
    else if (strcmp(param->com_param, "SES_TIMEOUT") == 0)
    {
        int_value = HTTPSRV_CFG_SES_TIMEOUT;
    }
    else if (strcmp(param->com_param, "SEND_TIMEOUT") == 0)
    {
        int_value = HTTPSRV_CFG_SEND_TIMEOUT;
    }
    else if (strcmp(param->com_param, "RECEIVE_TIMEOUT") == 0)
    {
        int_value = HTTPSRV_CFG_RECEIVE_TIMEOUT;
    }
    else if (strcmp(param->com_param, "WEBSOCKET_ENABLED") == 0)
    {
        int_value = HTTPSRV_CFG_WEBSOCKET_ENABLED;
    }
    else if (strcmp(param->com_param, "WOLFSSL_ENABLE") == 0)
    {
        int_value = HTTPSRV_CFG_WOLFSSL_ENABLE;
    }
    else if (strcmp(param->com_param, "MBEDTLS_ENABLE") == 0)
    {
        int_value = HTTPSRV_CFG_MBEDTLS_ENABLE;
    }

    if (string_value != NULL)
    {
        HTTPSRV_ssi_write(param->ses_handle, string_value, strlen(string_value));
    }
    else
    {
        sprintf(str, "%d", int_value);
        HTTPSRV_ssi_write(param->ses_handle, str, strlen(str));
    }

    return (0);
}

/* Decode URL encoded string in place. */
static void cgi_urldecode(char *url)
{
    char *src = url;
    char *dst = url;

    while (*src != '\0')
    {
        if ((*src == '%') && (isxdigit((unsigned char)*(src + 1))) && (isxdigit((unsigned char)*(src + 2))))
        {
            *src       = *(src + 1);
            *(src + 1) = *(src + 2);
            *(src + 2) = '\0';
            *dst++     = strtol(src, NULL, 16);
            src += 3;
        }
        else
        {
            *dst++ = *src++;
        }
    }
    *dst = '\0';
}

#if HTTPSRV_CFG_WEBSOCKET_ENABLED
/*
 * Echo plugin code - simple plugin which echoes any message it receives back to
 * client.
 */
uint32_t ws_echo_connect(void *param, WS_USER_CONTEXT_STRUCT context)
{
#if DEBUG_WS
    PRINTF("WebSocket echo client connected.\r\n");
#endif
    return (0);
}

uint32_t ws_echo_disconnect(void *param, WS_USER_CONTEXT_STRUCT context)
{
#if DEBUG_WS
    PRINTF("WebSocket echo client disconnected.\r\n");
#endif
    return (0);
}

uint32_t ws_echo_message(void *param, WS_USER_CONTEXT_STRUCT context)
{
    WS_send(&context); /* Send back what was received.*/
#if DEBUG_WS
    if (context.data.type == WS_DATA_TEXT)
    {
        /* Print received text message to console. */
        context.data.data_ptr[context.data.length] = 0;
        PRINTF("WebSocket message received:\r\n%s\r\n", context.data.data_ptr);
    }
    else
    {
        /* Inform user about binary message. */
        PRINTF("WebSocket binary data with length of %d bytes received.", context.data.length);
    }
#endif

    return (0);
}

uint32_t ws_echo_error(void *param, WS_USER_CONTEXT_STRUCT context)
{
#if DEBUG_WS
    PRINTF("WebSocket error: 0x%X.\r\n", context.error);
#endif
    return (0);
}

WS_PLUGIN_STRUCT ws_tbl[] = {{"/echo", ws_echo_connect, ws_echo_message, ws_echo_error, ws_echo_disconnect, NULL},
                             {0, 0, 0, 0, 0, 0}};
#endif /* HTTPSRV_CFG_WEBSOCKET_ENABLED */

/*!
 * @brief Callback function to generate TXT mDNS record for HTTP service.
 */
static void http_srv_txt(struct mdns_service *service, void *txt_userdata)
{
    mdns_resp_add_service_txtitem(service, "path=/", 6);
}

/*!
 * @brief Configure and enable MDNS service.
 */
void http_server_enable_mdns(struct netif *netif, const char *mdns_hostname)
{
    LOCK_TCPIP_CORE();
    mdns_resp_init();
    mdns_resp_add_netif(netif, mdns_hostname);
    mdns_resp_add_service(netif, mdns_hostname, "_http", DNSSD_PROTO_TCP, 80, http_srv_txt, NULL);
    UNLOCK_TCPIP_CORE();

    (void)strncpy(s_mdns_hostname, mdns_hostname, sizeof(s_mdns_hostname) - 1);
    s_mdns_hostname[sizeof(s_mdns_hostname) - 1] = '\0'; // Make sure string will be always terminated.
}

/*!
 * @brief Initializes server.
 */
void http_server_socket_init(void)
{
    HTTPSRV_PARAM_STRUCT params;
    uint32_t httpsrv_handle;

    /* Init Fs*/
    HTTPSRV_FS_init(httpsrv_fs_data);

    /* Init HTTPSRV parameters.*/
    memset(&params, 0, sizeof(params));
    params.root_dir    = "";
    params.index_page  = "/index.html";
    params.auth_table  = auth_realms;
    params.cgi_lnk_tbl = cgi_lnk_tbl;
    params.ssi_lnk_tbl = ssi_lnk_tbl;
#if HTTPSRV_CFG_WEBSOCKET_ENABLED
    params.ws_tbl = ws_tbl;
#endif

    /* Init HTTP Server.*/
    httpsrv_handle = HTTPSRV_init(&params);
    if (httpsrv_handle == 0)
    {
        PRINTF(("HTTPSRV_init() is Failed"));
    }
}

/*!
 * @brief Prints IP configuration.
 */
void http_server_print_ip_cfg(struct netif *netif)
{
    PRINTF("\r\n************************************************\r\n");
    PRINTF(" HTTP Server example\r\n");
    PRINTF("************************************************\r\n");
    PRINTF(" IPv4 Address     : %s\r\n", ip4addr_ntoa(netif_ip4_addr(netif)));
    PRINTF(" IPv4 Subnet mask : %s\r\n", ip4addr_ntoa(netif_ip4_netmask(netif)));
    PRINTF(" IPv4 Gateway     : %s\r\n", ip4addr_ntoa(netif_ip4_gw(netif)));
    PRINTF(" mDNS hostname    : %s\r\n", s_mdns_hostname);
    PRINTF("************************************************\r\n");
}
