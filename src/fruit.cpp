#include <SMS/MoveBG/ResetFruit.hxx>

#include <BetterSMS/module.hxx>


// Fruit don't time out
SMS_WRITE_32(SMS_PORT_REGION(0x8040C918, 0x80404078, 0, 0), 0x7FFFFFFF);