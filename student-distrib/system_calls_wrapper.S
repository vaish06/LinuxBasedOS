
.text
	.global system_calls_linkage

.align 4
system_calls_linkage: 
	# linkage
	# rutu: reserve all registers


	cmpl $0, %eax
	jna system_error
	cmpl $11, %eax
	jnb system_error	
	decl %eax

	# save registers
	pushw %fs
	pushw %es
	pushw %ds
	pushl %eax
	# C arguments as much as required by the function
	pushl %ebp
	pushl %edi
	pushl %esi
	pushl %edx
	pushl %ecx
	pushl %ebx

	sti
	call *syscall_table(, %eax, 4)
	
	popl %ebx
	popl %ecx
	popl %edx
	popl %esi
	popl %edi
	popl %ebp
	addl $4, %esp	# discard syscall number in eax saved on the stack
	popw %ds
	popw %es
	popw %fs
	
	iret

system_error:
	movl $-1, %eax
	iret

syscall_table:
	.long halt, execute, read, write, open, close, getargs, vidmap, set_handler, sigreturn

