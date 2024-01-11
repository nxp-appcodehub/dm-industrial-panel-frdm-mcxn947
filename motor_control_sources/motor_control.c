/*
 * motor_control.c
 *
 *  Created on: Nov 21, 2023
 *      Author: nxg02659
 */

#include "motor_control.h"

#define M1_END_OF_ISR \
    {                 \
        __DSB();      \
        __ISB();      \
    }

#define PCR_IBE_ibe1 0x01u

volatile static uint32_t g_rpm = 0;

volatile static bool_t bDemoModeSpeed = false;

volatile static mid_app_cmd_t g_eMidCmd;                  /* Start/Stop MID command */
volatile static ctrl_m1_mid_t g_sSpinMidSwitch;           /* Control Spin/MID switching */

void motor_control_init_pins()
{
	// PWM ----------------------------------
    CLOCK_EnableClock(kCLOCK_Port2);

    PORT_SetPinMux(PORT2, 2U, kPORT_MuxAlt5);
    PORT_SetPinMux(PORT2, 3U, kPORT_MuxAlt5);
    PORT_SetPinMux(PORT2, 4U, kPORT_MuxAlt5);
    PORT_SetPinMux(PORT2, 5U, kPORT_MuxAlt5);
    PORT_SetPinMux(PORT2, 6U, kPORT_MuxAlt5);
    PORT_SetPinMux(PORT2, 7U, kPORT_MuxAlt5);

    PORT2->PCR[2] = ((PORT2->PCR[2] & (~(PORT_PCR_IBE_MASK))) | PORT_PCR_IBE(PCR_IBE_ibe1));
    PORT2->PCR[3] = ((PORT2->PCR[3] & (~(PORT_PCR_IBE_MASK))) | PORT_PCR_IBE(PCR_IBE_ibe1));
    PORT2->PCR[4] = ((PORT2->PCR[4] & (~(PORT_PCR_IBE_MASK))) | PORT_PCR_IBE(PCR_IBE_ibe1));
    PORT2->PCR[5] = ((PORT2->PCR[5] & (~(PORT_PCR_IBE_MASK))) | PORT_PCR_IBE(PCR_IBE_ibe1));
    PORT2->PCR[6] = ((PORT2->PCR[6] & (~(PORT_PCR_IBE_MASK))) | PORT_PCR_IBE(PCR_IBE_ibe1));
    PORT2->PCR[7] = ((PORT2->PCR[7] & (~(PORT_PCR_IBE_MASK))) | PORT_PCR_IBE(PCR_IBE_ibe1));
    // --------------------------------------
}

void motor_control_init()
{
	uint32_t ui32PrimaskReg = DisableGlobalIRQ();
	motor_control_init_pins();
	// Init motor control ------------------------------------------------------------------------------
	MCDRV_Init_M1(); // Run timers and enable adc and pwm
	RTCESL_PQ_Init();
	M1_SetAppSwitch(false);

    /* Init application state - Spin or MID */
    g_sSpinMidSwitch.eAppState = kAppStateMID;

    if(g_sSpinMidSwitch.eAppState == kAppStateMID)
    {
      MID_Init_AR();
    }

    /* Spin state machine is default */
    g_sSpinMidSwitch.eAppState = kAppStateSpin;
	// -------------------------------------------------------------------------------------------------
    EnableGlobalIRQ(ui32PrimaskReg);
}

void motor_control_task()
{
	 switch(g_sSpinMidSwitch.eAppState)
	  {
	    case kAppStateSpin:
	        /* M1 state machine */
	        if(g_sSpinMidSwitch.bCmdRunMid == TRUE)
	        {
	          if((kSM_AppStop == M1_GetAppState()) && (FALSE == M1_GetAppSwitch()) )
	          {
	            MID_Init_AR();
	            g_sSpinMidSwitch.sFaultCtrlM1_Mid &= ~(FAULT_APP_SPIN);
	            g_eMidCmd = kMID_Cmd_Stop;                          /* Reset MID control command */
	            g_sSpinMidSwitch.eAppState = kAppStateMID;          /* MID routines will be processed */
	          }
	          else
	            g_sSpinMidSwitch.sFaultCtrlM1_Mid |= FAULT_APP_SPIN;

	          g_sSpinMidSwitch.bCmdRunMid = FALSE;                  /* Always clear request */
	        }

	        g_sSpinMidSwitch.bCmdRunM1 = FALSE;
	        break;
	    default:
	        /* MID state machine */
	        if(g_sSpinMidSwitch.bCmdRunM1 == TRUE)
	        {
	          if((g_eMidCmd == kMID_Cmd_Stop) && (kMID_Stop == MID_GetActualState()))
	          {
	            g_sSpinMidSwitch.sFaultCtrlM1_Mid &= ~(FAULT_APP_MID);
	            g_sM1Ctrl.eState = kSM_AppInit;                      /* Set Init state for M1 state machine */
	            g_sSpinMidSwitch.eAppState = kAppStateSpin;          /* Switch application state to Spin */
	          }
	          else
	            g_sSpinMidSwitch.sFaultCtrlM1_Mid |= FAULT_APP_MID;

	           /* Always clear request */
	          g_sSpinMidSwitch.bCmdRunM1 = FALSE;
	          g_sSpinMidSwitch.bCmdRunMid = FALSE;
	          break;
	        }

	        g_sSpinMidSwitch.bCmdRunMid = FALSE;
	        MID_Process_BL(&g_eMidCmd);
	        break;
	  }
}


void motor_control_set_speed(uint32_t rpm)
{
	g_rpm = rpm;
}

void motor_control_run()
{
    M1_SetAppSwitch(true);
	g_sM1Drive.eControl                  = kControlMode_SpeedFOC;
	g_sM1Drive.sMCATctrl.ui16PospeSensor = MCAT_SENSORLESS_CTRL;
	bDemoModeSpeed = true;
}

void motor_control_stop()
{
    M1_SetSpeed(0);
    M1_SetAppSwitch(false);
    bDemoModeSpeed = false;
}

void CTIMER0_IRQHandler(void)
{
	uint16_t state = M1_GetAppState();


	SM_StateMachineSlow(&g_sM1Ctrl);
	if(bDemoModeSpeed)
	{
		M1_SetSpeed((float_t)g_rpm);
	}

    /* Clear the match interrupt flag. */
    CTIMER0->IR |= CTIMER_IR_MR0INT(1U);

    /* Add empty instructions for correct interrupt flag clearing */
    M1_END_OF_ISR;
}

void ADC1_IRQHandler(void)
{
    uint32_t ui32PrimaskReg;

    /* Disable all interrupts before peripherals are initialized */
    ui32PrimaskReg = DisableGlobalIRQ();

	switch(g_sSpinMidSwitch.eAppState)
	{
		case kAppStateSpin:
			/* M1 state machine */
			SM_StateMachineFast(&g_sM1Ctrl);
		break;

		default:
			/* MID state machine */
			MID_ProcessFast_FL();
		break;
	}

    /* Enable interrupts  */
    EnableGlobalIRQ(ui32PrimaskReg);

    /* Clear the TCOMP INT flag */
    ADC1->STAT |= (uint32_t)(1U << 9);

    /* Add empty instructions for correct interrupt flag clearing */
    M1_END_OF_ISR;
}
