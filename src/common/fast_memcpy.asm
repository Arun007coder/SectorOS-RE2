global fast_memcpy
fast_memcpy:
    pushfd
    pushad

    mov ebx,[esp + 36 + 12]
    cmp ebx, 0
    jbe end_mcpy

    mov edi, [esp + 36 + 4]
    mov esi, [esp + 8 + 36]

    cld
    mov edx, esi
    neg edx
    and edx, 15
    cmp edx, ebx
    jle unaligned_copy
    mov edx, ebx

    unaligned_copy:
        mov ecx, edx
        rep movsb
        sub ebx, edx
        jz end_mcpy
        mov ecx, ebx
        shr ecx, 7
        jz mc_fl
        loop1:
            movaps  XMM0, [esi]
            movaps  XMM1, [esi+10h]
            movaps  XMM2, [esi+20h]
            movaps  XMM3, [esi+30h]
            movaps  XMM4, [esi+40h]
            movaps  XMM5, [esi+50h]
            movaps  XMM6, [esi+60h]
            movaps  XMM7, [esi+70h]
            movups  [edi], XMM0
            movups  [edi+10h], XMM1
            movups  [edi+20h], XMM2
            movups  [edi+30h], XMM3
            movups  [edi+40h], XMM4
            movups  [edi+50h], XMM5
            movups  [edi+60h], XMM6
            movups  [edi+70h], XMM7
            add esi, 80h
            add edi, 80h
            dec ecx
        jnz loop1
    mc_fl:
        and ebx, 7fH
        jz end_mcpy
        mov ecx, ebx
        rep movsb
end_mcpy:
    popad
    popfd
    ret

global fast_memset

fast_memset:
    pushfd
    pushad
    mov ebx,[esp + 36 + 12]
    cmp ebx, 0
    jbe end_mset
    mov edi, [esp + 36 + 4]
    mov esi, [esp + 8 + 36]
    mov edx, esi
    neg edx
    and edx, 15
    cmp edx, ebx
    jle unaligned_set
    mov edx, ebx

    unaligned_set:
        mov ecx, edx
        rep stosb
        sub ebx, edx
        jz end_mset
        mov ecx, ebx
        shr ecx, 7
        jz ms_fl
        loop2:
            movaps  XMM0, [esi]
            movaps  XMM1, [esi+10h]
            movaps  XMM2, [esi+20h]
            movaps  XMM3, [esi+30h]
            movaps  XMM4, [esi+40h]
            movaps  XMM5, [esi+50h]
            movaps  XMM6, [esi+60h]
            movaps  XMM7, [esi+70h]
            movups  [edi], XMM0
            movups  [edi+10h], XMM1
            movups  [edi+20h], XMM2
            movups  [edi+30h], XMM3
            movups  [edi+40h], XMM4
            movups  [edi+50h], XMM5
            movups  [edi+60h], XMM6
            movups  [edi+70h], XMM7
            add esi, 80h
            add edi, 80h
            dec ecx
        jnz loop2

    ms_fl:
        and ebx, 7fH
        jz end_mset
        mov ecx, ebx
        rep stosb
end_mset:
    popad
    popfd
    ret

global fast_memset32

; void fast_memset32(void *dst, uint32_t val, uint32_t count);

fast_memset32:
    pushfd
    pushad
    mov ebx,[esp + 36 + 12]
    cmp ebx, 0
    jbe end_mset32
    mov edi, [esp + 36 + 4]
    mov esi, [esp + 8 + 36]
    mov edx, esi
    neg edx
    and edx, 15
    cmp edx, ebx
    jle unaligned_set32
    mov edx, ebx

    unaligned_set32:
        mov ecx, edx
        rep stosd
        sub ebx, edx
        jz end_mset32
        mov ecx, ebx
        shr ecx, 3
        jz ms32_fl
        loop3:
            mov [edi], eax
            mov [edi+4], eax
            mov [edi+8], eax
            mov [edi+12], eax
            add edi, 16h
            dec ecx
        jnz loop3
    ms32_fl:
        and ebx, 7fH
        jz end_mset32
        mov ecx, ebx
        rep stosb

end_mset32:
    popad
    popfd
    ret
