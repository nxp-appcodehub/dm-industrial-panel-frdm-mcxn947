/*
 * Copyright 2024 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef MIKROE_IN_MCXN9_H_
#define MIKROE_IN_MCXN9_H_

#include <stdint.h>
#include <stdbool.h>
#include "fsl_clock.h"
#include "fsl_port.h"
#include "fsl_lpuart_freertos.h"
#include "fsl_lpuart.h"

/********************************************************************
 	 	 	 	 	 	 	    DEFINITIONS
*********************************************************************/
// Mikro e header --------------------------------------
#define MIKRO_E_AN_ADC		ADC1
#define MIKRO_E_AN_CHN		A0

#define MIKRO_E_RST_PORT	PORT1
#define MIKRO_E_RST_PIN		3u

#define MIKRO_E_CS_PORT		PORT3
#define MIKRO_E_CS_PIN		23u

#define MIKRO_E_SCK_PORT	PORT3
#define MIKRO_E_SCK_PIN		21u

#define MIKRO_E_MISO_PORT	PORT3
#define MIKRO_E_MISO_PIN	22u

#define MIKRO_E_MOSI_PORT	PORT3
#define MIKRO_E_MOSI_PIN	20u

#define MIKRO_E_PWM_PORT	PORT3
#define MIKRO_E_PWM_PIN		19u

#define MIKRO_E_INT_PORT	PORT5
#define MIKRO_E_INT_PIN		7u

#define MIKRO_E_UART		LPUART5
#define MIKRO_E_FLEXCOMM	5
#define MIKRO_E_INTERRPUT   LP_FLEXCOMM5_IRQn
#define MIKRO_E_RX_PORT		PORT1
#define MIKRO_E_RX_PIN		16u
#define MIKRO_E_TX_PORT		PORT1
#define MIKRO_E_TX_PIN		17u

#define MIKRO_E_SCL_PORT	PORT1
#define MIKRO_E_SCL_PIN		1u

#define MIKRO_E_SDA_PORT	PORT1
#define MIKRO_E_SDA_PIN		0u
// -----------------------------------------------------
/********************************************************************
 	 	 	 	 	 	 	  END DEFINITIONS
*********************************************************************/
typedef enum _mikroe_status_t
{
	k_mikroe_success,
	k_mikroe_error
} mikroe_status_t;

typedef enum _mikroe_uart_parity_t
{
	k_mikroe_uart_pariry_disable,
	k_mikroe_uart_parity_odd,
	k_mikroe_uart_parity_even
} mikroe_uart_parity_t;

typedef enum _mikroe_uart_stop_bit_t
{
	k_mikroe_uart_one_stop_bit,
	k_mikroe_uart_two_stop_bit
} mikroe_uart_stop_bit_t;

typedef struct _mikroe_uart_config_t
{
	bool 					mikroe_uart_with_rtos;
	uint32_t 				mikroe_uart_baudrate;
	mikroe_uart_parity_t 	mikroe_uart_parity;
	mikroe_uart_stop_bit_t 	mikroe_uart_stop_bit;
} mikroe_uart_config_t;

// Mikro-e initializations -------------------------------------------
mikroe_status_t init_mikro_e_analog();
mikroe_status_t deinit_mikro_e_analog();

mikroe_status_t init_mikro_e_reset();
mikroe_status_t deinit_mikro_e_reset();

mikroe_status_t init_mikro_e_spi(bool rtos);
mikroe_status_t deinit_mikro_e_spi(bool rtos);

mikroe_status_t init_mikro_e_pwm(bool rtos);
mikroe_status_t deinit_mikro_e_pwm(bool rtos);

mikroe_status_t init_mikro_e_interrupt(bool rtos);
mikroe_status_t deinit_mikro_e_interrupt(bool rtos);

mikroe_status_t init_mikro_e_uart(mikroe_uart_config_t* uart_config);
mikroe_status_t deinit_mikro_e_uart(bool rtos);

mikroe_status_t init_mikro_e_i2c(bool rtos);
mikroe_status_t deinit_mikro_e_i2c(bool rtos);
// --------------------------------------------------------------------

// Mikro-e interface --------------------------------------------------
mikroe_status_t mikroe_uart_send(bool rtos, uint8_t *buffer, uint32_t size);
mikroe_status_t mikroe_uart_recive(bool rtos, uint8_t *buffer, uint32_t size, int32_t *recived);
// --------------------------------------------------------------------


#endif /* MIKROE_IN_MCXN9_H_ */
