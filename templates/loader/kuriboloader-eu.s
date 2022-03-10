#inject(0x8029f4c8)

#   SMS PAL Kuribo Loader  #
#   Credits: Riidefi SMS   #

.set DVDOpen, 0x80343b70
.set DVDReadPrio, 0x80343fd0
.set DVDClose, 0x80343c38
#
.set createRoot__10JKRExpHeapFib, 0x802B9020   
.set OSReport, 0x8033c7c4

# As this may be a cheat code, we use absolute branching with __call

.global loadCMX
loadCMD:
    .set back_chain, -0x50
    .set istate, -0x4c
    .set dvd_handle, -0x48
    .set r30_save, -8
    .set r31_save, -4
    .set sender_lr,  4
    stwu      r1, back_chain(r1)
    mflr      r0
    stw       r0, 0x50+sender_lr(r1)
    stw       r31, 0x50+r31_save(r1)
    stw       r30, 0x50+r30_save(r1)

    # Init the heaps
    __call r12, createRoot__10JKRExpHeapFib

    addi      r4, r1, 0x50+dvd_handle
    # Disable interrupts, saving state
    mfmsr     r3
    rlwinm    r12, r3, 0,17,15
    mtmsr     r12
    extrwi    r3, r3, 1,16
    stw       r3, 0x50+istate(r1)

    # Try to open the file
    bl        cometBinStrEnd
    .string   "Kuribo!/System/KuriboKernel.bin"
    .align 4
    cometBinStrEnd:
    mflr r3
    __call    r12, DVDOpen
    cmpwi     r3, 0
    beq       fail

    # Load filesize, round 32B
    lwz       r3, 0x50-0x14(r1)
    addi      r0, r3, 0x1F
    clrrwi    r31, r0, 5
    
    lis r4, 0x8041
    ori r4, r4, 0xed80
    mr r30, r4

    # Read the file
    # mr        r4, r30
    mr        r5, r31
    addi      r3, r1, 0x50+dvd_handle
    li        r6, 0
    li        r7, 2
    __call    r12, DVDReadPrio

    # Close the file
    addi      r3, r1, 0x50+dvd_handle
    __call    r12, DVDClose

    # Call the prologue
#    lwz       r4, 0x04(r30)
#    mr        r3, r30
#    mtctr     r4
    mtctr r30
    bctrl

    bl        succStringEnd
    .string   "===\nKuribo Loader v1: Loaded\n===\n"
    .align 4
    succStringEnd:
    mflr r3
    crclr     4*cr1+eq
    __call    r12, OSReport
    b         return

fail:
    bl        failStrEnd
    .string   "===\nKuribo Loader v1: Failed to load. Make sure you have copied the folder onto your ISO!\n===\n"
    .align 4
    failStrEnd:
    mflr r3
    crclr     4*cr1+eq
    __call    r12, OSReport

return:
    # Restore interrupts OSRestoreInterrupts
    lwz       r3, 0x50+istate(r1)
    cmpwi     r3, 0
    mfmsr     r4
    beq       unset
    ori       r5, r4, 0x8000
    b         end
    unset:
        rlwinm    r5, r4, 0,17,15
    end:
        mtmsr     r5

        extrwi    r3, r4, 1,16
    lwz       r0, 0x50+sender_lr(r1)
    lwz       r31, 0x50+r31_save(r1)
    lwz       r30, 0x50+r30_save(r1)
    mtlr      r0
    addi      r1, r1, 0x50
