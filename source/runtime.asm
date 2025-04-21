section .text
extern printf
extern fflush
extern stdout
extern floor

global print_int
print_int:
    push rbp
    mov rbp, rsp
    sub rsp, 32  

    mov rdi, int_fmt
    mov rsi, [rbp+16] 
    xor rax, rax
    call printf

    mov rdi, [rel stdout]
    call fflush

    leave
    ret

int_fmt: db "%lld", 10, 0


global print_float
print_float:
    push rbp
    mov rbp, rsp
    sub rsp, 32

    mov rdi, float_fmt
    movsd xmm0, [rbp+16]
    xor rax, rax
    call printf

    mov rdi, [rel stdout]
    call fflush

    leave
    ret

float_fmt: db "%f", 10, 0


global print_bool
print_bool:
    push rbp
    mov rbp, rsp
    sub rsp, 32

    mov al, [rbp+16]
    cmp al, 0
    je .print_false
.print_true:
    mov rdi, bool_true
    jmp .end_bool
.print_false:
    mov rdi, bool_false
.end_bool:
    xor rax, rax
    call printf

    mov rdi, [rel stdout]
    call fflush

    leave
    ret

bool_true: db "true", 10, 0
bool_false: db "false", 10, 0


%define TYPE_INT   0
%define TYPE_FLOAT 1
%define TYPE_BOOL  2
%define TYPE_BOX   3

global print_box
print_box:
    push rbp
    mov rbp, rsp
    sub rsp, 32

    mov rdi, [rbp+16]

    mov rsi, box_open_fmt
    xor rax, rax
    call printf

    ; RCX - брой елементи
    mov rcx, [rdi] 
    add rdi, 8     

.loop_start:
    test rcx, rcx
    jz .loop_end

    mov rax, [rdi] 
    add rdi, 8

    cmp rax, TYPE_INT
    je .print_int_element
    cmp rax, TYPE_FLOAT
    je .print_float_element
    cmp rax, TYPE_BOOL
    je .print_bool_element
    cmp rax, TYPE_BOX
    je .print_box_element

    jmp .loop_next

.print_int_element:
    mov rsi, [rdi]
    push rdi
    mov rdi, rsi
    call print_int
    pop rdi
    add rdi, 8
    jmp .loop_next

.print_float_element:
    movsd xmm0, [rdi]
    push rdi
    sub rsp, 8
    movsd [rsp], xmm0
    call print_float
    add rsp, 8
    pop rdi
    add rdi, 8
    jmp .loop_next

.print_bool_element:
    mov al, [rdi]
    push rdi
    movzx rdi, al
    call print_bool
    pop rdi
    add rdi, 8
    jmp .loop_next

.print_box_element:
    mov rsi, [rdi]
    push rcx
    push rdi
    mov rdi, rsi 
    call print_box
    pop rdi
    pop rcx
    add rdi, 8
    jmp .loop_next

.loop_next:
    dec rcx
    cmp rcx, 0
    je .loop_start_end
    mov rsi, separator_fmt 
    xor rax, rax
    call printf
.loop_start_end:
    jmp .loop_start

.loop_end:
    mov rsi, box_close_fmt 
    xor rax, rax
    call printf

    mov rdi, [rel stdout]
    call fflush

    leave
    ret

box_open_fmt:  db "[",0
box_close_fmt: db "]",10,0
separator_fmt: db ", ",0

global int_to_bool
int_to_bool:
    mov rax, [rsp+8]
    cmp rax, 0
    sete al
    xor al, 1 
    mov [rsp+16], al
    ret


global float_to_bool
float_to_bool:
    movsd xmm0, [rsp+8]
    xorpd xmm1, xmm1
    ucomisd xmm0, xmm1
    setne al 
    mov [rsp+16], al
    ret


global bool_to_int
bool_to_int:
    mov al, [rsp+8]
    movzx rax, al
    mov [rsp+16], rax
    ret


global bool_to_float
bool_to_float:
    mov al, [rsp+8]
    cmp al, 0
    je .bool_float_false
.bool_float_true:
    movsd xmm0, float_one
    jmp .bool_float_end
.bool_float_false:
    xorpd xmm0, xmm0
.bool_float_end:
    movsd [rsp+16], xmm0
    ret

float_one: dq 1.0


global int_to_float
int_to_float:
    mov rax, [rsp+8]
    cvtsi2sd xmm0, rax
    movsd [rsp+16], xmm0
    ret


global float_to_int
float_to_int:
    movsd xmm0, [rsp+8]
    call floor
    cvttsd2si rax, xmm0
    mov [rsp+16], rax
    ret
