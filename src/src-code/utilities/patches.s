.section .text

.global shadowCrashPatch

shadowCrashPatch:
    cmpwi r4, 0
    li r0, 0
    beqlr-
    lhz r0, 0x18 (r4)
    blr

