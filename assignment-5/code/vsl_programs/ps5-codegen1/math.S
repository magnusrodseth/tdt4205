.section .rodata
intout: .asciz "%ld "
strout: .asciz "%s "
errout: .asciz "Wrong number of arguments"
string0: .asciz "num*2"
string1: .asciz "num+5"
string2: .asciz "num/3"
string3: .asciz "4*(-num)"
string4: .asciz "(num-1)*(num+1)*(num+3)"

.section .bss
.align 8
