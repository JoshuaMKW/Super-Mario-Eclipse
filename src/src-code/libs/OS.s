.text

.global DCDisable
.global ICDisable

DCDisable:
    sync
    mfspr r3, HID0
    andi. r3, r3, -0x4000
    mtspr HID0, r3
    blr

ICDisable:
    sync
    mfspr r3, HID0
    andi. r3, r3, -0x8000
    mtspr HID0, r3
    blr