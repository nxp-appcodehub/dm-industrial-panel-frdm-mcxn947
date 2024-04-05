/*
 * Copyright 2024 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef MOTOR_CONTROL_H_
#define MOTOR_CONTROL_H_

#include <stdint.h>
#include <stdbool.h>

#include "mc_periph_init.h"
#include "pin_mux.h"
#include "peripherals.h"
#include "fsl_port.h"
#include "fsl_device_registers.h"
#include "m1_sm_snsless_enc.h"
#include "clock_config.h"
#include "board.h"
#include "mid_sm_states.h"

void motor_control_init();

void motor_control_task();

void motor_control_set_speed(uint32_t rpm);

void motor_control_run();

void motor_control_stop();

#endif /* MOTOR_CONTROL_H_ */
