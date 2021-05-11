    .text
    .file "multiplayer.s"
    .globl avoidRecurseCall        # -- Begin function avoidRecurseCall
    .p2align	2
	.type	avoidRecurseCall,@function
avoidRecurseCall:                   # @avoidRecurseCall
.Lfunc_begin0:
    .cfi_startproc
# %bb.0:                                # %entry
    mflr 0
    mtctr 3
    mr 3, 4
    mr 4, 5
    mr 5, 6
    mr 6, 7
    mr 7, 8
    mr 8, 9
    mr 9, 10
    bctr
.Lfunc_end0:
	.size	avoidRecurseCall, .Lfunc_end0-.Lfunc_begin0
	.cfi_endproc
                                        # -- End function
    .ident	"clang version 13.0.0 (https://github.com/DotKuribo/llvm-project.git)"