.code

;------------------------------------------------------------------------------------------------------------
; Tutorial
;------------------------------------------------------------------------------------------------------------
; Fundamental Data Types
;   07	| 0-15	|		0 - 31	|  0 - 63	|		0 - 127		|
; char	| short	|		 int	| long long	| 
; Byte	| Word	| DoubleWord	| QuadWord	| DoubleQuadWord	|
;------------------------------------------------------------------------------------------------------------
; Reg | 31 | 15 | 07 |
; EAX | AX | AH | AL | int result = EAX | Return data here
; EBX | BX | BH | BL | int *ptr_t = abc | Pointer to data work with 
; ECX | CX | CH | CL | int i = 0		| Counter for string and loop operations
; EDX | DX | DH | DL | EDX = Div/devisr	| I/O pointer, use in math
;------------------------------------------------------------------------------------------------------------
; ESI | RSI | If cpy ptr to addrss in mem	| Where Get Data from
; EDI | RDI | Ptr to address write to		| Where Write Data to	| stosd | 
;------------------------------------------------------------------------------------------------------------
; Stack
;------------------------------------------------------------------------------------------------------------
; ESP | High stack address			| Don`t use if u can  | Stack ptr
; EBP | Access to (func param)		| Used to get address | Pointer to data on the stack
;------------------------------------------------------------------------------------------------------------
; COMMANDS
;------------------------------------------------------------------------------------------------------------
; stosd | rax -> rdi | write rax to rdi |
;------------------------------------------------------------------------------------------------------------
; FUNCTIONS
;------------------------------------------------------------------------------------------------------------
Make_Sum proc
; extern "C" int Make_Sum(int a, int b);
; PARAMS :
; RCX
; RDX
; R8
; R9
; return RAX

	mov eax, ecx
	add eax, edx

	ret

Make_Sum endp
;------------------------------------------------------------------------------------------------------------
Add_To_Array proc
; extern "C" int Add_To_Array(int *arr, int value, int length);
; PARAMS :
; RCX - arr
; RDX - value
; R8 - length
; RAX - return

;	mov [ rcx ], edx  ; *ptr = value
;	add rcx, 4  ; ptr = ptr + 1
;	dec r8

	push rax
	push rcx
	push rdx

	; for each cycle in asm
	mov rdi, rcx  ; addr | stosd use this to set data from eax
	mov eax, edx  ; data | stosd use this to send value to rdi
	mov rcx, r8  ; index | int i = 0 | rcx it is index to stosd func
	
	rep stosd  ; *ptr = value | ptr++ | Write eax to rdi and go to next 4 bytes if it`s int

	pop rdx
	pop rcx
	pop rax

	ret

Add_To_Array endp
;------------------------------------------------------------------------------------------------------------

end