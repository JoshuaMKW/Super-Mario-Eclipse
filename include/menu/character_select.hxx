#pragma once

#include <JSystem/J2D/J2DOrthoGraph.hxx>
#include <JSystem/J2D/J2DPicture.hxx>
#include <JSystem/J2D/J2DSetScreen.hxx>
#include <JSystem/JUtility/JUTTexture.hxx>
#include <JSystem/JDrama/JDRDirector.hxx>
#include <JSystem/JDrama/JDRDisplay.hxx>
#include <JSystem/JGadget/Vector.hxx>

#include <SMS/Player/Mario.hxx>
#include <SMS/MSound/MSound.hxx>
#include <SMS/MSound/MSoundSESystem.hxx>
#include <SMS/MarioUtil/gd-reinit-gx.hxx>
#include <SMS/MarioUtil/DrawUtil.hxx>
#include <SMS/System/Resolution.hxx>

#include <BetterSMS/module.hxx>
#include <BetterSMS/libs/anim2d.hxx>

using namespace BetterSMS;

enum class Player {
    MARIO,
    LUIGI,
    PIANTISSIMO,
    SHADOW_MARIO
};

struct CharacterInfo {
    Player mPlayer;
    J2DPicture *mIcon;
    J2DPicture *mLabel;
    J2DPicture *mPoster;
};

struct SelectionInfo {
    bool mIsSelected;
    u8 mIndex;
    J2DPicture *mIcon;
    TMarioGamePad *mController;
};

constexpr size_t PlayerMax = 2;

class CharacterSelectScreen;

class CharacterSelectDirector : public JDrama::TDirector {
    enum class State { INIT, CONTROL, EXIT };

public:
    friend class CharacterSelectScreen;

    CharacterSelectDirector(bool isMario, bool isLuigi, bool isPiantissimo, bool isShadowMario)
        : TDirector(), mState(State::INIT), mDisplay(nullptr), mSelectScreen(nullptr), mIsMario(isMario), mIsLuigi(isLuigi),
          mIsPiantissimo(isPiantissimo), mIsShadowMario(isShadowMario) {}
    ~CharacterSelectDirector() override;

    s32 direct() override;
    void setup(JDrama::TDisplay *);

private:
    s32 exit();
    void initialize();
    void initializeDramaHierarchy();
    void initializeLayout();

    static void *setupThreadFunc(void *);

private:
    State mState;
    JDrama::TDisplay *mDisplay;
    CharacterSelectScreen *mSelectScreen;
    bool mIsMario       : 1;
    bool mIsLuigi       : 1;
    bool mIsPiantissimo : 1;
    bool mIsShadowMario : 1;
};

class CharacterSelectScreen : public JDrama::TViewObj {
public:
    friend class CharacterSelectDirector;

    CharacterSelectScreen()
        : TViewObj("<CharacterSelectScreen>"), mScreen(nullptr), mSelectionInfos(),
          mCharacterInfos(), mIconAnimationTimer(0), mIconAnimationStage(false) {
        //mSelectionAnimator = SimpleTexAnimator(sLoadingIconTIMGs, 16);
    }

    ~CharacterSelectScreen() override {}

    void perform(u32 flags, JDrama::TGraphics *graphics) override {
        if ((flags & 0x1)) {
            processInput();
        }

        if ((flags & 0x3)) {
            updateUI();
        }

        if ((flags & 0x8)) {
            drawSelf();
        }
    }

private:
    void processInput() {
        for (auto &info : mSelectionInfos) {
            TMarioGamePad *controller  = info.mController;
            u8 character_id            = info.mIndex;
            u8 old_id                  = character_id;

            if (!info.mIsSelected) {
                if (controller->mButtons.mRapidInput &
                    (TMarioGamePad::DPAD_RIGHT | TMarioGamePad::MAINSTICK_RIGHT)) {
                    if (character_id < mCharacterInfos.size() - 1)
                        character_id += 1;
                }

                if (controller->mButtons.mRapidInput &
                    (TMarioGamePad::DPAD_LEFT | TMarioGamePad::MAINSTICK_LEFT)) {
                    if (character_id > 0)
                        character_id -= 1;
                }

                if (controller->mButtons.mRapidInput & TMarioGamePad::A) {
                    if (gpMSound->gateCheck(MSound::SE_TALK_OPEN)) {
                        MSoundSE::startSoundSystemSE(MSound::SE_TALK_OPEN, 0, nullptr, 0);
                    }
                    info.mIsSelected = true;
                }
            } else {
                if (controller->mButtons.mRapidInput & TMarioGamePad::B) {
                    if (gpMSound->gateCheck(MSound::SE_TALK_CLOSE)) {
                        MSoundSE::startSoundSystemSE(MSound::SE_TALK_CLOSE, 0, nullptr, 0);
                    }
                    info.mIsSelected = false;
                }
            }

            if (old_id != character_id) {
                if (gpMSound->gateCheck(MSound::SE_MENU_CURSOR_MOVE)) {
                    MSoundSE::startSoundSystemSE(MSound::SE_MENU_CURSOR_MOVE, 0, nullptr, 0);
                }
                info.mIndex = character_id;
            }
        }

        int ci = 0;
        for (int ci = 0; ci < mCharacterInfos.size(); ++ci) {
            auto &c_info = mCharacterInfos.at(ci);

            int num_selected = 0;

            for (auto &s_info : mSelectionInfos) {
                if (ci == s_info.mIndex) {
                    num_selected += 1;
                }
            }

            if (num_selected == 0)
                continue;

            const int margin_width = 5;
            const int hand_width = 38;

            int current_x = (c_info.mPoster->mRect.mX1 + 68) - (hand_width * num_selected) / 2 -
                            (margin_width * (num_selected - 1)) / 2;
            for (auto &s_info : mSelectionInfos) {
                if (ci != s_info.mIndex) {
                    continue;
                }
                J2DPicture *select_picture = s_info.mIcon;
                select_picture->mRect.move(current_x, c_info.mPoster->mRect.mY2 + 14);
                current_x += hand_width + margin_width;
            }
        }
    }

    void updateUI() {
        if (mIconAnimationTimer++ > 13) {
            J2DPane *pane       = mScreen->search('icls');
            if (!pane)
                return;

            bool iconFlipper = mIconAnimationStage;

            JSUPtrLink *ptrlink = pane->mChildrenList.mFirst;
            while (ptrlink) {
                auto *picture = reinterpret_cast<J2DPicture *>(ptrlink->mItemPtr);
                picture->mRotation += (iconFlipper ? -10.0f : 10.0f);
                ptrlink = ptrlink->mNextLink;
                iconFlipper ^= true;
            }

            mIconAnimationTimer = 0;
            mIconAnimationStage ^= true;
        }
    }

    void drawSelf() {
        ReInitializeGX();
        SMS_DrawInit();

        J2DOrthoGraph ortho(0, 0, BetterSMS::getScreenOrthoWidth(), SMSGetTitleRenderHeight());
        ortho.setup2D();

        mScreen->draw(0, 0, &ortho);
    }

    J2DSetScreen *mScreen;
    JGadget::TVector<SelectionInfo> mSelectionInfos;
    JGadget::TVector<CharacterInfo> mCharacterInfos;
    u32 mIconAnimationTimer;
    bool mIconAnimationStage;
};