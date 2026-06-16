#include "maths/maths.h"

#include <math.h>
#include <stdlib.h>
#include <time.h>

MGAPI f32 msin(f32 x) { return sin(x); }

MGAPI f32 mcos(f32 x) { return cos(x); }

MGAPI f32 mtan(f32 x) { return tan(x); }

MGAPI f32 masin(f32 x) { return asin(x); }

MGAPI f32 macos(f32 x) { return acos(x); }

MGAPI f32 matan(f32 x) { return atan(x); }

MGAPI f32 matan2(f32 x, f32 y) { return atan2(x, y); }

MGAPI f32 msqrt(f32 x) { return sqrt(x); }

MGAPI i32 mabs(i32 x) { return abs(x); }

MGAPI i32 random_i32() {
  return rand();
}

MGAPI i32 random_range_i32(i32 min, i32 max) {
  return rand() % (max - min + 1) + min;
}

MGAPI f32 random_f32();

MGAPI f32 random_range_f32(f32 min, f32 max);
