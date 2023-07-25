	.text
	.global first_task_enrty
	.extern first_task_main

first_task_enrty:
	mov	%ss ,%ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %ss
	mov %ax, %fs
	mov %ax, %gs
	jmp first_task_main