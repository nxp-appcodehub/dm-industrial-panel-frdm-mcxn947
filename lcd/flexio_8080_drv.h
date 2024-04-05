/*
 * Copyright (c) 2015-2016, Freescale Semiconductor, Inc.
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

#ifndef __FSL_FLEXIO_8080_DRV_H__
#define __FSL_FLEXIO_8080_DRV_H__

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "fsl_common.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
//#if defined(KL28Z7_SERIES)
//
//#define MULBEAT_WR_DMA_CHN_IRQ          DMA0_04_IRQn
//#define MULBEAT_RD_DMA_CHN_IRQ          DMA0_15_IRQn
//#define MULBEAT_WR_DMA_IRQ_HANDLER      DMA3_0_CH0_IRQHandler
//#define MULBEAT_RD_DMA_IRQ_HANDLER      DMA0_15_IRQHandler
//
//#else
//    #error "No valid MCU part is defined."
//#endif

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
typedef struct _flexio_8080_pins_handler
{
    void (*Config_Data_Pin)(void);
    void (*Config_WR_FlexIO)(void);
    void (*Config_WR_GPIO)(void);
    void (*Config_RD_FlexIO)(void);
    void (*Config_RD_GPIO)(void);
    void (*Config_CS_GPIO)(void);
    void (*Set_CS_Pin)(bool level);
    void (*Config_RS_GPIO)(void);
    void (*Set_RS_Pin)(bool level);
    void (*Config_ReSet_GPIO)(void);
    void (*Set_ReSet_Pin)(bool level);    
}flexio_8080_pins_handler_t;

typedef struct _flexio_8080_type
{
    uint8_t BusWidth;
    uint8_t TmrDiv;

    uint8_t MulBeatWR_ShfQty;
    uint8_t MulBeatRD_ShfQty;

    uint8_t Data0PinIdx;
    uint8_t WRPinIdx;
    uint8_t RDPinIdx;

    uint32_t SglBeatWR_ShfIdx;
    uint32_t SglBeatRD_ShfIdx;
    uint32_t MulBeatWR_ShfIdx[2];
    uint32_t MulBeatRD_ShfIdx[2];
    uint32_t TmrIdx;

    uint32_t MulBeatWR_EDMA_Ch;
    uint32_t MulBeatRD_EDMA_Ch;
}FLEXIO_8080_Type;

typedef struct _flexio_8080_SglBeat_reg_config
{
    uint32_t ShiftConfig;
    uint32_t ShiftCtl;
    uint32_t TimCmp;
    uint32_t TimCfg;
    uint32_t TimCtl;
}flexio_8080_SglBeat_reg_config_t;

typedef struct _flexio_8080_MulBeatWR_reg_config
{
    uint32_t ShiftConfig;
    uint32_t ShiftCtl;
    uint32_t BufShiftCtl;
    uint32_t TimCmp;
    uint32_t TimCfg;
    uint32_t TimCtl;
}flexio_8080_MulBeatWR_reg_config_t;

typedef struct _flexio_8080_MulBeatRD_reg_config
{
    uint32_t BufShiftConfig;
    uint32_t ShiftConfig;
    uint32_t ShiftCtl;
    uint32_t TimCmp;
    uint32_t TimCfg;
    uint32_t TimCtl;
}flexio_8080_MulBeatRD_reg_config_t;

typedef struct _edma_tcd_config {
    uint32_t srcAddr;
    uint32_t destAddr;

    int16_t srcOffset;
    int16_t destOffset;

    uint32_t srcLastAddrAdjust;
    uint32_t destLastAddrAdjust;

    uint16_t srcModulo;
    uint16_t destModulo;

    uint16_t srcTransferSize;
    uint16_t destTransferSize;

    uint32_t minorLoopCount;
    uint16_t majorLoopCount;
} edma_tcd_config_t;

typedef struct _edma_user_config {
    void * Buffer;
    uint32_t TotalSize;
}edma_user_config_t;

extern bool WR_DMATransferDone;
extern bool RD_DMATransferDone;

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @breief Initialize FlexIO module for emulating 8080 bus
 */
void FLEXIO_8080_Init(void);

/*!
 * @breief Get default FlexIO 8080 type configurations.
 * You can change the constant values in this function based on your preference.
 */
void FLEXIO_8080_Type_GetDefaultConfig(void);

/*!
 * @breief Get register configurations for single-beat writing.
 * @param reg_config - The FlexIO register configurations.
 */
static void FLEXIO_8080_SglBeatWR_GetRegConfig(flexio_8080_SglBeat_reg_config_t * reg_config);


/*!
 * @breief Get register configurations for multi-beat writing.
 * @param reg_config - The FlexIO register configurations.
 */
static void FLEXIO_8080_MulBeatWR_GetRegConfig(flexio_8080_MulBeatWR_reg_config_t * reg_config);

/*!
 * @breief Get register configurations for multi-beat reading.
 * @param reg_config - The FlexIO register configurations.
 */
static void FLEXIO_8080_MulBeatRD_GetRegConfig(flexio_8080_MulBeatRD_reg_config_t * reg_config);

/*!
 * @brief Configure FlexIO registers for 8080 single-beat writing.
 * @param reg_config - The FlexIO register configurations.
 */
static void FLEXIO_8080_ConfigSglBeatWR(flexio_8080_SglBeat_reg_config_t const * reg_config);

/*!
 * @brief Configure FlexIO registers for 8080 single-beat reading.
 * @param reg_config - The FlexIO register configurations.
 */
static void FLEXIO_8080_ConfigSglBeatRD(flexio_8080_SglBeat_reg_config_t const * reg_config);

/*!
 * @brief Configure FlexIO registers for 8080 multi-beat writing.
 * @param reg_config - The FlexIO register configurations.
 */
static void FLEXIO_8080_ConfigMulBeatWR(flexio_8080_MulBeatWR_reg_config_t const * reg_config);

/*!
 * @brief Configure FlexIO registers for 8080 multi-beat reading.
 * @param reg_config - The FlexIO register configurations.
 */
static void FLEXIO_8080_ConfigMulBeatRD(flexio_8080_MulBeatRD_reg_config_t const * reg_config);


/*!
 * @brief Write command with single parameter in single-beat writing mode using polling method.
 * @param cmdIdx - command index
 * @param value - command value
 */
void FLEXIO_8080_SglBeatWR_1Prm(uint32_t const cmdIdx, uint16_t const value);

/*!
 * @brief Write command with multi parameters in single-beat writing mode using polling method.
 * @param cmdIdx - command index
 * @param buffer - buffer used to store command value
 * @param length - command size
 */
void FLEXIO_8080_SglBeatWR_nPrm(uint32_t const cmdIdx, uint16_t const * buffer,
                                                       uint32_t const length);

/*!
 * @brief Write command with multi repeated parameters in single-beat writing mode using polling method.
 * It is used to fill LCD panel with solid color.
 * @param cmdIdx - command index
 * @param value - command value
 * @param length - command size
 */
void FLEXIO_8080_SglBeatWR_nSamePrm(uint32_t const cmdIdx, uint16_t const value,
                                                           uint32_t const length);

/*!
 * @brief Read command with single parameter in single-beat reading mode using polling method.
 * @param cmdIdx - command index
 * @return - the reading result
 */
uint16_t FLEXIO_8080_SglBeatRD_1Prm(uint32_t const cmdIdx);

/*!
 * @brief Read command with multi parameters in single-beat reading mode using polling method.
 * @param cmdIdx - command index
 * @param buffer - buffer used to store command value
 * @param length - command size
 */
void FLEXIO_8080_SglBeatRD_nPrm(uint32_t const cmdIdx, uint16_t * buffer,
                                                uint32_t const length);

/*!
 * @brief Write command with multi parameters in multi-beat writing mode using DMA method.
 * The total transfer size is configured in the DMA configuration function.
 * @param cmdIdx - command index
 * @param buffer - buffer used to store command value
 * @param length - command size
 */
void FLEXIO_8080_MulBeatWR_nPrm(uint32_t const cmdIdx, uint16_t const * buffer,
                                                        uint32_t const length);

   
                                
void FLEXIO_8080_test_io(void);
void Demo_FLEXIO_8080_Init(void);

#if defined(__cplusplus)
}
#endif


#endif /* __FSL_FLEXIO_8080_DRV_H__ */

/* EOF */
