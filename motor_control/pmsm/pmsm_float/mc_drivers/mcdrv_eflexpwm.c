/*
 * Copyright 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2021 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "mcdrv_eflexpwm.h"
#include "mc_periph_init.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

static bool_t s_statusPass;

/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * @brief Function updates FTM value register
 *
 * @param this   Pointer to the current object
 * TODO - Deadtime compensation?
 * @return none
 */
void MCDRV_eFlexPwm3PhSet(mcdrv_eflexpwm_t *this)
{
    PWM_Type * const pCurrentPwm = this->pui32PwmBaseAddress;

    GMCLIB_3COOR_T_F16 sUABCtemp;

    int16_t w16ModuloHalf;
    int16_t w16PwmValPhA;
    int16_t w16PwmValPhB;
    int16_t w16PwmValPhC;

    /* pointer to duty cycle structure */
    sUABCtemp = *this->psUABC;

    /* Get PWM_modulo/2 from PWM register, always correct regardless of PWM runtime setting, variable used for update of duty cycles of all 3 phases */
    w16ModuloHalf = (pCurrentPwm->SM[0].VAL1+1);

    /* Phase A - duty cycle calculation */
    w16PwmValPhA = (w16ModuloHalf * sUABCtemp.f16A) >> 15;
    pCurrentPwm->SM[0].VAL2 = (uint16_t)(-w16PwmValPhA); // rising edge value register update
    pCurrentPwm->SM[0].VAL3 = (uint16_t)w16PwmValPhA;  // falling edge value register update, no need to calculate it

    /* Phase B - duty cycle calculation */
    w16PwmValPhB = (w16ModuloHalf * sUABCtemp.f16B) >> 15;
    pCurrentPwm->SM[1].VAL2 = (uint16_t)(-w16PwmValPhB); // rising edge value register update
    pCurrentPwm->SM[1].VAL3 = (uint16_t)w16PwmValPhB; // falling edge value register update, no need to calculate it

    /* Phase C - duty cycle calculation */
    w16PwmValPhC = (w16ModuloHalf * sUABCtemp.f16C) >> 15;
    pCurrentPwm->SM[2].VAL2 = (uint16_t)(-w16PwmValPhC); // rising edge value register update
    pCurrentPwm->SM[2].VAL3 = (uint16_t)w16PwmValPhC; // falling edge value register update, no need to calculate it

    pCurrentPwm->MCTRL |= PWM_MCTRL_LDOK(15);

}

/*!
 * @brief Function enables PWM outputs
 *
 * @param this   Pointer to the current object
 *
 * @return none
 */
void MCDRV_eFlexPwm3PhOutEn(mcdrv_eflexpwm_t *this)
{
    PWM_Type * const pCurrentPwm = this->pui32PwmBaseAddress;

    pCurrentPwm->SM[0].VAL2 = 0U;
    pCurrentPwm->SM[1].VAL2 = 0U;
    pCurrentPwm->SM[2].VAL2 = 0U;
    pCurrentPwm->SM[3].VAL2 = 0U;

    pCurrentPwm->SM[0].VAL3 = 0U;
    pCurrentPwm->SM[1].VAL3 = 0U;
    pCurrentPwm->SM[2].VAL3 = 0U;
    pCurrentPwm->SM[3].VAL3 = 0U;

    /* PWM module trigger on VAL4 enabled for ADC synchronization */
    pCurrentPwm->SM[0].VAL4 = (uint16_t)(-(M1_PWM_MODULO/2));

    /* Clear fault flag (we're in safe mode so the PWM won't run if there's an error condition */
    pCurrentPwm->FSTS |= PWM_FSTS_FFLAG(1U);

    /* Start PWMs (set load OK flags and run) */
    pCurrentPwm->MCTRL |= PWM_MCTRL_CLDOK(15);
    pCurrentPwm->MCTRL |= PWM_MCTRL_LDOK(15);
    pCurrentPwm->MCTRL |= PWM_MCTRL_RUN(15);

    /* Enable A&B (Top & Bottm) PWM outputs of submodules one, two and three */
    pCurrentPwm->OUTEN |= PWM_OUTEN_PWMA_EN(0xF);
    pCurrentPwm->OUTEN |= PWM_OUTEN_PWMB_EN(0xF);

    /* Enable & setup interrupt from PWMA */
    if( PWM0 == (pCurrentPwm) )
    {
            NVIC_SetPriority(FLEXPWM0_SUBMODULE0_IRQn, 0U);
            NVIC_EnableIRQ(FLEXPWM0_SUBMODULE2_IRQn);
    }
    else
    {
            NVIC_SetPriority(FLEXPWM1_SUBMODULE0_IRQn, 0U);
            NVIC_EnableIRQ(FLEXPWM1_SUBMODULE0_IRQn);
    }

}

/*!
 * @brief Function disables PWM outputs
 *
 * @param this   Pointer to the current object
 *
 * @return none
 */
void MCDRV_eFlexPwm3PhOutDis(mcdrv_eflexpwm_t *this)
{
    PWM_Type * const pCurrentPwm = this->pui32PwmBaseAddress;

    /* Disable A&B (Top & Bottm) PWM outputs of submodules one, two and three */
    pCurrentPwm->OUTEN &= (~PWM_OUTEN_PWMA_EN(0xF));
    pCurrentPwm->OUTEN &= (~PWM_OUTEN_PWMB_EN(0xF));

}

/*!
 * @brief Function initialite PWM outputs structure
 *
 * @param this   Pointer to the current object
 *
 * @return none
 */
void MCDRV_eFlexPwm3PhOutInit(mcdrv_eflexpwm_t *this)
{

}

/*!
 * @brief Function return actual value of over current flag
 *
 * @param this   Pointer to the current object
 *
 * @return boot_t true on success
 */
bool_t MCDRV_eFlexPwm3PhFltGet(mcdrv_eflexpwm_t *this)
{
    /* read over-current flags */
//    s_statusPass = (((this->pui32PwmBaseAddress->FSTS & PWM_FSTS_FFPIN_MASK) >> 8) &
//                    (1 << this->ui16FaultFixNum | 1 << this->ui16FaultAdjNum));
  
    s_statusPass = (((0U) >> 8) &
                    (1 << this->ui16FaultFixNum | 1 << this->ui16FaultAdjNum));
  
    /* clear faults flag */
    this->pui32PwmBaseAddress->FSTS = ((this->pui32PwmBaseAddress->FSTS & ~(uint16_t)(PWM_FSTS_FFLAG_MASK)) |
                                       (1 << this->ui16FaultFixNum | 1 << this->ui16FaultAdjNum));

    return ((s_statusPass > 0));
}

void MCDRV_eFlexPwm3PhFltTryClr(mcdrv_eflexpwm_t *this)
{
    PWM_Type * const pCurrentPwm = this->pui32PwmBaseAddress;

    /* We can clear the FFLAGs only if the respective FFPIN (raw fault input) isn't set. */
    const uint8_t u8FfpinNoErrorMask =
                    (uint8_t)(~(((pCurrentPwm->FSTS) & PWM_FSTS_FFLAG_MASK) >> PWM_FSTS_FFLAG_SHIFT));

    pCurrentPwm->FSTS |= PWM_FSTS_FFLAG(u8FfpinNoErrorMask);
}
