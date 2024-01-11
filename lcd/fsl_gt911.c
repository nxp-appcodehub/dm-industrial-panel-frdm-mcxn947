/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_common.h"
#include "fsl_lpi2c.h"
#include "fsl_gt911.h"

status_t GT911_Init(gt911_handle_t *handle, LPI2C_Type *base)
{
    lpi2c_master_transfer_t *xfer = &(handle->xfer);
    assert(handle);
    assert(base);
    if (!handle || !base)
    {
        return kStatus_InvalidArgument;
    }

    handle->base = base;
    /* clear transfer structure and buffer */
    memset(xfer, 0, sizeof(*xfer));
    memset(handle->touch_buf, 0, GT911_TOUCH_DATA_LEN);
      
    return kStatus_Success;
}

status_t GT911_Denit(gt911_handle_t *handle)
{
    assert(handle);

    if (!handle)
    {
        return kStatus_InvalidArgument;
    }

    handle->base = NULL;
    return kStatus_Success;
}

status_t GT911_ReadTouchData(gt911_handle_t *handle)
{
    assert(handle);
    if (!handle)
    {
        return kStatus_InvalidArgument;
    }
    lpi2c_master_transfer_t *xfer = &(handle->xfer);
    static uint32_t flag_clear = 0;
    status_t result;
   
    /* prepare transfer structure for reading touch data */
    xfer->slaveAddress   = GT911_I2C_ADDRESS;
    xfer->direction      = kLPI2C_Read;
    xfer->subaddress     = GT911_TOUCH_COOR_REGISTER_ADDR;
    xfer->subaddressSize = 2;
    xfer->data           = handle->touch_buf;
    xfer->dataSize       = 4;
    xfer->flags          = kLPI2C_TransferDefaultFlag;
    result = LPI2C_MasterTransferBlocking(handle->base, &handle->xfer);    
    if (result != kStatus_Success){
      return result;
    } 
    /* prepare transfer structure for reading touch data */
    xfer->slaveAddress   = GT911_I2C_ADDRESS;
    xfer->direction      = kLPI2C_Read;
    xfer->subaddress     = GT911_TOUCH_REGISTER_STATUS_ADDR;
    xfer->subaddressSize = 2;
    xfer->data           = &handle->touch_buf[4];
    xfer->dataSize       = 1;
    xfer->flags          = kLPI2C_TransferDefaultFlag;
    result = LPI2C_MasterTransferBlocking(handle->base, &handle->xfer);    
    if (result != kStatus_Success){
      return result;
    } 
    /* prepare transfer structure for reading touch data */
    xfer->slaveAddress   = GT911_I2C_ADDRESS;
    xfer->direction      = kLPI2C_Write;
    xfer->subaddress     = GT911_TOUCH_REGISTER_STATUS_ADDR;
    xfer->subaddressSize = 2;
    xfer->data           = &flag_clear;
    xfer->dataSize       = 1;
    xfer->flags          = kLPI2C_TransferDefaultFlag;    
    return LPI2C_MasterTransferBlocking(handle->base, &handle->xfer);  
}
//#include "fsl_debug_console.h"
//            PRINTF("\r\nx:%d,y:%d\r\n",*touch_x,*touch_y);

status_t GT911_GetSingleTouch(gt911_handle_t *handle, touch_event_t *touch_event, int *touch_x, int *touch_y)
{
    status_t status;
    touch_event_t touch_event_local;

    status = GT911_ReadTouchData(handle);

    if (status == kStatus_Success)
    {
       if (touch_event == NULL)
        {
            touch_event = &touch_event_local;
        }        
        if(handle->touch_buf[4] >0){    
          *touch_event =kTouch_Down; 
          handle->touch_buf[4] = 0;
        }else{
           *touch_event =kTouch_Up; 
        }
       
//#if LCD_ST7796S_TFT    
//        /* Update coordinates only if there is touch detected */
//        if (*touch_event == kTouch_Down)
//        {
//            if (touch_x)
//            {
//                *touch_x = (handle->touch_buf[3]<<8)|handle->touch_buf[2];
//            }
//            if (touch_y)
//            {
//                *touch_y = 319 - ((handle->touch_buf[1]<<8)|handle->touch_buf[0]);
//            }
//        }      
//    }
//#endif   
//#if LCD_ST7796S_IPS 
        /* Update coordinates only if there is touch detected */
        if (*touch_event == kTouch_Down)
        {
            if (touch_x)
            {
               *touch_x = (handle->touch_buf[3]<<8)|handle->touch_buf[2];
            }
            if (touch_y)
            {
               *touch_y = 319 -((handle->touch_buf[1]<<8)|handle->touch_buf[0]);
            }
        }      
//#endif
}
  return status;
}

