.section .text

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