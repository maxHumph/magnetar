/**
 * Copyright 2026 Your Name
 * SPDX-License-Identifier: Apache-2.0
 * 
 * @file clock.h
 */

#pragma once

#include "define.h"

typedef struct Clock {
  f64 start_time;
  f64 elapsed_time;
} Clock;

void clock_update(Clock* clock);

void clock_start(Clock* clock);

void clock_stop(Clock* clock);
