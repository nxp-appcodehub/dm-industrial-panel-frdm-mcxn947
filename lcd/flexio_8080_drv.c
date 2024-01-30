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

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "flexio_8080_drv.h"
#include "lcd_drv.h"
#include "pin_mux.h"
#include "board.h"

/*******************************************************************************
 * Variables
 ******************************************************************************/
static FLEXIO_8080_Type FlexIO_8080_Inst;
FLEXIO_8080_Type * pFlexIO_8080_Inst = &FlexIO_8080_Inst;

static flexio_8080_pins_handler_t flexio_8080_pins_handler;
flexio_8080_pins_handler_t * pflexio_8080_pins_handler = &flexio_8080_pins_handler;

static flexio_8080_SglBeat_reg_config_t SglBeatWR_Reg_Config;
static flexio_8080_MulBeatWR_reg_config_t MulBeatWR_Reg_Config;

bool WR_DMATransferDone;
bool RD_DMATransferDone;

static uint32_t MulBeatCountRemain;
static uint16_t * MulBeatDataRemain;

static void (*pDMAIntCallback)(void);

bool DMAFillColorMode = false;
 uint32_t test_data = 0x1f<<0;
 uint8_t __attribute__((aligned(32))) test_picture[320*240*2];

/*******************************************************************************
 * Functions
 ******************************************************************************/

 
 
 
/*!
 * @breief Initialize FlexIO module for emulating 8080 bus.
 */
void FLEXIO_8080_Init(void)
{
    /* Configure pin MUX */
    flexio_8080_pins_handler.Config_Data_Pin();
    flexio_8080_pins_handler.Config_CS_GPIO();
    flexio_8080_pins_handler.Config_RS_GPIO();
    flexio_8080_pins_handler.Config_WR_GPIO();
    flexio_8080_pins_handler.Config_RD_GPIO();
    flexio_8080_pins_handler.Config_ReSet_GPIO();
    flexio_8080_pins_handler.Set_ReSet_Pin(true);


    FLEXIO_8080_SglBeatWR_GetRegConfig(&SglBeatWR_Reg_Config);
    FLEXIO_8080_MulBeatWR_GetRegConfig(&MulBeatWR_Reg_Config);
    
    /* Configure FlexIO module registers */
    FLEXIO0->CTRL = FLEXIO_CTRL_DBGE_MASK  & ~FLEXIO_CTRL_FLEXEN_MASK;   
}
/*!
 * @breief Get default FlexIO 8080 type configurations.
 * You can change the constant values in this function based on your preference.
 */
void FLEXIO_8080_Type_GetDefaultConfig(void)
{
    FlexIO_8080_Inst.BusWidth = 16U;
    FlexIO_8080_Inst.TmrDiv = 2;//6U;          /* The divider is 6 rather than 4. The LCD moule could not bare to high speed. */  
    
    FlexIO_8080_Inst.MulBeatWR_ShfQty = 8U;

    FlexIO_8080_Inst.SglBeatWR_ShfIdx = 0U;         /* must be 0 or 4 */
    FlexIO_8080_Inst.MulBeatWR_ShfIdx[0] = 0U;      /* must be 0 or 4 */
    FlexIO_8080_Inst.MulBeatWR_ShfIdx[1] = 7U;      /* equals WR_ShfIdx[0] + (ShfQty - 1) */
    FlexIO_8080_Inst.TmrIdx = 0U;

    FlexIO_8080_Inst.MulBeatWR_EDMA_Ch = 0U;
}

/*!
 * @breief Get register configurations for single-beat writing.
 */
static void FLEXIO_8080_SglBeatWR_GetRegConfig(flexio_8080_SglBeat_reg_config_t * reg_config)
{
    reg_config->ShiftConfig =
        FLEXIO_SHIFTCFG_INSRC(1U)                                               /* Shifter input */
      | FLEXIO_SHIFTCFG_PWIDTH(FlexIO_8080_Inst.BusWidth-1U);                   /* Bus width */

    reg_config->ShiftCtl =
        FLEXIO_SHIFTCTL_TIMSEL(FlexIO_8080_Inst.TmrIdx)                         /* Shifter's assigned timer index */
      | FLEXIO_SHIFTCTL_TIMPOL(0U)                                              /* Shift on posedge of shift clock */
      | FLEXIO_SHIFTCTL_PINCFG(3U)                                              /* Shifter's pin configured as output */
      | FLEXIO_SHIFTCTL_PINSEL(FlexIO_8080_Inst.Data0PinIdx)                    /* Shifter's pin start index */
      | FLEXIO_SHIFTCTL_PINPOL(0U)                                              /* Shifter's pin active high */
      | FLEXIO_SHIFTCTL_SMOD(2U);                                               /* Shifter mode as transmit */

    reg_config->TimCmp =
        (1U * 2U -1 << 8)                                                       /* TIMCMP[15:8] = number of beats x 2 – 1 */
      | (FlexIO_8080_Inst.TmrDiv/2U - 1U);                                      /* TIMCMP[7:0] = baud rate divider / 2 – 1 */

    reg_config->TimCfg =
        FLEXIO_TIMCFG_TIMOUT(0U)                                                /* Timer output logic one when enabled and not affected by reset */
      | FLEXIO_TIMCFG_TIMDEC(0U)                                                /* Timer decrement on FlexIO clock, shift clock equals timer output */
      | FLEXIO_TIMCFG_TIMRST(0U)                                                /* Timer never reset */
      | FLEXIO_TIMCFG_TIMDIS(2U)                                                /* Timer disabled on timer compare */
      | FLEXIO_TIMCFG_TIMENA(2U);                                               /* Timer enabled on trigger high */


    reg_config->TimCtl =
        FLEXIO_TIMCTL_TRGSEL((((FlexIO_8080_Inst.SglBeatWR_ShfIdx) << 2) | 1U)) /* Timer trigger selected as shifter's status flag */
      | FLEXIO_TIMCTL_TRGPOL(1U)                                                /* Timer trigger polarity as active low */
      | FLEXIO_TIMCTL_TRGSRC(1U)                                                /* Timer trigger source as internal */
      | FLEXIO_TIMCTL_PINCFG(3U)                                                /* Timer' pin configured as output */
      | FLEXIO_TIMCTL_PINSEL(FlexIO_8080_Inst.WRPinIdx)                         /* Timer' pin index: WR pin */
      | FLEXIO_TIMCTL_PINPOL(1U)                                                /* Timer' pin active low */
      | FLEXIO_TIMCTL_TIMOD(1U);                                                /* Timer mode as dual 8-bit counters baud/bit */
}

/*!
 * @breief Get register configurations for multi-beat writing.
 */
static void FLEXIO_8080_MulBeatWR_GetRegConfig(flexio_8080_MulBeatWR_reg_config_t * reg_config)
{
    uint8_t MulBeatWR_BeatQty = FlexIO_8080_Inst.MulBeatWR_ShfQty * 32U / FlexIO_8080_Inst.BusWidth;

    reg_config->ShiftConfig =
        FLEXIO_SHIFTCFG_INSRC(1U)                                               /* Shifter input from next shifter's output */
      | FLEXIO_SHIFTCFG_SSTOP(0U)                                               /* Shifter stop bit disabled */
      | FLEXIO_SHIFTCFG_SSTART(0U)                                              /* Shifter start bit disabled and loading data on enabled */
      | FLEXIO_SHIFTCFG_PWIDTH(FlexIO_8080_Inst.BusWidth-1U);                   /* Bus width */

    reg_config->ShiftCtl =
        FLEXIO_SHIFTCTL_TIMSEL(FlexIO_8080_Inst.TmrIdx)                         /* Shifter's assigned timer index */
      | FLEXIO_SHIFTCTL_TIMPOL(0U)                                              /* Shift on posedge of shift clock */
      | FLEXIO_SHIFTCTL_PINCFG(3U)                                              /* Shifter's pin configured as output */
      | FLEXIO_SHIFTCTL_PINSEL(FlexIO_8080_Inst.Data0PinIdx)                    /* Shifter's pin start index */
      | FLEXIO_SHIFTCTL_PINPOL(0U)                                              /* Shifter's pin active high */
      | FLEXIO_SHIFTCTL_SMOD(2U);                                               /* Shifter mode as transmit */

    reg_config->BufShiftCtl =
        FLEXIO_SHIFTCTL_TIMSEL(FlexIO_8080_Inst.TmrIdx)                         /* Shifter's assigned timer index */
      | FLEXIO_SHIFTCTL_TIMPOL(0U)                                              /* Shift on posedge of shift clock */
      | FLEXIO_SHIFTCTL_PINCFG(0U)                                              /* Shifter's pin configured as output disabled */
      | FLEXIO_SHIFTCTL_PINSEL(FlexIO_8080_Inst.Data0PinIdx)                    /* Shifter's pin start index */
      | FLEXIO_SHIFTCTL_PINPOL(0U)                                              /* Shifter's pin active high */
      | FLEXIO_SHIFTCTL_SMOD(2U);                                               /* Shifter mode transmit */

    reg_config->TimCmp =
        ((MulBeatWR_BeatQty * 2U - 1) << 8)                                     /* TIMCMP[15:8] = number of beats x 2 – 1 */
      | (FlexIO_8080_Inst.TmrDiv/2U - 1U);                                      /* TIMCMP[7:0] = baud rate divider / 2 – 1 */

    reg_config->TimCfg =
        FLEXIO_TIMCFG_TIMOUT(0U)                                                /* Timer output logic one when enabled and not affected by reset */
      | FLEXIO_TIMCFG_TIMDEC(0U)                                                /* Timer decrement on FlexIO clock, shift clock equals timer output */
      | FLEXIO_TIMCFG_TIMRST(0U)                                                /* Timer never reset */
      | FLEXIO_TIMCFG_TIMDIS(2U)                                                /* Timer disabled on timer compare */
      | FLEXIO_TIMCFG_TIMENA(2U)                                                /* Timer enabled on trigger high */
      | FLEXIO_TIMCFG_TSTOP(0U)                                                 /* Timer stop bit disabled */
      | FLEXIO_TIMCFG_TSTART(0U);                                               /* Timer start bit disabled */

    reg_config->TimCtl =
        FLEXIO_TIMCTL_TRGSEL((((FlexIO_8080_Inst.MulBeatWR_ShfIdx[1]) << 2) | 1U))/* Timer trigger selected as shifter's status flag */
      | FLEXIO_TIMCTL_TRGPOL(1U)                                                /* Timer trigger polarity as active low */
      | FLEXIO_TIMCTL_TRGSRC(1U)                                                /* Timer trigger source as internal */
      | FLEXIO_TIMCTL_PINCFG(3U)                                                /* Timer' pin configured as output */
      | FLEXIO_TIMCTL_PINSEL(FlexIO_8080_Inst.WRPinIdx)                         /* Timer' pin index: WR pin */
      | FLEXIO_TIMCTL_PINPOL(1U)                                                /* Timer' pin active low */
      | FLEXIO_TIMCTL_TIMOD(1U);                                                /* Timer mode as dual 8-bit counters baud/bit */
}

/*!
 * @brief Configure FlexIO registers for 8080 single-beat writing.
 */
static void FLEXIO_8080_ConfigSglBeatWR(flexio_8080_SglBeat_reg_config_t const * reg_config)
{
    /* Disable and reset FlexIO */
    FLEXIO0->CTRL &= ~FLEXIO_CTRL_FLEXEN_MASK;
    FLEXIO0->CTRL |= FLEXIO_CTRL_SWRST_MASK;
    FLEXIO0->CTRL &= ~FLEXIO_CTRL_SWRST_MASK;

    /* Configure the shifters */
    FLEXIO0->SHIFTCFG[FlexIO_8080_Inst.SglBeatWR_ShfIdx] = reg_config->ShiftConfig;

    FLEXIO0->SHIFTCTL[FlexIO_8080_Inst.SglBeatWR_ShfIdx] = reg_config->ShiftCtl;

    /* Configure the timer for shift clock */
    FLEXIO0->TIMCMP[FlexIO_8080_Inst.TmrIdx] = reg_config->TimCmp;
    FLEXIO0->TIMCFG[FlexIO_8080_Inst.TmrIdx] = reg_config->TimCfg;
    FLEXIO0->TIMCTL[FlexIO_8080_Inst.TmrIdx] = reg_config->TimCtl;
    /* Enable FlexIO */
    FLEXIO0->CTRL |= FLEXIO_CTRL_FLEXEN_MASK;
}


/*!
 * @brief Configure FlexIO registers for 8080 multi-beat writing.
 */
static void FLEXIO_8080_ConfigMulBeatWR(flexio_8080_MulBeatWR_reg_config_t const * reg_config)
{
    uint32_t i;

    /* Disable and reset FlexIO */
    FLEXIO0->CTRL &= ~FLEXIO_CTRL_FLEXEN_MASK;
    FLEXIO0->CTRL |= FLEXIO_CTRL_SWRST_MASK;
    FLEXIO0->CTRL &= ~FLEXIO_CTRL_SWRST_MASK;

    /* Configure the shifters */
    for(i=FlexIO_8080_Inst.MulBeatWR_ShfIdx[0]; i<=FlexIO_8080_Inst.MulBeatWR_ShfIdx[1]; i++)
    {
        FLEXIO0->SHIFTCFG[i] = reg_config->ShiftConfig;
    }

    FLEXIO0->SHIFTCTL[FlexIO_8080_Inst.MulBeatWR_ShfIdx[0]] = reg_config->ShiftCtl;

    for(i=FlexIO_8080_Inst.MulBeatWR_ShfIdx[0]+1U; i<=FlexIO_8080_Inst.MulBeatWR_ShfIdx[1]; i++)
    {
        FLEXIO0->SHIFTCTL[i] = reg_config->BufShiftCtl;
    }

    /* Configure the timer for shift clock */
    FLEXIO0->TIMCMP[FlexIO_8080_Inst.TmrIdx] = reg_config->TimCmp;
    FLEXIO0->TIMCFG[FlexIO_8080_Inst.TmrIdx] = reg_config->TimCfg;
    FLEXIO0->TIMCTL[FlexIO_8080_Inst.TmrIdx] = reg_config->TimCtl;

    /* Enable FLexIO */
    FLEXIO0->CTRL |= FLEXIO_CTRL_FLEXEN_MASK;
}
static void flexio_8080_mulbeat_dma_config(uint32_t scr_addr, uint32_t des_addr, uint32_t Nbytes, bool isfillcolormode){
    
    RESET_PeripheralReset(kDMA0_RST_SHIFT_RSTn);
//    DMA0->MP_CSR |= DMA_MP_CSR_HAE_MASK|DMA_MP_CSR_EDBG_MASK;
    DMA0->CH[0].CH_MUX = DMA_CH_MUX_SRC(68);
//    DMA0->CH[0].CH_MUX = DMA_CH_MUX_SRC(62); //shifter 1
    DMA0->CH[0].TCD_SADDR = scr_addr;//&test_data;
    DMA0->CH[0].TCD_NBYTES_MLOFFNO = DMA_TCD_NBYTES_MLOFFNO_NBYTES(32);
//    DMA0->CH[0].TCD_NBYTES_MLOFFNO = DMA_TCD_NBYTES_MLOFFNO_NBYTES(8);
    
    if(isfillcolormode == true){    
        DMA0->CH[0].TCD_SOFF =0;
        DMA0->CH[0].TCD_ATTR = DMA_TCD_ATTR_SSIZE(1) | DMA_TCD_ATTR_DSIZE(5);
//        DMA0->CH[0].TCD_ATTR = DMA_TCD_ATTR_SSIZE(3) | DMA_TCD_ATTR_DSIZE(3);
        DMA0->CH[0].TCD_SLAST_SDA = 0; 
        
//    DMA0->CH[0].TCD_CITER_ELINKNO =Nbytes/8;
//    DMA0->CH[0].TCD_BITER_ELINKNO = Nbytes/8;
    }else{
      
        DMA0->CH[0].TCD_SOFF = 32;
//        DMA0->CH[0].TCD_SOFF = 8;
        DMA0->CH[0].TCD_ATTR = DMA_TCD_ATTR_SSIZE(5) | DMA_TCD_ATTR_DSIZE(5);
//        DMA0->CH[0].TCD_ATTR = DMA_TCD_ATTR_SSIZE(3) | DMA_TCD_ATTR_DSIZE(3);
        DMA0->CH[0].TCD_SLAST_SDA = 0;

        
    }
    DMA0->CH[0].TCD_DADDR = des_addr;// (uint32_t)&FLEXIO0->SHIFTBUF[0];
    DMA0->CH[0].TCD_DOFF = 0;
    DMA0->CH[0].TCD_CITER_ELINKNO = Nbytes/32;
    DMA0->CH[0].TCD_BITER_ELINKNO = Nbytes/32;

    DMA0->CH[0].TCD_CSR = DMA_TCD_CSR_INTMAJOR_MASK;  
    DMA0->CH[0].TCD_DLAST_SGA = 0;
    NVIC_EnableIRQ(EDMA_0_CH0_IRQn);


}
/*!
* @brief Enable DMA request for multi-beat writing.
*/
static void FLEXIO_8080_MulBeatWR_DMA_Enable(bool enable)
{
    if(enable)
    {
       DMA0->CH[FlexIO_8080_Inst.MulBeatWR_EDMA_Ch].CH_CSR |= DMA_CH_CSR_ERQ_MASK;
    }
    else
    {
       DMA0->CH[FlexIO_8080_Inst.MulBeatWR_EDMA_Ch].CH_CSR &= ~DMA_CH_CSR_ERQ_MASK;

    }
}

/*!
* @brief DMA channel 0 interrupt handler for multi-beat writing.
*/
void EDMA_0_CH0_IRQHandler(void)
{
    if(false != ( DMA0->CH[FlexIO_8080_Inst.MulBeatWR_EDMA_Ch].CH_INT & DMA_CH_INT_INT_MASK))      
    {
        DMA0->CH[FlexIO_8080_Inst.MulBeatWR_EDMA_Ch].CH_CSR &= ~DMA_CH_CSR_ERQ_MASK;        
        DMA0->CH[FlexIO_8080_Inst.MulBeatWR_EDMA_Ch].CH_CSR |=  DMA_CH_CSR_DONE_MASK; 
        DMA0->CH[FlexIO_8080_Inst.MulBeatWR_EDMA_Ch].CH_INT |=  DMA_CH_INT_INT_MASK;

        if(pDMAIntCallback != NULL)
        {
            pDMAIntCallback();
            pDMAIntCallback = NULL;
        }
    }
}

/*!
 * @brief Write command with single parameter in single-beat writing mode using polling method.
 */
void FLEXIO_8080_SglBeatWR_1Prm(uint32_t const cmdIdx, uint16_t const value)
{
    /* Configure RD pin as GPIO function, WR pin as FlexIO function */
    flexio_8080_pins_handler.Config_RD_GPIO();
    
    flexio_8080_pins_handler.Config_WR_GPIO();

    /* Configure FlexIO with 1-beat write configuration */
    FLEXIO_8080_ConfigSglBeatWR(&SglBeatWR_Reg_Config);
    flexio_8080_pins_handler.Config_WR_FlexIO();

    /* Assert CS, RS pins */
    flexio_8080_pins_handler.Set_CS_Pin(false);
    flexio_8080_pins_handler.Set_RS_Pin(false);

    /* Write command index */
    FLEXIO0->SHIFTBUF[FlexIO_8080_Inst.SglBeatWR_ShfIdx] = cmdIdx;
    /*Wait for transfer to be completed */
    while(0 == (FLEXIO0->TIMSTAT & (1U << FlexIO_8080_Inst.TmrIdx)))
    {
    }

    /* De-assert RS pin */
    flexio_8080_pins_handler.Set_RS_Pin(true);

    /* Use polling method for data transfer */
    while(0 == (FLEXIO0->SHIFTSTAT & (1U << FlexIO_8080_Inst.SglBeatWR_ShfIdx)))
    {
    }
    FLEXIO0->TIMSTAT |= (1U << FlexIO_8080_Inst.TmrIdx);
    FLEXIO0->SHIFTBUF[FlexIO_8080_Inst.SglBeatWR_ShfIdx] = value;

    /*Wait for transfer to be completed */
    while(0 == (FLEXIO0->TIMSTAT & (1U << FlexIO_8080_Inst.TmrIdx)))
    {
    }

    /* De-assert CS pin */
    flexio_8080_pins_handler.Set_CS_Pin(true);
}

/*!
 * @brief Write command with multi parameters in single-beat writing mode using polling method.
 */
void FLEXIO_8080_SglBeatWR_nPrm(uint32_t const cmdIdx, uint16_t const * buffer,
                                                       uint32_t const length)
{
    /* Configure RD pin as GPIO function, WR pin as FlexIO function */
    flexio_8080_pins_handler.Config_RD_GPIO();
    flexio_8080_pins_handler.Config_WR_GPIO();

    /* Configure FlexIO with 1-beat write configuration */
    FLEXIO_8080_ConfigSglBeatWR(&SglBeatWR_Reg_Config);
    flexio_8080_pins_handler.Config_WR_FlexIO();

    /* Assert CS, RS pins */
    flexio_8080_pins_handler.Set_CS_Pin(false);
    flexio_8080_pins_handler.Set_RS_Pin(false);

    /* Write command index */
    FLEXIO0->SHIFTBUF[FlexIO_8080_Inst.SglBeatWR_ShfIdx] = cmdIdx;
    

    /*Wait for transfer to be completed */
    while(0 == (FLEXIO0->TIMSTAT & (1U << FlexIO_8080_Inst.TmrIdx)))
    {
    }

    /* De-assert RS pin */
    flexio_8080_pins_handler.Set_RS_Pin(true);

    if(length)
    {
        /* Use polling method for data transfer */
        for(uint32_t i=0; i<length-1U; i++)
        {
            while(0 == (FLEXIO0->SHIFTSTAT & (1U << FlexIO_8080_Inst.SglBeatWR_ShfIdx)))//wait for buffer empty
            {
            }
            FLEXIO0->SHIFTBUF[FlexIO_8080_Inst.SglBeatWR_ShfIdx] = *buffer++;
        }
        /* Write the last byte */
        while(0 == (FLEXIO0->SHIFTSTAT & (1U << FlexIO_8080_Inst.SglBeatWR_ShfIdx)))
        {
        }
        FLEXIO0->TIMSTAT |= (1U << FlexIO_8080_Inst.TmrIdx);
        FLEXIO0->SHIFTBUF[FlexIO_8080_Inst.SglBeatWR_ShfIdx] = *buffer++;
        /*Wait for transfer to be completed */
        while(0 == (FLEXIO0->TIMSTAT & (1U << FlexIO_8080_Inst.TmrIdx)))
        {
        }
    }

    /* De-assert CS pin */
    flexio_8080_pins_handler.Set_CS_Pin(true);
}

/*!
 * @brief Write command with multi repeated parameters in single-beat writing mode using polling method.
 * It is used to fill LCD panel with solid color.
 */
 void FLEXIO_8080_SglBeatWR_nSamePrm(uint32_t const cmdIdx, uint16_t const value,
                                                           uint32_t const length)
{
    /* Configure RD pin as GPIO function, WR pin as FlexIO function */
    flexio_8080_pins_handler.Config_RD_GPIO();
    flexio_8080_pins_handler.Config_WR_GPIO();

    /* Configure FlexIO with 1-beat write configuration */
    FLEXIO_8080_ConfigSglBeatWR(&SglBeatWR_Reg_Config);
    flexio_8080_pins_handler.Config_WR_FlexIO();

    /* Assert CS, RS pins */
    flexio_8080_pins_handler.Set_CS_Pin(false);
    flexio_8080_pins_handler.Set_RS_Pin(false);

    /* Write command index */
    FLEXIO0->SHIFTBUF[FlexIO_8080_Inst.SglBeatWR_ShfIdx] = cmdIdx;

    /*Wait for transfer to be completed */
    while(0 == (FLEXIO0->TIMSTAT & (1U << FlexIO_8080_Inst.TmrIdx)))
    {
    }

    /* De-assert RS pin */
    flexio_8080_pins_handler.Set_RS_Pin(true);

    if(length)
    {
        /* Use polling method for data transfers */
        for(uint32_t i=0; i<length-1U; i++)
        {

            while(0 == (FLEXIO0->SHIFTSTAT & (1U << FlexIO_8080_Inst.SglBeatWR_ShfIdx)))
            {
            }
            GPIO4->PSOR |=(1U << 0);
            FLEXIO0->SHIFTBUF[FlexIO_8080_Inst.SglBeatWR_ShfIdx] = value;
            GPIO4->PCOR |=(1U << 0);

        }

        /* Write the last byte */
        while(0 == (FLEXIO0->SHIFTSTAT & (1U << FlexIO_8080_Inst.SglBeatWR_ShfIdx)))
        {
        }
        FLEXIO0->TIMSTAT |= (1U << FlexIO_8080_Inst.TmrIdx);
        FLEXIO0->SHIFTBUF[FlexIO_8080_Inst.SglBeatWR_ShfIdx] = value;

        /*Wait for transfer to be completed */
        while(0 == (FLEXIO0->TIMSTAT & (1U << FlexIO_8080_Inst.TmrIdx)))
        {
        }
    }

    /* De-assert CS pin */
    flexio_8080_pins_handler.Set_CS_Pin(true);
}



/*!
 * @brief callback function for FLEXIO_8080_MulBeatWR_nPrm()
 * It is called in the DMA ISR, used to complete the transfer.
 */
static void FLEXIO_8080_MulBeatWR_Callback(void)
{
    LED_RED_TOGGLE();
    while(0 == (FLEXIO0->SHIFTSTAT & (1U << FlexIO_8080_Inst.MulBeatWR_ShfIdx[1])))
    {
    }

    /* Wait the last multi-beat transfer to be completed. Clear the timer flag
    before the completing of the last beat. The last beat may has been completed
    at this point, then code would be dead in the while() below. So mask the
    while() statement and use the software delay .*/
    FLEXIO0->TIMSTAT |= (1U << FlexIO_8080_Inst.TmrIdx);

#if 0
    /* Wait timer flag to be set to ensure the completing of the last beat. */
    while(0 == (FLEXIO0->TIMSTAT & (1U << FlexIO_8080_Inst.TmrIdx)))
    {
    }
#else
    /* Wait some time to ensure the completing of the last beat. */
    for(uint32_t i=0; i<50U; i++)
    {
    }
#endif

    if(MulBeatCountRemain)
    {
        /* Configure FlexIO with 1-beat write configuration */
        FLEXIO_8080_ConfigSglBeatWR(&SglBeatWR_Reg_Config);

        /* Use polling method for data transfer */
        for(uint32_t i=0; i<MulBeatCountRemain-1U; i++)
        {
            while(0 == (FLEXIO0->SHIFTSTAT & (1U << FlexIO_8080_Inst.SglBeatWR_ShfIdx)))
            {
            }
            FLEXIO0->SHIFTBUF[FlexIO_8080_Inst.SglBeatWR_ShfIdx] = *MulBeatDataRemain++;
        }

        /* Write the last byte */
        while(0 == (FLEXIO0->SHIFTSTAT & (1U << FlexIO_8080_Inst.SglBeatWR_ShfIdx)))
        {
        }
        FLEXIO0->TIMSTAT |= (1U << FlexIO_8080_Inst.TmrIdx);
        FLEXIO0->SHIFTBUF[FlexIO_8080_Inst.SglBeatWR_ShfIdx] = *MulBeatDataRemain++;

        /*Wait for transfer to be completed */
        while(0 == (FLEXIO0->TIMSTAT & (1U << FlexIO_8080_Inst.TmrIdx)))
        {
        }
    }
    
    /*Wait for transfer to be completed */
 //Daniel Check why this cause problems   while(0 == (FLEXIO0->TIMSTAT & (1U << FlexIO_8080_Inst.TmrIdx)))
    {
    }
    /* De-assert CS pin */
    flexio_8080_pins_handler.Set_CS_Pin(true);

    WR_DMATransferDone = true;
}


/*!
 * @brief Write command with multi parameters in multi-beat writing mode using DMA method.
 * The total transfer size is configured in the DMA configuration function.
 */
void FLEXIO_8080_MulBeatWR_nPrm(uint32_t const cmdIdx, uint16_t const * buffer,
                                                        uint32_t const length)
{
    uint32_t BeatsPerMinLoop = FlexIO_8080_Inst.MulBeatWR_ShfQty * 2U;
    edma_user_config_t edma_user_config;

    if(length < BeatsPerMinLoop)
    {
        FLEXIO_8080_SglBeatWR_nPrm(cmdIdx, buffer, length);
    }
    else
    {
        MulBeatCountRemain = length % BeatsPerMinLoop;
        MulBeatDataRemain = (uint16_t *)buffer + (length - MulBeatCountRemain);

        edma_user_config.Buffer = (void *)buffer;
        edma_user_config.TotalSize = (length - MulBeatCountRemain) << 1U;       /* in bytes */

        if(DMAFillColorMode){ 
          flexio_8080_mulbeat_dma_config((uint32_t)&buffer[0], (uint32_t)&FLEXIO0->SHIFTBUF[0], length*2, true );
        }else{
          flexio_8080_mulbeat_dma_config((uint32_t)&buffer[0], (uint32_t)&FLEXIO0->SHIFTBUF[0], length*2, false );

        }
        /* Configure RD pin as GPIO function, WR pin as FlexIO function */
        flexio_8080_pins_handler.Config_RD_GPIO();
        flexio_8080_pins_handler.Config_WR_GPIO();
        
        /* Configure FlexIO with 1-beat write configuration */
        FLEXIO_8080_ConfigSglBeatWR(&SglBeatWR_Reg_Config);
        flexio_8080_pins_handler.Config_WR_FlexIO();

        /* Assert CS, RS pins */
        flexio_8080_pins_handler.Set_CS_Pin(false);
        flexio_8080_pins_handler.Set_RS_Pin(false);

        /* Write command index */
        FLEXIO0->SHIFTBUF[FlexIO_8080_Inst.SglBeatWR_ShfIdx] = cmdIdx;

        /*Wait for transfer to be completed*/
        while(0 == (FLEXIO0->TIMSTAT & (1U << FlexIO_8080_Inst.TmrIdx)))
        {
        }

        /* De-assert RS pin */
        flexio_8080_pins_handler.Set_RS_Pin(true);



        flexio_8080_pins_handler.Config_WR_GPIO();

        /* Configure FlexIO with multi-beat write configuration */
        FLEXIO_8080_ConfigMulBeatWR(&MulBeatWR_Reg_Config);
        flexio_8080_pins_handler.Config_WR_FlexIO();

        FLEXIO0->SHIFTSDEN |= 1U << FlexIO_8080_Inst.MulBeatWR_ShfIdx[1];
         /* Start data transfer by using DMA */
        WR_DMATransferDone = false;
        pDMAIntCallback = FLEXIO_8080_MulBeatWR_Callback;
        FLEXIO_8080_MulBeatWR_DMA_Enable(true);
        
    }
}

 /*!
 * @brief Initialize FlexIO for this demo.
 */
void Demo_FLEXIO_8080_Init(void)
{

    CLOCK_EnableClock(kCLOCK_Dma0);

    CLOCK_AttachClk(kPLL0_to_FLEXIO);
    CLOCK_EnableClock(kCLOCK_Flexio);

	#ifdef LCD_ST7796S_IPS
    	CLOCK_SetClkDiv(kCLOCK_DivFlexioClk, 8u);
	#else
		#ifdef LCD_ST7796S_TFT
    		CLOCK_SetClkDiv(kCLOCK_DivFlexioClk, 8u);
		#else
			#ifdef LCD_SSD1963
    			CLOCK_SetClkDiv(kCLOCK_DivFlexioClk, 3u);
			#endif
		#endif
	#endif


    /* Setup FlexIO 8080 pins handler functions */
    pflexio_8080_pins_handler->Config_Data_Pin = FLEXIO_8080_Config_Data_Pin;
    pflexio_8080_pins_handler->Config_WR_FlexIO = FLEXIO_8080_Config_WR_FlexIO;
    pflexio_8080_pins_handler->Config_WR_GPIO = FLEXIO_8080_Config_WR_GPIO;
    pflexio_8080_pins_handler->Config_RD_FlexIO = FLEXIO_8080_Config_RD_FlexIO;
    pflexio_8080_pins_handler->Config_RD_GPIO = FLEXIO_8080_Config_RD_GPIO;
    pflexio_8080_pins_handler->Config_CS_GPIO = FLEXIO_8080_Config_CS_GPIO;
    pflexio_8080_pins_handler->Set_CS_Pin = FLEXIO_8080_Set_CS_Pin;
    pflexio_8080_pins_handler->Config_RS_GPIO = FLEXIO_8080_Config_RS_GPIO;
    pflexio_8080_pins_handler->Set_RS_Pin = FLEXIO_8080_Set_RS_Pin;
    
    pflexio_8080_pins_handler->Config_ReSet_GPIO = FLEXIO_8080_Config_ReSet_GPIO;
    pflexio_8080_pins_handler->Set_ReSet_Pin = FLEXIO_8080_Set_ReSet_Pin;
    
    FLEXIO_8080_Type_GetDefaultConfig();

    pFlexIO_8080_Inst->Data0PinIdx = FLEXIO_DATA0_FXIOD_INDEX;
    pFlexIO_8080_Inst->WRPinIdx = FLEXIO_WR_PIN_FXIOD_INDEX;
    pFlexIO_8080_Inst->RDPinIdx = FLEXIO_RD_PIN_FXIOD_INDEX;

    FLEXIO_8080_Init();
}

/* EOF */
