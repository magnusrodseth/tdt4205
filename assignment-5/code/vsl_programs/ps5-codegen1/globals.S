.section .rodata
intout: .asciz "%ld "
strout: .asciz "%s "
errout: .asciz "Wrong number of arguments"
string0: .asciz "On startup, A and B are:"
string1: .asciz "Now, B is"
string2: .asciz "Here, A is"

.section .bss
.align 8
.A: .zero 8
.B: .zero 8
.dummy: .zero 8
