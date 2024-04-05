/*
* Copyright 2024 NXP
* NXP Confidential and Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#ifndef GUI_GUIDER_H
#define GUI_GUIDER_H
#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"

typedef struct
{
  
	lv_obj_t *screen;
	bool screen_del;
	lv_obj_t *screen_img_2;
	lv_obj_t *screen_Date;
	lv_obj_t *screen_Clock;
	lv_obj_t *screen_StationsView;
	lv_obj_t *screen_StationsView_StationTileview1;
	lv_obj_t *screen_StationsView_StationTileview2;
	lv_obj_t *screen_StationsView_StationTileview3;
	lv_obj_t *screen_StationsView_StationTileview4;
	lv_obj_t *screen_TittleLabel1;
	lv_obj_t *screen_TSISliderPos;
	lv_obj_t *screen_Position;
	lv_obj_t *screen_Elect1;
	lv_obj_t *screen_E1Value;
	lv_obj_t *screen_TempCelsius;
	lv_meter_indicator_t *screen_TempCelsius_scale_1_ndline_0;
	lv_obj_t *screen_TittleLabel2;
	lv_obj_t *screen_DCelsius;
	lv_obj_t *screen_meter_1;
	lv_meter_indicator_t *screen_meter_1_scale_1_ndline_0;
	lv_obj_t *screen_DFahrenheit;
	lv_obj_t *screen_TittleLabel3;
	lv_obj_t *screen_LVGL;
	lv_obj_t *screen_HttpServer;
	lv_obj_t *screen_Idle;
	lv_obj_t *screen_Other;
	lv_obj_t *screen_LVGLBullet;
	lv_obj_t *screen_IdleBullet;
	lv_obj_t *screen_OtherBullet;
	lv_obj_t *screen_HttpServerBullet;
	lv_obj_t *screen_LVGLLabel;
	lv_obj_t *screen_HttpLabel;
	lv_obj_t *screen_OtherLabel;
	lv_obj_t *screen_IdleLabel;
	lv_obj_t *screen_LVGLPercentage;
	lv_obj_t *screen_ServerPercentage;
	lv_obj_t *screen_OtherPercentage;
	lv_obj_t *screen_IdlePercentage;
	lv_obj_t *screen_tittleLabel4;
	lv_obj_t *screen_CounterArc;
	lv_obj_t *screen_MinusButton;
	lv_obj_t *screen_MinusButton_label;
	lv_obj_t *screen_PlusButton;
	lv_obj_t *screen_PlusButton_label;
	lv_obj_t *screen_CounterLabel;
	lv_obj_t *screen_BarIndicator;
	lv_obj_t *screen_SliderControl;
	lv_obj_t *screen_SliderPosition;
	lv_obj_t *screen_StationButton1;
	lv_obj_t *screen_StationButton1_label;
	lv_obj_t *screen_StationButton2;
	lv_obj_t *screen_StationButton2_label;
	lv_obj_t *screen_StationButton3;
	lv_obj_t *screen_StationButton3_label;
	lv_obj_t *screen_StationButton4;
	lv_obj_t *screen_StationButton4_label;
}lv_ui;

void ui_init_style(lv_style_t * style);
void init_scr_del_flag(lv_ui *ui);
void setup_ui(lv_ui *ui);
extern lv_ui guider_ui;

void update_screen_values(lv_ui *ui);
void setup_scr_screen(lv_ui *ui);
LV_IMG_DECLARE(_NXP_Logo_RGB_Colour_alpha_100x50);
LV_IMG_DECLARE(_Bullet3_alpha_10x10);
LV_IMG_DECLARE(_Bullet1_alpha_10x10);
LV_IMG_DECLARE(_Bullet4_alpha_10x10);
LV_IMG_DECLARE(_Bullet2_alpha_10x10);
LV_IMG_DECLARE(_TouchC_alpha_50x50);
LV_IMG_DECLARE(_TouchB_alpha_50x50);
LV_IMG_DECLARE(_TouchC_alpha_50x50);
LV_IMG_DECLARE(_TouchB_alpha_50x50);
LV_IMG_DECLARE(_TempC_alpha_50x50);
LV_IMG_DECLARE(_TempB_alpha_50x50);
LV_IMG_DECLARE(_TempC_alpha_50x50);
LV_IMG_DECLARE(_TempB_alpha_50x50);
LV_IMG_DECLARE(_Station3c_alpha_50x50);
LV_IMG_DECLARE(_Station3b_alpha_50x50);
LV_IMG_DECLARE(_Station3c_alpha_50x50);
LV_IMG_DECLARE(_Station3b_alpha_50x50);
LV_IMG_DECLARE(_ControlC_alpha_50x50);
LV_IMG_DECLARE(_ControlB_alpha_50x50);
LV_IMG_DECLARE(_ControlC_alpha_50x50);
LV_IMG_DECLARE(_ControlB_alpha_50x50);

LV_FONT_DECLARE(lv_font_montserratMedium_14)
LV_FONT_DECLARE(lv_font_montserratMedium_10)
LV_FONT_DECLARE(lv_font_montserratMedium_20)


#ifdef __cplusplus
}
#endif
#endif
