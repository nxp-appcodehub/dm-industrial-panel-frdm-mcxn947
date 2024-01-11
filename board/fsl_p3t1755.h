/*
 * Copyright 2022 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_P3T1755_H_
#define _FSL_P3T1755_H_

#include "fsl_common.h"
#include "fsl_i3c.h"

#define EXAMPLE_MASTER             I3C1
#define I3C_MASTER_CLOCK_FREQUENCY CLOCK_GetI3cClkFreq(1)
#define SENSOR_SLAVE_ADDR          0x48U
#define I3C_TIME_OUT_INDEX 100000000U

#define SENSOR_ADDR 0x08U
#define CCC_RSTDAA  0x06U
#define CCC_SETDASA 0x87

#ifndef EXAMPLE_I2C_BAUDRATE
#define EXAMPLE_I2C_BAUDRATE 400000
#endif
#ifndef EXAMPLE_I3C_OD_BAUDRATE
#define EXAMPLE_I3C_OD_BAUDRATE 1500000
#endif
#ifndef EXAMPLE_I3C_PP_BAUDRATE
#define EXAMPLE_I3C_PP_BAUDRATE 4000000
#endif

/* Registers. */
#define P3T1755_TEMPERATURE_REG (0x00U)
#define P3T1755_CONFIG_REG      (0x01U)

/*! @brief Define sensor access function. */
typedef status_t (*sensor_write_transfer_func_t)(uint8_t deviceAddress,
                                                 uint32_t regAddress,
                                                 uint8_t *regData,
                                                 size_t dataSize);
typedef status_t (*sensor_read_transfer_func_t)(uint8_t deviceAddress,
                                                uint32_t regAddress,
                                                uint8_t *regData,
                                                size_t dataSize);

typedef struct _p3t1755_handle
{
    sensor_write_transfer_func_t writeTransfer;
    sensor_read_transfer_func_t readTransfer;
    uint8_t sensorAddress;
} p3t1755_handle_t;

typedef struct _p3t1755_config
{
    sensor_write_transfer_func_t writeTransfer;
    sensor_read_transfer_func_t readTransfer;
    uint8_t sensorAddress;
} p3t1755_config_t;

#if defined(__cplusplus)
extern "C" {
#endif

static void i3c_master_callback(I3C_Type *base, i3c_master_handle_t *handle, status_t status, void *userData);

status_t I3C_WriteSensor(uint8_t deviceAddress, uint32_t regAddress, uint8_t *regData, size_t dataSize);

status_t I3C_ReadSensor(uint8_t deviceAddress, uint32_t regAddress, uint8_t *regData, size_t dataSize);

status_t p3t1755_set_dynamic_address(void);

/*!
 * @brief Create handle for P3T1755, reset the sensor per user configuration.
 *
 * @param p3t1755_handle The pointer to #p3t1755_handle_t.
 *
 * @return kStatus_Success if success or kStatus_Fail if error.
 */
status_t P3T1755_Init(p3t1755_handle_t *handle, p3t1755_config_t *config);

/*!
 * @brief Write Register with register data buffer.
 *
 * @param handle The pointer to #p3t1755_handle_t.
 * @param regAddress register address to write.
 * @param regData The pointer to data buffer to be write to the reg.
 * @param dataSize Size of the regData.
 *
 * @return kStatus_Success if success or kStatus_Fail if error.
 */
status_t P3T1755_WriteReg(p3t1755_handle_t *handle, uint32_t regAddress, uint8_t *regData, size_t dataSize);

/*!
 * @brief Read Register to speficied data buffer.
 *
 * @param handle The pointer to #p3t1755_handle_t.
 * @param regAddress register address to read.
 * @param regData The pointer to data buffer to store the read out data.
 * @param dataSize Size of the regData to be read.
 *
 * @return kStatus_Success if success or kStatus_Fail if error.
 */
status_t P3T1755_ReadReg(p3t1755_handle_t *handle, uint32_t regAddress, uint8_t *regData, size_t dataSize);

/*!
 * @brief Read temperature data.
 *
 * @param handle The pointer to #p3t1755_handle_t.
 * @param temperature The pointer to temperature data.
 *
 * @return kStatus_Success if success or kStatus_Fail if error.
 */
status_t P3T1755_ReadTemperature(p3t1755_handle_t *handle, double *temperature);

double P3T1755_get_temperature();

void P3T1755_InitTemperature(void);


#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _FSL_P3T1755_H_ */
