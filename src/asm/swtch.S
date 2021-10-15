/*
 * Context switch
 *
 *   void swtch(struct context **old, struct context *new);
 * 
 * Save the current registers on the stack, creating
 * a struct context, and save its address in *old.
 * Switch stacks to new and pop previously-saved registers.
 *
 * Function `swtch` is call-return semantic.
 * However, the pc+4 of the caller is saved in the context.
 * Therefore, we can store pc+4 in a caller-saved register (like x15),
 * and use `br x15` instead of `ret` in the return stage.
 */
.global swtch
swtch:
    /* TODO: Lab3 Schedule */
	
