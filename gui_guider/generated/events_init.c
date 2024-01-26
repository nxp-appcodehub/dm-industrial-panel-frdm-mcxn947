/*
 * Copyright 2023-2024 NXP
 * SPDX-License-Identifier: MIT
 * The auto-generated can only be used on NXP devices
 */

#include "events_init.h"
#include <stdio.h>
#include "lvgl.h"
#include "lcd_drv.h"
#include "sensors.h"

extern lv_ui guider_ui;

void events_init(lv_ui *ui)
{
}

void video_play(lv_ui *ui)
{

}

static void screen_StationButton1_event_handler(lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);
	switch (code)
	{
	case LV_EVENT_PRESSED:
	{
		set_actual_window(1);
		lv_obj_set_tile(guider_ui.screen_StationsView, guider_ui.screen_StationsView_StationTileview1, LV_ANIM_OFF);
		//lv_obj_set_tile_id(guider_ui.screen_StationsView, 0, 0, true);
	}
		break;
	default:
		break;
	}
}

static void screen_StationButton2_event_handler(lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);
	switch (code)
	{
	case LV_EVENT_PRESSED:
	{
		set_actual_window(2);
		lv_obj_set_tile(guider_ui.screen_StationsView, guider_ui.screen_StationsView_StationTileview2, LV_ANIM_OFF);
		//lv_obj_set_tile_id(guider_ui.screen_StationsView, 1, 0, true);
	}
		break;
	default:
		break;
	}
}

static void screen_StationButton3_event_handler(lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);
	switch (code)
	{
	case LV_EVENT_PRESSED:
	{
		set_actual_window(3);
		lv_obj_set_tile(guider_ui.screen_StationsView, guider_ui.screen_StationsView_StationTileview3, LV_ANIM_OFF);
		//lv_obj_set_tile_id(guider_ui.screen_StationsView, 2, 0, true);
	}
		break;
	default:
		break;
	}
}

static void screen_StationButton4_event_handler(lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);
	switch (code)
	{
	case LV_EVENT_PRESSED:
	{
		set_actual_window(4);
		lv_obj_set_tile(guider_ui.screen_StationsView, guider_ui.screen_StationsView_StationTileview4, LV_ANIM_OFF);
		//lv_obj_set_tile_id(guider_ui.screen_StationsView, 3, 0, true);
	}
		break;
	default:
		break;
	}
}

static void screen_MinusButton_event_handler(lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);
	switch (code)
	{
		case LV_EVENT_PRESSED:
		{
			decrement_counter();
		}
		break;

		default:

		break;
	}
}

static void screen_PlusButton_event_handler(lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);
	switch (code)
	{
		case LV_EVENT_PRESSED:
		{
			increment_counter();
		}
		break;

		default:

		break;
	}
}

static void screen_SliderControl_event_handler(lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);
	switch (code)
	{
	case LV_EVENT_VALUE_CHANGED:
	{
	}
		break;
	default:
		break;
	}
}

void events_init_screen(lv_ui *ui)
{
	lv_obj_add_event_cb(ui->screen_StationButton1, screen_StationButton1_event_handler, LV_EVENT_PRESSED, ui);
	lv_obj_add_event_cb(ui->screen_StationButton2, screen_StationButton2_event_handler, LV_EVENT_PRESSED, ui);
	lv_obj_add_event_cb(ui->screen_StationButton3, screen_StationButton3_event_handler, LV_EVENT_PRESSED, ui);
	lv_obj_add_event_cb(ui->screen_StationButton4, screen_StationButton4_event_handler, LV_EVENT_PRESSED, ui);
	lv_obj_add_event_cb(ui->screen_MinusButton, screen_MinusButton_event_handler, LV_EVENT_PRESSED, ui);
	lv_obj_add_event_cb(ui->screen_PlusButton, screen_PlusButton_event_handler, LV_EVENT_PRESSED, ui);
	lv_obj_add_event_cb(ui->screen_SliderControl, screen_SliderControl_event_handler, LV_EVENT_PRESSED, ui);
}
