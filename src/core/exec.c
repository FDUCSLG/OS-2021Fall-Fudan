#include <elf.h>

#include "trap.h"

#include <fs/file.h>
#include <fs/inode.h>

#include <aarch64/mmu.h>
#include <core/console.h>
#include <core/proc.h>
#include <core/sched.h>
#include <core/virtual_memory.h>

#include <elf.h>

static uint64_t auxv[][2] = {{AT_PAGESZ, PAGE_SIZE}};

int execve(const char *path, char *const argv[], char *const envp[]) {
	/* TODO: Lab9 Shell */
	
    return -1;
}
