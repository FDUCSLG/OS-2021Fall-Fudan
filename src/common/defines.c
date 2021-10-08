#include <aarch64/intrinsic.h>
#include <common/defines.h>

NORETURN void no_return() {
    arch_disable_trap();
    while (1)
        arch_wfi();
}
