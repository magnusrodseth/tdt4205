.section .rodata
intout: .asciz "%ld "
strout: .asciz "%s "
errout: .asciz "Wrong number of arguments"
string0: .asciz "The values of the array are:"

.section .bss
.align 8
.array: .zero 425329408
