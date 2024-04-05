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

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "lcd_drv.h"
#include "flexio_8080_drv.h"
#include "stdbool.h"

/*******************************************************************************
 * Variables
 ******************************************************************************/
#pragma data_alignment = 32
uint16_t EmptyFrame[10];
const AreaPoints_t LCDArea = {0U, 0U, LCD_H_POINTS-1U, LCD_V_POINTS-1U};
const AreaPoints_t LCDArea_BG = {0U, 0U, 800-1U, 480-1U};
extern bool DMAFillColorMode;
/*******************************************************************************
 * Functions
 ******************************************************************************/
/*!
 * Delay for some time.
 */
static void Delay(uint32_t t)
{
    for(; t!=0U; t--)
    {
        for(volatile uint32_t j=0U; j<20U; j++)
        {
        }
    }
}



#ifdef LCD_ST7796S_IPS
/*!
 * Initialize the LCD module.
 */
void LCD_ST7796S_IPS_Init(void)
{
    uint8_t Command;
    uint16_t CommandValue[20];

    Command = 0x11;
    FLEXIO_8080_SglBeatWR_nPrm(Command, CommandValue, 0U);
    Delay(1000U);
    
    Command = 0Xf0;
    CommandValue[0U] = 0xc3;
    FLEXIO_8080_SglBeatWR_nPrm(Command, CommandValue, 1U);
    Command = 0Xf0;
    CommandValue[0U] = 0x96;
    FLEXIO_8080_SglBeatWR_nPrm(Command, CommandValue, 1U);
    Command = 0X21;
    FLEXIO_8080_SglBeatWR_nPrm(Command, CommandValue, 0U);
    Command = 0x36;
    CommandValue[0U] = 0x08 | 0x20;
    FLEXIO_8080_SglBeatWR_nPrm(Command, CommandValue, 1U);
    Command = 0x3A;
    CommandValue[0U] = 0x05;
    FLEXIO_8080_SglBeatWR_nPrm(Command, CommandValue, 1U);
    Command = 0XB4;
    CommandValue[0U] = 0x01;
    FLEXIO_8080_SglBeatWR_nPrm(Command, CommandValue, 1U);
    Command = 0XB1;
    CommandValue[0U] = 0x80;
    CommandValue[1U] = 0x10;
    FLEXIO_8080_SglBeatWR_nPrm(Command, CommandValue, 2U); 
    Command = 0XB5;
    CommandValue[0U] = 0x1F;
    CommandValue[1U] = 0x50;
    CommandValue[2U] = 0x00;
    CommandValue[3U] = 0x20;
    FLEXIO_8080_SglBeatWR_nPrm(Command, CommandValue, 4U); 
    Command = 0XB6;
    CommandValue[0U] = 0x8A;
    CommandValue[1U] = 0x07;
    CommandValue[2U] = 0x3B;
    FLEXIO_8080_SglBeatWR_nPrm(Command, CommandValue, 3U); 
    Command = 0XC0;
    CommandValue[0U] = 0x80;
    CommandValue[1U] = 0x64;
    FLEXIO_8080_SglBeatWR_nPrm(Command, CommandValue, 2U);   
    Command = 0Xc1;
    CommandValue[0U] = 0x13;
    FLEXIO_8080_SglBeatWR_nPrm(Command, CommandValue, 1U);
    Command = 0XC2;
    CommandValue[0U] = 0xA7;
    FLEXIO_8080_SglBeatWR_nPrm(Command, CommandValue, 1U);  
    Command = 0Xc5;
    CommandValue[0U] = 0x09;
    FLEXIO_8080_SglBeatWR_nPrm(Command, CommandValue, 1U); 
    
    Command = 0Xe8;
    CommandValue[0U] = 0x40;
    CommandValue[1U] = 0x8A;
    CommandValue[2U] = 0x00;
    CommandValue[3U] = 0x00;
    CommandValue[4U] = 0x29;
    CommandValue[5U] = 0x19;
    CommandValue[6U] = 0xA5;
    CommandValue[7U] = 0x33;
    FLEXIO_8080_SglBeatWR_nPrm(Command, CommandValue, 8U);    
    
    Command = 0Xe0;
    CommandValue[0U] = 0xf0;
    CommandValue[1U] = 0x06;
    CommandValue[2U] = 0x0B;
    CommandValue[3U] = 0x07;
    CommandValue[4U] = 0x06;
    CommandValue[5U] = 0x05;
    CommandValue[6U] = 0x2E;
    CommandValue[7U] = 0x33;
    CommandValue[8U] = 0x46;
    CommandValue[9U] = 0x38;
    CommandValue[10U] = 0x13;
    CommandValue[11U] = 0x13;
    CommandValue[12U] = 0x2C;
    CommandValue[13U] = 0x32;
    FLEXIO_8080_SglBeatWR_nPrm(Command, CommandValue, 14U);    
      
    
    Command = 0Xf0;
    CommandValue[0U] = 0x3c;
    FLEXIO_8080_SglBeatWR_nPrm(Command, CommandValue, 1U); 
    Command = 0Xf0;
    CommandValue[0U] = 0x69;
    FLEXIO_8080_SglBeatWR_nPrm(Command, CommandValue, 1U); 
    Command = 0X29;
    FLEXIO_8080_SglBeatWR_nPrm(Command, CommandValue, 0U);
}

#else
	#ifdef LCD_ST7796S_TFT
	/*!
	 * Initialize the LCD module.
	 */
	void LCD_ST7796S_TFT_Init(void)
	{
		uint8_t Command;
		uint16_t CommandValue[20];

		Command = 0x11;
		FLEXIO_8080_SglBeatWR_nPrm(Command, CommandValue, 0U);
		Delay(1000U);

		Command = 0Xf0;
		CommandValue[0U] = 0xc3;
		FLEXIO_8080_SglBeatWR_nPrm(Command, CommandValue, 1U);
		Command = 0Xf0;
		CommandValue[0U] = 0x96;
		FLEXIO_8080_SglBeatWR_nPrm(Command, CommandValue, 1U);
		Command = 0x36;
		CommandValue[0U] = 0x08 | 0x20;
		FLEXIO_8080_SglBeatWR_nPrm(Command, CommandValue, 1U);
		Command = 0x3A;
		CommandValue[0U] = 0x05;
		FLEXIO_8080_SglBeatWR_nPrm(Command, CommandValue, 1U);

		Command = 0Xe8;
		CommandValue[0U] = 0x40;
		CommandValue[1U] = 0x82;
		CommandValue[2U] = 0x07;
		CommandValue[3U] = 0x18;
		CommandValue[4U] = 0x27;
		CommandValue[5U] = 0x0a;
		CommandValue[6U] = 0xb6;
		CommandValue[7U] = 0x33;
		FLEXIO_8080_SglBeatWR_nPrm(Command, CommandValue, 8U);

		Command = 0Xc5;
		CommandValue[0U] = 0x27;
		FLEXIO_8080_SglBeatWR_nPrm(Command, CommandValue, 1U);
		Command = 0Xc2;
		CommandValue[0U] = 0xa7;
		FLEXIO_8080_SglBeatWR_nPrm(Command, CommandValue, 1U);
		Command = 0Xe0;
		CommandValue[0U] = 0xf0;
		CommandValue[1U] = 0x01;
		CommandValue[2U] = 0x06;
		CommandValue[3U] = 0x0f;
		CommandValue[4U] = 0x12;
		CommandValue[5U] = 0x1d;
		CommandValue[6U] = 0x36;
		CommandValue[7U] = 0x54;
		CommandValue[8U] = 0x44;
		CommandValue[9U] = 0x0c;
		CommandValue[10U] = 0x18;
		CommandValue[11U] = 0x16;
		CommandValue[12U] = 0x13;
		CommandValue[13U] = 0x15;
		FLEXIO_8080_SglBeatWR_nPrm(Command, CommandValue, 14U);

		Command = 0Xe1;
		CommandValue[0U] = 0xf0;
		CommandValue[1U] = 0x01;
		CommandValue[2U] = 0x05;
		CommandValue[3U] = 0x0a;
		CommandValue[4U] = 0x0b;
		CommandValue[5U] = 0x07;
		CommandValue[6U] = 0x32;
		CommandValue[7U] = 0x44;
		CommandValue[8U] = 0x44;
		CommandValue[9U] = 0x0c;
		CommandValue[10U] = 0x18;
		CommandValue[11U] = 0x17;
		CommandValue[12U] = 0x13;
		CommandValue[13U] = 0x16;
		FLEXIO_8080_SglBeatWR_nPrm(Command, CommandValue, 14U);


		Command = 0Xf0;
		CommandValue[0U] = 0x3c;
		FLEXIO_8080_SglBeatWR_nPrm(Command, CommandValue, 1U);
		Command = 0Xf0;
		CommandValue[0U] = 0x69;
		FLEXIO_8080_SglBeatWR_nPrm(Command, CommandValue, 1U);

		Command = 0X29;
		FLEXIO_8080_SglBeatWR_nPrm(Command, CommandValue, 0U);
	}
	#else
		#ifdef LCD_SSD1963
		#define SSD_HOR_RESOLUTION		800
		#define SSD_VER_RESOLUTION		480
		#define SSD_HOR_PULSE_WIDTH		1
		#define SSD_HOR_BACK_PORCH		46
		#define SSD_HOR_FRONT_PORCH		210

		#define SSD_VER_PULSE_WIDTH		1
		#define SSD_VER_BACK_PORCH		23
		#define SSD_VER_FRONT_PORCH		22

		#define SSD_HT	(SSD_HOR_RESOLUTION+SSD_HOR_BACK_PORCH+SSD_HOR_FRONT_PORCH)
		#define SSD_HPS	(SSD_HOR_BACK_PORCH)
		#define SSD_VT 	(SSD_VER_RESOLUTION+SSD_VER_BACK_PORCH+SSD_VER_FRONT_PORCH)
		#define SSD_VPS (SSD_VER_BACK_PORCH)
		/*!
		 * Initialize the LCD module.
		 */
		void LCD_SSD1963_Init(void)
		{
			uint8_t Command;
			uint16_t CommandValue[20];

			Command = 0xE2;
			CommandValue[0U] = 0x1D;
			CommandValue[1U] = 0x02;
			CommandValue[2U] = 0x04;
			FLEXIO_8080_SglBeatWR_nPrm(Command, CommandValue, 3U);
			Delay(100U);

			Command = 0xE0;
			CommandValue[0U] = 0x01;
			FLEXIO_8080_SglBeatWR_nPrm(Command, CommandValue, 1U);
			Delay(100U);

			Command = 0xE0;
			CommandValue[0U] = 0x03;
			FLEXIO_8080_SglBeatWR_nPrm(Command, CommandValue, 1U);
			Delay(100U);

			Command = 0x01;
			FLEXIO_8080_SglBeatWR_nPrm(Command, CommandValue, 0U);
			Delay(100U);

			Command = 0xE6;
			CommandValue[0U] = 0x2F;
			CommandValue[1U] = 0xFF;
			CommandValue[2U] = 0xFF;
			FLEXIO_8080_SglBeatWR_nPrm(Command, CommandValue, 3U);

			Command = 0xB0;
			CommandValue[0U] = 0x20;
			CommandValue[1U] = 0x00;
			CommandValue[2U] = (SSD_HOR_RESOLUTION - 1) >> 8;
			CommandValue[3U] = SSD_HOR_RESOLUTION - 1;
			CommandValue[4U] = (SSD_VER_RESOLUTION - 1) >> 8;
			CommandValue[5U] = SSD_VER_RESOLUTION - 1;
			CommandValue[6U] = 0x00;
			FLEXIO_8080_SglBeatWR_nPrm(Command, CommandValue, 7U);

			Command = 0xB4;
			CommandValue[0U] = (SSD_HT - 1) >> 8;
			CommandValue[1U] = SSD_HT - 1;
			CommandValue[2U] = SSD_HPS >> 8;
			CommandValue[3U] = SSD_HPS;
			CommandValue[4U] = SSD_HOR_PULSE_WIDTH - 1;
			CommandValue[5U] = 0x00;
			CommandValue[6U] = 0x00;
			CommandValue[7U] = 0x00;
			FLEXIO_8080_SglBeatWR_nPrm(Command, CommandValue, 8U);

			Command = 0xB6;
			CommandValue[0U] = (SSD_VT - 1) >> 8;
			CommandValue[1U] = SSD_VT - 1;
			CommandValue[2U] = SSD_VPS >> 8;
			CommandValue[3U] = SSD_VPS;
			CommandValue[4U] = SSD_VER_FRONT_PORCH - 1;
			CommandValue[5U] = 0x00;
			CommandValue[6U] = 0x00;
			FLEXIO_8080_SglBeatWR_nPrm(Command, CommandValue, 7U);

			Command = 0xF0;
			CommandValue[0U] = 0x03;
			FLEXIO_8080_SglBeatWR_nPrm(Command, CommandValue, 1U);

			Command = 0x29;
			FLEXIO_8080_SglBeatWR_nPrm(Command, CommandValue, 0U);

			Command = 0xD0;
			CommandValue[0U] = 0x00;
			FLEXIO_8080_SglBeatWR_nPrm(Command, CommandValue, 1U);

			Command = 0xBE;
			CommandValue[0U] = 0x05;
			CommandValue[1U] = 0xFF;
			CommandValue[2U] = 0x01;
			CommandValue[3U] = 0x00;
			CommandValue[4U] = 0x00;
			CommandValue[5U] = 0x00;
			FLEXIO_8080_SglBeatWR_nPrm(Command, CommandValue, 6U);

			Command = 0xB8;
			CommandValue[0U] = 0x03;
			CommandValue[1U] = 0x01;
			FLEXIO_8080_SglBeatWR_nPrm(Command, CommandValue, 2U);

			Command = 0xBA;
			CommandValue[0U] = 0X01;
			FLEXIO_8080_SglBeatWR_nPrm(Command, CommandValue, 1U);

			Delay(100U);
		}
		#endif
	#endif
#endif
/*!
 * Set the filling area.
 */
void LCD_SetWindow(const AreaPoints_t * area)
{
    uint8_t Command;
    uint16_t CommandValue[4];

    Command = 0x2A;
    CommandValue[0U] = area->x1 >> 8U;
    CommandValue[1U] = area->x1 & 0xFF;
    CommandValue[2U] = area->x2 >> 8U;
    CommandValue[3U] = area->x2 & 0xFF;
    FLEXIO_8080_SglBeatWR_nPrm(Command, CommandValue, 4U);

    Command = 0x2B;
    CommandValue[0U] = area->y1 >> 8U;
    CommandValue[1U] = area->y1 & 0xFF;
    CommandValue[2U] = area->y2 >> 8U;
    CommandValue[3U] = area->y2 & 0xFF;
    FLEXIO_8080_SglBeatWR_nPrm(Command, CommandValue, 4U);
}

void LCD_FillColorPolling(const AreaPoints_t * area, const uint16_t color)
{
    uint32_t points = (area->x2-area->x1+1) * (area->y2-area->y1+1);
    LCD_SetWindow(area);
    FLEXIO_8080_SglBeatWR_nSamePrm(0x2C, color, points);
}

void LCD_FillColorWhole(const uint16_t color)
{
#if 1
    /* Use multi-beat and DMA to fill color. */
    LCD_FillColorDMA(&LCDArea_BG, color);
#else
    /* Use single-beat and polling to fill color */
    LCD_FillColorPolling(&LCDArea_BG, color);
#endif
}

void LCD_FillColorDMA(const AreaPoints_t * area, const uint16_t color)
{
    uint32_t transfers = (area->x2-area->x1+1) * (area->y2-area->y1+1);

    LCD_SetWindow(area);

    EmptyFrame[0] = color;
    EmptyFrame[1] = color;
    EmptyFrame[2] = color;
    EmptyFrame[3] = color;
    EmptyFrame[4] = color;
    EmptyFrame[5] = color;
    EmptyFrame[6] = color;
    EmptyFrame[7] = color;

    DMAFillColorMode = true;

    FLEXIO_8080_MulBeatWR_nPrm(0x2C, EmptyFrame, transfers);
    while(WR_DMATransferDone == false)
    {
        /* You can add some necessary statements here.*/
    }
    DMAFillColorMode = false;
}

void LCD_FillPicDMA( AreaPoints_t * area,  uint16_t * pic)
{
    uint32_t transfers = (area->x2-area->x1 + 1) * (area->y2-area->y1 + 1);
    LCD_SetWindow(area);

    FLEXIO_8080_MulBeatWR_nPrm(0x2C, pic, transfers);
    while(WR_DMATransferDone == false)
    {
        /* You can add some necessary statements here.*/
    }
}

/* EOF */
