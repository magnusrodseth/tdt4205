.section .rodata
intout: .asciz "%ld "
strout: .asciz "%s "
errout: .asciz "Wrong number of arguments"
string0: .asciz "Inside main, the arguments are:"
string1: .asciz "Inside otherFunc, the arguments are:"

.section .bss
.align 8
