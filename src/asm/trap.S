/* `exception_vector.S` send all traps here. */
.global trap_entry
trap_entry:
    /* use `stp`/`ldp` in favor of `str`/`ldr` to maintain stack alignment. */
	/*
     * Build your trap frame.
     * Hint:
     * 1. Use stp/ldp in favor of str/ldr to maintain stack alignment
          For example, `stp x1, x2, [sp, #-16]!` is equivalent to 
          first `push x2` and then `push x1`. Be careful about the order.
     * 2. Use mrs/msr to move from/to system registers such as
     *    ELR_EL1, SPSR_EL1, and SP_EL0.
     */

    /* TODO: Lab3 Interrupt. */


    /*
     * Call trap(struct *trapframe).
     * Hint: The first argument is a stack pointer.
     */

    /* TODO: Lab3 Interrupt */


/* return falls through to `trap_return`. */
.global trap_return
trap_return:
	/*
     * Restore registers.
     * Hint: `ldp x1, x2, [sp], #16` is equivalent to first `pop x1`
     * and then `pop x2`.
     */
    /* TODO: Lab3 Interrupt */

