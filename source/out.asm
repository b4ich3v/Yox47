section .text
global _start
extern exit
extern malloc
extern printf
extern fflush

fib:
push rbp
mov rbp, rsp
push rbx
sub rsp, 8
mov rax, [rbp+16]
mov [rbp-8], rax ; param n
mov rax, 0
push rax
mov rax, [rbp-8] ; n
pop rcx
cmp rax, rcx
sete al
movzx rax, al
cmp rax, 0
je .Lelse_0
mov rax, 0
jmp .return
jmp .Lend_if_0
.Lelse_0:
.Lend_if_0:
mov rax, 1
push rax
mov rax, [rbp-8] ; n
pop rcx
cmp rax, rcx
sete al
movzx rax, al
cmp rax, 0
je .Lelse_1
mov rax, 1
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
sub rsp, 8
call fib
add rsp, 16
push rax
mov rax, 1
push rax
mov rax, [rbp-8] ; n
pop rcx
sub rax, rcx
push rax
sub rsp, 8
call fib
add rsp, 16
pop rcx
add rax, rcx
jmp .return
.return:
pop rbx
mov rsp, rbp
pop rbp
ret

banner:
push rbp
mov rbp, rsp
push rbx
sub rsp, 8
mov rax,61
mov [rbp - 8], rax ; init c
sub rsp, 8
mov rax, 0
mov [rbp - 16], rax ; init i
jmp .Lcond_for_0
.Lbody_for_0:
mov rax, [rbp-8] ; c
push rax
sub rsp, 8
call print
add rsp, 16
mov rax, 1
push rax
mov rax, [rbp-16] ; i
pop rcx
add rax, rcx
push rax
mov rax, [rbp-16] ; i
pop rcx
.Lcond_for_0:
mov rax, 10
push rax
mov rax, [rbp-16] ; i
pop rcx
cmp rax, rcx
setl al
movzx rax, al
cmp rax, 0
jne .Lbody_for_0
.Lend_for_0:
.return:
pop rbx
mov rsp, rbp
pop rbp
ret

main:
push rbp
mov rbp, rsp
push rbx
call banner
add rsp, 0
sub rsp, 8
mov rax, 7
mov [rbp - 8], rax ; init k
sub rsp, 8
mov rax, [rbp-8] ; k
push rax
sub rsp, 8
call fib
add rsp, 16
mov [rbp - 16], rax ; init fk
sub rsp, 8
; allocate space for box with 4 elements
mov rdi, 72
call malloc
mov rbx, rax
mov [rbx], 4
add rbx, 8
mov qword [rbx], 0
add rbx, 8
mov rax, [rbp-16] ; fk
mov [rbx], rax
add rbx, 8
mov qword [rbx], 0
add rbx, 8
mov rax, [rbp-8] ; k
mov [rbx], rax
add rbx, 8
mov qword [rbx], 0
add rbx, 8
mov rax,88
mov [rbx], rax
add rbx, 8
mov qword [rbx], 2
add rbx, 8
mov rax, 1
mov [rbx], rax
add rbx, 8
sub rbx, 64
sub rbx, 8
mov rax, rbx
mov [rbp - 24], rax ; init data
mov rax, [rbp-24] ; data
push rax
sub rsp, 8
call print
add rsp, 16
mov rax, [rbp-16] ; fk
push rax ; value to match
mov rax, 8
pop rcx
push rcx
cmp rax, rcx
je .Lcase_0_0
mov rax, 13
pop rcx
push rcx
cmp rax, rcx
je .Lcase_0_1
; default case
mov rax, 1
push rax
mov rax, [rbp-16] ; fk
pop rcx
add rax, rcx
push rax
mov rax, [rbp-16] ; fk
pop rcx
jmp .Lend_choose_0
.Lcase_0_0:
mov rax, 80
push rax
mov rax, [rbp-16] ; fk
pop rcx
add rax, rcx
push rax
mov rax, [rbp-16] ; fk
pop rcx
jmp .Lend_choose_0
.Lcase_0_1:
mov rax, 130
push rax
mov rax, [rbp-16] ; fk
pop rcx
add rax, rcx
push rax
mov rax, [rbp-16] ; fk
pop rcx
jmp .Lend_choose_0
.Lend_choose_0:
sub rsp, 8
mov rax, 0
mov [rbp - 32], rax ; init counter
jmp .Lcond_while_0
.Lbody_while_0:
mov rax, [rbp-32] ; counter
push rax
sub rsp, 8
call print
add rsp, 16
mov rax, 1
push rax
mov rax, [rbp-32] ; counter
pop rcx
add rax, rcx
push rax
mov rax, [rbp-32] ; counter
pop rcx
.Lcond_while_0:
mov rax, 3
push rax
mov rax, [rbp-32] ; counter
pop rcx
cmp rax, rcx
setl al
movzx rax, al
cmp rax, 0
jne .Lbody_while_0
.Lend_while_0:
sub rsp, 8
mov rax, 42
mov [rbp - 40], rax ; init fk
mov rax, [rbp-40] ; fk
push rax
sub rsp, 8
call print
add rsp, 16
mov rax, [rbp-40] ; fk
push rax
sub rsp, 8
call print
add rsp, 16
call banner
add rsp, 0
mov rax, [rbp-40] ; fk
jmp .return
.return:
pop rbx
mov rsp, rbp
pop rbp
ret

_start:
call main
mov rdi, 0
call exit
