#include <aarch64/intrinsic.h>
#include <core/console.h>
#include <core/physical_memory.h>
#include <core/virtual_memory.h>


NORETURN void main() {
    init_char_device();
    init_console();
	/* TODO: Lab1 print */

    init_memory_manager();
    init_virtual_memory();

}
