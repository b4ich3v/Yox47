section .text
    extern  printf, fflush, stdout, floor

global print_int
print_int:
    push rbp
    mov  rbp, rsp
    push rbx               
    sub  rsp, 8            

    mov  rdi, int_fmt
    mov  rsi, [rbp+16]     
    xor  rax, rax
    call printf

    mov  rdi, [rel stdout]
    call fflush

    add  rsp, 8
    pop  rbx
    pop  rbp
    ret

global print_float
print_float:
    push rbp
    mov  rbp, rsp
    push rbx
    sub  rsp, 8

    mov  rdi, float_fmt
    movsd xmm0, [rbp+16]
    mov  rax, 1            
    call printf

    mov  rdi, [rel stdout]
    call fflush

    add  rsp, 8
    pop  rbx
    pop  rbp
    ret

global print_bool
print_bool:
    push rbp
    mov  rbp, rsp
    push rbx
    sub  rsp, 8

    mov  al, [rbp+16]
    cmp  al, 0
    jne  .print_true
    mov  rdi, bool_false
    jmp  .picked
.print_true:
    mov  rdi, bool_true
.picked:
    xor  rax, rax
    call printf

    mov  rdi, [rel stdout]
    call fflush

    add  rsp, 8
    pop  rbx
    pop  rbp
    ret

%define TYPE_INT   0
%define TYPE_FLOAT 1
%define TYPE_BOOL  2
%define TYPE_BOX   3

global print_box
print_box:
    push rbp
    mov  rbp, rsp
    push rbx
    sub  rsp, 8

    mov  rbx, [rbp+16]     

    mov  rdi, box_open_fmt
    xor  rax, rax
    call printf

    mov  rcx, [rbx]         
    add  rbx, 8             

.loop_start:
    test rcx, rcx
    jz   .loop_end

    mov  rax, [rbx]         
    add  rbx, 8             

    cmp  rax, TYPE_INT
    je   .print_int_element
    cmp  rax, TYPE_FLOAT
    je   .print_float_element
    cmp  rax, TYPE_BOOL
    je   .print_bool_element
    cmp  rax, TYPE_BOX
    je   .print_box_element
    jmp  .after_element

.print_int_element:
    mov  rax, [rbx]
    push rax
    call print_int
    add  rsp, 8
    jmp  .after_element

.print_float_element:
    movq xmm0, [rbx]
    sub  rsp, 8
    movsd [rsp], xmm0
    call print_float
    add  rsp, 8
    jmp  .after_element

.print_bool_element:
    movzx rax, byte [rbx]
    push rax
    call print_bool
    add  rsp, 8
    jmp  .after_element

.print_box_element:
    mov  rax, [rbx]
    push rax
    call print_box
    add  rsp, 8
    jmp  .after_element

.after_element:
    add  rbx, 8              
    dec  rcx
    jz   .after_sep
    mov  rdi, separator_fmt
    xor  rax, rax
    call printf
.after_sep:
    jmp  .loop_start

.loop_end:
    mov  rdi, box_close_fmt
    xor  rax, rax
    call printf

    mov  rdi, [rel stdout]
    call fflush

    add  rsp, 8
    pop  rbx
    pop  rbp
    ret

global int_to_bool
int_to_bool:
    mov  rax, [rsp+8]
    cmp  rax, 0
    sete al
    mov  [rsp+16], al
    ret

global float_to_bool
float_to_bool:
    movsd xmm0, [rsp+8]
    xorpd xmm1, xmm1
    ucomisd xmm0, xmm1
    setne al
    mov   [rsp+16], al
    ret

global bool_to_int
bool_to_int:
    movzx rax, byte [rsp+8]
    mov   [rsp+16], rax
    ret

global bool_to_float
bool_to_float:
    mov   al, [rsp+8]
    cmp   al, 0
    je    .bf_false
    movsd xmm0, [rel float_one]
    jmp   .bf_done
.bf_false:
    xorpd xmm0, xmm0
.bf_done:
    movsd [rsp+16], xmm0
    ret

float_one: dq 1.0

global int_to_float
int_to_float:
    mov   rax, [rsp+8]
    cvtsi2sd xmm0, rax
    movsd [rsp+16], xmm0
    ret

global float_to_int
float_to_int:
    movsd xmm0, [rsp+8]
    call  floor
    cvttsd2si rax, xmm0
    mov   [rsp+16], rax
    ret

section .rodata
int_fmt:       db "%lld",10,0
float_fmt:     db "%f",10,0
bool_true:     db "true",10,0
bool_false:    db "false",10,0
box_open_fmt:  db "[",0
box_close_fmt: db "]",10,0
separator_fmt: db ", ",0
