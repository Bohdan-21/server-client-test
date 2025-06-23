global get_mem
global free_mem

extern malloc
extern perror
extern exit


section .data
err_msg db "get_mem", 0



section .text

;procedure get_mem
get_mem:    push    ebp
            mov     ebp, esp

            push    dword [ebp + 8]
            call    malloc
            add     esp, 4
            
            test    eax, eax
            jnz     .ok


            push    err_msg
            call    perror
            add     esp, 4


            push    1
            call exit

.ok:
            mov     esp, ebp
            pop     ebp
            ret

;procedure free_mem
free_mem:   push    ebp
            mov     ebp, esp

            pop     ebp
            ret




section .note.GNU-stack noalloc noexec nowrite progbits