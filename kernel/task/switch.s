global context_switch
extern schedule
extern get_current_proc
extern irq_eoi

context_switch:
	;push all
	push rdi
	push rsi
	push rax
	push rbx
	push rcx
	push rdx
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
	push rbp

	call get_current_proc
	;the current proc is in rax

	;save rsp
	mov qword[rax + 8], rsp 

	call schedule

	call get_current_proc

	;now reload cr3 and rsp
	mov rbx, qword[rax]
	mov cr3, rbx
	mov rsp, qword[rax + 8]

	;pop all
	pop rbp
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rax
	pop rbx
	pop rcx
	pop rdx
	pop rsi
	pop rdi

	;end of interrupt
	push rdi
	xor rdi, rdi
	call irq_eoi
	pop rdi

	iretq