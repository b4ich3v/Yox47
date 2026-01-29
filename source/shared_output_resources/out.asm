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
sub rsp, 16
mov rax, 2222
mov [rbp - 8], rax ; init number1
mov rax, 3
mov [rbp - 16], rax ; init number2
mov rax, [rbp-16] ; number2
push rax
mov rax, [rbp-8] ; number1
pop rcx
add rax, rcx
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
