#pragma once

#include "funcs.hxx"

#define malloc(size, alignment) __nw__FUli(size, alignment)
#define hmalloc(heap, size, alignment) __nw__FUlP7JKRHeapi(size, heap, alignment)
#define calloc(size, alignment) memset(malloc(size, alignment), 0, size)
#define hcalloc(heap, size, alignment) memset(hmalloc(size, heap, alignment), 0, size)
#define free(pointer) __dl__FPv(pointer)
#define angleToRadians ((float (*)(float angle))0x80003400)
#define radiansToAngle ((float (*)(float angle))0x8000341C)
#define TMario__setAttributes ((void (*)(TMario *gpMario))0x800050D4)
#define sqrtf(input) powf(input, 0.5)
#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))
#define PI 3.14159265f

#define dcbst(_val) asm volatile("dcbst 0, %0" \
                                 :             \
                                 : "r"(_val))
#define dcbf(_val) asm volatile("dcbf 0, %0" \
                                :            \
                                : "r"(_val))
#define icbi(_val) asm volatile("icbi 0, %0" \
                                :            \
                                : "r"(_val))

#define SME_DEBUG true

#define DEVICE_TYPE_GAMECUBE_DVD (('G' << 24) | ('D' << 16) | ('V' << 8) | 'D')
#define DVD_ERROR_CANCELED -3
#define DVD_ERROR_COVER_CLOSED -4
#define DVD_ERROR_FATAL -1
#define DVD_ERROR_IGNORED -2
#define DVD_ERROR_OK 0
#define DVD_RESETHARD 0
#define DVD_RESETNONE 2
#define DVD_RESETSOFT 1
#define DVD_SetUserData(block, data) ((block)->usrdata = (data))
#define DVD_SPINMOTOR_ACCEPT 0x00004000
#define DVD_SPINMOTOR_CHECKDISK 0x00008000
#define DVD_SPINMOTOR_DOWN 0x00000000
#define DVD_SPINMOTOR_UP 0x00000100
#define DVD_STATE_BUSY 1
#define DVD_STATE_CANCELED 10
#define DVD_STATE_COVER_CLOSED 3
#define DVD_STATE_COVER_OPEN 5
#define DVD_STATE_END 0
#define DVD_STATE_FATAL_ERROR -1
#define DVD_STATE_IGNORED 8
#define DVD_STATE_MOTOR_STOPPED 7
#define DVD_STATE_NO_DISK 4
#define DVD_STATE_RETRY 11
#define DVD_STATE_WAITING 2
#define DVD_STATE_WRONG_DISK 6

#define MAX_SHINES 300
#define MEM1_LO (u32 *)0x80000000
#define MEM1_HI (u32 *)0x81800000

#define BGM_DOLPIC 0x80010001
#define BGM_BIANCO 0x80010002
#define BGM_MAMMA 0x80010003
#define BGM_PINNAPACO_SEA 0x80010004
#define BGM_PINNAPACO 0x80010005
#define BGM_MARE_SEA 0x80010006
#define BGM_MONTEVILLAGE 0x80010007
#define BGM_SHILENA 0x80010008
#define BGM_RICCO 0x80010009
#define BGM_GET_SHINE 0x8001000A
#define BGM_CHUBOSS 0x8001000B
#define BGM_MISS 0x8001000C
#define BGM_BOSS 0x8001000D
#define BGM_MAP_SELECT 0x8001000E
#define BGM_BOSSPAKU_DEMO 0x8001000F
#define BGM_MAIN_TITLE 0x80010010
#define BGM_CHUBOSS2 0x80010011
#define BGM_EXTRA 0x80010012
#define BGM_DELFINO 0x80010013
#define BGM_MAREVILLAGE 0x80010014
#define BGM_CORONA 0x80010015
#define BGM_KAGEMARIO 0x80010016
#define BGM_CAMERA 0x80010017
#define BGM_MONTE_ONSEN 0x80010018
#define BGM_MECHAKUPPA 0x80010019
#define BGM_AIRPORT 0x8001001A
#define BGM_UNDERGROUND 0x8001001B
#define BGM_TITLEBACK 0x8001001C
#define BGM_MONTE_NIGHT 0x8001001D
#define BGM_CASINO 0x8001001E
#define BGM_EVENT 0x8001001F
#define BGM_TIME_IVENT 0x80010020
#define BGM_SKY_AND_SEA 0x80010021
#define BGM_MONTE_RESCUE 0x80010022
#define BGM_MERRY_GO_ROUND 0x80010023
#define BGM_SCENARIO_SELECT 0x80010024
#define BGM_FANFARE_CASINO 0x80010025
#define BGM_FANFARE_RACE 0x80010026
#define BGM_CAMERA_KAGE 0x80010027
#define BGM_GAMEOVER 0x80010028
#define BGM_BOSSHANA_2ND3RD 0x80010029
#define BGM_BOSSGESO_2ND3RD 0x8001002A
#define BGM_CHUBOSS_MANTA 0x8001002B
#define BGM_MONTE_LAST 0x8001002C
#define BGM_SHINE_APPEAR 0x8001002D
#define BGM_KUPPA 0x8001002E
#define BGM_SPACEWORLD 0x80011030

#define SND_PIPE 9598

//#define GFX_OFF				0x09A4 - E3
#define GFX_OFF 0x07F4

//#define Fruit IDs
#define FRUIT_COCONUT 0x40000390
#define FRUIT_PAPAYA 0x40000391
#define FRUIT_PINEAPPLE 0x40000392
#define FRUIT_DURIAN 0x40000393
#define FRUIT_BANANA 0x40000394

//define global addresses
#define WaterAddr 0x803DD898
#define CustomInfoInstance 0x80003BD0
#define YoshiColor 0x8039F934
#define ShineShadowCoordinates 0x803A1C94
#define YoshiJuiceColor 0x803DD89C
#define TApplicationInstance 0x803E9700
#define SVolumeList 0x804042B4
#define StreamVolume 0x8040C1C0
#define CPolarSubCameraInstance 0x8040D0A8
#define TCameraShakeInstance 0x8040D0B8
#define TCardLoadInstance 0x8040DDE0
#define TMapInstance 0x8040DE98
#define TMapCollisionDataInstance 0x8040DEA0
#define TPollutionManagerInstance 0x8040DED0
#define GamePad1Instance 0x8040E0D0
#define GamePad2Instance 0x8040E0D4
#define GamePad3Instance 0x8040E0D8
#define GamePad4Instance 0x8040E0DC
#define TMarioInstance 0x8040E108
#define TMarioCoordinates 0x8040E10C
#define TWaterManagerInstance 0x8040E138
#define TFlagManagerInstance 0x8040E160
#define TMarDirectorInstance 0x8040E178
#define MSoundInstance 0x8040E17C
#define TCardManagerInstance 0x8040E184
#define ARCBufferMario 0x8040E198
#define GameHeapBlock 0x8040E1A4
#define ArBkConsole 0x8040E1A8
#define ArBkGuide 0x8040E1B0
#define DemoBGM 0x8040E1EC
#define StageBGM 0x8040E1F0
#define JKRSystemHeap 0x8040E290
#define JKRCurrentHeap 0x8040E294
#define JKRRootHeap 0x8040E298
#define StreamPitch 0x80417248
#define StreamSpeed 0x8041731C
