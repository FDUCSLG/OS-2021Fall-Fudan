#include <aarch64/intrinsic.h>
#include <common/defines.h>

NO_RETURN NO_INLINE void no_return() {
    arch_disable_trap();
    while (1)
        arch_wfi();
}
