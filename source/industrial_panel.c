/*
 * Copyright (c) 2013 - 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017, 2024 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "FreeRTOS.h"
#include "task.h"


//#if LWIP_SOCKET
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>


#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "pin_mux.h"
#include "lvgl_support.h"
#include "clock_config.h"
#include "board.h"

#include "fsl_lpuart.h"
#include "fsl_debug_console.h"
#include "flexio_8080_drv.h"
#include "fsl_inputmux.h"
#include "string.h"
#include "fsl_clock.h"
#include "fsl_lpi2c.h"
#include "fsl_lpadc.h"
#include "fsl_vref.h"
#include "lvgl.h"
#include "fsl_ft5406.h"
#include "gui_guider.h"
#include "events_init.h"
#include "custom.h"
#include "fsl_p3t1755.h"
#include "lcd_drv.h"
#include "sensors.h"
#include "gui_guider.h"
#include "motor_control.h"
#include "web_uart_interface.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define BOARD_LED_GPIO     BOARD_LED_RED_GPIO
#define BOARD_LED_GPIO_PIN BOARD_LED_RED_GPIO_PIN
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
void enableCache64Memory(void);
/*******************************************************************************
 * Variables
 ******************************************************************************/
static volatile bool s_lvgl_initialized = false;
lv_ui guider_ui;
volatile static bool g_gui_guider_init = false;

/*******************************************************************************
 * Code
 ******************************************************************************/



/*!
 * @brief The main function containing server thread.
 */
static void server_task(void *arg)
{
	uint8_t cnt = 0;
	bool state;
	vTaskSetApplicationTaskTag( NULL, ( void * ) 1);/*Task Tag*/

    while(1)
    {
        state = web_uart_interface_receive();
		if(true == state)
		{
			web_uart_interface_send();
		}
        vTaskDelay(200);
    }
    vTaskDelete(NULL);
}

#if LV_USE_LOG
static void print_cb(const char *buf)
{
    PRINTF("\r%s\n", buf);
}
#endif

static void IdleTask(void *param)
{
	vTaskSetApplicationTaskTag( NULL, ( void * ) 4);/*Task Tag*/
    while (1)
    {
    }
}

static void UpdateSensorTask(void *param)
{
	vTaskSetApplicationTaskTag( NULL, ( void * ) 3);/*Task Tag*/
	while(false == g_gui_guider_init);
    for (;;)
    {
    	update_values();
    	motor_control_task();
        vTaskDelay(40);
    }
}

static void AppTask(void *param)
{
	vTaskSetApplicationTaskTag( NULL, ( void * ) 2);/*Task Tag*/
#if LV_USE_LOG
    lv_log_register_print_cb(print_cb);
#endif

    lv_init();
    lv_port_disp_init();
    lv_port_indev_init();

    s_lvgl_initialized = true;

    setup_ui(&guider_ui);
    events_init(&guider_ui);
    custom_init(&guider_ui);
    g_gui_guider_init = true;
    for (;;)
    {
        lv_task_handler();
        update_screen_values(&guider_ui);
        vTaskDelay(30);
    }
}



/*!
 * @brief Main function
 */
int main(void)
{
    char ch;
	BaseType_t stat;
    vref_config_t vrefConfig;

    /* Init board hardware. */
    CLOCK_SetupClockCtrl(kCLOCK_FRO12MHZ_ENA);
	CLOCK_EnableClock(kCLOCK_InputMux);
    /* attach FRO 12M to FLEXCOMM4 (debug console) */
    CLOCK_SetClkDiv(kCLOCK_DivFlexcom4Clk, 1u);
    CLOCK_AttachClk(BOARD_DEBUG_UART_CLK_ATTACH);
    /* Init I2C2 for Touch, LCD, PMIC*/
    CLOCK_AttachClk(kFRO12M_to_FLEXCOMM2);
    CLOCK_EnableClock(kCLOCK_LPFlexComm2);
    CLOCK_EnableClock(kCLOCK_LPI2c2);
    CLOCK_SetClkDiv(kCLOCK_DivFlexcom2Clk, 0u);
    CLOCK_SetClkDiv(kCLOCK_DivFlexcom2Clk, 1u);
    
	/* Attach PLL0 clock to I3C, 150MHz / 6 = 25MHz. */
	CLOCK_SetClkDiv(kCLOCK_DivI3c1FClk, 6U);
	CLOCK_AttachClk(kPLL0_to_I3C1FCLK);

    SYSCON->LPCAC_CTRL &= ~1;   // rocky: enable LPCAC ICache
    SYSCON->NVM_CTRL &= SYSCON->NVM_CTRL & ~(1<<2|1<<4); // enable flash Data cache   

    /* Board pin init */
    CLOCK_EnableClock(kCLOCK_Gpio3);

    // Enable ADC clock --------------------------
    CLOCK_SetClkDiv(kCLOCK_DivAdc0Clk, 1u);
    CLOCK_AttachClk(kFRO_HF_to_ADC0);
    // -------------------------------------------

    // Enable Vref -------------------------------
    SPC0->ACTIVE_CFG1 |= 0x1;
    VREF_GetDefaultConfig(&vrefConfig);
    // Initialize the VREF mode.
    VREF_Init(VREF0, &vrefConfig);
    // Get a 1.8V reference voltage.
    VREF_SetTrim21Val(VREF0, 8U);
    // -------------------------------------------


    BOARD_InitPins();
    BOARD_PowerMode_OD();
    BOARD_InitBootClocks();
    BOARD_InitDebugConsole();
    BOARD_InitSensors();
    BOARD_InitRTC();
    P3T1755_InitTemperature();
    enableCache64Memory();
    web_uart_interface_init();


    LED_RED_INIT(LOGIC_LED_OFF);
    LED_BLUE_INIT(LOGIC_LED_OFF);

    PRINTF("##############################################\n\r");
    PRINTF("#   FRDM-MCXN947 INDUSTRIAL PANEL DEMO WIFI  #\n\r");
    PRINTF("#                 version 1.0                #\n\r");
    PRINTF("#               www.nxp.com/MCX              #\n\r");
    PRINTF("##############################################\n\r");
	
	 /* Init FlexIO for this demo. */
    Demo_FLEXIO_8080_Init();    

	LCD_ST7796S_IPS_Init();

    BOARD_InitTimer();
    motor_control_init();

    /* create server task in RTOS */
    if (xTaskCreate(server_task, "server", configMINIMAL_STACK_SIZE + 50, NULL, tskIDLE_PRIORITY + 2, NULL) != pdPASS)
    {
        PRINTF("main(): Task creation failed.", 0);
        __BKPT(0);
    }

    if (xTaskCreate(AppTask, "lvgl", configMINIMAL_STACK_SIZE + 1500, NULL, tskIDLE_PRIORITY + 3, NULL) != pdPASS)
    {
        PRINTF("Failed to create lvgl task");
        while (1)
            ;
    }

    if(xTaskCreate(UpdateSensorTask, "sensor", configMINIMAL_STACK_SIZE + 400, NULL, tskIDLE_PRIORITY+1, NULL) != pdPASS)
    {
    	 PRINTF("Failed to create sensor task");
    }

    if(xTaskCreate(IdleTask, "idle", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL) != pdPASS)
    {
    	 PRINTF("Failed to create idle task");
    }

    vTaskStartScheduler();

    for (;;)
    {
    } /* should never get here */
}
/*!
 * @brief Malloc failed hook.
 */
void vApplicationMallocFailedHook(void)
{
    for (;;)
        ;
}

/*!
 * @brief FreeRTOS tick hook.
 */
void vApplicationTickHook(void)
{
    if (s_lvgl_initialized)
    {
        lv_tick_inc(1);
    }
}

/*!
 * @brief Stack overflow hook.
 */
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    (void)pcTaskName;
    (void)xTask;

    for (;;)
        ;
}

void enableCache64Memory(void)
{
    /* Make sure the FlexSPI clock is enabled. This is required to access the cache64
     * registers. If running from FlexSPI the clock should already be enabled anyway.
     */
    SYSCON->AHBCLKCTRLSET[0] = SYSCON_AHBCLKCTRL0_FLEXSPI_MASK;

    /* Configure the CACHE64_POLSEL */
    CACHE64_POLSEL0->REG0_TOP = 0x7FFC00;   /* configure reg0 to cover the whole FlexSPI */
    CACHE64_POLSEL0->POLSEL = ( CACHE64_POLSEL_POLSEL_REG0_POLICY(1)  // region 0 = write-through
                               | CACHE64_POLSEL_POLSEL_REG1_POLICY(3)  // region 1 = invalid
                               | CACHE64_POLSEL_POLSEL_REG2_POLICY(3) );  // region 2 = invalid


    /* Configure the CACHE64_CTRL */
    CACHE64_CTRL0->CCR = (CACHE64_CTRL_CCR_INVW0_MASK       /* Set invaliate bits for both ways */
                          | CACHE64_CTRL_CCR_INVW1_MASK
                          | CACHE64_CTRL_CCR_GO_MASK        /* Set GO bit to start the invalidate */
                          | CACHE64_CTRL_CCR_ENWRBUF_MASK   /* Enable write buffer - shouldn't matter because we aren't writing */
                          | CACHE64_CTRL_CCR_ENCACHE_MASK );  /* Enable the cache */
}
