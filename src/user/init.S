#include <core/syscallno.h>

.global icode
.global ispin
.global eicode

icode:
    mov     x8, #SYS_myexecve
    adr     x0, init
    mov     x1, #0
    mov     x2, #0
    svc     #0
	mov     x8, #SYS_myexit
    svc     #0

ispin:
    b       ispin

init:
    .string "/init\0"

.align 4
eicode:
