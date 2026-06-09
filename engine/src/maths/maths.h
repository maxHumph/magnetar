/**
 * @file maths.h
 */

#pragma once

#include "define.h"

#define M_PI 3.14159265358979323846f
#define M_TWO_PI (2.0f * M_PI)
#define M_HALF_PI (0.5f * M_PI)
#define M_QUARTER_PI (0.25f * M_PI)
#define M_ONE_OVER_PI (1.0f / M_PI)
#define M_ONE_OVER_TWO_PI (1.0f / TWO_PI)
#define M_TO_RAD(x) (x * (M_PI / 180.0f))
#define M_TO_DEG(x) (x * (180.0f / M_PI))

MGAPI f32 msin(f32 x);
MGAPI f32 mcos(f32 x);
MGAPI f32 mtan(f32 x);
MGAPI f32 masin(f32 x);
MGAPI f32 macos(f32 x);
MGAPI f32 matan(f32 x);
MGAPI f32 matan2(f32 x, f32 y);

MGAPI f32 msqrt(f32 x);
MGAPI i32 mabs(i32 x);

MGINLINE b8 is_power_of_2(u64 val) { return (val != 0) && ((val & (val - 1)) == 0); }

MGAPI i32 random_i32();
MGAPI i32 random_range_i32(i32 min, i32 max);

MGAPI f32 random_f32();
MGAPI f32 random_range_f32(f32 min, f32 max);
