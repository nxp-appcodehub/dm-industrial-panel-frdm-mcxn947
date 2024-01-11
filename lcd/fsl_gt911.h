/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_GT911_H_
#define _FSL_GT911_H_

#include "fsl_common.h"

/*!
 * @addtogroup gt911
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief GT911 I2C address. */
#define GT911_I2C_ADDRESS (0x5d)//(0x14) //

/*! @brief GT911 raw touch data length. */
#define GT911_TOUCH_DATA_LEN (10)

/*! @brief GT911 point 1 coordinate register address*/
#define GT911_TOUCH_COOR_REGISTER_ADDR (0x8150)
   
/*! @brief GT911 status register address*/
#define GT911_TOUCH_REGISTER_STATUS_ADDR (0x814E)
   
typedef enum _touch_event
{
    kTouch_Down     = 0, /*!< The state changed to touched. */
    kTouch_Up       = 1, /*!< The state changed to not touched. */
    kTouch_Contact  = 2, /*!< There is a continuous touch being detected. */
    kTouch_Reserved = 3  /*!< No touch information available. */
} touch_event_t;

typedef struct _gt911_handle
{
    LPI2C_Type *base;
    lpi2c_master_transfer_t xfer;
    uint8_t touch_buf[GT911_TOUCH_DATA_LEN];
} gt911_handle_t;

status_t GT911_Init(gt911_handle_t *handle, LPI2C_Type *base);

status_t GT911_Denit(gt911_handle_t *handle);

status_t GT911_GetSingleTouch(gt911_handle_t *handle, touch_event_t *touch_event, int *touch_x, int *touch_y);

#endif
