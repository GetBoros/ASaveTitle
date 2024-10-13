.code

; Tutorial
; Reg | 31 | 15 | 07 |
; EAX | AX | AH | AL | int result = EAX | Return data here
; EBX | BX | BH | BL | int *ptr_t = abc | Pointer to data work with 
; ECX | CX | CH | CL | int i = 0		| Counter for string and loop operations
; EDX | DX | DH | DL | EDX = Div/devisr	| I/O pointer, use in math

; ESI | If cpy ptr to addrss in mem	| Where Get Data from
; EDI | Ptr to address write to		| Where Write Data to
; ESP | High stack address			| Stack ptr
; EBP | Access to (func param)		| Pointer to data on the stack

Make_Sum proc
; RCX
; RDX
; R8
; R9
; return RAX


	ret

Make_Sum endp

end