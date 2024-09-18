/*
 * Copyright 2022 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdint.h>
#include "fsl_common.h"
#include "fsl_debug_console.h"
#include "board.h"
#if defined(SDK_I2C_BASED_COMPONENT_USED) && SDK_I2C_BASED_COMPONENT_USED
#include "fsl_lpi2c.h"
#endif /* SDK_I2C_BASED_COMPONENT_USED */
#if defined(LPFLEXCOMM_INIT_NOT_USED_IN_DRIVER) && LPFLEXCOMM_INIT_NOT_USED_IN_DRIVER
#include "fsl_lpflexcomm.h"
#endif /* LPFLEXCOMM_INIT_NOT_USED_IN_DRIVER */
#include "fsl_spc.h"
#include "fsl_lptmr.h"
#include "fsl_irtc.h"

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
/* Initialize debug console. */

volatile static lpadc_conv_command_config_t g_LpadcCommandConfigStruct; /* Structure to configure conversion command. */
volatile static lpadc_conv_command_config_t g_LpadcCommandConfigStruct2; /* Structure to configure conversion command. */

void BOARD_InitDebugConsole(void)
{
    /* attach 12 MHz clock to FLEXCOMM0 (debug console) */
    CLOCK_AttachClk(BOARD_DEBUG_UART_CLK_ATTACH);

    RESET_ClearPeripheralReset(BOARD_DEBUG_UART_RST);

    uint32_t uartClkSrcFreq = BOARD_DEBUG_UART_CLK_FREQ;

#if defined(LPFLEXCOMM_INIT_NOT_USED_IN_DRIVER) && LPFLEXCOMM_INIT_NOT_USED_IN_DRIVER
    LP_FLEXCOMM_Init(BOARD_DEBUG_UART_INSTANCE, LP_FLEXCOMM_PERIPH_LPUART);
#endif /* LPFLEXCOMM_INIT_NOT_USED_IN_DRIVER */

    DbgConsole_Init(BOARD_DEBUG_UART_INSTANCE, BOARD_DEBUG_UART_BAUDRATE, BOARD_DEBUG_UART_TYPE, uartClkSrcFreq);
}

void BOARD_InitDebugConsole_Core1(void)
{
    /* attach 12 MHz clock to FLEXCOMM1 (debug console) */
    //    CLOCK_AttachClk(BOARD_DEBUG_UART_CLK_ATTACH_CORE1);

    RESET_ClearPeripheralReset(BOARD_DEBUG_UART_RST_CORE1);

    uint32_t uartClkSrcFreq = BOARD_DEBUG_UART_CLK_FREQ_CORE1;

    DbgConsole_Init(BOARD_DEBUG_UART_INSTANCE_CORE1, BOARD_DEBUG_UART_BAUDRATE_CORE1, BOARD_DEBUG_UART_TYPE_CORE1,
                    uartClkSrcFreq);
}

#if defined(SDK_I2C_BASED_COMPONENT_USED) && SDK_I2C_BASED_COMPONENT_USED
void BOARD_LPI2C_Init(LPI2C_Type *base, uint32_t clkSrc_Hz)
{
    lpi2c_master_config_t lpi2cConfig = {0};

    /*
     * lpi2cConfig.debugEnable = false;
     * lpi2cConfig.ignoreAck = false;
     * lpi2cConfig.pinConfig = kLPI2C_2PinOpenDrain;
     * lpi2cConfig.baudRate_Hz = 100000U;
     * lpi2cConfig.busIdleTimeout_ns = 0;
     * lpi2cConfig.pinLowTimeout_ns = 0;
     * lpi2cConfig.sdaGlitchFilterWidth_ns = 0;
     * lpi2cConfig.sclGlitchFilterWidth_ns = 0;
     */
    LPI2C_MasterGetDefaultConfig(&lpi2cConfig);
    LPI2C_MasterInit(base, &lpi2cConfig, clkSrc_Hz);
}

status_t BOARD_LPI2C_Send(LPI2C_Type *base,
                          uint8_t deviceAddress,
                          uint32_t subAddress,
                          uint8_t subAddressSize,
                          uint8_t *txBuff,
                          uint8_t txBuffSize)
{
    lpi2c_master_transfer_t xfer;

    xfer.flags          = kLPI2C_TransferDefaultFlag;
    xfer.slaveAddress   = deviceAddress;
    xfer.direction      = kLPI2C_Write;
    xfer.subaddress     = subAddress;
    xfer.subaddressSize = subAddressSize;
    xfer.data           = txBuff;
    xfer.dataSize       = txBuffSize;

    return LPI2C_MasterTransferBlocking(base, &xfer);
}

status_t BOARD_LPI2C_Receive(LPI2C_Type *base,
                             uint8_t deviceAddress,
                             uint32_t subAddress,
                             uint8_t subAddressSize,
                             uint8_t *rxBuff,
                             uint8_t rxBuffSize)
{
    lpi2c_master_transfer_t xfer;

    xfer.flags          = kLPI2C_TransferDefaultFlag;
    xfer.slaveAddress   = deviceAddress;
    xfer.direction      = kLPI2C_Read;
    xfer.subaddress     = subAddress;
    xfer.subaddressSize = subAddressSize;
    xfer.data           = rxBuff;
    xfer.dataSize       = rxBuffSize;

    return LPI2C_MasterTransferBlocking(base, &xfer);
}

void BOARD_Accel_I2C_Init(void)
{
    BOARD_LPI2C_Init(BOARD_ACCEL_I2C_BASEADDR, BOARD_ACCEL_I2C_CLOCK_FREQ);
}

status_t BOARD_Accel_I2C_Send(uint8_t deviceAddress, uint32_t subAddress, uint8_t subaddressSize, uint32_t txBuff)
{
    uint8_t data = (uint8_t)txBuff;

    return BOARD_LPI2C_Send(BOARD_ACCEL_I2C_BASEADDR, deviceAddress, subAddress, subaddressSize, &data, 1);
}

status_t BOARD_Accel_I2C_Receive(
    uint8_t deviceAddress, uint32_t subAddress, uint8_t subaddressSize, uint8_t *rxBuff, uint8_t rxBuffSize)
{
    return BOARD_LPI2C_Receive(BOARD_ACCEL_I2C_BASEADDR, deviceAddress, subAddress, subaddressSize, rxBuff, rxBuffSize);
}

void BOARD_Codec_I2C_Init(void)
{
    BOARD_LPI2C_Init(BOARD_CODEC_I2C_BASEADDR, BOARD_CODEC_I2C_CLOCK_FREQ);
}

status_t BOARD_Codec_I2C_Send(
    uint8_t deviceAddress, uint32_t subAddress, uint8_t subAddressSize, const uint8_t *txBuff, uint8_t txBuffSize)
{
    return BOARD_LPI2C_Send(BOARD_CODEC_I2C_BASEADDR, deviceAddress, subAddress, subAddressSize, (uint8_t *)txBuff,
                            txBuffSize);
}

status_t BOARD_Codec_I2C_Receive(
    uint8_t deviceAddress, uint32_t subAddress, uint8_t subAddressSize, uint8_t *rxBuff, uint8_t rxBuffSize)
{
    return BOARD_LPI2C_Receive(BOARD_CODEC_I2C_BASEADDR, deviceAddress, subAddress, subAddressSize, rxBuff, rxBuffSize);
}
#endif /* SDK_I2C_BASED_COMPONENT_USED */


void BOARD_InitSensors(void)
{
    lpadc_config_t lpadcConfigStruct;
    lpadc_conv_trigger_config_t lpadcTriggerConfigStruct;;
    lpadc_conv_trigger_config_t lpadcTriggerConfigStruct2;

    /* Init ADC peripheral. */
    LPADC_GetDefaultConfig(&lpadcConfigStruct);
    lpadcConfigStruct.enableAnalogPreliminary = true;
    lpadcConfigStruct.powerLevelMode          = kLPADC_PowerLevelAlt4;
    lpadcConfigStruct.referenceVoltageSource = kLPADC_ReferenceVoltageAlt2;
    lpadcConfigStruct.conversionAverageMode = kLPADC_ConversionAverage128;
    lpadcConfigStruct.FIFO0Watermark = FSL_FEATURE_LPADC_TEMP_SENS_BUFFER_SIZE - 1U;
    LPADC_Init(ADC0, &lpadcConfigStruct);
    LPADC_DoResetFIFO0(ADC0);

    /* Do ADC calibration. */
    LPADC_SetOffsetValue(ADC0, 0x10u, 0x10u);
    LPADC_DoAutoCalibration(ADC0);

    /* Set conversion CMD configuration for temperature sensor. */
    LPADC_GetDefaultConvCommandConfig((lpadc_conv_command_config_t *) &g_LpadcCommandConfigStruct);
    g_LpadcCommandConfigStruct.channelNumber       = DEMO_LPADC_TEMP_SENS_CHANNEL;
    g_LpadcCommandConfigStruct.sampleChannelMode   = DEMO_LPADC_SAMPLE_TEMP_CHANNEL_MODE;
    g_LpadcCommandConfigStruct.sampleTimeMode      = kLPADC_SampleTimeADCK131;
    g_LpadcCommandConfigStruct.hardwareAverageMode = kLPADC_HardwareAverageCount128;
    g_LpadcCommandConfigStruct.loopCount = FSL_FEATURE_LPADC_TEMP_SENS_BUFFER_SIZE - 1U;
    g_LpadcCommandConfigStruct.conversionResolutionMode = kLPADC_ConversionResolutionHigh;
    LPADC_SetConvCommandConfig(ADC0, 1u, (lpadc_conv_command_config_t *) &g_LpadcCommandConfigStruct);
    /* Set conversion CMD configuration for touch pad. */
    g_LpadcCommandConfigStruct.channelNumber       = DEMO_LPADC_TOUCH_SENS_CHANNEL;
    g_LpadcCommandConfigStruct.sampleChannelMode   = DEMO_LPADC_SAMPLE_TOUCH_CHANNEL_MODE;
    LPADC_SetConvCommandConfig(ADC0, 2u, (lpadc_conv_command_config_t *) &g_LpadcCommandConfigStruct);

    /* Set trigger configuration. */
    LPADC_GetDefaultConvTriggerConfig(&lpadcTriggerConfigStruct);
    //LPADC_GetDefaultConvTriggerConfig(&lpadcTriggerConfigStruct2);
    lpadcTriggerConfigStruct.targetCommandId = 1u;
    //lpadcTriggerConfigStruct2.targetCommandId = DEMO_LPADC_USER_CMDID2;
    LPADC_SetConvTriggerConfig(ADC0, 0U, &lpadcTriggerConfigStruct); /* Configurate the trigger0. */
    lpadcTriggerConfigStruct.targetCommandId = 2u;
    LPADC_SetConvTriggerConfig(ADC0, 1U, &lpadcTriggerConfigStruct); /* Configurate the trigger1. */

	LPADC_EnableInterrupts(ADC0, kLPADC_FIFO0WatermarkInterruptEnable);
    EnableIRQ(ADC0_IRQn);

    /* Eliminate the first two inaccurate results. */
    LPADC_DoSoftwareTrigger(ADC0, 3u); /* 1U is trigger0 mask. */
}
 void BOARD_InitTimer(void)
 {
	 lptmr_config_t config;
	 LPTMR_GetDefaultConfig(&config);
	 config.prescalerClockSource = kLPTMR_PrescalerClock_0;
	 LPTMR_Init(LPTMR0, &config);
	 LPTMR_SetTimerPeriod(LPTMR0, USEC_TO_COUNT(1000000, 12000000));
	/* Enable timer interrupt */
	LPTMR_EnableInterrupts(LPTMR0, kLPTMR_TimerInterruptEnable);

	/* Enable at the NVIC */
	EnableIRQ(LPTMR0_IRQn);
	LPTMR_StartTimer(LPTMR0);
 }


 void BOARD_InitRTC(void)
 {
	 irtc_datetime_t datetime;

	 datetime.day     = 17;
	 datetime.month   = 1;
	 datetime.year    = 2024u;
	 datetime.hour    = 11;
	 datetime.minute  = 25;
	 datetime.second  = 35;
	 datetime.weekDay = 3;

	 CLOCK_SetupClk16KClocking(kCLOCK_Clk16KToVbat | kCLOCK_Clk16KToMain);

	 irtc_config_t irtcConfig;
	 IRTC_GetDefaultConfig(&irtcConfig);
	 IRTC_Init(RTC, &irtcConfig);
	 IRTC_SetDatetime(RTC, &datetime);
 }




/* Update Active mode voltage for OverDrive mode. */
void BOARD_PowerMode_OD(void)
{
    spc_active_mode_dcdc_option_t opt = {
        .DCDCVoltage       = kSPC_DCDC_NormalVoltage,
        .DCDCDriveStrength = kSPC_DCDC_NormalDriveStrength,
    };
    SPC_SetActiveModeDCDCRegulatorConfig(SPC0, &opt);

    spc_sram_voltage_config_t cfg = {
        .operateVoltage       = kSPC_sramOperateAt1P2V,
        .requestVoltageUpdate = true,
    };
    SPC_SetSRAMOperateVoltage(SPC0, &cfg);
}
