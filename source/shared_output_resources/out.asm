default rel
section .text
global _start
extern exit
extern print_int
extern print_bool
extern print_char
extern print_float
extern print_box
extern yox_malloc
extern print_newline

main:
push rbp
mov rbp, rsp
push rbx
mov rax, 100000
push rax
call print_int
add rsp, 8
mov rax, 0
jmp .return
.return:
pop rbx
mov rsp, rbp
pop rbp
ret

_start:
call main
mov rdi, rax
call exit

section .rodata
.LC_fmt_int   db "%lld", 10, 0
.LC_fmt_float db "%f",   10, 0
