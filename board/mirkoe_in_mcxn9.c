/*
 * Copyright 2024 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "mikroe_in_mcxn9.h"
#include "fsl_lpuart.h"

#define  BACKGROUND_BUFFER_SIZE 100

// RTOS handlers -----------------------------------------------------
// UART --------------------------
volatile static uint8_t 				g_background_buffer_reception[BACKGROUND_BUFFER_SIZE];
volatile static lpuart_rtos_handle_t 	g_lpuart_rtos_handle;
volatile static lpuart_handle_t 		g_lpuart_handle;
volatile static lpuart_rtos_config_t	g_lpuart_rtos_config =
{
		.base 			= LPUART5,
		.srcclk 		= 0,
		.baudrate 		= 115200,
		.parity 		= kLPUART_ParityDisabled,
		.stopbits 		= kLPUART_OneStopBit,
		.buffer 		= (uint8_t*)g_background_buffer_reception,
		.buffer_size	= BACKGROUND_BUFFER_SIZE,
		.enableRxRTS 	= false,
		.enableTxCTS 	= false
};
// -------------------------------
// -------------------------------------------------------------------

// LPUART ------------------------------------------------------------
volatile static uint8_t *g_tx_buff_pointer 					= 0;
volatile static uint8_t  g_tx_data_len						= 0;
volatile static uint8_t  g_tx_cnt							= 0;
volatile static uint8_t  g_rx_buff[BACKGROUND_BUFFER_SIZE]	= {0};
volatile static uint8_t  g_rx_received_len					= 0;
// -------------------------------------------------------------------

// Mikro-e initializations -------------------------------------------
mikroe_status_t init_mikro_e_analog()
{
	// Todo: init mikroe analog
	return k_mikroe_success;
}

mikroe_status_t deinit_mikro_e_analog()
{
	// Todo: deinit mikroe analog
	return k_mikroe_success;
}

mikroe_status_t init_mikro_e_reset()
{
	// Todo: init mikroe reset
	return k_mikroe_success;
}

mikroe_status_t deinit_mikro_e_reset()
{
	// Todo: deinit mikroe reset
	return k_mikroe_success;
}

mikroe_status_t init_mikro_e_spi(bool rtos)
{
	// Todo: init mikroe spi
	return k_mikroe_success;
}

mikroe_status_t deinit_mikro_e_spi(bool rtos)
{
	// Todo: deinit mikroe spi
	return k_mikroe_success;
}

mikroe_status_t init_mikro_e_pwm(bool rtos)
{
	// Todo: init mikroe pwm
	return k_mikroe_success;
}

mikroe_status_t deinit_mikro_e_pwm(bool rtos)
{
	// Todo: deinit mikroe pwm
	return k_mikroe_success;
}

mikroe_status_t init_mikro_e_interrupt(bool rtos)
{
	// Todo: init mikroe interrupt
	return k_mikroe_success;
}

mikroe_status_t deinit_mikro_e_interrupt(bool rtos)
{
	// Todo: deinit mikroe interrupt
	return k_mikroe_success;
}

mikroe_status_t init_mikro_e_uart(mikroe_uart_config_t* uart_config)
{
	lpuart_config_t lpuart_config;
	port_pin_config_t pin_config =
	{
            kPORT_PullDisable,
            kPORT_LowPullResistor,
            kPORT_FastSlewRate,
            kPORT_PassiveFilterDisable,
            kPORT_OpenDrainDisable,
			kPORT_HighDriveStrength,
            kPORT_MuxAlt2,
            kPORT_InputBufferEnable,
            kPORT_InputNormal,
            kPORT_UnlockRegister
	};
	CLOCK_EnableClock(kCLOCK_Port1);
	PORT_SetPinConfig(MIKRO_E_TX_PORT, MIKRO_E_TX_PIN, &pin_config);
	PORT_SetPinConfig(MIKRO_E_RX_PORT, MIKRO_E_RX_PIN, &pin_config);
    CLOCK_SetClkDiv(kCLOCK_DivFlexcom5Clk, 1u);
    CLOCK_AttachClk(kFRO12M_to_FLEXCOMM5);
    NVIC_SetPriority(MIKRO_E_INTERRPUT, 0);

	if(true == uart_config->mikroe_uart_with_rtos)
	{
		g_lpuart_rtos_config.baudrate = uart_config->mikroe_uart_baudrate;
		g_lpuart_rtos_config.srcclk   = CLOCK_GetLPFlexCommClkFreq(MIKRO_E_FLEXCOMM);

		if(k_mikroe_uart_parity_odd == uart_config->mikroe_uart_parity) 		g_lpuart_rtos_config.parity = kLPUART_ParityOdd;
		else if(k_mikroe_uart_parity_even == uart_config->mikroe_uart_parity) 	g_lpuart_rtos_config.parity = kLPUART_ParityEven;
		else 																	g_lpuart_rtos_config.parity = kLPUART_ParityDisabled;

		LPUART_RTOS_Init((lpuart_rtos_handle_t*)&g_lpuart_rtos_handle, (lpuart_handle_t*)&g_lpuart_handle, (lpuart_rtos_config_t*)&g_lpuart_rtos_config);
	}
	else
	{
		LPUART_GetDefaultConfig(&lpuart_config);
		lpuart_config.enableRx = true;
		lpuart_config.enableTx = true;
		LPUART_Init(LPUART5, &lpuart_config, CLOCK_GetLPFlexCommClkFreq(MIKRO_E_FLEXCOMM));
	    LPUART_EnableInterrupts(LPUART5, kLPUART_RxDataRegFullInterruptEnable);
	    EnableIRQ(LP_FLEXCOMM5_IRQn);
	}
	return k_mikroe_success;
}

mikroe_status_t deinit_mikro_e_uart(bool rtos)
{
	// Todo: deinit mikroe uart
	return k_mikroe_success;
}

mikroe_status_t init_mikro_e_i2c(bool rtos)
{
	// Todo: init mikroe i2c
	return k_mikroe_success;
}

mikroe_status_t deinit_mikro_e_i2c(bool rtos)
{
	// Todo: deinit mikroe i2c
	return k_mikroe_success;
}
// --------------------------------------------------------------------

// Mikro-e interface --------------------------------------------------
mikroe_status_t mikroe_uart_send(bool rtos, uint8_t *buffer, uint32_t size)
{
	if(true == rtos)
	{
		LPUART_RTOS_Send((lpuart_rtos_handle_t*)&g_lpuart_rtos_handle, buffer, size);
	}
	else
	{
		g_tx_buff_pointer 	= buffer;
		g_tx_data_len		= size;
		g_tx_cnt			= 0;
	    LPUART_EnableInterrupts(LPUART5, kLPUART_TxDataRegEmptyInterruptEnable);
	}
	return k_mikroe_success;
}
mikroe_status_t mikroe_uart_recive(bool rtos, uint8_t *buffer, uint32_t size, int32_t *recived)
{
	uint8_t index;
	uint8_t index_received;
	if(true == rtos)
	{
		LPUART_RTOS_Receive((lpuart_rtos_handle_t*)&g_lpuart_rtos_handle, buffer, size, (size_t*)recived);
	}
	else
	{
		size = (size < g_rx_received_len)? size:g_rx_received_len;
		for(index = 0; index < size; index ++)
		{
			buffer[index] = g_rx_buff[index];
		}
		*recived = g_rx_received_len;
		g_rx_received_len = 0;
	}
	return k_mikroe_success;
}
// --------------------------------------------------------------------

#ifdef LPUART_NO_RTOS
void LP_FLEXCOMM5_IRQHandler(void)
{
	uint32_t status_flags = LPUART_GetStatusFlags(LPUART5);
	uint8_t fifo_len;
	uint8_t rx_received;
	if(kLPUART_RxDataRegFullFlag & status_flags)
	{
		fifo_len = LPUART_GetRxFifoCount(LPUART5);
		rx_received = g_rx_received_len + fifo_len;
		if(BACKGROUND_BUFFER_SIZE > rx_received)
		{
			LPUART_ReadBlocking(LPUART5, (uint8_t*)g_rx_buff+g_rx_received_len, fifo_len);
			g_rx_received_len = rx_received;
		}
		else
		{
			g_rx_received_len = 0;
		}
	}
	else
	{
		if(g_tx_data_len > g_tx_cnt)
		{
			LPUART_WriteBlocking(LPUART5, (uint8_t*)(g_tx_buff_pointer+g_tx_cnt), 1);
			g_tx_cnt ++;
		}
		else
		{
		    LPUART_DisableInterrupts(LPUART5, kLPUART_TxDataRegEmptyInterruptEnable);
		}
	}
	LPUART_ClearStatusFlags(LPUART5, status_flags);
}
#endif
