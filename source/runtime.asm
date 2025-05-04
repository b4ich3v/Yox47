default rel
section .text

global print_text, print_int, print_float, print_bool, print_char, print_box, print_newline, exit
global int_to_bool, float_to_bool, bool_to_int, bool_to_float, int_to_float, float_to_int
extern  GetStdHandle, WriteFile, ExitProcess, floor

STD_OUTPUT_HANDLE equ -11

print_text:
    push    rbp
    mov     rbp, rsp
    
    push    rbx
    push    rdi
    push    rsi
    sub     rsp, 32              

    mov     ecx, STD_OUTPUT_HANDLE
    call    GetStdHandle         
    mov     rbx, rax

    mov     rcx, rbx             
    mov     rdx, rdi             
    mov     r8,  rsi             
    lea     r9,  [rbp-8]         
    mov     qword [rsp+32], 0   
    call    WriteFile

    add     rsp, 32
    pop     rsi
    pop     rdi
    pop     rbx
    pop     rbp
    ret

print_newline:
    push    rbp
    mov     rbp, rsp
    push    rdi
    push    rsi
    sub     rsp, 32              

    lea     rdi, [rel newline]   
    mov     rsi, 1               
    call    print_text

    add     rsp, 32
    pop     rsi
    pop     rdi
    pop     rbp
    ret

print_int:
    push    rbp
    mov     rbp, rsp
    sub     rsp, 32              

    mov     rax, [rbp+16]        
    lea     rdi, [rel int_buffer + 20] 
    xor     ebx, ebx             

    cmp     rax, 0
    jge     .convert
    neg     rax
    mov     bl, 1

.convert:
    mov     rcx, 10
.digit_loop:
    xor     rdx, rdx
    div     rcx                   
    add     dl, '0'
    dec     rdi
    mov     [rdi], dl
    test    rax, rax
    jnz     .digit_loop

    cmp     bl, 1
    jne     .ready
    dec     rdi
    mov     byte [rdi], '-'

.ready:
    lea     rsi, [int_buffer + 20]
    sub     rsi, rdi             
    call    print_text

    leave
    ret

print_float:
    push    rbp
    mov     rbp, rsp
    sub     rsp, 64              

    movq    xmm0, [rbp+16]       
    mov     rax, 1000
    cvtsi2sd xmm1, rax
    mulsd   xmm0, xmm1           
    cvttsd2si rax, xmm0             

    mov     rcx, 1000
    xor     rdx, rdx
    div     rcx                  
    mov     r8,  rax             

    mov     rcx, 10
    mov     rbx, rdx
    mov     rsi, 3
    lea     rdi, [rel float_buffer + 32]
.frac_loop:
    xor     rdx, rdx
    mov     rax, rbx
    div     rcx
    mov     rbx, rax
    add     dl, '0'
    dec     rdi
    mov     [rdi], dl
    dec     rsi
    jnz     .frac_loop

    dec     rdi
    mov     byte [rdi], '.'

    mov     rax, r8
    mov     rbx, rax
    cmp     rbx, 0
    jne     .int_loop
    dec     rdi
    mov     byte [rdi], '0'
    jmp     .emit

.int_loop:
    xor     rdx, rdx
    mov     rax, rbx
    div     rcx
    mov     rbx, rax
    add     dl, '0'
    dec     rdi
    mov     [rdi], dl
    test    rax, rax
    jnz     .int_loop

.emit:
    lea     rsi, [float_buffer + 32]
    sub     rsi, rdi
    call    print_text
    leave
    ret

print_bool:
    push    rbp
    mov     rbp, rsp
    sub     rsp, 32

    mov     al, [rbp+16]
    cmp     al, 0
    je      .print_false

.print_true:
    lea     rdi, [rel str_true]
    mov     rsi, 4
    call    print_text
    jmp     .done

.print_false:
    lea     rdi, [rel str_false]
    mov     rsi, 5
    call    print_text

.done:
    leave
    ret

print_char:
    push    rbp
    mov     rbp, rsp
    sub     rsp, 32

    mov     al, [rbp+16]
    mov     [char_buffer], al
    lea     rdi, [rel char_buffer]
    mov     rsi, 1
    call    print_text

    leave
    ret

print_box:
    push    rbp
    mov     rbp, rsp
    push    rbx
    sub     rsp, 8

    mov     rbx, [rbp+16]       

    lea     rdi, [rel box_open_fmt]
    mov     rsi, 1
    call    print_text

    mov     rcx, [rbx]           
    add     rbx, 8               

.loop:
    test    rcx, rcx
    jz      .end

    mov     rax, [rbx]           
    add     rbx, 8

    cmp     rax, 0
    je      .tag_int
    cmp     rax, 1
    je      .tag_float
    cmp     rax, 2
    je      .tag_bool
    cmp     rax, 3
    je      .tag_box
    cmp     rax, 4
    je      .tag_char
    jmp     .after                

.tag_int:
    mov     rax, [rbx]
    push    rax
    call    print_int
    add     rsp, 8
    jmp     .after

.tag_float:
    movq    xmm0, [rbx]
    sub     rsp, 8
    movsd   [rsp], xmm0
    call    print_float
    add     rsp, 8
    jmp     .after

.tag_bool:
    movzx   rax, byte [rbx]
    push    rax
    call    print_bool
    add     rsp, 8
    jmp     .after

.tag_box:
    mov     rax, [rbx]
    push    rax
    call    print_box
    add     rsp, 8
    jmp     .after

.tag_char:
    movzx   rax, byte [rbx]
    push    rax
    call    print_char
    add     rsp, 8

.after:
    add     rbx, 8
    dec     rcx
    jz      .skip_sep
    lea     rdi, [rel separator_fmt]
    mov     rsi, 2
    call    print_text
.skip_sep:
    jmp     .loop

.end:
    lea     rdi, [rel box_close_fmt]
    mov     rsi, 2
    call    print_text

    add     rsp, 8
    pop     rbx
    pop     rbp
    ret

exit:
    mov     ecx, 0
    call    ExitProcess

int_to_bool:
    mov     rax, [rsp+8]
    test    rax, rax
    setne   al
    movzx   rax, al
    mov     [rsp+16], rax
    ret

float_to_bool:
    movsd   xmm0, [rsp+8]
    xorpd   xmm1, xmm1
    ucomisd xmm0, xmm1
    setne   al
    movzx   rax, al
    mov     [rsp+16], rax
    ret

bool_to_int:
    movzx   rax, byte [rsp+8]
    mov     [rsp+16], rax
    ret

bool_to_float:
    mov     al, [rsp+8]
    cmp     al, 0
    je      .zero
    movsd   xmm0, [rel float_one]
    jmp     .done
.zero:
    xorpd   xmm0, xmm0
.done:
    movsd   [rsp+16], xmm0
    ret

int_to_float:
    mov     rax, [rsp+8]
    cvtsi2sd xmm0, rax
    movsd   [rsp+16], xmm0
    ret

float_to_int:
    movsd   xmm0, [rsp+8]
    call    floor              
    cvttsd2si rax, xmm0
    mov     [rsp+16], rax
    ret

section .data
int_buffer      times 21 db 0
float_buffer    times 33 db 0
char_buffer     db 0
str_true        db "true"
str_false       db "false"
box_open_fmt    db "["
box_close_fmt   db "]", 10, 0
separator_fmt   db ", ", 0
float_one       dq 1.0
newline         db 10
