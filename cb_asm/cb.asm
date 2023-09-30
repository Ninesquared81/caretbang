format PE64 console
include 'win64ax.inc'

section '.code' code readable executable
  start:
        lea      rdi, [loops]  ; Loop stack base pointer.
        mov      rsi, rdi      ; Loop stack pointer.
        lea      r14, [source] ; ^! instruction pointer.
  main_loop:
        ;; get character
        mov      cl, [r14]
        inc      r14
        test     cl, cl
        jz       main_loop_end
  cb_caret:
        cmp      cl, '^'
        jne      cb_bang
        push     word 0
        jmp      main_loop
  cb_bang:
        cmp      cl, '!'
        jne      cb_pop
        inc      byte [rsp]
        jmp      main_loop
  cb_pop:
        cmp     cl, '*'
        jne     cb_dupe
        pop     ax             ; Dummy register.
        jmp     main_loop
  cb_dupe:
        cmp     cl, ':'
        jne     cb_swap
        mov     al, [rsp]
        push    ax            ; Stack is word-aligned, so use word size register.
        jmp     main_loop
  cb_swap:
        cmp     cl, '%'
        jne     cb_discover
        mov     ax, [rsp+2]
        mov     bx,  [rsp]
        mov     [rsp], ax
        mov     [rsp], bx
        jmp     main_loop
  cb_discover:
        cmp     cl, '@'
        jne     cb_plus
        mov     ax, [rsp+4]
        mov     bx, [rsp+2]
        mov     [rsp+4], bx
        mov     bx, [rsp]
        mov     [rsp], ax
        mov     [rsp+2], bx
        jmp     main_loop
  cb_plus:
        cmp     cl, '+'
        jne     cb_minus
        pop     ax
        add     [rsp], ax
        jmp     main_loop
  cb_minus:
        cmp     cl, '-'
        jne     cb_next_char
        pop     ax
        sub     [rsp], ax
        jmp     main_loop
  cb_next_char:
        cmp     cl, ','
        jne     cb_print
        mov     r12, rcx
        mov     r13, rsp
        and     spl, 0F0h
        sub     rsp, 32
        call    [getchar]
        mov     rsp, r13
        mov     rcx, r12
        xor     ebx, ebx  ; Set rbx to zero.
        cmp     eax, ebx
        cmovl   eax, ebx  ; In ^!, we use 0 on EOF.
        push    ax
        jmp     main_loop
  cb_print:
        cmp     cl, '.'
        jne     cb_loop_start
        mov     r12, rcx
        mov     r13, rsp
        xor     rcx, rcx        ; Clear higher bits of rcx.
        pop     cx
        xor     ch, ch          ; Clear higher byte of cx.
        and     spl, 0F0h
        sub     rsp, 32
        call    [putchar]
        mov     rsp, r13
        mov     rcx, r12
        jmp     main_loop
  cb_loop_start:
        cmp     cl, '['
        jne     cb_loop_end
        mov     [rsi], r14
        add     rsi, 8
        pop     ax
        test    al, al
        jnz     main_loop       ; Continue into loop.
        call    seek_loop_end
        jmp     main_loop
  cb_loop_end:
        cmp     cl, ']'
        jne     cb_next_thing
        mov     rdx, ']'
        cmp     rsi, loops
        je      bad_char_error
        mov     r13, rsi        ; Save old value of rsi.
        sub     rsi, 8
        mov     r12, [rsi]      ; Loop start address.
        xor     ax, ax
        pop     ax
        test    ax, ax
        cmovnz  r14, r12        ; Set ip to start of loop if value is non-zero.
        cmovnz  rsi, r13        ; Reset loop stack if looping (undo pop).
        jmp     main_loop
  cb_next_thing:
        jmp     main_loop

  main_loop_end:
        ;; Exit the process.
        xor      rcx, rcx
        and      spl, 0F0h
        sub      rsp, 32
        call     [ExitProcess]

;; ==== seek_loop_end() ====
  seek_loop_end:
        lea     rdi, [rsi-8]
        xor     rdx, rdx
        mov     al, '['
        mov     ah, ']'
        xor     rdx, rdx
        mov     dl, al
  sle_loop:
        mov     cl, [r14]
        inc     r14
        lea     r12, [rsi+8]    ; Incremented address.
        lea     r13, [rsi-8]    ; Decremented address.
        mov     rbx, [rsi]
        mov     r15, rsi
        cmp     cl, al          ; '['
        cmove   rbx, r14
        cmove   rsi, r12        ; Increment.
        cmp     cl, ah          ; ']'
        cmove   rsi, r13        ; Decrement.
        mov     [r15], rbx      ; Write into old stack top.
        jne     sle_loop
        cmp     r15, loops      ; Check if loop stack was empty.
        je      eof_error
        cmp     rsi, rdi
        jne     sle_loop
        ret

;; ==== eof_error(char c) ====
  eof_error:
        lea     rcx, [eof_error_msg]
        and     spl, 0F0h
        sub     rsp, 32
        call    [printf]
        mov     rcx, 1          ; Exit Failure.
        call    [ExitProcess]

;; ==== bad_char_error(char c) ====
  bad_char_error:
        lea     rcx, [bad_char_error_msg]
        and     spl, 0F0h
        sub     rsp, 32
        call    [printf]
        mov     rcx, 1          ; Exit Failure.
        call    [ExitProcess]

section '.idata' import data readable
        library\
                kernel, 'kernel32.dll',\
                msvcrt, 'msvcrt.dll'

        import kernel,\
               ExitProcess, 'ExitProcess'

        import msvcrt,\
               printf, 'printf',\
               getchar, 'getchar',\
               putchar, 'putchar'

section '.rdata' data readable
  eof_error_msg:      db "Unexpected EOF while parsing. Unmatched '%c'",13,10,0
  bad_char_error_msg: db "Unexpected character '%c' while parsing.",13,10,0

section '.data' data readable writeable
  source:
        db      ",:[.,:]*",0


section '.bss' data readable writeable
  loops:
        rb      4*100
;  source:
;        rb      1024*1024



