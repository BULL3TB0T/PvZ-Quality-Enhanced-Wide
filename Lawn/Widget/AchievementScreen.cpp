#include "GameButton.h"
#include "../Achievements.h"
#include "../../LawnApp.h"
#include "../System/Music.h"
#include "../System/PlayerInfo.h"
#include "AchievementScreen.h"
#include "GameSelector.h"
#include "../../Resources.h"
#include "../../Sexy.TodLib/TodDebug.h"
#include "../../Sexy.TodLib/TodFoley.h"
#include "../../Sexy.TodLib/TodCommon.h"
#include "../../SexyAppFramework/Debug.h"
#include "../../Sexy.TodLib/TodStringFile.h"
#include "../../SexyAppFramework/WidgetManager.h"
#include "../../GameConstants.h"

AchievementScreen::AchievementScreen(LawnApp* theApp)
{
	mApp = theApp;
	TodLoadResources("DelayLoad_Achievements");
    TodLoadResources("DelayLoad_AwardScreen");
    mScrollAmount = 0;
    mScrollPosition = 0;
    mTweenTimer = 0;
    mGoesDown = false;

    mBackButton = MakeNewButton(0, this, "", nullptr, Sexy::IMAGE_BLANK,
        Sexy::IMAGE_ACHIEVEMENT_BACK_GLOW, Sexy::IMAGE_ACHIEVEMENT_BACK_GLOW);
    mBackButton->Resize(18 + BOARD_ADDITIONAL_WIDTH, 568 + mScrollPosition - BOARD_OFFSET_Y, 111, 26);

    mRockButton = MakeNewButton(1, this, "", nullptr, Sexy::IMAGE_ACHIEVEMENT_MORE,
        Sexy::IMAGE_ACHIEVEMENT_MORE_HIGHLIGHT, Sexy::IMAGE_ACHIEVEMENT_MORE_HIGHLIGHT);
    mRockButton->Resize(710 + BOARD_ADDITIONAL_WIDTH, 470 + mScrollPosition - BOARD_OFFSET_Y, IMAGE_ACHIEVEMENT_MORE->mWidth, IMAGE_ACHIEVEMENT_MORE->mHeight);
}
AchievementScreen::~AchievementScreen()
{
    delete mBackButton;
    delete mRockButton;
}

void AchievementScreen::Draw(Graphics* g)
{
    int yPosIndex = 0;
    g->DrawImage(Sexy::IMAGE_ACHIEVEMENT_TILE, 0, mScrollPosition + BOARD_OFFSET_Y - 10);
    g->DrawImage(Sexy::IMAGE_ACHIEVEMENT_SELECTOR_TILE, 0, -BOARD_OFFSET_Y + mScrollPosition);
   for (int i = 1; i <= 70; i++)
    {
       if (i == 70)
       {
           g->DrawImage(Sexy::IMAGE_ACHIEVEMENT_TILE_CHINA, 0, Sexy::IMAGE_ACHIEVEMENT_TILE_CHINA->mHeight * i + mScrollPosition - 30 + -BOARD_OFFSET_Y);
       }
       else
           g->DrawImage(Sexy::IMAGE_ACHIEVEMENT_TILE, 0, Sexy::IMAGE_ACHIEVEMENT_TILE->mHeight * i + mScrollPosition + -BOARD_OFFSET_Y);
    }
   g->DrawImage(Sexy::IMAGE_ACHIEVEMENT_ROCK, mRockButton->mX, mRockButton->mY);

   //positions from re-plants-vs-zombies (by @Patoke)
   g->DrawImage(Sexy::IMAGE_ACHIEVEMENT_BOOKWORM, BOARD_ADDITIONAL_WIDTH, 1125 + mScrollPosition);
   g->DrawImage(Sexy::IMAGE_ACHIEVEMENT_BEJEWELED, BOARD_ADDITIONAL_WIDTH, 2250 + mScrollPosition);
   g->DrawImage(Sexy::IMAGE_ACHIEVEMENT_CHUZZLE, BOARD_ADDITIONAL_WIDTH, 4500 + mScrollPosition);
   g->DrawImage(Sexy::IMAGE_ACHIEVEMENT_PEGGLE, BOARD_ADDITIONAL_WIDTH, 6750 + mScrollPosition);
   g->DrawImage(Sexy::IMAGE_ACHIEVEMENT_PIPE, BOARD_ADDITIONAL_WIDTH, 9000 + mScrollPosition);
   g->DrawImage(Sexy::IMAGE_ACHIEVEMENT_ZUMA, BOARD_ADDITIONAL_WIDTH, 11250 + mScrollPosition);

    g->SetScale(0.9f, 0.9f,0,0);
    for (int i = 0; i < TOTAL_ACHIEVEMENTS; i++)
    {
            yPosIndex++;
            SexyString aAchievementName = StrFormat(_S("[%s]"), mApp->mAchievement->ReturnAchievementName(i).c_str());
            SexyString aAchievementDesc = StrFormat(_S("[%s_DESCRIPTION]"), mApp->mAchievement->ReturnAchievementName(i).c_str());
            int yPos = 178 + (72 * (i / 2)) + mScrollPosition;
            int xPos = 120 + BOARD_ADDITIONAL_WIDTH;
            if (i % 2 != 0)
            {
                xPos = 429 + BOARD_ADDITIONAL_WIDTH;
            }
            xPos += 120;
            TodDrawString(g, aAchievementName, xPos - 10, yPos + 17, Sexy::FONT_DWARVENTODCRAFT15, Color(21, 175, 0) , DS_ALIGN_LEFT);
            //TodDrawString(g, aAchievementDesc, 150, yPos + 50, Sexy::FONT_DWARVENTODCRAFT24, Color(255, 255, 255, 255), DS_ALIGN_LEFT);
            TodDrawStringWrapped(g, aAchievementDesc, Rect(xPos - 10, yPos + 19, 230, 260), Sexy::FONT_DWARVENTODCRAFT12, Color(255, 255, 255, 255), DS_ALIGN_LEFT);
            g->SetColorizeImages(true);
            g->SetColor(mApp->mPlayerInfo->mEarnedAchievements[i] ? Color(255, 255, 255) : Color(255, 255, 255, 32));
            g->DrawImageCel(Sexy::IMAGE_ACHIEVEMENTS_PORTRAITS, xPos - 90, yPos, i);
            g->SetColorizeImages(false);
    }
    g->SetScale(1.0f, 1.0f, 0, 0);

}

void AchievementScreen::KeyDown(KeyCode theKey) {
    if (mApp->mWidgetManager->mFocusWidget != this)
        return;
    if (theKey == KEYCODE_UP) {
        mScrollPosition += 15;
    }
    else if (theKey == KEYCODE_DOWN) {
        mScrollPosition -= 15;
    }
    else if (theKey == KEYCODE_ESCAPE)
    {
        ExitScreen();
    }
}

void AchievementScreen::AddedToManager(WidgetManager* theWidgetManager)
{
	Widget::AddedToManager(theWidgetManager);
    AddWidget(mBackButton);
    AddWidget(mRockButton);
}

//0x42F6B0
void AchievementScreen::RemovedFromManager(WidgetManager* theWidgetManager)
{
	Widget::RemovedFromManager(theWidgetManager);
    RemoveWidget(mBackButton);
    RemoveWidget(mRockButton);
}

//0x42F720
void AchievementScreen::ButtonPress(int theId)
{
	mApp->PlaySample(Sexy::SOUND_BUTTONCLICK);
}

void AchievementScreen::Update()
{
    if (mTweenTimer > 0)
    {
        int direction;
        if (mGoesDown)
            direction = -234;
        else
            direction = 0;
        DoButtonMovement(mScrollPosition, direction);
    }


    mBackButton->Resize(128 + BOARD_ADDITIONAL_WIDTH, 55 + mScrollPosition - BOARD_OFFSET_Y, 130, 80);
    mRockButton->Resize(710 + BOARD_ADDITIONAL_WIDTH, 470 + mScrollPosition - BOARD_OFFSET_Y, IMAGE_ACHIEVEMENT_MORE->mWidth, IMAGE_ACHIEVEMENT_MORE->mHeight);
    mMaxScrollPosition = 15162;
    float aScrollSpeed = mBaseScrollSpeed + abs(mScrollAmount) * mScrollAccel;
    mScrollPosition = ClampFloat(mScrollPosition -= mScrollAmount * aScrollSpeed, -mMaxScrollPosition, 0);
    mScrollAmount *= (1.0f - mScrollAccel);
}

//0x42F740
void AchievementScreen::ButtonDepress(int theId)
{
    if (theId == 0)
    {
        ExitScreen();
    }
    if (theId == 1)
    {
        mTweenTimer = 110;
        mGoesDown = !mGoesDown;
        if (mGoesDown)
        {
            mRockButton->mButtonImage = IMAGE_ACHIEVEMENT_TOP;
            mRockButton->mDownImage = IMAGE_ACHIEVEMENT_TOP_HIGHLIGHT;
            mRockButton->mOverImage = IMAGE_ACHIEVEMENT_TOP_HIGHLIGHT;
        }
        else
        {
            mRockButton->mButtonImage = IMAGE_ACHIEVEMENT_MORE;
            mRockButton->mDownImage = IMAGE_ACHIEVEMENT_MORE_HIGHLIGHT;
            mRockButton->mOverImage = IMAGE_ACHIEVEMENT_MORE_HIGHLIGHT;
        }
    }
}

void AchievementScreen::DoButtonMovement(int StartX, int FinalX)
{
    mScrollPosition = TodAnimateCurve(200, 0, mTweenTimer, StartX, FinalX, TodCurves::CURVE_EASE_IN_OUT);
    mTweenTimer--;
}

void AchievementScreen::MouseWheel(int theDelta)
{
    if (mApp->mGameSelector->mMovementTimer > 0)
        return;
    mScrollAmount -= mBaseScrollSpeed * theDelta;
    mScrollAmount -= mScrollAmount * mScrollAccel;
}

void AchievementScreen::ExitScreen()
{
    mScrollPosition = 0;
    mApp->mGameSelector->mMovementTimer = 75;
    mApp->mGameSelector->mDestinationY = 0;
    mApp->mGameSelector->mSelectorState = SELECTOR_IDLE;
    mApp->mGameSelector->mEnableButtonsTransition = true;
    mApp->mWidgetManager->SetFocus(mApp->mGameSelector);
    mBackButton->SetDisabled(true);
    mRockButton->SetDisabled(true);
}
