/*
 * Copyright 2019-2021 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "lvgl_support.h"
#include "lvgl.h"
#if defined(SDK_OS_FREE_RTOS)
#include "FreeRTOS.h"
#include "semphr.h"
#endif

#include "board.h"
#include "lvgl_support.h"
#include "fsl_gpio.h"
#include "fsl_debug_console.h"
#include "fsl_lpi2c.h"
#include "fsl_port.h"
#include "fsl_lpflexcomm.h"
#include "lcd_drv.h"


#ifdef LCD_ST7796S_IPS
	#include "fsl_gt911.h"
#else
	#ifdef LCD_ST7796S_TFT
		#include "fsl_gt911.h"
	#else
		#ifdef LCD_SSD1963
			#include "fsl_ft5406.h"
		#endif
	#endif
#endif

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define USE_PCA9420_DRIVE_LCD        1
/* I2C for the touch. */
#define DEMO_TOUCH_I2C               LPI2C2
#define DEMO_TOUCH_I2C_CLOCK_FREQ    CLOCK_GetBusClkFreq()
#define DEMO_I2C_BAUDRATE            100000U

#define LP_FLEXCOMM2_INSTANCE        2U

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void DEMO_FlushDisplay(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p);
static void DEMO_Rounder(lv_disp_drv_t * disp_drv, lv_area_t * area);
static void DEMO_InitTouch(void);
static void DEMO_ReadTouch(lv_indev_drv_t *drv, lv_indev_data_t *data);
static void DEMO_FlushDisplay(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p);
/*******************************************************************************
 * Variables
 ******************************************************************************/
#ifdef LCD_ST7796S_IPS
	gt911_handle_t touchHandle;
#else
	#ifdef LCD_ST7796S_TFT
		gt911_handle_t touchHandle;
	#else
		#ifdef LCD_SSD1963
			ft5406_handle_t touchHandle;
		#endif
	#endif
#endif
/* Touch interrupt occurs. */
volatile bool touchFlag = false;
static volatile bool s_transferDone;
SDK_ALIGN(static uint8_t s_frameBuffer[1][LCD_VIRTUAL_BUF_SIZE * LCD_FB_BYTE_PER_PIXEL], 32);
/*******************************************************************************
 * Code
 ******************************************************************************/

void lv_port_disp_init(void)
{
    static lv_disp_draw_buf_t disp_buf;

    memset(s_frameBuffer, 0, sizeof(s_frameBuffer));
    lv_disp_draw_buf_init(&disp_buf, s_frameBuffer[0], NULL, LCD_VIRTUAL_BUF_SIZE);

    /*-----------------------------------
     * Register the display in LittlevGL
     *----------------------------------*/

    static lv_disp_drv_t disp_drv;      /*Descriptor of a display driver*/
    lv_disp_drv_init(&disp_drv); /*Basic initialization*/

    /*Set the resolution of the display*/
    disp_drv.hor_res = LCD_WIDTH;
    disp_drv.ver_res = LCD_HEIGHT;

    /*Used to copy the buffer's content to the display*/
    disp_drv.flush_cb = DEMO_FlushDisplay;
    disp_drv.rounder_cb = DEMO_Rounder; 
    /*Set a display buffer*/
    disp_drv.draw_buf = &disp_buf;

    /*Finally register the driver*/
    lv_disp_drv_register(&disp_drv);
}
static void DEMO_Rounder(lv_disp_drv_t * disp_drv, lv_area_t * area)
{
  /* Update the areas as needed.
   * For example it makes the area to start only on 8th rows and have Nx8 pixel height.*/
   area->x1 &= ~0x1f;
   area->x2 |= 0x1f;
}

static void DEMO_FlushDisplay(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
    const uint8_t *pdata = (const uint8_t *)color_p;

    LCD_FillPicDMA((AreaPoints_t *)area, (uint16_t *)pdata);

    lv_disp_flush_ready(disp_drv);
}

#if 0
status_t PCA9420_Init(ft5406_handle_t *handle, LPI2C_Type *base)
{
    lpi2c_master_transfer_t *xfer = &(handle->xfer);
    status_t status;
    uint8_t mode;

    assert(handle);
    assert(base);

    if (!handle || !base)
    {
        return kStatus_InvalidArgument;
    }

    handle->base = base;

    /* clear transfer structure and buffer */
    memset(xfer, 0, sizeof(*xfer));
    memset(handle->touch_buf, 0, FT5406_TOUCH_DATA_LEN);
#if USE_PCA9420_DRIVE_LCD
    /* init SW2_OUT voltage to 3V3 */
    mode                 = 0X3F;
    xfer->slaveAddress   = 0x61;
    xfer->direction      = kLPI2C_Write;
    xfer->subaddress     = 0x23;
    xfer->subaddressSize = 1;
    xfer->data           = &mode;
    xfer->dataSize       = 1;
    xfer->flags          = kLPI2C_TransferDefaultFlag;
    status = LPI2C_MasterTransferBlocking(handle->base, &handle->xfer);
    
    /* init LDO2_OUT voltage to 1V8 */
    mode                 = 0xC;
    xfer->slaveAddress   = 0x61;
    xfer->direction      = kLPI2C_Write;
    xfer->subaddress     = 0x25;
    xfer->subaddressSize = 1;
    xfer->data           = &mode;
    xfer->dataSize       = 1;
    xfer->flags          = kLPI2C_TransferDefaultFlag;
    status = LPI2C_MasterTransferBlocking(handle->base, &handle->xfer);
#else 
    /* init SW2_OUT voltage to 1V8 */
    mode                 = 0xC;
    xfer->slaveAddress   = 0x61;
    xfer->direction      = kLPI2C_Write;
    xfer->subaddress     = 0x23;
    xfer->subaddressSize = 1;
    xfer->data           = &mode;
    xfer->dataSize       = 1;
    xfer->flags          = kLPI2C_TransferDefaultFlag;
    status = LPI2C_MasterTransferBlocking(handle->base, &handle->xfer);
    
    /* init LDO2_OUT voltage to 3V3 */
    mode                 = 0x39;
    xfer->slaveAddress   = 0x61;
    xfer->direction      = kLPI2C_Write;
    xfer->subaddress     = 0x25;
    xfer->subaddressSize = 1;
    xfer->data           = &mode;
    xfer->dataSize       = 1;
    xfer->flags          = kLPI2C_TransferDefaultFlag;
    status = LPI2C_MasterTransferBlocking(handle->base, &handle->xfer);
#endif    
    return status;
}
#endif

void lv_port_indev_init(void)
{
    static lv_indev_drv_t indev_drv;

    /*------------------
     * Touchpad
     * -----------------*/

    /*Initialize your touchpad */
    DEMO_InitTouch();

    /*Register a touchpad input device*/
    lv_indev_drv_init(&indev_drv);
    indev_drv.type    = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = DEMO_ReadTouch;
    lv_indev_drv_register(&indev_drv);
}

#if 0
void PMIC_Init(void)
{
     lpi2c_master_config_t masterConfig;

    /*
     * masterConfig.enableMaster = true;
     * masterConfig.baudRate_Bps = 100000U;
     * masterConfig.enableTimeout = false;
     */
    LPI2C_MasterGetDefaultConfig(&masterConfig);

    /* Change the default baudrate configuration */
    masterConfig.baudRate_Hz = DEMO_I2C_BAUDRATE;

    LP_FLEXCOMM_Init(LP_FLEXCOMM2_INSTANCE, LP_FLEXCOMM_PERIPH_LPI2C);
    /* Initialize the I2C master peripheral */
    LPI2C_MasterInit(DEMO_TOUCH_I2C, &masterConfig, CLOCK_GetFreq(kCLOCK_Fro12M));

    PCA9420_Init(&touchHandle, DEMO_TOUCH_I2C);
}
#endif

static void DEMO_InitTouch(void)
{
    status_t status;
     lpi2c_master_config_t masterConfig;

    /*
     * masterConfig.enableMaster = true;
     * masterConfig.baudRate_Bps = 100000U;
     * masterConfig.enableTimeout = false;
     */
    LPI2C_MasterGetDefaultConfig(&masterConfig);

    /* Change the default baudrate configuration */
    masterConfig.baudRate_Hz = DEMO_I2C_BAUDRATE;

    LP_FLEXCOMM_Init(LP_FLEXCOMM2_INSTANCE, LP_FLEXCOMM_PERIPH_LPI2C);
    /* Initialize the I2C master peripheral */
    LPI2C_MasterInit(DEMO_TOUCH_I2C, &masterConfig, CLOCK_GetFreq(kCLOCK_Fro12M));

    /* Initialize the touch handle. */
	#ifdef LCD_ST7796S_IPS
    	status = GT911_Init(&touchHandle, DEMO_TOUCH_I2C);
	#else
		#ifdef LCD_ST7796S_TFT
			status = GT911_Init(&touchHandle, DEMO_TOUCH_I2C);
		#else
			#ifdef LCD_SSD1963
				status = FT5406_Init(&touchHandle, DEMO_TOUCH_I2C);
			#endif
		#endif
	#endif

    if (status != kStatus_Success)
    {
        PRINTF("Touch panel init failed\n");
        assert(0);
    }
}

/* Will be called by the library to read the touchpad */
static void DEMO_ReadTouch(lv_indev_drv_t *drv, lv_indev_data_t *data)
{
    touch_event_t touch_event;
    static int touch_x = 0;
    static int touch_y = 0;

    data->state = LV_INDEV_STATE_REL;

	#ifdef LCD_ST7796S_IPS
    	if (kStatus_Success == GT911_GetSingleTouch(&touchHandle, &touch_event, &touch_x, &touch_y))
		{
			if ((touch_event == kTouch_Down) || (touch_event == kTouch_Contact))
			{
				data->state = LV_INDEV_STATE_PR;
			}
		}
	#else
		#ifdef LCD_ST7796S_TFT
			if (kStatus_Success == GT911_GetSingleTouch(&touchHandle, &touch_event, &touch_x, &touch_y))
			{
				if ((touch_event == kTouch_Down) || (touch_event == kTouch_Contact))
				{
					data->state = LV_INDEV_STATE_PR;
				}
			}
		#else
			#ifdef LCD_SSD1963
				if (kStatus_Success == FT5406_GetSingleTouch(&touchHandle, &touch_event, &touch_x, &touch_y))
				{
					if ((touch_event == kTouch_Down) || (touch_event == kTouch_Contact))
					{
						data->state = LV_INDEV_STATE_PR;
					}
				}
			#endif
		#endif
	#endif


    /*Set the last pressed coordinates*/
    data->point.x = touch_x;
    data->point.y = touch_y;
}
