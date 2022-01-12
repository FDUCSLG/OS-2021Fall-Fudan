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

/* 
 * Step1: Load data from the file stored in `path`.
 * The first `sizeof(struct Elf64_Ehdr)` bytes is the ELF header part.
 * You should check the ELF magic number and get the `e_phoff` and `e_phnum`
 * which is the starting byte of program header.
 * 
 * Step2: Load program headers
 * Program headers are stored like:
 * struct Elf64_Phdr phdr[e_phnum];
 * e_phoff is the address of phdr[0].
 * For each program header, if the type is LOAD, you should:
 * (1) allocate memory, va region [vaddr, vaddr+memsz)
 * (2) copy [offset, offset + filesz) of file to va [vaddr, vaddr+filesz) of memory
 * 
 * Step3: Allocate and initialize user stack.
 * 
 * The va of the user stack is not required to be any fixed value. It can be randomized.
 * 
 * Push argument strings.
 *
 * The initial stack is like
 *
 *   +-------------+
 *   | auxv[o] = 0 | 
 *   +-------------+
 *   |    ....     |
 *   +-------------+
 *   |   auxv[0]   |
 *   +-------------+
 *   | envp[m] = 0 |
 *   +-------------+
 *   |    ....     |
 *   +-------------+
 *   |   envp[0]   |
 *   +-------------+
 *   | argv[n] = 0 |  n == argc
 *   +-------------+
 *   |    ....     |
 *   +-------------+
 *   |   argv[0]   |
 *   +-------------+
 *   |    argc     |
 *   +-------------+  <== sp
 *
 * where argv[i], envp[j] are 8-byte pointers and auxv[k] are
 * called auxiliary vectors, which are used to transfer certain
 * kernel level information to the user processes.
 *
 * ## Example 
 *
 * ```
 * sp -= 8; *(size_t *)sp = AT_NULL;
 * sp -= 8; *(size_t *)sp = PGSIZE;
 * sp -= 8; *(size_t *)sp = AT_PAGESZ;
 *
 * sp -= 8; *(size_t *)sp = 0;
 *
 * // envp here. Ignore it if your don't want to implement envp.
 *
 * sp -= 8; *(size_t *)sp = 0;
 *
 * // argv here.
 *
 * sp -= 8; *(size_t *)sp = argc;
 *
 * // Stack pointer must be aligned to 16B!
 *
 * thisproc()->tf->sp = sp;
 * ```
 * 
 * There are two important entry point addresses:
 * (1) Address of the first user-level instruction: that's stored in elf_header.entry
 * (2) Adresss of the main function: that's stored in memory (loaded in part2)
 *
 */
int execve(const char *path, char *const argv[], char *const envp[]) {
	/* TODO: Lab9 Shell */
	
    return -1;
}
