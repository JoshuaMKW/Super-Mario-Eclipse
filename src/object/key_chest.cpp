#include <Dolphin/GX.h>
#include <Dolphin/MTX.h>
#include <Dolphin/types.h>

#include <BetterSMS/libs/constmath.hxx>
#include <BetterSMS/module.hxx>
#include <JSystem/J3D/J3DModel.hxx>
#include <JSystem/J3D/J3DModelLoaderDataBase.hxx>
#include <JSystem/JDrama/JDRNameRef.hxx>
#include <SMS/M3DUtil/MActor.hxx>
#include <SMS/M3DUtil/MActorKeeper.hxx>
#include <SMS/MSound/MSoundSESystem.hxx>
#include <SMS/Manager/ModelWaterManager.hxx>
#include <SMS/Map/BGCheck.hxx>
#include <SMS/Player/Mario.hxx>
#include <object/follow_key.hxx>
#include <SMS/Manager/ItemManager.hxx>
#include <object/key_chest.hxx>
#include "player.hxx"
TKeyChest::TKeyChest(const char *name) : TMapObjGeneral(name) {}

TKeyChest::~TKeyChest() {}

void TKeyChest::kill() {}
void TKeyChest::control() { 
    TMapObjGeneral::control(); }

void TKeyChest::playIdleAnim() {}

void TKeyChest::touchPlayer(THitActor *actor) {

    OSReport("Touched Actor ID %X: \n ", actor->mObjectID);

    if (actor->mObjectID == OBJECT_ID_MARIO) {
        mFollowActor = reinterpret_cast<TMario *>(actor);
        auto *data   = SME::Player::getEclipseData(mFollowActor);
        if (data->mIsHoldingKey)
        {

            OSReport("Touching Chest With Key");
            auto *nameRef = TMarNameRefGen::getInstance()->getRootNameRef();
            u16 keyCode   = JDrama::TNameRef::calcKeyCode("follow_key");
            if (TNameRef *p = nameRef->searchF(keyCode, "follow_key")) {
                TFollowKey *foll = reinterpret_cast<TFollowKey *>(p);
                foll->makeObjDead();

                gpItemManager->makeShineAppearWithTime("Treasure_Shine", 0, mTranslation.x,
                                                       mTranslation.y + 60.f, mTranslation.z,0,0,0);
               
            }
        }
    }
}
static hit_data keyChest_hit_data{.mAttackRadius  = 300.0f,
                                  .mAttackHeight  = 300.0f,
                                  .mReceiveRadius = 300.0f,
                                  .mReceiveHeight = 300.0f};

static obj_hit_info keyChest_collision_data{
    ._00 = 1, .mType = 0xFC000000, ._08 = 0, .mHitData = &keyChest_hit_data};

ObjData keyChestData{.mMdlName  = "key_chest",
                     .mObjectID = 0x80000400 /*0x80000FFF*/,
                     .mLiveManagerName = gLiveManagerName,  // const_cast<char *>("木マネージャー")
                     .mObjKey           = gUnkManagerName,  // const_cast<char *>("waterballoon"),
                     .mAnimInfo         = nullptr,
                     .mObjCollisionData = &keyChest_collision_data,
                     .mMapCollisionInfo = nullptr,
                     .mSoundInfo        = nullptr,
                     .mPhysicalInfo     = nullptr,
                     .mSinkData         = nullptr,
                     ._28               = nullptr,
                     .mBckMoveData      = nullptr,
                     ._30               = 50.0f,
                     .mUnkFlags         = 0x10004000 /*0x02130100*/,
                     .mKeyCode          = cexp_calcKeyCode("key_chest")};