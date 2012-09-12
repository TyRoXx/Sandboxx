[Bits 16]
org 0x7C00
jmp main

message db 'hallo',0

main:
    xor     bx, bx                     ; set segment registers
    mov     ds, bx
    mov     es, bx

    mov     ax, 0x7C00                 ; set the stack
    mov     ss, ax                     ; this instruction disables interrupts
                                       ; for one instruction, thus no interrupt
                                       ; can occur when the stack is "unstable"
                                       ; (known ss, but unknown sp)
    xor     sp, sp                     ; interrupts are disabled here

    cld                                ; clear direction flag. We rely on this later.

    mov ax, 0x1112
    int 0x10                           ; set 80x50 text mode and 8x8 font

    mov si, message
    call print_string
	
end: hlt
jmp end

print_string:
    mov ah, 0x0E      ; BIOS function 0x0E: teletype
.loop:
    lodsb             ; grab a byte from SI
    test al, al       ; NUL?
    jz .done          ; if the result is zero: get out
    int 0x10          ; else: print out the character
    jmp .loop
.done:
    ret

times 510-($-$$) db 0	; Fill the rest of the sector with zero's
dw 0xAA55		; Add the boot loader signature to the end
