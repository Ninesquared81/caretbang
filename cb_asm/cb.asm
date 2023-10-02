format PE64 console
include 'win64ax.inc'

section '.code' code readable executable
  start:
        ;int3
        lea      rsi, [loops]  ; Loop stack base pointer.
        lea      rdi, [aux]    ; Auxiliary stack pointer.
        lea      r14, [source] ; ^! instruction pointer.
  main_loop:
        ;; get character
        mov      bl, [r14]
        inc      r14
        test     bl, bl
        jz       main_loop_end
  cb_caret:
        cmp      bl, '^'
        jne      cb_bang
        push     word 0
        jmp      main_loop
  cb_bang:
        cmp      bl, '!'
        jne      cb_pop
        inc      byte [rsp]
        jmp      main_loop
  cb_pop:
        cmp     bl, '*'
        jne     cb_dupe
        pop     ax             ; Dummy register.
        jmp     main_loop
  cb_dupe:
        cmp     bl, ':'
        jne     cb_swap
        mov     ax, [rsp]
        push    ax            ; Stack is word-aligned, so use word size register.
        jmp     main_loop
  cb_swap:
        cmp     bl, '%'
        jne     cb_discover
        mov     ax, [rsp+2]
        mov     dx,  [rsp]
        mov     [rsp], ax
        mov     [rsp+2], dx
        jmp     main_loop
  cb_discover:
        cmp     bl, '@'
        jne     cb_plus
        mov     ax, [rsp+4]
        mov     dx, [rsp+2]
        mov     cx, [rsp]
        mov     [rsp], ax
        mov     [rsp+2], cx
        mov     [rsp+4], dx
        jmp     main_loop
  cb_plus:
        cmp     bl, '+'
        jne     cb_minus
        pop     ax
        add     [rsp], al
        jmp     main_loop
  cb_minus:
        cmp     bl, '-'
        jne     cb_next_char
        pop     ax
        sub     [rsp], al
        jmp     main_loop
  cb_next_char:
        cmp     bl, ','
        jne     cb_print
        mov     r13, rsp
        and     spl, 0F0h
        sub     rsp, 32
        call    [getchar]
        mov     rsp, r13
        xor     edx, edx  ; Set rdx to zero.
        cmp     eax, edx
        cmovl   eax, edx  ; In ^!, we use 0 on EOF.
        push    ax
        jmp     main_loop
  cb_print:
        cmp     bl, '.'
        jne     cb_loop_start
        xor     rcx, rcx        ; Clear higher bits of rcx.
        pop     cx
        xor     ch, ch          ; Clear higher byte of cx.
        mov     r13, rsp
        and     spl, 0F0h
        sub     rsp, 32
        call    [putchar]
        mov     rsp, r13
        jmp     main_loop
  cb_loop_start:
        cmp     bl, '['
        jne     cb_loop_end
        mov     [rsi], r14
        add     rsi, 8
        pop     ax
        test    al, al
        jnz     main_loop       ; Continue into loop.
        call    seek_loop_end
        jmp     main_loop
  cb_loop_end:
        cmp     bl, ']'
        jne     cb_put_aux
        mov     rdx, ']'
        cmp     rsi, loops
        je      bad_char_error
        mov     r13, rsi        ; Save old value of rsi.
        sub     rsi, 8
        mov     r12, [rsi]      ; Loop start address.
        pop     ax
        test    al, al
        cmovnz  r14, r12        ; Set ip to start of loop if value is non-zero.
        cmovnz  rsi, r13        ; Reset loop stack if looping (undo pop).
        jmp     main_loop
  cb_put_aux:
        cmp     bl, '>'
        jne     cb_get_aux
        pop     ax
        mov     [rdi], al
        inc     rdi
        jmp     main_loop
  cb_get_aux:
        cmp     bl, '<'
        jne     cb_comment_start
        dec     rdi
        mov     al, [rdi]
        push    ax
        jmp     main_loop
  cb_comment_start:
        cmp     bl, '('
        jne     cb_comment_end
        call    seek_comment_end
        jmp     main_loop
  cb_comment_end:
        ;; Dummy instruction to catch if user had imbalanced parentheses.
        cmp     bl, ')'
        jne     cb_next_thing
        xor     rdx, rdx
        mov     dl, bl
        call    bad_char_error


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
        lea     r9, [rsi-8]    ; Save start of loop.
        mov     al, '['
        mov     ah, ']'
        xor     rdx, rdx
        mov     dl, al         ; ']'
  sle_loop:
        mov     bl, [r14]
        inc     r14
        test    bl, bl
        jz      eof_error      ; Unterminated loop.
        cmp     bl, '('
        jne     sle_not_a_comment
        call    seek_comment_end
  sle_not_a_comment:
        lea     r12, [rsi+8]    ; Incremented address.
        lea     r13, [rsi-8]    ; Decremented address.
        mov     rcx, [rsi]
        mov     r8, rsi
        cmp     bl, al          ; '['
        cmove   rcx, r14
        cmove   rsi, r12        ; Increment.
        cmp     bl, ah          ; ']'
        cmove   rsi, r13        ; Decrement.
        mov     [r8], rcx      ; Write into old stack top.
        jne     sle_loop
        cmp     r8, loops      ; Check if loop stack was empty.
        je      eof_error
        cmp     rsi, r9
        jne     sle_loop
        ret

;; ==== seek_comment_end() ====
  seek_comment_end:
        mov    rcx, rsi        ; Save old top of loop stack.
        mov    [rsi], r14      ; Push current ip to loop stack.
        add    rsi, 8
        mov    al, '('
        mov    ah, ')'
        xor    rdx, rdx
        mov    dl, al          ; '('
  sce_loop:
        mov    bl, [r14]
        inc    r14
        test   bl, bl
        jz     eof_error       ; Unterminated comment.
        lea    r8, [rsi+8]     ; Incremented address.
        lea    r9, [rsi-8]     ; Decremented address.
        mov    [rsi], r14      ; rsi points to one past the end, so we won't overwrite anything.
        cmp    bl, al          ; '('
        cmove  rsi, r8         ; Increment.
        cmp    bl, ah          ; ')'
        cmove  rsi, r9         ; Decrement.
        jne    sce_loop
        mov    dl, ah          ; ')'
        cmp    rsi, rcx
        jg     sce_loop        ; rcx > rsi
        jl     bad_char_error  ; Unmatched ')'.
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
        db      "^!!:[:>:+:](+++(.....[)])<<:<<@<::>@+.%>%:>@:>+!!!!:!.:@+:::..!!!:.<<:<<@:@:@+<:@+.%>%.+%>+!!!.:.!!!...<!.<<+.",0
        ;db ",:[.,:]*",0

section '.bss' data readable writeable
  loops:
        rb      4*100
;  source:
;        rb      1024*1024
  aux:
        rb      1024



