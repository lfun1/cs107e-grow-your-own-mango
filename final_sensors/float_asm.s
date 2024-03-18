/* File: float_asm.s
 * -----------------
 * Function to turn on and configure single-precision floating-point.
 * Lisa Fung
 */

.attribute arch, "rv64imf_zicsr"  # from timer_asm.s, include 'f' flag for float

# setup adapted from interrupt_asm.s
.globl config_float
config_float:
    # turn on floating-point unit (bits 13-14)
    # 1 in FS[0:1] means 'Initial'
    li a0,1<<13
    csrs mstatus,a0
    
	# read floating-point unit
    csrr a0,mstatus
    ret
    
