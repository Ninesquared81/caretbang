format PE64 console
include 'win64ax.inc'

section '.code' code readable executable
  start:
        ;; int3
        and     spl, 0F0h
        sub     rsp, 32
        call    [GetProcessHeap]
        mov     [heap_handle], rax
        test    rax, rax
        jz      win_error
        call    get_source
        lea     rsi, [loops]  ; Loop stack base pointer.
        lea     rdi, [aux]    ; Auxiliary stack pointer.
        lea     r14, [source] ; ^! instruction pointer.
        xor     eax, eax
        push    rax
        mov     rbp, rsp      ; Set base pointer.
  main_loop:
        ;; get character
        cmp     rsp, rbp
        jg      empty_stack_error
        mov     bx, [r14]
        inc     r14
        test    bx, bx
        jz      main_loop_end
  cb_caret:
        cmp     bl, '^'
        jne     cb_bang
        push    word 0
        jmp     main_loop
  cb_bang:
        cmp     bl, '!'
        jne     cb_pop
        inc     byte [rsp]
        jmp     main_loop
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
        xor     ecx, ecx        ; Clear higher bits of rcx.
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
        jne     cb_exit
        cmp     rdi, aux
        je      empty_stack_error
        dec     rdi
        mov     al, [rdi]
        push    ax
        jmp     main_loop
  cb_exit:
        cmp     bl, '$'
        jne     cb_main_has_items
        cmp     rsp, rbp
        jge     empty_stack_error
        xor     ecx, ecx
        pop     cx
        xor     ch, ch
        and     spl, 0F0h
        sub     rsp, 32
        call    [ExitProcess]
  cb_main_has_items:
        cmp     bl, '?'
        jne     cb_aux_has_items
        cmp     rsp, rbp
        setl    al              ; Stack grows downwards, so the condition is inverted.
        push    ax
        jmp     main_loop
  cb_aux_has_items:
        cmp     bl, ';'
        jne     main_loop
        cmp     rdi, aux
        setg    al
        push    ax
        jmp     main_loop

  main_loop_end:
        ;; Exit the process.
        xor     rcx, rcx
        and     spl, 0F0h
        sub     rsp, 32
        call    [ExitProcess]

;; ==== seek_loop_end() ====
  seek_loop_end:
        lea     r9, [rsi-8]     ; Save start of loop.
        mov     al, '['
        mov     ah, ']'
        xor     rdx, rdx
        mov     dl, ah          ; ']'
  sle_loop:
        mov     bx, [r14]
        inc     r14
        test    bl, bl
        jz      eof_error       ; Unterminated loop.
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
        mov     [r8], rcx       ; Write into old stack top.
        jne     sle_loop
        cmp     r8, loops       ; Check if loop stack was empty.
        je      eof_error
        cmp     rsi, r9
        jne     sle_loop
        ret

;; ==== char *seek_comment_end(file_contents, file_end_ptr, stride) ====
  seek_comment_end:
        push    rbp
    .main_body:
        lea     rax, [rcx+1] ; Pointer to file_contents.
        mov     rcx, 1       ; Comment counter.
    .loop:
        mov     r9b, [rax]
        add     rax, r8
        cmp     rax, rdx
        jge     .error_eof
      .start_comment:
        cmp     r9b, '('
        jne     .end_comment
        inc     rcx
        jmp     .loop
      .end_comment:
        cmp     r9b, ')'
        jne     .loop
        dec     rcx
        test    rcx, rcx
        jnz     .loop
    .exit:
        pop     rbp
        ret
    .error_eof:
        mov     rcx, '('
        call    eof_error_warn
        xor     rax, rax
        jmp     .exit

;; ==== get_source() ====
  get_source:
        ;; int3
        push    rbp
        push    rbx
        push    r12
        push    r13
        push    r14 ; Had_error
        mov     rbp, rsp
        sub     rsp, 2*8  ; Space for two qwords (argc, file_size).
        mov     rbx, rsp  ; Pointer to argc.
        xor     r14, r14
  gs_main_body:
        sub     rsp, 32
        call    [GetCommandLineW]  ; Raw command line argument string.
        mov     rcx, rax
        mov     rdx, rbx
        call    [CommandLineToArgvW]
        test    rax, rax
        jz      win_error
        mov     r12, rax
        mov     rcx, [rbx]  ; argc
        mov     rdx, r12    ; argv
        lea     r8, [rbx+8] ; file_size
        call    parse_args
        mov     r13, rax    ; pointer to buffer containing file contents.
        test    rax, rax
        jz      gs_error_free_argv
  gs_move_to_source_buffer:
        mov     rcx, r13
        mov     rdx, [rbx+8]
        call    move_to_source_buffer
        test    rax, rax
        jz      gs_error_free_file_contents
  gs_free_file_contents:
        mov     rcx, [heap_handle]
        xor     rdx, rdx
        mov     r8, r13
        call    [HeapFree]
        test    al, al
        jnz     gs_free_argv
        call    win_error_warn
        mov     [exit_code_error], eax
        jmp     gs_error_free_argv
  gs_free_argv:
        mov     rcx, r12
        call    [LocalFree]
        test    rax, rax
        jnz     win_error
  gs_exit:
        mov     rsp, rbp
        pop     r14
        pop     r13
        pop     r12
        pop     rbx
        pop     rbp
        ret
  gs_error_free_file_contents:
        mov     rcx, [heap_handle]
        xor     rdx, rdx
        mov     r8, r13
        call    [HeapFree]
  gs_error_free_argv:
        mov     rcx, r12
        call    [LocalFree]
        test    rax, rax
        jnz     win_error
  gs_error:
        mov     ecx, dword [exit_code_error]
        call    [ExitProcess]

;; ==== char *parse_args(argc, argv, *size) ====
  parse_args:
        push    rbp
        push    rbx ; Buffer.
        push    rdi ; Address of size.
        push    rsi ; Heap handle.
        push    r12 ; File handle.
        push    r13 ; argv.
        mov     rbp, rsp
        mov     r13, rdx  ; argv
        mov     rdi, r8
        mov     rsi, [heap_handle]
        xor     rbx, rbx  ; Initialize rbx (return) to NULL.
        push    rax       ; Dummy push to align stack.
        sub     rsp, 32
        cmp     ecx, 2    ; argc
        jl      too_few_args_error
        je      pa_open_file
        ;; Warn that further args are ignored.
        lea     rcx, [extra_args_warn_msg]
        call    [printf]
        mov     rcx, [r13]  ; argv[0]
        call    print_usage
  pa_open_file:
        ;; int3
        mov     rcx, [r13+8]                            ; argv[1], filename
        mov     rdx, GENERIC_READ                       ; Open for reading only.
        xor     r8, r8                                  ; No sharing.
        xor     r9, r9                                  ; No security attributes.
        mov     dword [rsp],    OPEN_EXISTING           ; Fail if the file doesn't exist.
        mov     dword [rsp+8],  FILE_ATTRIBUTE_NORMAL   ; Normal file attributes.
        mov     dword [rsp+16], r9d                     ; No template file.
        sub     rsp, 32
        call    [CreateFileW]
        mov     r12, rax                                ; File handle.
        cmp     rax, INVALID_HANDLE_VALUE
        je      pa_error
  pa_get_filesize:
        mov     rcx, r12
        lea     rdx, [rdi]
        call    [GetFileSizeEx]
        test    al, al
        jz      pa_error_close_file
  pa_alloc_buffer:
        mov     rcx, rsi   ; Heap handle.
        xor     rdx, rdx   ; No flags.
        mov     r8, [rdi]  ; File (hence buffer) size.
        call    [HeapAlloc]
        mov     rbx, rax
        test    rax, rax
        jz      pa_close_file        ; Cannot call GetLastError.
  pa_read_file:
        mov     rcx, r12             ; File handle.
        lea     rdx, [rbx]           ; Pointer to buffer.
        mov     r8, [rdi]            ; Max number of bytes to read.
        lea     r9, [rsp+32+8]       ; Reserve space for lpNumberOfBytesRead.
        mov     [rsp+32], dword 0
        call    [ReadFile]
        test    eax, eax
        jz      pa_error_free_buffer_close_file
  pa_close_file:
        mov     rcx, r12  ; File handle.
        call    [CloseHandle]
        test    al, al
        jz      pa_error_free_buffer_only
  pa_exit:
        mov     rax, rbx  ; Return value.
        mov     rsp, rbp
        pop     r13
        pop     r12
        pop     rsi
        pop     rdi
        pop     rbx
        pop     rbp
        ret
  pa_error_free_buffer_only:
        call    win_error_warn
        mov     [exit_code_error], eax
        mov     rcx, rsi ; Heap handle.
        xor     rdx, rdx ; No options.
        mov     r8, rbx  ; Biffer.
        call    [HeapFree]
        xor     rbx, rbx
        test    al, al
        jnz     pa_exit
        jmp     pa_error
  pa_error_free_buffer_close_file:
        call    win_error_warn
        mov     [exit_code_error], eax
        mov     rcx, rsi ; Heap handle.
        xor     rdx, rdx ; No options.
        mov     r8, rbx  ; Buffer.
        call    [HeapFree]
        test    al, al
        jnz     pa_error_close_file_exit
  pa_error_close_file:
        call    win_error_warn
        mov     [exit_code_error], eax
  pa_error_close_file_exit:
        xor     rbx, rbx
        mov     rcx, r12
        call    [CloseHandle]
        test    al, al
        jnz     pa_exit
  pa_error:
        xor     rbx, rbx
        call    win_error_warn
        mov     [exit_code_error], eax
        jmp     pa_exit

;; ==== bool move_to_source_buffer(file_contents, filesize)
  move_to_source_buffer:
        push    rbp
        push    rbx ; Source buffer pointer
        push    rdi ; Current character
        push    rsi ; Source buffer end
        push    r12 ; File contents buffer end
        push    r13 ; Stride (important for multibyte encodings).
        mov     rbp, rsp
    .main_body:
        push    rax ; Dummy push to align stack.
        lea     rbx, [source]
        lea     rsi, [source + source_size]
        lea     r12, [rcx + rdx]
        cmp     rcx, r12
        jg      .error_memory
        mov     r13, 1 ; Default stride for ASCII and UTF-8
    .guess_encoding:
        ;; Look for a BOM.
        ;; We check for UTF-16 BE/LE and if not, assume ASCII/UTF-8/Windows-1252.
        ;; Note that it is safe to not distinguish these three since we only care about certain
        ;; ASCII characters (the instructions). These do not clash with 1252 since it is merely
        ;; an extension of ASCII, and neither does it clash with extra bytes of UTF-8, since the
        ;; first bit is always set for multibyte codepoints (in every byte of the codepoint).
        ;; Note, however, that a Windows-1252 file starting with FF FE / FE FF will be
        ;; misinterpreted as UTF-16.
        cmp     rdx, 1
        jl      .exit           ; Zero, skip entire function.
        je      .main_loop      ; One byte; assume ASCII et al.
        mov     r8w, word [rcx]
        cmp     r8w, 0FEFFh
        je      .utf16_le
        cmp     r8w, 0FFEFh
        jne     .main_loop      ; Assume UTF-8/ASCII/Windows-1252.
      .utf16_be:
        inc     rcx             ; Skip MSB.
      .utf16_le:
        inc     r13             ; Set stride to 2.
        add     rcx, r13        ; Skip BOM.
        cmp     rdx, 2
        je      .exit           ; BOM was only thing in file.
    .main_loop:
        mov     dil, [rcx]
      .comment_start:
        cmp     dil, '('
        jne     .comment_end
        push    rdx
        push    rcx
        mov     rdx, r12 ; End pointer.
        mov     r8, r13  ; Stride.
        call    seek_comment_end
        pop     rdx
        pop     rcx
        test    rax, rax
        jz      .error
        mov     rcx, rax
        jmp     .main_loop_update
      .comment_end:
        cmp     dil, ')'
        jne     .other_instructions
        xor     ecx, ecx
        mov     cl, dil
        call    bad_char_error_warn
        jmp     .error
      .other_instructions:
        cmp     dil, '^'
        je      .instruction
        cmp     dil, '!'
        je      .instruction
        cmp     dil, '*'
        je      .instruction
        cmp     dil, ':'
        je      .instruction
        cmp     dil, '%'
        je      .instruction
        cmp     dil, '@'
        je      .instruction
        cmp     dil, '+'
        je      .instruction
        cmp     dil, '-'
        je      .instruction
        cmp     dil, '.'
        je      .instruction
        cmp     dil, ','
        je      .instruction
        cmp     dil, '['
        je      .instruction
        cmp     dil, ']'
        je      .instruction
        cmp     dil, '>'
        je      .instruction
        cmp     dil, '<'
        je      .instruction
        cmp     dil, '$'
        je      .instruction
        cmp     dil, '?'
        je      .instruction
        cmp     dil, ';'
        jne     .main_loop_update
      .instruction:
        cmp     rbx, rsi
        je      .error_memory
        mov     [rbx], dil
        inc     rbx
      .main_loop_update:
        add     rcx, r13
        cmp     rcx, r12
        jl      .main_loop
    .main_loop_end:
        mov     rax, 1   ; Success.
    .exit:
        mov     rsp, rbp
        pop     r13
        pop     r12
        pop     rsi
        pop     rdi
        pop     rbx
        pop     rbp
        ret
    .error_memory:
        sub     rsp, 32
        mov     rcx, [file_too_big_msg]
        call    [printf]
    .error:
        xor     rax, rax ; Error.
        jmp     .exit

;; ==== too_few_args_error(argc, argv) ====
  too_few_args_error:
        lea     rcx, [too_few_args_error_msg]
        mov     r12, [rdx]  ; argv[0]
        and     spl, 0F0h
        sub     rsp, 32
        call    [printf]
        mov     rcx, r12  ; argv[0]
        call    print_usage
        mov     rcx, 1
        call    [ExitProcess]

;; ==== print_usage(name) ====
  print_usage:
        push    rbp
        mov     rbp, rsp
        sub     rsp, 32
        mov     rdx, rcx  ; argv[0]
        lea     rcx, [usage_msg]
        call    [printf]
        mov     rsp, rbp
        pop     rbp
        ret

;; ==== win_error_warn() ====
  win_error_warn:
        push    rbp
        push    rbx
        push    r12
        push    r13
        mov     rbp, rsp
  wew_main_body:
        FLAGS = FORMAT_MESSAGE_ALLOCATE_BUFFER or FORMAT_MESSAGE_FROM_SYSTEM
        push    rax               ; Dummy push to align stack.
        lea     r12, [rsp]        ; Reserve stack slot for message pointer.
        sub     rsp, 64
  wew_retry:
        call    [GetLastError]
        mov     rbx, rax                ; Error code.
        mov     rcx, FLAGS              ; Flags.
        xor     rdx, rdx                ; Parameter ignored.
        mov     r8, rax                 ; Error code.
        xor     r9, r9                  ; Language ID (0).
        mov     [rsp+32],    r12        ; Address to write pointer to message into.
        mov     [rsp+32+8],  dword 0    ; nSize.
        mov     [rsp+32+16], dword 0    ; arguments (NULL).
        call    [FormatMessageW]
        test    rax, rax
        jz      wew_error_error
        lea     rcx, [win_error_msg]
        mov     rdx, rbx
        mov     r8, [r12]
        call    [printf]
        mov     rcx, [r12]
        call    [LocalFree]
        test    rax, rax
        jnz     wew_error_error
  wew_exit:
        mov     rax, rbx  ; Return value: error code.
        mov     rsp, rbp
        pop     r13
        pop     r12
        pop     rbx
        pop     rbp
        ret
  wew_error_error:
        MAX_ERROR_COUNT = 32
        inc     r13  ; Error count.
        cmp     r13, MAX_ERROR_COUNT
        jge     wew_error_final
        lea     rcx, [win_error_error_msg]
        call    [printf]
        jmp     wew_retry
  wew_error_final:
        call    [GetLastError]
        mov     rbx, rdx ; Original error code.
        mov     rax, rbx ; Final error code.
        lea     rcx, [win_error_final_msg]
        call    [printf]
        jmp     wew_exit

;; ==== win_error() ====
  win_error:
        and     rsp, 0F0h
        sub     rsp, 32
        call    win_error_warn
        mov     rcx, rax
        call    [ExitProcess]

;; ==== eof_error_warn(ch) ====
  eof_error_warn:
        push    rbp
        lea     rcx, [eof_error_msg]
        sub     rsp, 32
        call    [printf]
        mov     rsp, rbp
        pop     rbp
        ret

;; ==== eof_error(ch) ====
  eof_error:
        and     spl, 0F0h
        call    eof_error_warn
        mov     rcx, 1          ; Exit Failure.
        call    [ExitProcess]

;; ==== bad_char_error_warn(ch) ====
  bad_char_error_warn:
        push    rbp
        mov     rbp, rsp
    .main_body:
        xor     rdx, rdx
        mov     dl, cl
        lea     rcx, [bad_char_error_msg]
        sub     rsp, 32
        call    [printf] 
    .exit:
        mov     rsp, rbp
        pop     rbp
        ret

;; ==== bad_char_error() ====
  bad_char_error:
        lea     rcx, [bad_char_error_msg]
        xor     edx, edx
        mov     dl, bl
        and     spl, 0F0h
        sub     rsp, 32
        call    [printf]
        mov     rcx, 1          ; Exit Failure.
        call    [ExitProcess]

;; ==== empty_stack_error() ====
  empty_stack_error:
        lea     rcx, [empty_stack_error_msg]
        xor     edx, edx
        mov     dl, bl
        mov     r8, r14
        sub     r8, source
        dec     r8
        and     spl, 0F0h
        sub     rsp, 32
        call    [printf]
        mov     rcx, 1
        call    [ExitProcess]

section '.idata' import data readable
        library\
                kernel, 'kernel32.dll',\
                shell, 'shell32.dll',\
                msvcrt, 'msvcrt.dll'

        import kernel,\
                CloseHandle, 'CloseHandle',\
                CreateFileW, 'CreateFileW',\
                ExitProcess, 'ExitProcess',\
                FormatMessageW, 'FormatMessageW',\
                GetCommandLineW, 'GetCommandLineW',\
                GetFileSizeEx, 'GetFileSizeEx',\
                GetLastError, 'GetLastError',\
                GetProcessHeap, 'GetProcessHeap',\
                GetUserDefaultUILanguage, 'GetUserDefaultUILanguage',\
                HeapAlloc, 'HeapAlloc',\
                HeapFree, 'HeapFree',\
                LocalFree, 'LocalFree',\
                ReadFile, 'ReadFile'

        import shell,\
                CommandLineToArgvW, 'CommandLineToArgvW'

        import msvcrt,\
                printf, 'printf',\
                getchar, 'getchar',\
                putchar, 'putchar'

section '.rdata' data readable
  eof_error_msg:      db "Unexpected EOF while parsing. Unmatched '%c'",10,0
  bad_char_error_msg: db "Unexpected character '%c' while parsing.",10,0
  empty_stack_error_msg:
        db      "Insufficient space on stack to carry out operation '%c' (offset %d).",10,0
  win_error_msg:
        db      "An error occurred [%d]: %ls",10,0
  win_error_error_msg:
        db      "An error occurred while handling another error [%d]:",10,0
  win_error_final_msg:
        db      "An error ocurred [%d] while handling another error [%d].",10,\
                "Maximum number of error messages reached.",10,0
  too_few_args_error_msg:
        db      "Too few arguments.",10,0
  usage_msg:
        db      "Usage: %ls filename",10,0
  extra_args_warn_msg:
        db      "Warning: extra arguments ignored.",10,0
  file_too_big_msg:
        db      "Source file too large. Try a smaller file.",10,0

section '.data' data readable writeable
  exit_code_error dd 1

section '.bss' data readable writeable
  loops:
        rb      8*100

  source:
        source_size = 1024*1024
        rb      source_size
  aux:
        rb      1024
  heap_handle:
        rq      1

