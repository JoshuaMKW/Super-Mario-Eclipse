.set TRUE, 1
# Use for comparisons where you need to check if something has returned true

.set FALSE, 0
# Use for comparisons where you need to check if something has returned false

.set NULLPTR, 0
# Use for when you need to check if a pointer is null

.set NULL, 0
# Use for when you are checking for a null character

.set NONE, __none__
# Use as an argument to macros for specifying that you aren't providing an optional argument
#
# An example usage is: __set_count_register r0 NONE

.set THIS, r3
# Use to specify the instance of a class who's function was called
#
# Value: r3

.set SELF, r3
# Use to specify the instance of a class who's function was called
#
# Value: r3

.set AMERICA, 'E'
# The American region specifier, used in disc IDs and in region flexible assembly

.set EUROPE, 'P'
# The European region specifier, used in disc IDs and in region flexible assembly

.set JAPAN, 'J'
# The Japanese region specifier, used in disc IDs and in region flexible assembly

.set KOREA, 'K'
# The Korean region specifier, used in disc IDs and in region flexible assembly

.set CHINA, 'C'
# The Chinese region specifier, used in region flexible assembly

.set TAIWAN, 'T'
# The Taiwanese region specifier, used in disc IDs and in region flexible assembly

.macro __set register value
# Syntax: __set <register> <value>
#
# Sets a register to be <value>
    .if \value > 0x7FFF || \value < -0x7FFF
        .if ((\value >> 16) != 0) && ((\value & 0xFFFF) == 0)
            lis \register, \value@h
        .else
            lis \register, \value@h
            ori \register, \register, \value & 0xFFFF
        .endif
    .else
        li \register, \value
    .endif
.endm

.macro __push_stack floor_gpr size save_lr
# Syntax: __push_stack <floor_gpr> <size> <save_lr>
#
# Pushes the stack frame by <size>, storing all gprs starting at <floor_gpr>
# <save_lr> preserves the lr state, and is an optional argument, use NONE if not desired
    .if \size <= 0
        .error "Negative and null stack sizes are invalid"
    .endif

    .if \save_lr == TRUE
        mflr r0
        stw r0, 0x4 (sp)
    .endif

    stwu sp, (((\size + 15) & -16) * -1) (sp)

    .if \floor_gpr == 31 || \floor_gpr == r31
        stw \floor_gpr, 0x8 (sp)
    .else
        stmw \floor_gpr, 0x8 (sp)
    .endif
.endm

.macro __pop_stack floor_gpr size restore_lr
# Syntax: __pop_stack <floor_gpr> <size> <save_lr>
#
# Pops the stack frame by <size>, restoring all gprs starting at <floor_gpr>
# <restore_lr> restores the lr state, and is an optional argument; use NONE if not desired
    .if \size <= 0
        .error "Negative and null stack sizes are invalid"
    .endif
    
    .if \floor_gpr == 31 || \floor_gpr == r31
        lwz \floor_gpr, 0x8 (sp)
    .else
        lmw \floor_gpr, 0x8 (sp)
    .endif

    addi sp, sp, ((\size + 15) & -16)

    .if \restore_lr == TRUE
        lwz r0, 0x4 (sp)
        mtlr r0
    .endif
.endm

.macro __set_count_register register value
# Syntax: __set_count_register <register> <value>
#
# Sets the count register to be the value contained by <register>
# <value> sets the value of <register> before the operation, and is an optional argument; use NONE if not desired
    .if \value == NONE
        mtctr \register
    .else
        __set \register, \value
        mtctr \register
    .endif
.endm

.macro __set_link_register register value
# Syntax: __set_link_register <register> <value>
#
# Sets the link register to be the value contained by <register>
# <value> sets the value of <register> before the operation, and is an optional argument; use NONE if not desired
    .if \value == NONE
        mtlr \register
    .else
        __set \register, \value
        mtlr \register
    .endif
.endm

.macro __set_spr spr register value
# Syntax: __set_spr <register> <value>
#
# Sets the special purpose register specified by <spr> to be the value contained by <register>
# <value> sets the value of <register> before the operation, and is an optional argument; use NONE if not desired
    .if \value == NONE
        mtlr \register
    .else
        __set \register, \value
        mtspr \spr \register
    .endif
.endm

.macro __branch register address
# Syntax: __branch <register> <address>
#
# Branches to the address contained by <register>
# <address> sets the value of <register> before the operation, and is an optional argument; use NONE if not desired
    .if \address == NONE
        mtctr \register
    .else
        __set \register \address
        mtctr \register
    .endif
	bctr
.endm

.macro __call register address
# Syntax: __call <register> <address>
#
# Calls the function using the address contained by <register>
# <address> sets the value of <register> before the operation, and is an optional argument; use NONE if not desired
    .if \address == NONE
        mtctr \register
    .else
        __set \register \address
        mtctr \register
    .endif
	bctrl
.endm

.macro __OSGetTime regA regB
# Syntax: __OSGetTime <regA> <regB>
#
# Returns the synced time base value into the registers <regA> (upper) and <regB> (lower)
    mftbu \regA
    mftbl \regB
    mftbu r0
    cmpw \regA, r0
    bne+ -0x10
.endm

.macro __OSGetTick register
# Syntax: __OSGetTick <register>
#
# Returns the lower 32 bits of the time base value into <register>
    mftbl \register
.endm
