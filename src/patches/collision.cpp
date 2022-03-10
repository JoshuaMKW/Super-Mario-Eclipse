#include "SME.hxx"

#if SME_NO_DOWNWARP

// Make illegal data not downwarp anymore
SME_WRITE_32(SME_PORT_REGION(0x8018D08C, 0x80185914, 0, 0), 0x60000000);

#endif