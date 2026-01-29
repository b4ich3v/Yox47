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

square:
push rbp
mov rbp, rsp
push rbx
sub rsp, 8
mov rax, [rbp+16]
mov [rbp-8], rax ; param n
mov rax, [rbp-8] ; n
push rax
mov rax, [rbp-8] ; n
pop rcx
imul rax, rcx
jmp .return
.return:
pop rbx
mov rsp, rbp
pop rbp
ret

sumBox:
push rbp
mov rbp, rsp
push rbx
sub rsp, 8
mov rax, [rbp+16]
mov [rbp-8], rax ; param b
sub rsp, 8
mov rax, [rbp+24]
mov [rbp-16], rax ; param len
sub rsp, 8
mov rax, 0
mov [rbp - 24], rax ; init s
sub rsp, 8
mov rax, 0
mov [rbp - 32], rax ; init i
jmp .Lcond_for_0
.Lbody_for_0:
mov rax, [rbp-8] ; b
push rax
mov rax, [rbp-32] ; i
pop rbx
imul rax, 16
add rbx, 8
add rbx, rax
add rbx, 8
mov rax, [rbx]
push rax
mov rax, [rbp-24] ; s
pop rcx
add rax, rcx
mov [rbp-24], rax ; assign s
mov rax, 1
push rax
mov rax, [rbp-32] ; i
pop rcx
add rax, rcx
mov [rbp-32], rax ; assign i
.Lcond_for_0:
mov rax, [rbp-16] ; len
push rax
mov rax, [rbp-32] ; i
pop rcx
cmp rax, rcx
setl al
movzx rax, al
cmp rax, 0
jne .Lbody_for_0
.Lend_for_0:
mov rax, [rbp-24] ; s
jmp .return
.return:
pop rbx
mov rsp, rbp
pop rbp
ret

main:
push rbp
mov rbp, rsp
push rbx
sub rsp, 8
mov rax, 7
mov [rbp - 8], rax ; init a
sub rsp, 8
mov rax, [rbp-8] ; a
push rax
call square
add rsp, 8
mov [rbp - 16], rax ; init sq
mov rax, [rbp-16] ; sq
push rax
call print_int
add rsp, 8
call print_newline
sub rsp, 8
movsd xmm0, [rel LC_float_3_14]
movq rax, xmm0
mov [rbp - 24], rax ; init x
sub rsp, 8
movsd xmm0, [rel LC_float_2_0]
movq rax, xmm0
mov [rbp - 32], rax ; init y
sub rsp, 8
mov rax, [rbp-32] ; y
movq xmm0, rax
sub rsp, 8
movq [rsp], xmm0
mov rax, [rbp-24] ; x
movq xmm0, rax
movq xmm1, [rsp]
add rsp, 8
mulsd xmm0, xmm1
movq rax, xmm0
mov [rbp - 40], rax ; init z
mov rax, [rbp-40] ; z
movq xmm0, rax
push rax
call print_float
add rsp, 8
call print_newline
sub rsp, 8
mov  rdi, 88
call yox_malloc
mov  rdx, rax
mov  rax, 5
mov  [rdx], rax
lea  rbx, [rdx+8]
mov qword [rbx], 0
add rbx, 8
push rbx
mov rax, 1
pop  rbx
mov  [rbx], rax
add rbx, 8
mov qword [rbx], 0
add rbx, 8
push rbx
mov rax, 2
pop  rbx
mov  [rbx], rax
add rbx, 8
mov qword [rbx], 0
add rbx, 8
push rbx
mov rax, 3
pop  rbx
mov  [rbx], rax
add rbx, 8
mov qword [rbx], 0
add rbx, 8
push rbx
mov rax, 4
pop  rbx
mov  [rbx], rax
add rbx, 8
mov qword [rbx], 0
add rbx, 8
push rbx
mov rax, 5
pop  rbx
mov  [rbx], rax
add rbx, 8
mov  rax, rdx
mov [rbp - 48], rax ; init arr
sub rsp, 8
mov rax, 5
mov [rbp - 56], rax ; init len
call print_newline
sub rsp, 8
mov rax, [rbp-56] ; len
push rax
mov rax, [rbp-48] ; arr
push rax
call sumBox
add rsp, 16
mov [rbp - 64], rax ; init total
mov rax, [rbp-64] ; total
push rax
call print_int
add rsp, 8
call print_newline
mov rax, [rbp-48] ; arr
push rax
mov rax, 2
pop rbx
imul rax, 16
add rbx, 8
add rbx, rax
add rbx, 8
mov rax, [rbx]
push rax ; value to match
mov rax, 10
pop rcx
push rcx
cmp rax, rcx
je .Lcase_0_0
add rsp, 8
; default case
sub rsp, 8
mov rax,63
mov [rbp - 72], rax ; init ch
mov rax, [rbp-72] ; ch
push rax
call print_char
add rsp, 8
call print_newline
jmp .Lend_choose_0
jmp .Lend_choose_0
.Lcase_0_0:
add rsp, 8
sub rsp, 8
mov rax,65
mov [rbp - 80], rax ; init ch
mov rax, [rbp-80] ; ch
push rax
call print_char
add rsp, 8
call print_newline
jmp .Lend_choose_0
jmp .Lend_choose_0
.Lend_choose_0:
sub rsp, 8
mov rax, 10
push rax
mov rax, [rbp-64] ; total
pop rcx
cmp rax, rcx
setg al
movzx rax, al
mov [rbp - 88], rax ; init flag
mov rax, [rbp-88] ; flag
push rax
call print_bool
add rsp, 8
call print_newline
sub rsp, 8
mov rax,88
mov [rbp - 96], rax ; init c
mov rax, [rbp-96] ; c
push rax
call print_char
add rsp, 8
call print_newline
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

section .rodata align=8
LC_float_2_0: dq 2.0
LC_float_3_14: dq 3.14
.LC_fmt_int   db "%lld", 10, 0
.LC_fmt_float db "%f",   10, 0
