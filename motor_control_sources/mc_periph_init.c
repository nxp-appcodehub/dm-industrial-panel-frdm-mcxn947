/*
 * Copyright 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2021 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "mc_periph_init.h"
#include "peripherals.h"
#include "fsl_common.h"
#include "fsl_gpio.h"
#include "fsl_inputmux.h"
#include "fsl_port.h"

/*******************************************************************************
 * Defines
 ******************************************************************************/

/*******************************************************************************
 * Typedef
 ******************************************************************************/

typedef struct AdcPinProperties_t
{
	ADC_Type * const pToAdcBase;

	const uint8_t u8AdcChannel;
	const lpadc_sample_channel_mode_t sampleChannelMode;
} AdcPinProperties_t;

typedef struct AdcInitData_t
{
    ADC_Type * const PtoAdc;
    const uint32_t u32ClockDivider;
    const clock_div_name_t ClockDivName;
    const clock_attach_id_t ClockAttachId;
} AdcInitData_t;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void InitFlexPWM1(void);
static void InitAdc1(void);
static void InitSlowLoop(void);
static void InitClock(void);
static void InitInputmux(void);
static void InitVref(void);
static void InitQD1(void);

/*******************************************************************************
 * Constants
 ******************************************************************************/

/* Set the VDDA as the LPADC reference voltage source */
static const lpadc_reference_voltage_source_t DEMO_LPADC_VREF_SOURCE =
    kLPADC_ReferenceVoltageAlt3;

/* Configuration of all ADC chanels to be set-up */
static const AdcPinProperties_t AdcPinsProperties[] =
{
    { ADC1, 0U, kLPADC_SampleChannelSingleEndSideB },   // CURA
    { ADC1, 10U, kLPADC_SampleChannelSingleEndSideB },  // CURB
    { ADC1, 11U, kLPADC_SampleChannelSingleEndSideB },  // CURC
    { ADC1, 12U, kLPADC_SampleChannelSingleEndSideB },  // DCBV
};

/* ADC instances init data */
static const AdcInitData_t AdcInitData[] =
{
    { ADC0, 11U, kCLOCK_DivAdc0Clk, kPLL0_to_ADC0 }, // (150 / (11+1)) = 12.5 MHz
    { ADC1, 11U, kCLOCK_DivAdc1Clk, kPLL0_to_ADC1 }  // (150 / (11+1)) = 12.5 MHz
};

/*******************************************************************************
 * Variables
 ******************************************************************************/
/* configuration structure for 3-phase PWM mc driver */
mcdrv_eflexpwm_t g_sM1Pwm3ph;

/* structure for current and voltage measurement*/
mcdrv_adc_t g_sM1AdcSensor;

/* Structure for Encoder driver */
mcdrv_qd_enc_t g_sM1Enc;

/* Clock setup structure */
clock_setup_t g_sClockSetup;


/*******************************************************************************
 * Public functions
 ******************************************************************************/

/*!
 * @brief   void MCDRV_Init_M1(void)
 *           - Motor control driver main initialization
 *           - Calls initialization functions of peripherals required for motor
 *             control functionality
 *
 * @param   void
 *
 * @return  none
 */
void MCDRV_Init_M1(void)
{
    /* Init application clock dependent variables */
    InitClock();
    
    /* Init slow loop counter */
    InitSlowLoop();
    
    /* Init signal multiplexing */
    InitInputmux();             

    /* Init Vref */
    InitVref();
    
    /* 6-channel PWM peripheral init for M1 */
    InitFlexPWM1();
    
    /* Init ADC */
    InitAdc1();
    
    /* Qudrature decoder peripheral init */
    InitQD1();
}


/*******************************************************************************
 * Local functions
 ******************************************************************************/

/*!
 * @brief   void InitPWM1(void)
 *           - Initialization of the eFlexPWM1 peripheral for motor M1
 *           - 3-phase center-aligned PWM
 *
 * @param   void
 *
 * @return  none
 */
static void InitFlexPWM1()
{
    uint32_t u32TmpRegisterContent;
    uint8_t u8SubmoduleIndex;

    // Enable eFlexPWM1 AHB clock
    CLOCK_EnableClock(kCLOCK_Pwm1);
    
    // Enable Submodules 0 - 2 clocks
    SYSCON->PWM1SUBCTL |= SYSCON_PWM1SUBCTL_CLK0_EN_MASK |
                          SYSCON_PWM1SUBCTL_CLK1_EN_MASK |
                          SYSCON_PWM1SUBCTL_CLK2_EN_MASK;

    /* Init SMs 0 - 2 *********************************************************/
    
    for( u8SubmoduleIndex = 0U; u8SubmoduleIndex < (ARRAY_SIZE(PWM1->SM) - 1U) ; ++u8SubmoduleIndex )
    {
        PWM1->SM[u8SubmoduleIndex].INIT = PWM_INIT_INIT((uint16_t)(-(M1_PWM_MODULO / 2)));
        PWM1->SM[u8SubmoduleIndex].VAL0 = 0U;
        PWM1->SM[u8SubmoduleIndex].VAL1 = PWM_VAL1_VAL1((uint16_t)((M1_PWM_MODULO / 2) - 1));
        PWM1->SM[u8SubmoduleIndex].VAL2 = 0U;
        PWM1->SM[u8SubmoduleIndex].VAL3 = 0U;

        /* Reload is generated at every opportunity */
        PWM1->SM[u8SubmoduleIndex].CTRL |= PWM_CTRL_LDFQ(0);

        /* Full cycle reload */
        PWM1->SM[u8SubmoduleIndex].CTRL |= PWM_CTRL_FULL_MASK;
        
        /* Setup SM1 & SM2 to be driven by SM0 signals */
        if( u8SubmoduleIndex > 0U )
        {
            u32TmpRegisterContent = PWM1->SM[u8SubmoduleIndex].CTRL2;
          
            /* Set SM1 & SM2 to be clocked by SM0 */
            u32TmpRegisterContent &= (~PWM_CTRL2_CLK_SEL_MASK);
            u32TmpRegisterContent |= (PWM_CTRL2_CLK_SEL(0x02U));
            
            /* Master reload signal from SM0 is used to reload registers of SM1 & SM2*/
            u32TmpRegisterContent |= PWM_CTRL2_RELOAD_SEL_MASK;
            
            /* Master sync from SM0 causes initialization of SM1 & SM2 */
            u32TmpRegisterContent &= (~PWM_CTRL2_INIT_SEL_MASK);
            u32TmpRegisterContent |= PWM_CTRL2_INIT_SEL(0x02U);
            
            PWM1->SM[u8SubmoduleIndex].CTRL2 = u32TmpRegisterContent;
        }

        /* Set initial deadtime */
        PWM1->SM[u8SubmoduleIndex].DTCNT0 = ((M1_PWM_DEADTIME * (MCU_CLOCK_FREQ / 1000000U)) / 1000U);
        PWM1->SM[u8SubmoduleIndex].DTCNT1 = ((M1_PWM_DEADTIME * (MCU_CLOCK_FREQ / 1000000U)) / 1000U);

        /* Setup fault 0 - 2 (CMPs 0 - 2 outputs) trigger */
        PWM1->SM[u8SubmoduleIndex].DISMAP[0] = 0xF000U;
        
    }    

    /* PWM1 module 0 trigger on VAL4 enabled for ADC synchronization */
    PWM1->SM[0].VAL4 = PWM_VAL4_VAL4((uint16_t)((-(M1_PWM_MODULO / 2))));
    PWM1->SM[0].TCTRL |= PWM_TCTRL_OUT_TRIG_EN(1 << 4);
    
    /* PWM outputs are re-enabled at PWM full cycle */
    PWM1->FSTS |= PWM_FSTS_FFULL(0x07U);
    
    /* PWM fault filter - 3 Fast periph. clocks sample rate,
       2 clock cycles filter period */
    PWM1->FFILT = (PWM1->FFILT & ~PWM_FFILT_FILT_PER_MASK) | PWM_FFILT_FILT_PER(2U);
    
    /* Setup PWM fault control register */
    u32TmpRegisterContent = PWM1->FCTRL;
    
    u32TmpRegisterContent &= ~(PWM_FCTRL_FLVL_MASK | PWM_FCTRL_FAUTO_MASK | PWM_FCTRL_FSAFE_MASK | PWM_FCTRL_FIE_MASK);
    u32TmpRegisterContent |= PWM_FCTRL_FIE(0U);         /* FAULT 0 & FAULT 1 - Interrupt disable */
    u32TmpRegisterContent |= PWM_FCTRL_FLVL(0U);        /* A logic 0 on the fault input indicates a fault condition */
    u32TmpRegisterContent |= PWM_FCTRL_FAUTO(0U);       /* Manual fault clearing */
    u32TmpRegisterContent |= PWM_FCTRL_FSAFE(0x7U);     /* Outputs 0 - 2 set to safe mode */
    
    PWM1->FCTRL = u32TmpRegisterContent;
    
    /* Clear all fault flags */
    PWM1->FSTS |= PWM_FSTS_FFLAG(0x07U);

    /* Start PWM1 SMs 0-2 (set load OK flags and run) */
    PWM1->MCTRL |= PWM_MCTRL_CLDOK(0x07U);
    PWM1->MCTRL |= PWM_MCTRL_LDOK(0x07U);
    PWM1->MCTRL |= PWM_MCTRL_RUN(0x07U);

    /* Enable  PWM0 "A" and "B" output of SMs 0-2,*/
    PWM1->OUTEN |= PWM_OUTEN_PWMA_EN(0x07U);
    PWM1->OUTEN |= PWM_OUTEN_PWMB_EN(0x07U);
    
    /* eFlexPWM base address */
    g_sM1Pwm3ph.pui32PwmBaseAddress = PWM1;
    g_sM1Pwm3ph.ui16FaultFixNum = M1_FAULT_NUM; /* PWMA fixed-value over-current fault number */
    g_sM1Pwm3ph.ui16FaultAdjNum = M1_FAULT_NUM; /* PWMA adjustable over-current fault number */
}


/*!
 * @brief   void InitADC(void)
 *           - Initialization of the ADC peripheral
 *
 * @param   void
 *
 * @return  none
 */
static void InitAdc1(void)
{
    const AdcInitData_t * const pCurrentInitData = &(AdcInitData[1U]);
  
    lpadc_conv_trigger_config_t lpadcTriggerConfig;
    lpadc_conv_command_config_t lpadcCommandConfig;
    lpadc_config_t lpadcConfig;
    uint32_t u32AdcPropertyIndex;
    
    // Init the lpadcConfig struct
    LPADC_GetDefaultConfig(&lpadcConfig);
    lpadcConfig.enableAnalogPreliminary = true;
    lpadcConfig.referenceVoltageSource = DEMO_LPADC_VREF_SOURCE;
    lpadcConfig.conversionAverageMode = kLPADC_ConversionAverage128;
    
    /* Init ADCS */
    uint32_t u32CommandIndex = 0U;

    /* Set clocks */
    CLOCK_SetClkDiv(pCurrentInitData->ClockDivName, pCurrentInitData->u32ClockDivider);
    CLOCK_AttachClk(pCurrentInitData->ClockAttachId);

    LPADC_Init(pCurrentInitData->PtoAdc, &lpadcConfig);

    LPADC_DoOffsetCalibration(pCurrentInitData->PtoAdc);
    LPADC_DoAutoCalibration(pCurrentInitData->PtoAdc);

    /* Init commands */
    for( u32AdcPropertyIndex = 0U; u32AdcPropertyIndex < ARRAY_SIZE(AdcPinsProperties); ++u32AdcPropertyIndex )
    {
        const AdcPinProperties_t * const pCurrentAdcPinProperty = &(AdcPinsProperties[u32AdcPropertyIndex]);

        if( pCurrentAdcPinProperty->pToAdcBase == (pCurrentInitData->PtoAdc) )
        {  
            LPADC_GetDefaultConvCommandConfig(&lpadcCommandConfig);

            lpadcCommandConfig.channelNumber = pCurrentAdcPinProperty->u8AdcChannel;
            lpadcCommandConfig.sampleChannelMode = pCurrentAdcPinProperty->sampleChannelMode;
            lpadcCommandConfig.conversionResolutionMode = kLPADC_ConversionResolutionStandard;
            lpadcCommandConfig.sampleTimeMode = kLPADC_SampleTimeADCK131;

            /* Commands are one-based! Plus this then serves as the last current ADC-related command index. */
            ++u32CommandIndex;
            lpadcCommandConfig.chainedNextCommandNumber = u32CommandIndex + 1U;

            LPADC_SetConvCommandConfig( pCurrentInitData->PtoAdc, u32CommandIndex, &lpadcCommandConfig );
        }
    }

    /* Init triggers (use trigger 0). */
    LPADC_GetDefaultConvTriggerConfig(&lpadcTriggerConfig);
    lpadcTriggerConfig.targetCommandId = 1U;
    lpadcTriggerConfig.enableHardwareTrigger = true;
    LPADC_SetConvTriggerConfig(pCurrentInitData->PtoAdc, 0U, &lpadcTriggerConfig);
    
    /* Enable TCOMP interrupt. */
    LPADC_EnableInterrupts(ADC1, ADC_IE_TCOMP_IE(0xFU));
    NVIC_SetPriority(ADC1_IRQn, 0U);
    NVIC_EnableIRQ(ADC1_IRQn);
    
    /* ADC1 base address */
    g_sM1AdcSensor.pToAdcBase = pCurrentInitData->PtoAdc;
}


/*!
 * @brief   void InitSlowLoop(void)
 *           - Initialization of the CTIMER0 peripheral
 *           - performs slow control loop counter
 *
 * @param   void
 *
 * @return  none
 */
static void InitSlowLoop(void)
{
    /* Use 96 MHz clock for some of the Ctimer0. */
    CLOCK_SetClkDiv(kCLOCK_DivCtimer0Clk, 1u);
    CLOCK_AttachClk(kFRO_HF_to_CTIMER0);
    
    /* Enable Timer0 clock. */
    SYSCON->AHBCLKCTRLSET[1] |= SYSCON_AHBCLKCTRL1_TIMER0_MASK;

    /* Enable Timer0 clock reset. */
    SYSCON->PRESETCTRLSET[1] = SYSCON_PRESETCTRL1_TIMER0_RST_MASK;             /* Set bit. */
    while (0u == (SYSCON->PRESETCTRL1 & SYSCON_PRESETCTRL1_TIMER0_RST_MASK))   /* Wait until it reads 0b1 */
    {
    }
    
    /* Clear Timer0 clock reset. */                                  
    SYSCON->PRESETCTRLCLR[1] = SYSCON_PRESETCTRL1_TIMER0_RST_MASK;             /* Clear bit */
    while (SYSCON_PRESETCTRL1_TIMER0_RST_MASK ==                               /* Wait until it reads 0b0 */     
          (SYSCON->PRESETCTRL1 & SYSCON_PRESETCTRL1_TIMER0_RST_MASK))
    {
    }

    /* Configure match control register. */
    CTIMER0->MCR |= CTIMER_MCR_MR0R(1U)  |   /* Enable reset of TC after it matches with MR0. */
                    CTIMER_MCR_MR0I(1U);     /* Enable interrupt generation after TC matches with MR0. */
    
    /* Configure match register. */
    CTIMER0->MR[0] = (CLOCK_GetFreq(kCLOCK_FroHf))  /* Get CTimer0 frequency for correct set Match register value. */
                     / M1_SLOW_LOOP_FREQ;           /* Set slow control loop frequency in Hz. */
    
    /* Configure interrupt register. */
    CTIMER0->IR = CTIMER_IR_MR0INT_MASK;     /* Set interrupt flag for match channel 0. */
    NVIC_SetPriority(CTIMER0_IRQn, 1U);
    NVIC_EnableIRQ(CTIMER0_IRQn);            /* Enable LEVEL1 interrupt and update the call back function. */

    /* Configure timer control register. */
    CTIMER0->TCR |= CTIMER_TCR_CEN_MASK;     /* Start the timer counter. */
}


/*!
* @brief   void InitClock(void)
*          - Core, bus, flash clock setup
*
* @param   void
*
* @return  none
*/
static void InitClock(void)
{
    uint32_t ui32CyclesNumber = 0U;

    /* Calculate clock dependant variables for PMSM control algorithm */
    g_sClockSetup.ui32FastPeripheralClock = CLOCK_GetFreq(kCLOCK_CoreSysClk);
    g_sClockSetup.ui32CpuFrequency = CLOCK_GetFreq(kCLOCK_CoreSysClk);

    /* Parameters for motor */
    g_sClockSetup.ui16M1PwmFreq   = M1_PWM_FREQ; /* 10 kHz */
    g_sClockSetup.ui16M1PwmModulo = (g_sClockSetup.ui32FastPeripheralClock) / g_sClockSetup.ui16M1PwmFreq;
    ui32CyclesNumber = ((M1_PWM_DEADTIME * (g_sClockSetup.ui32FastPeripheralClock / 1000000U)) / 1000U);
    g_sClockSetup.ui16M1PwmDeadTime   = ui32CyclesNumber;
    g_sClockSetup.ui16M1SpeedLoopFreq = M1_SLOW_LOOP_FREQ; /* 1kHz */
}


/*!
 * @brief   void InitInputmux(void)
 *           - Initialization of the Input Multiplexing
 *
 * @param   void
 *
 * @return  none
 */
static void InitInputmux(void)
{
    //CLOCK_EnableClock(kCLOCK_InputMux);

    INPUTMUX->ADC1_TRIG[0] = 32U;   // ADC1 trigger 0 <- PWM1_SM0_MUX_TRIG0
}


/*!
 * @brief   void InitVref(void)
 *
 * @param   void
 *
 * @return  none
 */
static void InitVref(void)
{
    // Power up the VREF
    //SPC0->ACTIVE_CFG1 |= 0x01U;
    
    // Enable VREF clock
    //CLOCK_EnableClock(kCLOCK_Vref);

    /* Trim 1.0 V to 1.625 V */
    VREF0->UTRIM = 0x06U;  // Coarse tune to 1.6 V
    VREF0->UTRIM |= (0x3FU << 8U) ; //Fine tune to 1.625 V

    VREF0->CSR = VREF_CSR_BUF21EN_MASK      |
                 VREF_CSR_LPBG_BUF_EN_MASK  |
                 VREF_CSR_CHOPEN_MASK       |
                 VREF_CSR_ICOMPEN_MASK      |
                 VREF_CSR_HI_PWR_LV_MASK    |
                 VREF_CSR_REGEN_MASK        |
                 VREF_CSR_LPBGEN_MASK       |
                 VREF_CSR_HCBGEN_MASK;
}


/*!
 * @brief   void InitQD1(void)
 *           - Initialization of the Quadrature Encoder 1 peripheral
 *           - performs speed and position sensor processing
 *
 * @param   void
 *
 * @return  none
 */
static void InitQD1(void)
{
    const port_pin_config_t encTriggerPortConfig =
    {   /* Internal pull-up/down resistor is disabled */
        kPORT_PullDisable,
        /* Low internal pull resistor value is selected. */
        kPORT_LowPullResistor,
        /* Fast slew rate is configured */
        kPORT_FastSlewRate,
        /* Passive input filter is disabled */
        kPORT_PassiveFilterDisable,
        /* Open drain output is disabled */
        kPORT_OpenDrainDisable,
        /* Low drive strength is configured */
        kPORT_LowDriveStrength,
        /* Pin is configured as TRIG_INx */
        kPORT_MuxAlt1,
        /* Digital input enabled */
        kPORT_InputBufferEnable,
        /* Digital input is not inverted */
        kPORT_InputNormal,
        /* Pin Control Register fields [15:0] are not locked */
        kPORT_UnlockRegister
    };
    
    // Setup clocks ************************************************************
    // Enable clock to ENC modules
    CLOCK_EnableClock(kCLOCK_Enc1);
    
    // Enable clock to the related PORT modules */
    CLOCK_EnableClock(kCLOCK_Port1);
    CLOCK_EnableClock(kCLOCK_Port2);
    CLOCK_EnableClock(kCLOCK_Port4);
    
    // Enable INPUTMUX0 clock
    CLOCK_EnableClock(kCLOCK_InputMux0);
    
    // Setup ports  ************************************************************   
    PORT_SetPinConfig(PORT1, 22U, &encTriggerPortConfig);   // ENC_A
    PORT_SetPinConfig(PORT2, 0U, &encTriggerPortConfig);    // ENC_B
    
    // Setup INPUTMUX  *********************************************************
    INPUTMUX_AttachSignal(INPUTMUX0, 0U, kINPUTMUX_TrigIn3ToEnc1Phasea);    //TRIG_IN3 -> ENC_A
    INPUTMUX_AttachSignal(INPUTMUX0, 0U, kINPUTMUX_TrigIn5ToEnc1Phaseb);    //TRIG_IN5 -> ENC_B
    
    // Setup ecoder ************************************************************
    /* Initialization modulo counter to encoder number of pulses * 4 - 1 */
    ENC1->LMOD = (M1_POSPE_ENC_PULSES * 4U) - 1U;

    /* Enable modulo counting and revolution counter increment on roll-over */
    ENC1->CTRL2 = (ENC_CTRL2_MOD_MASK | ENC_CTRL2_REVMOD_MASK);

    /* Pass initialization data into encoder driver structure */
    /* encoder position and speed measurement */
    g_sM1Enc.pui32QdBase   = (ENC_Type *)ENC1;
    g_sM1Enc.sTo.fltPGain  = M1_POSPE_KP_GAIN;
    g_sM1Enc.sTo.fltIGain  = M1_POSPE_KI_GAIN;
    g_sM1Enc.sTo.fltThGain = M1_POSPE_INTEG_GAIN;
    g_sM1Enc.a32PosMeGain  = M1_POSPE_MECH_POS_GAIN;
    g_sM1Enc.ui16Pp        = M1_MOTOR_PP;
    g_sM1Enc.bDirection    = M1_POSPE_ENC_DIRECTION;
    g_sM1Enc.fltSpdEncMin  = M1_POSPE_ENC_N_MIN;
    MCDRV_QdEncSetDirection(&g_sM1Enc);
      
    /* Initialization modulo counter*/
    MCDRV_QdEncSetPulses(&g_sM1Enc);

    /* Enable modulo counting and revolution counter increment on roll-over */
    ENC1->CTRL2 = (ENC_CTRL2_MOD_MASK | ENC_CTRL2_REVMOD_MASK);
    
    /* Initialization modulo counter to encoder number of pulses * 4 - 1 */
    ENC1->LMOD = (M1_POSPE_ENC_PULSES * 4U) - 1U;
       
}
