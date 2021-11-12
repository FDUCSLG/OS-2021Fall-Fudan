#include <core/syscallno.h>

.global loop_start
.global loop_end

loop_start:
    mov 	x3, #0x20
    mov     x8, #SYS_myprint
    mov 	x0, #0
    mov		x1, #1
loop:
    svc     #0
    add		x0, x0, x1
    cmp		x0, x3
    bne		loop
sss:
    b sss

.align 4
loop_end:
