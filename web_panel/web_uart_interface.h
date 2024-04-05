/*
 * Copyright 2024 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef WEB_UART_INTERFACE_H_
#define WEB_UART_INTERFACE_H_

#include <stdint.h>
#include <stdbool.h>

void web_uart_interface_init();

void web_uart_interface_send();

bool web_uart_interface_receive();


#endif /* WEB_UART_INTERFACE_H_ */
