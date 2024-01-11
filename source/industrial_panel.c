/*
 * Copyright (c) 2013 - 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017, 2024 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "FreeRTOS.h"
#include "task.h"

#include "lwip/opt.h"

//#if LWIP_SOCKET
#include <stdio.h>

#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>

#include "ethernetif.h"

#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "pin_mux.h"
#include "lvgl_support.h"
#include "clock_config.h"
#include "board.h"
#ifndef configMAC_ADDR
#include "fsl_silicon_id.h"
#endif
#include "fsl_phy.h"

#include "lwip/netif.h"
#include "lwip/sys.h"
#include "lwip/arch.h"
#include "lwip/api.h"
#include "lwip/tcpip.h"
#include "lwip/ip.h"
#include "lwip/netifapi.h"
#include "lwip/sockets.h"
#include "netif/etharp.h"

#include "httpsrv.h"
#include "httpsrv_freertos.h"
#include "lwip/apps/mdns.h"

#include "fsl_phylan8741.h"
#include "fsl_enet.h"

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
#include "motor_control.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/


/* IP address configuration. */
#ifndef configIP_ADDR0
#define configIP_ADDR0 192
#endif
#ifndef configIP_ADDR1
#define configIP_ADDR1 168
#endif
#ifndef configIP_ADDR2
#define configIP_ADDR2 0
#endif
#ifndef configIP_ADDR3
#define configIP_ADDR3 102
#endif

/* Netmask configuration. */
#ifndef configNET_MASK0
#define configNET_MASK0 255
#endif
#ifndef configNET_MASK1
#define configNET_MASK1 255
#endif
#ifndef configNET_MASK2
#define configNET_MASK2 255
#endif
#ifndef configNET_MASK3
#define configNET_MASK3 0
#endif

/* Gateway address configuration. */
#ifndef configGW_ADDR0
#define configGW_ADDR0 192
#endif
#ifndef configGW_ADDR1
#define configGW_ADDR1 168
#endif
#ifndef configGW_ADDR2
#define configGW_ADDR2 0
#endif
#ifndef configGW_ADDR3
#define configGW_ADDR3 100
#endif

/* Ethernet configuration. */
extern phy_lan8741_resource_t g_phy_resource;
#define EXAMPLE_ENET_BASE    ENET0
#define EXAMPLE_PHY_ADDRESS  BOARD_ENET0_PHY_ADDRESS
#define EXAMPLE_PHY_OPS      &phylan8741_ops
#define EXAMPLE_PHY_RESOURCE &g_phy_resource
#define EXAMPLE_CLOCK_FREQ   (50000000U)

#ifndef MDNS_HOSTNAME
#define MDNS_HOSTNAME "lwip-http"
#endif

#ifndef EXAMPLE_NETIF_INIT_FN
/*! @brief Network interface initialization function. */
#define EXAMPLE_NETIF_INIT_FN ethernetif0_init
#endif /* EXAMPLE_NETIF_INIT_FN */

#ifndef HTTPD_STACKSIZE
#define HTTPD_STACKSIZE DEFAULT_THREAD_STACKSIZE
#endif

#ifndef HTTPD_PRIORITY
#define HTTPD_PRIORITY DEFAULT_THREAD_PRIO
#endif   

#define BOARD_LED_GPIO     BOARD_LED_RED_GPIO
#define BOARD_LED_GPIO_PIN BOARD_LED_RED_GPIO_PIN
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
void http_server_socket_init(void);
void enableCache64Memory(void);
/*******************************************************************************
 * Variables
 ******************************************************************************/
phy_lan8741_resource_t g_phy_resource;

static phy_handle_t phyHandle;

static struct netif netif;
static volatile bool s_lvgl_initialized = false;
lv_ui guider_ui;
volatile static bool g_gui_guider_init = false;

/*******************************************************************************
 * Code
 ******************************************************************************/

static void MDIO_Init(void)
{
    (void)CLOCK_EnableClock(s_enetClock[ENET_GetInstance(EXAMPLE_ENET_BASE)]);
    ENET_SetSMI(EXAMPLE_ENET_BASE, CLOCK_GetCoreSysClkFreq());
}

static status_t MDIO_Write(uint8_t phyAddr, uint8_t regAddr, uint16_t data)
{
    return ENET_MDIOWrite(EXAMPLE_ENET_BASE, phyAddr, regAddr, data);
}

static status_t MDIO_Read(uint8_t phyAddr, uint8_t regAddr, uint16_t *pData)
{
    return ENET_MDIORead(EXAMPLE_ENET_BASE, phyAddr, regAddr, pData);
}


/*!
 * @brief Initializes lwIP stack.
 */
static void stack_init(void)
{
    ip4_addr_t netif_ipaddr, netif_netmask, netif_gw;
    ethernetif_config_t enet_config = {.phyHandle   = &phyHandle,
                                       .phyAddr     = EXAMPLE_PHY_ADDRESS,
                                       .phyOps      = EXAMPLE_PHY_OPS,
                                       .phyResource = EXAMPLE_PHY_RESOURCE,
                                       .srcClockHz  = EXAMPLE_CLOCK_FREQ,
#ifdef configMAC_ADDR
                                       .macAddress = configMAC_ADDR
#endif
    };

    tcpip_init(NULL, NULL);

    /* Set MAC address. */
#ifndef configMAC_ADDR
    (void)SILICONID_ConvertToMacAddr(&enet_config.macAddress);
#endif

    IP4_ADDR(&netif_ipaddr, configIP_ADDR0, configIP_ADDR1, configIP_ADDR2, configIP_ADDR3);
    IP4_ADDR(&netif_netmask, configNET_MASK0, configNET_MASK1, configNET_MASK2, configNET_MASK3);
    IP4_ADDR(&netif_gw, configGW_ADDR0, configGW_ADDR1, configGW_ADDR2, configGW_ADDR3);

    netifapi_netif_add(&netif, &netif_ipaddr, &netif_netmask, &netif_gw, &enet_config, EXAMPLE_NETIF_INIT_FN,
                       tcpip_input);
    netifapi_netif_set_default(&netif);
    netifapi_netif_set_up(&netif);

    while (ethernetif_wait_linkup(&netif, 5000) != ERR_OK)
    {
        PRINTF("PHY Auto-negotiation failed. Please check the cable connection and link partner setting.\r\n");
    }

    http_server_enable_mdns(&netif, MDNS_HOSTNAME);

    http_server_print_ip_cfg(&netif);
}

/*!
 * @brief The main function containing server thread.
 */
static void main_task(void *arg)
{
	vTaskSetApplicationTaskTag( NULL, ( void * ) 1);/*Task Tag*/
    LWIP_UNUSED_ARG(arg);

    stack_init();
    http_server_socket_init();
    while(1)
    {
        update_server_sensors();
        vTaskDelay(40);
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
        vTaskDelay(40);
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


    LED_RED_INIT(LOGIC_LED_OFF);
    LED_BLUE_INIT(LOGIC_LED_OFF);

    PRINTF("#########################################\n\r");
    PRINTF("#   FRDM-MCXN947 INDUSTRIAL PANEL DEMO  #\n\r");
    PRINTF("#              version 1.0              #\n\r");
    PRINTF("#            www.nxp.com/MCX            #\n\r");
    PRINTF("#########################################\n\r");
	
	   /* Init FlexIO for this demo. */
    Demo_FLEXIO_8080_Init();    


	/*NXP low cost LCD module*/
#ifdef LCD_ST7796S_IPS
	LCD_ST7796S_IPS_Init();
#else
	#ifdef LCD_ST7796S_TFT
		LCD_ST7796S_TFT_Init();
	#else
		#ifdef LCD_SSD1963
			LCD_SSD1963_Init();
		#endif
	#endif
#endif

    CLOCK_AttachClk(MUX_A(CM_ENETRMIICLKSEL, 0));
    CLOCK_EnableClock(kCLOCK_Enet);
    SYSCON0->PRESETCTRL2 = SYSCON_PRESETCTRL2_ENET_RST_MASK;
    SYSCON0->PRESETCTRL2 &= ~SYSCON_PRESETCTRL2_ENET_RST_MASK;

    MDIO_Init();

    BOARD_InitTimer();
    motor_control_init();

    g_phy_resource.read  = MDIO_Read;
    g_phy_resource.write = MDIO_Write;
    /* create server task in RTOS */
    if (xTaskCreate(main_task, "main", HTTPD_STACKSIZE, NULL, HTTPD_PRIORITY, NULL) != pdPASS)
    {
        PRINTF("main(): Task creation failed.", 0);
        __BKPT(0);
    }
 
    stat = xTaskCreate(AppTask, "lvgl", configMINIMAL_STACK_SIZE + 1500, NULL, tskIDLE_PRIORITY + 4, NULL);

    if (pdPASS != stat)
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
