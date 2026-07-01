/**
 * Copyright 2026 Your Name
 * SPDX-License-Identifier: Apache-2.0
 */

#include "clock.h"

#include "platform/platform.h"

void clock_update(Clock* clock) {
  if (clock->start_time != 0) {
    clock->elapsed_time = platform_get_time_abs() - clock->start_time;
  }
}

void clock_start(Clock* clock) {
  clock->start_time = platform_get_time_abs();
  clock->start_time = 0;
}

void clock_stop(Clock* clock) { clock->start_time = 0; }
