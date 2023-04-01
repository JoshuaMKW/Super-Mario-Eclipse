#include <Dolphin/types.h>
#include <Dolphin/mem.h>
#include <Dolphin/ctype.h>
#include <Dolphin/string.h>
#include <Dolphin/CARD.h>
#include <Dolphin/VI.h>

#include <JSystem/JDrama/JDRViewObjPtrListT.hxx>
#include <JSystem/JDrama/JDRCamera.hxx>
#include <JSystem/JDrama/JDRDStage.hxx>
#include <JSystem/JDrama/JDRDStageGroup.hxx>
#include <JSystem/JDrama/JDRScreen.hxx>
#include <JSystem/JUtility/JUTColor.hxx>
#include <JSystem/JUtility/JUTRect.hxx>
#include <JSystem/J2D/J2DPicture.hxx>
#include <JSystem/J2D/J2DTextBox.hxx>
#include <JSystem/JUtility/JUTTexture.hxx>
#include <JSystem/JKernel/JKRDvdRipper.hxx>

#include <SMS/Manager/FlagManager.hxx>
#include <SMS/Manager/RumbleManager.hxx>
#include <SMS/GC2D/SMSFader.hxx>
#include <SMS/MSound/MSBGM.hxx>
#include <SMS/MSound/MSound.hxx>
#include <SMS/MSound/MSoundSESystem.hxx>
#include <SMS/System/Application.hxx>
#include <SMS/Camera/CubeManagerBase.hxx>
#include <SMS/MarioUtil/DrawUtil.hxx>
#include <SMS/MarioUtil/gd-reinit-gx.hxx>
#include <SMS/System/Application.hxx>
#include <SMS/System/CardManager.hxx>
#include <SMS/System/Resolution.hxx>
#include <SMS/System/RenderModeObj.hxx>
#include <SMS/Manager/FlagManager.hxx>
#include <SMS/raw_fn.hxx>

#include <BetterSMS/music.hxx>
#include <BetterSMS/libs/constmath.hxx>

#include "globals.hxx"

#include "menu/character_select.hxx"

constexpr size_t PlayerMax = 1;


static JKRMemArchive *sResourceArchive = nullptr;

CharacterSelectDirector::~CharacterSelectDirector() { gpMSound->exitStage(); }

s32 CharacterSelectDirector::direct() {
    TSMSFader *fader = gpApplication.mFader;

    if (mState == State::INIT) {
        if (!OSIsThreadTerminated(&gSetupThread))
            return 0;

        int *joinBuf[2];
        OSJoinThread(&gSetupThread, (void **)joinBuf);

        gpMSound->initSound();
        gpMSound->enterStage(MS_WAVE_DELFINO_PLAZA, 1, 2);

        Music::setMaxVolume(0x7F);
        Music::setVolume(0x7F, 0x7F);
        Music::queueSong("character_select");
        Music::playSong();

        fader->startFadeinT(0.8f);

        if (mSelectScreen->mCharacterInfos.size() == 1) {
            for (auto &s_info : mSelectScreen->mSelectionInfos) {
                s_info.mHandIcon->mIsVisible = false;
            }
        }

        mState = State::CONTROL;
        return 0;
    }

    TDirector::direct();

#define SMS_CHECK_RESET_FLAG(gamepad) (((1 << (gamepad)->mPort) & TMarioGamePad::mResetFlag) == 1)

    if (mState == State::CONTROL) {
        if (fader->mFadeStatus == TSMSFader::FADE_OFF || fader->mFadeStatus == TSMSFader::FADE_IN) {
            if (SMS_CHECK_RESET_FLAG(gpApplication.mGamePads[0])) {
                mSelectScreen->mShouldReadInput = false;
                fader->startWipe(TSMSFader::WipeRequest::FADE_DIAMOND_OUT, 1.0f, 0.0f);
            }
        } else if (fader->mFadeStatus == TSMSFader::FADE_ON) {
            return TApplication::CONTEXT_GAME_INTRO;
        }

        if (mSelectScreen->mCharacterInfos.size() == 1) {
            mState = State::EXIT;
        } else {
            bool any_unselected = false;
            for (auto &s_info : mSelectScreen->mSelectionInfos) {
                if (!s_info.mIsSelected) {
                    any_unselected = true;
                    break;
                }
            }

            if (!any_unselected) {
                mState = State::EXIT;
            }
        }
    }

#undef SMS_CHECK_RESET_FLAG


    s32 ret = 1;
    switch (mState) {
    case State::INIT:
        break;
    case State::CONTROL:
        mSelectScreen->mPerformFlags &= ~0b0001;
        
        if (fader->mFadeStatus == TSMSFader::FADE_OFF) {
            mSelectScreen->mShouldReadInput = true;
        }

        break;
    case State::EXIT: {
        ret = exit();
        break;
    }
    }
    return ret;
}

void CharacterSelectDirector::initialize() {
    sResourceArchive = new JKRMemArchive();

    void *archive = JKRDvdRipper::loadToMainRAM("/data/char_select.szs", nullptr, EXPAND, 0, nullptr,
                                                JKRDvdRipper::HEAD,
                                                0, nullptr);

    SMS_DEBUG_ASSERT(archive, "Failed to find character select assets (./data/char_select.szs)!");

    sResourceArchive->mountFixed(archive, UNK_0);

    initializeDramaHierarchy();
    initializeLayout();
}

void CharacterSelectDirector::initializeDramaHierarchy() {
    auto *stageObjGroup = reinterpret_cast<JDrama::TDStageGroup *>(mViewObjStageGroup);
    auto *rootObjGroup  = new JDrama::TViewObjPtrListT<JDrama::TViewObj>("Root View Objs");
    mViewObjRoot        = rootObjGroup;
    
    JDrama::TRect screenRect{0, 0, SMSGetTitleRenderWidth(), SMSGetTitleRenderHeight()};

    auto *group2D = new JDrama::TViewObjPtrListT<JDrama::TViewObj>("Group 2D");
    {
        mSelectScreen = new CharacterSelectScreen();
        group2D->mViewObjList.insert(group2D->mViewObjList.end(), mSelectScreen);

        //mSaveErrorPanel = new SaveErrorPanel(this, mController);
        //mSaveErrorPanel->mPerformFlags |= 0b1011;  // Disable view and input by default
        //group2D->mViewObjList.insert(group2D->mViewObjList.end(), mSaveErrorPanel);

        rootObjGroup->mViewObjList.insert(rootObjGroup->mViewObjList.end(), group2D);
    }

    {
        auto *group3D = new JDrama::TViewObjPtrListT<JDrama::TViewObj>("Group 3D");

        rootObjGroup->mViewObjList.insert(rootObjGroup->mViewObjList.end(), group3D);
        stageObjGroup->mViewObjList.insert(stageObjGroup->mViewObjList.end(), group3D);
    }

    {
        auto *group2DParticle = new JDrama::TViewObjPtrListT<JDrama::TViewObj>("Group 2D Particle");

        rootObjGroup->mViewObjList.insert(rootObjGroup->mViewObjList.end(), group2DParticle);
        stageObjGroup->mViewObjList.insert(stageObjGroup->mViewObjList.end(), group2DParticle);
    }

    {
        auto *stageDisp = new JDrama::TDStageDisp("<DStageDisp>", {0});

        auto *efbCtrl = stageDisp->getEfbCtrlDisp();
        efbCtrl->setSrcRect(screenRect);

        rootObjGroup->mViewObjList.insert(rootObjGroup->mViewObjList.end(), stageDisp);
        stageObjGroup->mViewObjList.insert(stageObjGroup->mViewObjList.end(), stageDisp);
    }

    {
        auto *screen = new JDrama::TScreen(screenRect, "Screen 2D");

        auto *orthoProj                = new JDrama::TOrthoProj();
        orthoProj->mProjectionField[0] = -BetterSMS::getScreenRatioAdjustX();
        orthoProj->mProjectionField[2] = BetterSMS::getScreenRenderWidth();
        screen->assignCamera(orthoProj);

        screen->assignViewObj(group2D);

        rootObjGroup->mViewObjList.insert(rootObjGroup->mViewObjList.end(), screen);
        stageObjGroup->mViewObjList.insert(stageObjGroup->mViewObjList.end(), screen);
    }
}

void CharacterSelectDirector::initializeLayout() {
    const int screenOrthoWidth   = BetterSMS::getScreenOrthoWidth();
    const int screenRenderWidth  = BetterSMS::getScreenRenderWidth();
    const int screenRenderHeight = 480;
    const int screenAdjustX      = BetterSMS::getScreenRatioAdjustX();

    const f32 icon_ratio = scaleLinearAtAnchor(BetterSMS::getScreenToFullScreenRatio(), 1.8f, 1.0f);
    const f32 poster_ratio = scaleLinearAtAnchor(BetterSMS::getScreenToFullScreenRatio(), 1.2f, 1.0f);

    const int margin_width = 10;
    const int poster_width = 130;

    mSelectScreen->mScreen = new J2DSetScreen("layout.blo", sResourceArchive);
    mSelectScreen->mScreen->mRect = {0, 0, screenOrthoWidth, screenRenderHeight};

    J2DPane *iconListPane = mSelectScreen->mScreen->search('list');

    JUTTexture *poster_image = new JUTTexture("timg/poster.bti");

    if (mIsMario) {
        JUTTexture *character_icon = new JUTTexture("timg/mario.bti");
        JUTTexture *text_icon      = new JUTTexture("timg/mario_t.bti");

        CharacterInfo info;
        info.mPlayer = SME::CharacterID::MARIO;
        info.mIcon   = new J2DPicture('mari', {0, 0, 0, 0});
        info.mIcon->insert(character_icon, 0, 1.0f);
        info.mIcon->mRect = {30, 98, 120, 188};
        info.mLabel        = new J2DPicture('labm', {0, 0, 0, 0});
        info.mLabel->insert(text_icon, 0, 1.0f);
        info.mLabel->mRect = {28, 68, 122, 116};
        info.mPoster = new J2DPicture('posm', {0, 0, 0, 0});
        info.mPoster->insert(poster_image, 0, 1.0f);
        info.mPoster->mRect = {0, 20, poster_width, 200};

        iconListPane->mChildrenList.append(&info.mPoster->mPtrLink);
        iconListPane->mChildrenList.append(&info.mLabel->mPtrLink);
        iconListPane->mChildrenList.append(&info.mIcon->mPtrLink);

        mSelectScreen->mCharacterInfos.push_back(info);
    }

    if (mIsLuigi) {
        JUTTexture *character_icon = new JUTTexture("timg/luigi.bti");
        JUTTexture *text_icon      = new JUTTexture("timg/luigi_t.bti");

        CharacterInfo info;
        info.mPlayer = SME::CharacterID::LUIGI;
        info.mIcon   = new J2DPicture('lugi', {0, 0, 0, 0});
        info.mIcon->insert(character_icon, 0, 1.0f);
        info.mIcon->mRect = {30, 98, 120, 188};
        info.mLabel       = new J2DPicture('labl', {0, 0, 0, 0});
        info.mLabel->insert(text_icon, 0, 1.0f);
        info.mLabel->mRect = {28, 68, 122, 116};
        info.mPoster      = new J2DPicture('posm', {0, 0, 0, 0});
        info.mPoster->insert(poster_image, 0, 1.0f);
        info.mPoster->mRect = {0, 20, poster_width, 200};

        iconListPane->mChildrenList.append(&info.mPoster->mPtrLink);
        iconListPane->mChildrenList.append(&info.mLabel->mPtrLink);
        iconListPane->mChildrenList.append(&info.mIcon->mPtrLink);

        mSelectScreen->mCharacterInfos.push_back(info);
    }

    if (mIsPiantissimo) {
        JUTTexture *character_icon = new JUTTexture("timg/piantissimo.bti");
        JUTTexture *text_icon      = new JUTTexture("timg/piantissimo_t.bti");

        CharacterInfo info;
        info.mPlayer = SME::CharacterID::PIANTISSIMO;
        info.mIcon   = new J2DPicture('ptmo', {0, 0, 0, 0});
        info.mIcon->insert(character_icon, 0, 1.0f);
        info.mIcon->mRect = {30, 98, 120, 188};
        info.mLabel       = new J2DPicture('labp', {0, 0, 0, 0});
        info.mLabel->insert(text_icon, 0, 1.0f);
        info.mLabel->mRect = {28, 68, 122, 116};
        info.mPoster      = new J2DPicture('posm', {0, 0, 0, 0});
        info.mPoster->insert(poster_image, 0, 1.0f);
        info.mPoster->mRect = {0, 20, poster_width, 200};

        iconListPane->mChildrenList.append(&info.mPoster->mPtrLink);
        iconListPane->mChildrenList.append(&info.mLabel->mPtrLink);
        iconListPane->mChildrenList.append(&info.mIcon->mPtrLink);

        mSelectScreen->mCharacterInfos.push_back(info);
    }

    if (mIsShadowMario) {
        JUTTexture *character_icon = new JUTTexture("timg/shadow_mario.bti");
        JUTTexture *text_icon      = new JUTTexture("timg/shadow_mario_t.bti");

        CharacterInfo info;
        info.mPlayer = SME::CharacterID::SHADOW_MARIO;
        info.mIcon   = new J2DPicture('smar', {0, 0, 0, 0});
        info.mIcon->insert(character_icon, 0, 1.0f);
        info.mIcon->mRect = {30, 98, 120, 188};
        info.mLabel       = new J2DPicture('labs', {0, 0, 0, 0});
        info.mLabel->insert(text_icon, 0, 1.0f);
        info.mLabel->mRect = {28, 68, 122, 116};
        info.mPoster      = new J2DPicture('posm', {0, 0, 0, 0});
        info.mPoster->insert(poster_image, 0, 1.0f);
        info.mPoster->mRect = {0, 20, poster_width, 200};

        iconListPane->mChildrenList.append(&info.mPoster->mPtrLink);
        iconListPane->mChildrenList.append(&info.mLabel->mPtrLink);
        iconListPane->mChildrenList.append(&info.mIcon->mPtrLink);

        mSelectScreen->mCharacterInfos.push_back(info);
    }

    const int characters = mSelectScreen->mCharacterInfos.size();

    int current_x = 260 - (poster_width * characters) / 2 - (margin_width * (characters - 1)) / 2;
    for (int i = 0; i < characters; ++i) {
        CharacterInfo &info = mSelectScreen->mCharacterInfos.at(i);
        int scaled_x        = ((current_x + (poster_width / 2) - 260) * poster_ratio) + 260 - poster_width / 2;
        info.mIcon->mRect.move(scaled_x + 32, 96);
        info.mLabel->mRect.move(scaled_x + 28, 62);
        info.mPoster->mRect.move(scaled_x, 20);
        current_x += poster_width + margin_width;
    }

    for (int i = 0; i < PlayerMax; ++i) {
        SelectionInfo s_info = SelectionInfo();
        s_info.mIndex = 0;
        s_info.mIsSelected = false;
        s_info.mController = gpApplication.mGamePads[i];

        CharacterInfo &info = mSelectScreen->mCharacterInfos.at(i);

        char tex_path[32];
        snprintf(tex_path, 32, "timg/hand_p%i.bti", i+1);

        {
            JUTTexture *select_icon = new JUTTexture(tex_path);

            J2DPicture *select_picture = new J2DPicture('hnd\0' | i | 0x30, {0, 0, 0, 0});
            select_picture->insert(select_icon, 0, 1.0f);
            select_picture->mRect.move(info.mPoster->mRect.mX1 + 50, info.mPoster->mRect.mY2 + 14);
            select_picture->mRect.resize(38, 56);
            iconListPane->mChildrenList.append(&select_picture->mPtrLink);

            s_info.mHandIcon = select_picture;
        }

        {
            for (int n = 0; n < 16; ++n) {
                char goop_tex_path[64];
                snprintf(goop_tex_path, 64, "/char_select/timg/goop_%i/m_%i.bti", i + 1, n);

                auto *goop_anim_tex =
                    reinterpret_cast<const ResTIMG *>(JKRFileLoader::getGlbResource(goop_tex_path));
                s_info.mGoopTextures.push_back(goop_anim_tex);
            }

            JUTTexture *texture = new JUTTexture;
            auto *timg =
                reinterpret_cast<const ResTIMG *>(s_info.mGoopTextures.at(0));
            texture->mTexObj2.val[2] = 0;
            texture->storeTIMG(timg);
            texture->_50 = false;

            J2DPicture *goop_picture = new J2DPicture('gup\0' | i | 0x30, {0, 0, 0, 0});
            goop_picture->insert(texture, 0, 1.0f);
            goop_picture->mRect.resize(130, 130);
            iconListPane->mChildrenList.append(&goop_picture->mPtrLink);
            s_info.mGoopIcon = goop_picture;
        }

        mSelectScreen->mSelectionInfos.push_back(s_info);
    }

    // Adjust for ratios
    {
        J2DPane *bn_pane = mSelectScreen->mScreen->search('bnls');
        if (!bn_pane)
            return;


        JSUPtrLink *ptrlink = bn_pane->mChildrenList.mFirst;
        while (ptrlink) {
            auto *picture      = reinterpret_cast<J2DPicture *>(ptrlink->mItemPtr);
            if (picture->mTag != 'banr') {
                int img_width      = picture->mRect.mX2 - picture->mRect.mX1;
                int scaled_ofs =
                    ((picture->mRect.mX1 + img_width / 2) - 300) * icon_ratio - (img_width / 2);
                picture->mRect.mX1 = 300 + scaled_ofs;
                picture->mRect.mX2 = picture->mRect.mX1 + img_width;
            }
            ptrlink            = ptrlink->mNextLink;
        }
    }

    {
        J2DPane *ic_pane = mSelectScreen->mScreen->search('icls');
        if (!ic_pane)
            return;

        JSUPtrLink *ptrlink = ic_pane->mChildrenList.mFirst;
        while (ptrlink) {
            auto *picture      = reinterpret_cast<J2DPicture *>(ptrlink->mItemPtr);
            int img_width      = picture->mRect.mX2 - picture->mRect.mX1;
            int scaled_ofs     = ((picture->mRect.mX1 + img_width / 2) - 300) * icon_ratio - (img_width / 2);
            picture->mRect.mX1 = 300 + scaled_ofs;
            picture->mRect.mX2 = picture->mRect.mX1 + img_width;
            ptrlink            = ptrlink->mNextLink;
        }
    }
}

void CharacterSelectDirector::setup(JDrama::TDisplay *display) {
    mViewObjStageGroup             = new JDrama::TDStageGroup(display);
    mDisplay                       = display;

    for (int i = 0; i < 4; ++i) {
        gpApplication.mGamePads[i]->mState.mReadInput = false;
    }

    SMSRumbleMgr->reset();
    OSCreateThread(&gSetupThread, setupThreadFunc, this, gpSetupThreadStack + 0x10000, 0x10000, 17,
                   0);
    OSResumeThread(&gSetupThread);
}

void *CharacterSelectDirector::setupThreadFunc(void *param) {
    auto *director = reinterpret_cast<CharacterSelectDirector *>(param);
    director->initialize();
    return nullptr;
}

s32 CharacterSelectDirector::exit() {
    TSMSFader *fader = gpApplication.mFader;
    if (fader->mFadeStatus == TSMSFader::FADE_OFF) {
        mSelectScreen->mShouldReadInput = false;
        if (mExitWaitTimer++ > SMSGetVSyncTimesPerSec() * 1.5f) {
            gpApplication.mFader->startFadeoutT(0.3f);
            Music::stopSong(1.0f);
        }
        int i = 0;
        for (auto &s_info : mSelectScreen->mSelectionInfos) {
            SME::TGlobals::sCharacterIDList[i++] =
                mSelectScreen->mCharacterInfos.at(s_info.mIndex).mPlayer;
        }
    }

    return fader->mFadeStatus == TSMSFader::FADE_ON ? 5 : 1;
}

static bool sMario, sLuigi, sPiantissimo, sShadowMario;

BETTER_SMS_FOR_CALLBACK bool directCharacterSelectMenu(TApplication *app) {
    SMSSetupTitleRenderingInfo(app->mDisplay);

    app->mFader->setDisplaySize(SMSGetTitleRenderWidth(), SMSGetTitleRenderHeight());

    auto *director = new CharacterSelectDirector(sMario, sLuigi, sPiantissimo, sShadowMario);
    app->mDirector = director;

    director->setup(app->mDisplay);
    return false;
}

#if 1
static int flagCharacterSelectMenu(u8 state) {
    if (gpApplication.mContext != TApplication::CONTEXT_DIRECT_STAGE)
        return state;

    if (state != TApplication::CONTEXT_DIRECT_STAGE)
        return state;

    auto &next_scene = gpApplication.mNextScene;
    auto &cur_scene = gpApplication.mCurrentScene;
    auto &prev_scene = gpApplication.mPrevScene;

    if (next_scene.mAreaID == 40 && next_scene.mEpisodeID == 0) {
        // Planes and Trains
        //
        // Keeps double screen from happening on exstages when first play
        u32 id = SMS_getShineIDofExStage__FUc(next_scene.mAreaID);
        if (id != 255) {
            if (TFlagManager::smInstance->getShineFlag(id) == false) {
                if (TFlagManager::smInstance->getBool(0x3000D) == false) {
                    return state;
                }
            }
        }

        sMario       = true;
        sLuigi       = true;
        sPiantissimo = true;
        sShadowMario = false;
        return 11;
    }

    if ((next_scene.mAreaID == 40 && next_scene.mEpisodeID == 0) ||
        (next_scene.mAreaID == 41 && next_scene.mEpisodeID == 0)) {
        // Planes and Trains + Secret of the Rook
        //
        // Keeps double screen from happening on exstages when first play
        u32 id = SMS_getShineIDofExStage__FUc(next_scene.mAreaID);
        if (id != 255) {
            if (TFlagManager::smInstance->getShineFlag(id) == false) {
                if (TFlagManager::smInstance->getBool(0x3000D) == false) {
                    return state;
                }
            }
        }

        sMario       = true;
        sLuigi       = true;
        sPiantissimo = true;
        sShadowMario = false;
        return 11;
    }

    if (next_scene.mAreaID == 3) {
        // Mario's Dream
        const bool isFree = next_scene.mEpisodeID != 0;
        sMario       = true;
        sLuigi       = isFree;
        sPiantissimo = isFree;
        sShadowMario = false;
        return 11;
    } else if (next_scene.mAreaID == 14 && next_scene.mEpisodeID == 1) {
        // Casino
        sMario       = true;
        sLuigi       = true;
        sPiantissimo = true;
        sShadowMario = false;
        return 11;
    } else if (next_scene.mAreaID == 25 && next_scene.mEpisodeID == 0) {
        // Memphis
        return state;
    } else if (next_scene.mAreaID == 26 && next_scene.mEpisodeID == 0) {
        if (cur_scene.mAreaID == 25 && cur_scene.mEpisodeID == 0)
            return state;

        // Vaporwave
        sMario       = true;
        sLuigi       = true;
        sPiantissimo = false;
        sShadowMario = false;
        return 11;
    } else if (next_scene.mAreaID == 40 && next_scene.mEpisodeID == 0) {
        // Planes and Trains
        sMario       = true;
        sLuigi       = true;
        sPiantissimo = true;
        sShadowMario = false;
        return 11;
    } else if (next_scene.mAreaID != 6) {
        sMario       = true;
        sLuigi       = true;
        sPiantissimo = true;
        sShadowMario = false;
        return 11;
    }

    return state;
}
SMS_PATCH_B(SMS_PORT_REGION(0x802A637C, 0, 0, 0), flagCharacterSelectMenu);
#else
static int flagCharacterSelectMenu() {
    return 11;
}
SMS_PATCH_BL(SMS_PORT_REGION(0x80176160, 0, 0, 0), flagCharacterSelectMenu);
#endif

SMS_WRITE_32(SMS_PORT_REGION(0x8017606C, 0, 0, 0), 0x38000000);