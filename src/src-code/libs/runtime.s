.section .text

//---------------------------//
//            GPR            //
//---------------------------//

.global _savegpr_14
.global _savegpr_15
.global _savegpr_16
.global _savegpr_17
.global _savegpr_18
.global _savegpr_19
.global _savegpr_20
.global _savegpr_21
.global _savegpr_22
.global _savegpr_23
.global _savegpr_24
.global _savegpr_25
.global _savegpr_26
.global _savegpr_27
.global _savegpr_28
.global _savegpr_29
.global _savegpr_30
.global _savegpr_31

.global _restgpr_14
.global _restgpr_15
.global _restgpr_16
.global _restgpr_17
.global _restgpr_18
.global _restgpr_19
.global _restgpr_20
.global _restgpr_21
.global _restgpr_22
.global _restgpr_23
.global _restgpr_24
.global _restgpr_25
.global _restgpr_26
.global _restgpr_27
.global _restgpr_28
.global _restgpr_29
.global _restgpr_30
.global _restgpr_31

_savegpr_14:
    stw      r14, -0x4C(r11)
_savegpr_15:
    stw      r15, -0x48(r11)
_savegpr_16:
    stw      r16, -0x44(r11)
_savegpr_17:
    stw      r17, -0x40(r11)
_savegpr_18:
    stw      r18, -0x3C(r11)
_savegpr_19:
    stw      r19, -0x38(r11)
_savegpr_20:
    stw      r20, -0x34(r11)
_savegpr_21:
    stw      r21, -0x30(r11)
_savegpr_22:
    stw      r22, -0x2C(r11)
_savegpr_23:
    stw      r23, -0x28(r11)
_savegpr_24:
    stw      r24, -0x24(r11)
_savegpr_25:
    stw      r25, -0x20(r11)
_savegpr_26:
    stw      r26, -0x1C(r11)
_savegpr_27:
    stw      r27, -0x18(r11)
_savegpr_28:
    stw      r28, -0x14(r11)
_savegpr_29:
    stw      r29, -0x10(r11)
_savegpr_30:
    stw      r30, -0xC(r11)
_savegpr_31:
    stw      r31, -0x8(r11)
    blr

_restgpr_14:
    lwz      r14, -0x4C(r11)
_restgpr_15:
    lwz      r15, -0x48(r11)
_restgpr_16:
    lwz      r16, -0x44(r11)
_restgpr_17:
    lwz      r17, -0x40(r11)
_restgpr_18:
    lwz      r18, -0x3C(r11)
_restgpr_19:
    lwz      r19, -0x38(r11)
_restgpr_20:
    lwz      r20, -0x34(r11)
_restgpr_21:
    lwz      r21, -0x30(r11)
_restgpr_22:
    lwz      r22, -0x2C(r11)
_restgpr_23:
    lwz      r23, -0x28(r11)
_restgpr_24:
    lwz      r24, -0x24(r11)
_restgpr_25:
    lwz      r25, -0x20(r11)
_restgpr_26:
    lwz      r26, -0x1C(r11)
_restgpr_27:
    lwz      r27, -0x18(r11)
_restgpr_28:
    lwz      r28, -0x14(r11)
_restgpr_29:
    lwz      r29, -0x10(r11)
_restgpr_30:
    lwz      r30, -0xC(r11)
_restgpr_31:
    lwz      r31, -0x8(r11)
    blr

//---------------------------//
//            FPR            //
//---------------------------//

.global _savefpr_14
.global _savefpr_15
.global _savefpr_16
.global _savefpr_17
.global _savefpr_18
.global _savefpr_19
.global _savefpr_20
.global _savefpr_21
.global _savefpr_22
.global _savefpr_23
.global _savefpr_24
.global _savefpr_25
.global _savefpr_26
.global _savefpr_27
.global _savefpr_28
.global _savefpr_29
.global _savefpr_30
.global _savefpr_31

.global _restfpr_14
.global _restfpr_15
.global _restfpr_16
.global _restfpr_17
.global _restfpr_18
.global _restfpr_19
.global _restfpr_20
.global _restfpr_21
.global _restfpr_22
.global _restfpr_23
.global _restfpr_24
.global _restfpr_25
.global _restfpr_26
.global _restfpr_27
.global _restfpr_28
.global _restfpr_29
.global _restfpr_30
.global _restfpr_31

_savefpr_14:
    stfd      f14, -0x90(r11)
_savefpr_15:
    stfd      f15, -0x88(r11)
_savefpr_16:
    stfd      f16, -0x80(r11)
_savefpr_17:
    stfd      f17, -0x78(r11)
_savefpr_18:
    stfd      f18, -0x70(r11)
_savefpr_19:
    stfd      f19, -0x68(r11)
_savefpr_20:
    stfd      f20, -0x60(r11)
_savefpr_21:
    stfd      f21, -0x58(r11)
_savefpr_22:
    stfd      f22, -0x50(r11)
_savefpr_23:
    stfd      f23, -0x48(r11)
_savefpr_24:
    stfd      f24, -0x40(r11)
_savefpr_25:
    stfd      f25, -0x38(r11)
_savefpr_26:
    stfd      f26, -0x30(r11)
_savefpr_27:
    stfd      f27, -0x28(r11)
_savefpr_28:
    stfd      f28, -0x20(r11)
_savefpr_29:
    stfd      f29, -0x18(r11)
_savefpr_30:
    stfd      f30, -0x10(r11)
_savefpr_31:
    stfd      f31, -0x8(r11)
    blr

_restfpr_14:
    lfd      f14, -0x90(r11)
_restfpr_15:
    lfd      f15, -0x88(r11)
_restfpr_16:
    lfd      f16, -0x80(r11)
_restfpr_17:
    lfd      f17, -0x78(r11)
_restfpr_18:
    lfd      f18, -0x70(r11)
_restfpr_19:
    lfd      f19, -0x68(r11)
_restfpr_20:
    lfd      f20, -0x60(r11)
_restfpr_21:
    lfd      f21, -0x58(r11)
_restfpr_22:
    lfd      f22, -0x50(r11)
_restfpr_23:
    lfd      f23, -0x48(r11)
_restfpr_24:
    lfd      f24, -0x40(r11)
_restfpr_25:
    lfd      f25, -0x38(r11)
_restfpr_26:
    lfd      f26, -0x30(r11)
_restfpr_27:
    lfd      f27, -0x28(r11)
_restfpr_28:
    lfd      f28, -0x20(r11)
_restfpr_29:
    lfd      f29, -0x18(r11)
_restfpr_30:
    lfd      f30, -0x10(r11)
_restfpr_31:
    lfd      f31, -0x8(r11)
    blr