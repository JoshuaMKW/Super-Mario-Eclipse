.section .text

.global shadowCrashPatch
.global animationFreezeCheck

//NOTE: Doesnt work...

shadowCrashPatch:
    cmpwi r4, 0
    li r0, 0
    beqlr-
    lhz r0, 0x18 (r4)
    blr

animationFreezeCheck:
    lbz       r0, 0x64(r26)
    cmpwi     r0, 0xA
    beq-      .loc_0x38
    bge-      .loc_0x18
    cmpwi     r0, 0x5
    bne-      .loc_0x3C

    .loc_0x18:
    cmpwi     r0, 0xD
    bge-      .loc_0x3C
    lwz       r3, -0x60D8(r13)
    lwz       r3, 0x7C(r3)
    cmpwi     r3, 0x1302
    bne-      .loc_0x38
    cmpwi     r0, 0xB
    beq-      .loc_0x3C

    .loc_0x38:
    ori       r27, r27, 0x3

    .loc_0x3C:
    blr
