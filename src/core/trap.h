#pragma once

#include <core/trapframe.h>

#define ESR_EC_SHIFT 26
#define ESR_ISS_MASK 0xFFFFFF
#define ESR_IR_MASK  (1 << 25)

#define ESR_EC_UNKNOWN 0x00
#define ESR_EC_SVC64   0x15
#define ESR_EC_IABORT  0x20
#define ESR_EC_DABORT  0x24

void init_trap();
void trap_global_handler(Trapframe *frame);
NO_RETURN void trap_error_handler(u64 type);
