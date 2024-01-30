
/*
 * Copyright 2022 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "fsl_p3t1755.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*******************************************************************************
 * Variables
 ******************************************************************************/
volatile status_t g_completionStatus;
volatile bool g_masterCompletionFlag;
volatile static p3t1755_handle_t p3t1755Handle;
volatile static i3c_master_handle_t g_i3c_m_handle;
volatile static double g_temperature = 0;
const i3c_master_transfer_callback_t masterCallback =
{.slave2Master = NULL, .ibiCallback = NULL, .transferComplete = i3c_master_callback};
/*******************************************************************************
 * Code
 ******************************************************************************/

static void i3c_master_callback(I3C_Type *base, i3c_master_handle_t *handle, status_t status, void *userData)
{
    if (status == kStatus_Success)
    {
        g_masterCompletionFlag = true;
    }

    g_completionStatus = status;
}

status_t I3C_WriteSensor(uint8_t deviceAddress, uint32_t regAddress, uint8_t *regData, size_t dataSize)
{
    status_t result                  = kStatus_Success;
    i3c_master_transfer_t masterXfer = {0};
    uint32_t timeout                 = 0U;

    masterXfer.slaveAddress   = deviceAddress;
    masterXfer.direction      = kI3C_Write;
    masterXfer.busType        = kI3C_TypeI3CSdr;
    masterXfer.subaddress     = regAddress;
    masterXfer.subaddressSize = 1;
    masterXfer.data           = regData;
    masterXfer.dataSize       = dataSize;
    masterXfer.flags          = kI3C_TransferDefaultFlag;

    g_masterCompletionFlag = false;
    g_completionStatus     = kStatus_Success;
    result                 = I3C_MasterTransferNonBlocking(EXAMPLE_MASTER,(i3c_master_handle_t*) &g_i3c_m_handle, &masterXfer);
    if (kStatus_Success != result)
    {
        return result;
    }

    while (!g_masterCompletionFlag)
    {
        timeout++;
        if ((g_completionStatus != kStatus_Success) || (timeout > I3C_TIME_OUT_INDEX))
        {
            break;
        }
    }

    if (timeout == I3C_TIME_OUT_INDEX)
    {
        result = kStatus_Timeout;
    }
    result = g_completionStatus;

    return result;
}

status_t I3C_ReadSensor(uint8_t deviceAddress, uint32_t regAddress, uint8_t *regData, size_t dataSize)
{
    status_t result                  = kStatus_Success;
    i3c_master_transfer_t masterXfer = {0};
    uint32_t timeout                 = 0U;

    masterXfer.slaveAddress   = deviceAddress;
    masterXfer.direction      = kI3C_Read;
    masterXfer.busType        = kI3C_TypeI3CSdr;
    masterXfer.subaddress     = regAddress;
    masterXfer.subaddressSize = 1;
    masterXfer.data           = regData;
    masterXfer.dataSize       = dataSize;
    masterXfer.flags          = kI3C_TransferDefaultFlag;

    g_masterCompletionFlag = false;
    g_completionStatus     = kStatus_Success;
    result                 = I3C_MasterTransferNonBlocking(EXAMPLE_MASTER, (i3c_master_handle_t*)&g_i3c_m_handle, &masterXfer);
    if (kStatus_Success != result)
    {
        return result;
    }

    while (!g_masterCompletionFlag)
    {
        timeout++;
        if ((g_completionStatus != kStatus_Success) || (timeout > I3C_TIME_OUT_INDEX))
        {
            break;
        }
    }

    if (timeout == I3C_TIME_OUT_INDEX)
    {
        result = kStatus_Timeout;
    }
    result = g_completionStatus;

    return result;
}

status_t p3t1755_set_dynamic_address(void)
{
    status_t result                  = kStatus_Success;
    i3c_master_transfer_t masterXfer = {0};
    uint8_t g_master_txBuff[1];

    /* Reset dynamic address. */
    g_master_txBuff[0]      = CCC_RSTDAA;
    masterXfer.slaveAddress = 0x7E;
    masterXfer.data         = g_master_txBuff;
    masterXfer.dataSize     = 1;
    masterXfer.direction    = kI3C_Write;
    masterXfer.busType      = kI3C_TypeI3CSdr;
    masterXfer.flags        = kI3C_TransferDefaultFlag;
    result                  = I3C_MasterTransferBlocking(EXAMPLE_MASTER, &masterXfer);
    if (result != kStatus_Success)
    {
        return result;
    }

    /* Assign dynmic address. */
    memset(&masterXfer, 0, sizeof(masterXfer));
    g_master_txBuff[0]      = CCC_SETDASA;
    masterXfer.slaveAddress = 0x7E;
    masterXfer.data         = g_master_txBuff;
    masterXfer.dataSize     = 1;
    masterXfer.direction    = kI3C_Write;
    masterXfer.busType      = kI3C_TypeI3CSdr;
    masterXfer.flags        = kI3C_TransferNoStopFlag;
    result                  = I3C_MasterTransferBlocking(EXAMPLE_MASTER, &masterXfer);
    if (result != kStatus_Success)
    {
        return result;
    }

    memset(&masterXfer, 0, sizeof(masterXfer));
    g_master_txBuff[0]      = SENSOR_ADDR << 1;
    masterXfer.slaveAddress = SENSOR_SLAVE_ADDR;
    masterXfer.data         = g_master_txBuff;
    masterXfer.dataSize     = 1;
    masterXfer.direction    = kI3C_Write;
    masterXfer.busType      = kI3C_TypeI3CSdr;
    masterXfer.flags        = kI3C_TransferDefaultFlag;
    return I3C_MasterTransferBlocking(EXAMPLE_MASTER, &masterXfer);
}

status_t P3T1755_WriteReg(p3t1755_handle_t *handle, uint32_t regAddress, uint8_t *regData, size_t dataSize)
{
    status_t result;
    result = handle->writeTransfer(handle->sensorAddress, regAddress, regData, dataSize);

    return (result == kStatus_Success) ? result : kStatus_Fail;
}

status_t P3T1755_ReadReg(p3t1755_handle_t *handle, uint32_t regAddress, uint8_t *regData, size_t dataSize)
{
    status_t result;
    result = handle->readTransfer(handle->sensorAddress, regAddress, regData, dataSize);

    return (result == kStatus_Success) ? result : kStatus_Fail;
}

status_t P3T1755_Init(p3t1755_handle_t *handle, p3t1755_config_t *config)
{
    assert(handle != NULL);
    assert(config != NULL);
    assert(config->writeTransfer != NULL);
    assert(config->readTransfer != NULL);

    handle->writeTransfer = config->writeTransfer;
    handle->readTransfer  = config->readTransfer;
    handle->sensorAddress = config->sensorAddress;

    return kStatus_Success;
}

status_t P3T1755_ReadTemperature(p3t1755_handle_t *handle, double *temperature)
{
    status_t result = kStatus_Success;
    uint8_t data[2];

    result = P3T1755_ReadReg(handle, P3T1755_TEMPERATURE_REG, &data[0], 2);
    if (result == kStatus_Success)
    {
        *temperature = (double)((((uint16_t)data[0] << 8U) | (uint16_t)data[1]) >> 4U);
        *temperature = *temperature * 0.0625;
    }

    return result;
}

double P3T1755_get_temperature()
{
	double temperature = 0;
	status_t result = P3T1755_ReadTemperature((p3t1755_handle_t*)&p3t1755Handle, (double*)&temperature);
	if(kStatus_Success == result)
	{
		g_temperature = temperature;
	}
	return g_temperature;
}

void P3T1755_InitTemperature(void)
{
	status_t result = kStatus_Success;
	i3c_master_config_t masterConfig;
	p3t1755_config_t p3t1755Config;
	double temperature;


	I3C_MasterGetDefaultConfig(&masterConfig);
	masterConfig.baudRate_Hz.i2cBaud          = EXAMPLE_I2C_BAUDRATE;
	masterConfig.baudRate_Hz.i3cPushPullBaud  = EXAMPLE_I3C_PP_BAUDRATE;
	masterConfig.baudRate_Hz.i3cOpenDrainBaud = EXAMPLE_I3C_OD_BAUDRATE;
	masterConfig.enableOpenDrainStop          = false;
	masterConfig.disableTimeout               = true;
	I3C_MasterInit(EXAMPLE_MASTER, &masterConfig, I3C_MASTER_CLOCK_FREQUENCY);

	/* Create I3C handle. */
	I3C_MasterTransferCreateHandle(EXAMPLE_MASTER, (i3c_master_handle_t*)&g_i3c_m_handle, &masterCallback, NULL);

	result = p3t1755_set_dynamic_address();

	p3t1755Config.writeTransfer = I3C_WriteSensor;
	p3t1755Config.readTransfer  = I3C_ReadSensor;
	p3t1755Config.sensorAddress = SENSOR_ADDR;
	P3T1755_Init((p3t1755_handle_t*)&p3t1755Handle, &p3t1755Config);
}
