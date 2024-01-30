/*
 * Copyright (c) 2015-2016, Freescale Semiconductor, Inc.
 * Copyright 2024 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __FSL_LCD_DRV_H__
#define __FSL_LCD_DRV_H__

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "fsl_common.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
//#define LCD_ST7796S_TFT
#define LCD_ST7796S_IPS
//#define LCD_SSD1963

#define LCD_PIXEL_BYTES             2U
#define LCD_H_POINTS               480U
#define LCD_V_POINTS               320U
#define LCD_FRAME_PIXELS            (LCD_H_POINTS * LCD_V_POINTS)
#define LCD_FRAME_BYTES             (LCD_FRAME_PIXELS * LCD_PIXEL_BYTES)

/* Colors definitions */
#define White   0xFFFF
#define Black   0x0000
#define Red     0xF800
#define Green   0x07E0
#define Blue    0x001F

#define RGB888to565(color) ((((color) >> 19) & 0x1f) << 11) \
                        |((((color) >> 10) & 0x3f) << 5) \
                        |(((color) >> 3) & 0x1f)

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
typedef struct _AreaPoints_t
{
    uint16_t x1;
    uint16_t y1;
    uint16_t x2;
    uint16_t y2;
}AreaPoints_t;

/*******************************************************************************
 * Functions
 ******************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * Delay for some time.
 */
static void Delay(uint32_t t);

/*!
 * Initialize the LCD module.
 */
void LCD_Init(void);
void LCD_SSD1963_Init(void);
void LCD_ST7796S_TFT_Init(void);

void LCD_ST7796S_IPS_Init(void);




/*!
 * Set the filling area.
 */
void LCD_FillPicDMA( AreaPoints_t * area,  uint16_t * pic);

void LCD_SetWindow(const AreaPoints_t * area);

void LCD_FillColorPolling(const AreaPoints_t * area, const uint16_t color);

void LCD_FillColorDMA(const AreaPoints_t * area, const uint16_t color);

void LCD_FillPicPolling(const AreaPoints_t * area, const uint16_t * pic);


void LCD_FillColorWhole(const uint16_t color);

void LCD_FillPicWhole(const uint16_t *pic);




#if defined(__cplusplus)
}
#endif

#endif

/* EOF */
