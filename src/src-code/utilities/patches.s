    .text
    .file "patches.s"
    .globl mario_shadowCrashPatch             # -- Begin function mario_shadowCrashPatch
    .p2align	2
	.type	mario_shadowCrashPatch,@function
mario_shadowCrashPatch:                       # @mario_shadowCrashPatch
.Lfunc_begin0:
    .cfi_startproc
# %bb.0:                                # %entry
    cmpwi 4, 0
    li 0, 0
    beqlr-
    lhz 0, 0x18 (4)
    blr
.Lfunc_end0:
	.size	mario_shadowCrashPatch, .Lfunc_end0-.Lfunc_begin0
	.cfi_endproc
                                        # -- End function
    .ident	"clang version 13.0.0 (https://github.com/DotKuribo/llvm-project.git)"