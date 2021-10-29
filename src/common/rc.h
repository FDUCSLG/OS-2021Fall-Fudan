#pragma once

#include <common/defines.h>

typedef struct {
    isize count;
} RefCount;

// initialize reference count to zero.
void init_rc(RefCount *rc);

// atomic increment reference count by one.
void increment_rc(RefCount *rc);

// atomic decrement reference count by one. Returns true if reference
// count becomes zero or below.
bool decrement_rc(RefCount *rc);
