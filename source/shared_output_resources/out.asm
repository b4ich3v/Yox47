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

max2:
push rbp
mov rbp, rsp
push rbx
sub rsp, 16
mov rax, [rbp+16]
mov [rbp-8], rax ; param a
mov rax, [rbp+24]
mov [rbp-16], rax ; param b
mov rax, [rbp-16] ; b
push rax
mov rax, [rbp-8] ; a
pop rcx
cmp rax, rcx
setg al
movzx rax, al
cmp rax, 0
je .Lelse_0
mov rax, [rbp-8] ; a
jmp .return
jmp .Lend_if_0
.Lelse_0:
.Lend_if_0:
mov rax, [rbp-16] ; b
jmp .return
.return:
pop rbx
mov rsp, rbp
pop rbp
ret

fib:
push rbp
mov rbp, rsp
push rbx
sub rsp, 8
mov rax, [rbp+16]
mov [rbp-8], rax ; param n
mov rax, 1
push rax
mov rax, [rbp-8] ; n
pop rcx
cmp rax, rcx
setle al
movzx rax, al
cmp rax, 0
je .Lelse_1
mov rax, [rbp-8] ; n
jmp .return
jmp .Lend_if_1
.Lelse_1:
.Lend_if_1:
mov rax, 2
push rax
mov rax, [rbp-8] ; n
pop rcx
sub rax, rcx
push rax
call fib
add rsp, 8
push rax
mov rax, 1
push rax
mov rax, [rbp-8] ; n
pop rcx
sub rax, rcx
push rax
call fib
add rsp, 8
pop rcx
add rax, rcx
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
sub rsp, 32
mov rax, [rbp+16]
mov [rbp-8], rax ; param b
mov rax, [rbp+24]
mov [rbp-16], rax ; param len
mov rax, 0
mov [rbp - 24], rax ; init s
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
sub rsp, 168
mov rax, 7
mov [rbp - 8], rax ; init a
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
movsd xmm0, [rel LC_float_3_14]
movq rax, xmm0
mov [rbp - 24], rax ; init x
movsd xmm0, [rel LC_float_2_0]
movq rax, xmm0
mov [rbp - 32], rax ; init y
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
xorpd xmm1, xmm1
subsd xmm1, xmm0
movq xmm0, xmm1
movq rax, xmm0
mov [rbp - 48], rax ; init neg
mov rax, [rbp-40] ; z
movq xmm0, rax
push rax
call print_float
add rsp, 8
call print_newline
mov rax, [rbp-48] ; neg
movq xmm0, rax
push rax
call print_float
add rsp, 8
call print_newline
mov rax, 10
push rax
mov rax, [rbp-8] ; a
pop rcx
cmp rax, rcx
setl al
movzx rax, al
push rax
mov rax, 3
push rax
mov rax, [rbp-8] ; a
pop rcx
cmp rax, rcx
setg al
movzx rax, al
pop rcx
and rax, rcx
cmp rax, 0
setne al
movzx rax, al
mov [rbp - 56], rax ; init b1
mov rax, 0
push rax
mov rax, [rbp-8] ; a
pop rcx
cmp rax, rcx
sete al
movzx rax, al
push rax
mov rax, [rbp-56] ; b1
cmp rax, 0
sete al
movzx rax, al
pop rcx
or rax, rcx
cmp rax, 0
setne al
movzx rax, al
mov [rbp - 64], rax ; init b2
mov rax, [rbp-56] ; b1
push rax
call print_bool
add rsp, 8
call print_newline
mov rax, [rbp-64] ; b2
push rax
call print_bool
add rsp, 8
call print_newline
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
mov [rbp - 72], rax ; init arr
mov rax, [rbp-72] ; arr
push rax
call print_box
add rsp, 8
call print_newline
mov rax, 5
mov [rbp - 80], rax ; init len
mov rax, [rbp-80] ; len
push rax
mov rax, [rbp-72] ; arr
push rax
call sumBox
add rsp, 16
mov [rbp - 88], rax ; init total
mov rax, [rbp-88] ; total
push rax
call print_int
add rsp, 8
call print_newline
mov rax, 42
push rax
mov rax, [rbp-72] ; arr
push rax
mov rax, 1
pop rbx
imul rax, 16
add rbx, 8
add rbx, rax
add rbx, 8
mov qword [rbx-8], 0
pop rax
mov [rbx], rax
mov rax, [rbp-72] ; arr
push rax
call print_box
add rsp, 8
call print_newline
mov rax, [rbp-72] ; arr
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
mov rax,63
mov [rbp - 96], rax ; init ch
mov rax, [rbp-96] ; ch
push rax
call print_char
add rsp, 8
call print_newline
jmp .Lend_choose_0
jmp .Lend_choose_0
.Lcase_0_0:
add rsp, 8
mov rax,65
mov [rbp - 104], rax ; init ch
mov rax, [rbp-104] ; ch
push rax
call print_char
add rsp, 8
call print_newline
jmp .Lend_choose_0
jmp .Lend_choose_0
.Lend_choose_0:
mov rax, [rbp-8] ; a
push rax ; value to match
mov rax, 99
pop rcx
push rcx
cmp rax, rcx
je .Lcase_1_0
mov rax, 100
pop rcx
push rcx
cmp rax, rcx
je .Lcase_1_1
add rsp, 8
jmp .Lend_choose_1
.Lcase_1_0:
add rsp, 8
mov rax, 99
push rax
call print_int
add rsp, 8
call print_newline
jmp .Lend_choose_1
jmp .Lend_choose_1
.Lcase_1_1:
add rsp, 8
mov rax, 100
push rax
call print_int
add rsp, 8
call print_newline
jmp .Lend_choose_1
jmp .Lend_choose_1
.Lend_choose_1:
mov rax, 0
mov [rbp - 112], rax ; init i
mov rax, 0
mov [rbp - 120], rax ; init acc
jmp .Lcond_while_0
.Lbody_while_0:
mov rax, [rbp-112] ; i
push rax
mov rax, [rbp-120] ; acc
pop rcx
add rax, rcx
mov [rbp-120], rax ; assign acc
mov rax, 5
push rax
mov rax, [rbp-112] ; i
pop rcx
cmp rax, rcx
sete al
movzx rax, al
cmp rax, 0
je .Lelse_2
jmp .Lend_while_0
jmp .Lend_if_2
.Lelse_2:
.Lend_if_2:
mov rax, 1
push rax
mov rax, [rbp-112] ; i
pop rcx
add rax, rcx
mov [rbp-112], rax ; assign i
.Lcond_while_0:
mov rax, 10
push rax
mov rax, [rbp-112] ; i
pop rcx
cmp rax, rcx
setl al
movzx rax, al
cmp rax, 0
jne .Lbody_while_0
.Lend_while_0:
mov rax, [rbp-120] ; acc
push rax
call print_int
add rsp, 8
call print_newline
mov rax, 0
mov [rbp - 128], rax ; init sum
mov rax, 0
mov [rbp - 136], rax ; init j
jmp .Lcond_for_1
.Lbody_for_1:
mov rax, 2
push rax
mov rax, [rbp-136] ; j
pop rcx
imul rax, rcx
mov [rbp - 144], rax ; init sum
mov rax, [rbp-144] ; sum
push rax
call print_int
add rsp, 8
call print_newline
mov rax, 1
push rax
mov rax, [rbp-136] ; j
pop rcx
add rax, rcx
mov [rbp-136], rax ; assign j
.Lcond_for_1:
mov rax, 5
push rax
mov rax, [rbp-136] ; j
pop rcx
cmp rax, rcx
setl al
movzx rax, al
cmp rax, 0
jne .Lbody_for_1
.Lend_for_1:
mov rax, [rbp-128] ; sum
push rax
call print_int
add rsp, 8
call print_newline
mov rax, 8
push rax
call fib
add rsp, 8
mov [rbp - 152], rax ; init f
mov rax, [rbp-152] ; f
push rax
call print_int
add rsp, 8
call print_newline
mov rax,88
mov [rbp - 160], rax ; init c
mov rax, [rbp-160] ; c
push rax
call print_char
add rsp, 8
call print_newline
mov rax, 9
push rax
mov rax, 3
push rax
call max2
add rsp, 16
mov [rbp - 168], rax ; init m
mov rax, [rbp-168] ; m
push rax
call print_int
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
