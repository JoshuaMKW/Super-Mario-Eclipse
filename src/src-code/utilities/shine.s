    .text
    .file "shine.s"
    .globl shine_animationFreezeCheck         # -- Begin function shine_animationFreezeCheck
    .p2align	2
	.type	shine_animationFreezeCheck,@function
shine_animationFreezeCheck:                   # @shadowCrashPatch
.Lfunc_begin0:
    .cfi_startproc
# %bb.0:                                # %entry
    lbz       0, 0x64(26)
    cmpwi     0, 0xA
    beq-      .loc_0x38
    bge-      .loc_0x18
    cmpwi     0, 0x5
    bne-      .loc_0x3C

    .loc_0x18:
    cmpwi     0, 0xD
    bge-      .loc_0x3C
    lwz       3, -0x60D8(13)
    lwz       3, 0x7C(3)
    cmpwi     3, 0x1302
    bne-      .loc_0x38
    cmpwi     0, 0xB
    beq-      .loc_0x3C

    .loc_0x38:
    ori       27, 27, 0x3

    .loc_0x3C:
    blr
.Lfunc_end0:
	.size	shine_thinkCloseCamera, .Lfunc_end0-.Lfunc_begin0
	.cfi_endproc
                                        # -- End function
    .text
    .globl shine_thinkCloseCamera         # -- Begin function shine_thinkCloseCamera
    .p2align	2
	.type	shine_thinkCloseCamera,@function
shine_thinkCloseCamera:                   # @shine_thinkCloseCamera
.Lfunc_begin1:
    .cfi_startproc
# %bb.0:                                # %entry
    lbz 0, 0x190 (4)
    lwz 4, 0x154 (4)
    rlwinm. 4, 4, 0, 27, 27
    bne .Ltmp0
    li 0, 0
    .Ltmp0:
    blr
.Lfunc_end1:
	.size	shine_thinkCloseCamera, .Lfunc_end1-.Lfunc_begin1
	.cfi_endproc
                                        # -- End function
    .ident	"clang version 13.0.0 (https://github.com/DotKuribo/llvm-project.git)"