#include <Dolphin/types.h>

#include <JSystem/J3D/J3DMaterial.hxx>
#include <JSystem/J3D/J3DTexture.hxx>
#include <JSystem/JDrama/JDRGraphics.hxx>
#include <JSystem/JUtility/JUTColor.hxx>
#include <JSystem/JUtility/JUTTexture.hxx>

#include <SMS/M3DUtil/SDLModel.hxx>
#include <SMS/Manager/ItemManager.hxx>
#include <SMS/Map/MapCollisionData.hxx>
#include <SMS/MoveBG/Coin.hxx>
#include <SMS/rand.h>
#include <SMS/raw_fn.hxx>

#include "object/star_bit.hxx"
#include <Manager/MarioParticleManager.hxx>

const JUtility::TColor TStarBit::sStarBitColorArray[6]{
    {0xC5, 0x1B, 0x00, 0xFF}, // RED
    {0xE7, 0xA2, 0x00, 0xFF}, // YELLOW
    {0x42, 0xA2, 0x08, 0xFF}, // GREEN
    {0x21, 0x54, 0xB5, 0xFF}, // BLUE
    {0x82, 0x00, 0x9B, 0xFF}, // PURPLE
    {0x84, 0x82, 0x84, 0xFF}  // SILVER
};

static u32 s_particle_init_tick = 0;

TStarBit::TStarBit(const char *name)
    : TCoin(name), mIsMoving(true), mIsBouncing(true), mIsAttracting(false), mBounceSpeedY(20.0f),
      mGlowEffect(nullptr), mGlowSize(0.5f), mGlowAnmSpeed(1.0f) {
    mColor = getColorFromIndex(rand() % sizeof(sStarBitColorArray));
    mGlowTick = s_particle_init_tick++;
}

TStarBit::~TStarBit() {}

void TStarBit::appear() {
    TCoin::appear();
    mIsMoving   = true;
    mIsBouncing = true;
    mIsAttracting = false;
    mModelLoadFlags &= ~0x40000;
    mObjectType |= 1;
}

// This is a hack to force binding to work (calcs physics)
void TStarBit::bind() {
    u32 breh = mStateFlags.asU32 & 0x10;
    mStateFlags.asU32 &= ~0x10;
    TCoin::bind();
    mStateFlags.asU32 |= breh;
}

void TStarBit::load(JSUMemoryInputStream &in) {
    TCoin::load(in);
    in.read(&mIsMoving, sizeof(bool));

    int colorIdx;
    in.read(&colorIdx, sizeof(int));

    if (colorIdx >= 0 && colorIdx < 6) {
        mColor = getColorFromIndex(colorIdx);
    }

    if (mIsMoving) {
        mIsBouncing = true;
    }
}

void TStarBit::loadAfter() {
    TCoin::loadAfter();

    mRotationalVelocity.y *= 10.0f;

    J3DTevStage *tevStage = mActorData->mModel->mModelData->mStages[0];

    J3DGXColor color;
    color.rgba.r = mColor.r;
    color.rgba.g = mColor.g;
    color.rgba.b = mColor.b;
    color.rgba.a = mColor.a;
    // tevStage->mTevBlock->setTevColor(2, color);

    OSReport("Trying to set the TEV color!\n");

    J3DTevBlock *tevBlock = *(J3DTevBlock **)((u8 *)tevStage + 0x28);
    tevBlock->setTevKColor(0, color);

    gpItemManager->_70 = 5.0f;  // 2.0f rotation -> 5.0f rotation
}


void TStarBit::makeMActors() {
    mActorKeeper = new TMActorKeeper(mLiveManager, 1);
    if (mModelLoadFlags & 0x8000)
        mActorKeeper->mModelFlags = 0x112F0000;
    else
        mActorKeeper->mModelFlags = 0x102F0000;

    char modelPath[128];
    snprintf(modelPath, 128, "%s.bmd", mRegisterName);

    //mActorData = mActorKeeper->createMActor(modelPath, getSDLModelFlag());
    
    mActorData = new MActor(mLiveManager->getMActorAnmData());
    
    SDLModelData *data = (SDLModelData*)loadModelData__16TModelDataKeeperFPCcUlPCc(
        "starbit.bmd", mActorKeeper->mModelFlags, "/scene/mapObj");
    SDLModel *model = new SDLModel(data, getSDLModelFlag(), 1);

    mActorData->setModel(model, getSDLModelFlag());

    if (mModelLoadFlags & 0x4000) {
        mActorData->setLightID(0);
        mActorData->_40 = 0;
    }

    calcRootMatrix();
    mActorData->calc();
    mActorData->viewCalc();
}

void TStarBit::perform(u32 flags, JDrama::TGraphics *graphics) {
    TCoin::perform(flags, graphics);

    const bool isAlive = (mStateFlags.asU32 & 0x1) == 0;
    if (!isAlive) {
        return;
    }

    if ((flags & PERFORM_ON_MOVEMENT)) {
        if (mState == 1) {
            moveObject();
        }
    }

    if ((flags & PERFORM_ON_GX)) {
        if ((mGlowTick++ % 10) == 0) {
            mGlowEffect = gpMarioParticleManager->emitAndBindToPosPtr(297, &mTranslation, 1, this);
            if (mGlowEffect) {
                mGlowEffect->mSize1 = {mGlowSize, mGlowSize, mGlowSize};
                mGlowEffect->mSize3 = {mGlowSize, mGlowSize, mGlowSize};
            }
        }
    }
}

void TStarBit::moveObject() {
    for (int i = 0; i < mNumObjs; ++i) {
        THitActor *actor = mCollidingObjs[i];
        if (actor->mObjectID != OBJECT_ID_MARIO) {
            continue;
        }
        mIsAttracting = true;
        mAttractionPoint = &actor->mTranslation;
        mGravity         = 0.0f;
        break;
    }

    //mGroundY = gpMapCollisionData->checkGround(mTranslation.x, mTranslation.y, mTranslation.z, 0,
    //                                           &mFloorBelow);

    if (mIsAttracting && mAttractionPoint) {
        // Get normalized direction
        TVec3f direction = *mAttractionPoint - mTranslation;
        direction.y += 50.0f;
        PSVECNormalize(direction, direction);
        
        // Move towards it
        direction.scale(2.f);
        mSpeed += direction;
        
        // Drag
        mSpeed.scale(0.9f);
        mTranslation += mSpeed;
        return;
    }

    if (!mIsMoving) {
        mGravity = 0.0f;
        mSpeed   = {0.0f, 0.0f, 0.0f};
        return;
    }

    mGravity = 0.50f;

    TLiveActor::moveObject();

    mSpeed.y = Max(-75.0f, mSpeed.y);
}

void TStarBit::touchGround(TVec3f *point) {
    // Snap the Star Bit to the floor to prevent it from getting stuck
    mTranslation.y = mGroundY + 10.0f;

    // If this is true the bouncing is permanent
    if (mIsBouncing) {
        mSpeed.y = mBounceSpeedY;
        mSpeed.x *= 0.2f;
        mSpeed.z *= 0.2f;
    } else {
        mSpeed.y = -mSpeed.y * 0.7f;
        mSpeed.x *= 0.2f;
        mSpeed.z *= 0.2f;
    }

    if (mState == 2) {
        mObjectType &= ~1;
        mModelLoadFlags |= 0x40000;
        mStateFlags.asU32 |= 0x10;
        mStateTimer = mKillTimer;
        mState      = 1;
    }
}

void TStarBit::touchWater(THitActor *actor) {
    mIsAttracting = true;
    mAttractionPoint = gpMarioPos;
}

void TStarBit::taken(THitActor *actor) {
    if (PSVECDistance(mTranslation, actor->mTranslation) > 150.0f) {
        return;
    }

    TCoin::taken(actor);

    mSpeed = TVec3f::zero();
    mIsAttracting = false;
    mIsBouncing   = false;
}

#define MESSAGE_TOUCH_WATER 15
bool TStarBit::receiveMessage(THitActor *sender, u32 message) {
    if (TCoin::receiveMessage(sender, message)) {
        return true;
    }

    if (message == MESSAGE_TOUCH_WATER) {
        mIsAttracting    = true;
        mAttractionPoint = gpMarioPos;
        return false;
    }

    return false;
}

static hit_data starbit_hit_data{.mAttackRadius  = 300.0f,
                                 .mAttackHeight  = 300.0f,
                                 .mReceiveRadius = 300.0f,
                                 .mReceiveHeight = 300.0f};

static obj_hit_info starbit_collision_data{
    ._00 = 1, .mType = 0x81000000, ._08 = 0, .mHitData = &starbit_hit_data};

static sound_data starbit_sound_data{
    ._00             = 0xFFFFFFFF,
    ._04             = 0xFFFFFFFF,
    ._08             = 0xFFFFFFFF,
    .mUnkSoundID     = 0xFFFFFFFF,
    .mReboundSoundID = 0xFFFFFFFF,
    ._14             = 0xFFFFFFFF,
    ._18             = 0xFFFFFFFF,
    ._1C             = 0xFFFFFFFF,
    ._20             = 0xFFFFFFFF,
    ._24             = 0xFFFFFFFF,
};

static sound_info starbit_sound_info{.mLength = 10, .mSoundData = &starbit_sound_data};

constexpr const char *gCoinManagerName = "\x83\x41\x83\x43\x83\x65\x83\x80\x83\x7D\x83\x6C\x81\x5B\x83\x57\x83\x83\x81\x5B\x00\x00";

ObjData starBitData{.mMdlName          = "starbit",
                    .mObjectID         = 0x2000000E /*0x80000FFF*/,
                    .mLiveManagerName  = gCoinManagerName,  // const_cast<char *>("木マネージャー")
                    .mObjKey           = gUnkManagerName,   // const_cast<char *>("waterballoon"),
                    .mAnimInfo         = nullptr,
                    .mObjCollisionData = &starbit_collision_data,
                    .mMapCollisionInfo = nullptr,
                    .mSoundInfo        = &starbit_sound_info,
                    .mPhysicalInfo     = nullptr,
                    .mSinkData         = nullptr,
                    ._28               = nullptr,
                    .mBckMoveData      = nullptr,
                    ._30               = 50.0f,
                    .mUnkFlags         = 0x08144100 /*0x02130100*/,
                    .mKeyCode          = cexp_calcKeyCode("StarBit")};