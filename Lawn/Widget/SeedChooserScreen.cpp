#include "../Board.h"
#include "../Zombie.h"
#include "GameButton.h"
#include "StoreScreen.h"
#include "../Cutscene.h"
#include "../SeedPacket.h"
#include "../../LawnApp.h"
#include "AlmanacDialog.h"
#include "../System/Music.h"
#include "../../Resources.h"
#include "../../Lawn/Plant.h"
#include "../ToolTipWidget.h"
#include "SeedChooserScreen.h"
#include "../../GameConstants.h"
#include "../System/PlayerInfo.h"
#include "../System/PopDRMComm.h"
#include "../../SexyAppFramework/Debug.h"
#include "../../SexyAppFramework/Dialog.h"
#include "../../SexyAppFramework/MTRand.h"
#include "../../Sexy.TodLib/TodStringFile.h"
#include "../../SexyAppFramework/WidgetManager.h"
#include "../../SexyAppFramework/Slider.h"
#include "../../SexyAppFramework/Font.h"

const Rect cSeedClipRect = Rect(0, 123, BOARD_WIDTH, 420 + SEED_CHOOSER_EXTRA_HEIGHT);
const int cSeedPacketYOffset = 2;
const int cSeedPacketRows = 8;
const int cControllerOffset = 4;

SeedChooserScreen::SeedChooserScreen()
{
	mApp = (LawnApp*)gSexyAppBase;
	mBoard = mApp->mBoard;
	mClip = false;
	mSeedsInFlight = 0;
	mSeedsInBank = 0;
	mChooseState = CHOOSE_NORMAL;
	mViewLawnTime = 0;
	mToolTip = new ToolTipWidget();
	mScrollAmount = 0;
	mScrollPosition = 0;

	mStartButton = new GameButton(SeedChooserScreen::SeedChooserScreen_Start);
	mStartButton->mLabel = _S("[LETS_ROCK_BUTTON]");
	mStartButton->mButtonImage = Sexy::IMAGE_SEEDCHOOSER_BUTTON;
	mStartButton->mOverImage = nullptr;
	mStartButton->mDownImage = nullptr;
	mStartButton->mDisabledImage = Sexy::IMAGE_SEEDCHOOSER_BUTTON_DISABLED;
	mStartButton->mOverOverlayImage = Sexy::IMAGE_SEEDCHOOSER_BUTTON_GLOW;
	mStartButton->SetFont(Sexy::FONT_DWARVENTODCRAFT18YELLOW);
	mStartButton->mColors[ButtonWidget::COLOR_LABEL] = Color::White;
	mStartButton->mColors[ButtonWidget::COLOR_LABEL_HILITE] = Color::White;
	mStartButton->Resize(154, 545 + SEED_CHOOSER_EXTRA_HEIGHT, 156, 42);
	mStartButton->mTextOffsetY = -1;
	EnableStartButton(false);
	mStartButton->mParentWidget = this;

	int aButtonOffsetX = BOARD_ADDITIONAL_WIDTH * 2;
	mMenuButton = new GameButton(SeedChooserScreen::SeedChooserScreen_Menu);
	mMenuButton->mLabel = _S("[MENU_BUTTON]");
	mMenuButton->mDrawStoneButton = true;
	mMenuButton->Resize(681 + aButtonOffsetX, -10, 117, 46);
	mMenuButton->mParentWidget = this;

	mRandomButton = new GameButton(SeedChooserScreen::SeedChooserScreen_Random);
	mRandomButton->mLabel = _S("[DEBUG_PLAY_BUTTON]");
	mRandomButton->mButtonImage = Sexy::IMAGE_BLANK;
	mRandomButton->mOverImage = Sexy::IMAGE_BLANK;
	mRandomButton->mDownImage = Sexy::IMAGE_BLANK;
	mRandomButton->SetFont(Sexy::FONT_BRIANNETOD12);
	mRandomButton->mColors[0] = Color(255, 240, 0);
	mRandomButton->mColors[1] = Color(200, 200, 255);
	mRandomButton->Resize(332, 555 + SEED_CHOOSER_EXTRA_HEIGHT, 100, 30);
	mRandomButton->mParentWidget = this;

	Color aBtnColor = Color(42, 42, 90);
	Image* aBtnImage = Sexy::IMAGE_SEEDCHOOSER_BUTTON2;
	Image* aOverImage = Sexy::IMAGE_SEEDCHOOSER_BUTTON2_GLOW;
	int aImageWidth = aBtnImage->GetWidth();
	int aImageHeight = aOverImage->GetHeight();

	mViewLawnButton = new GameButton(SeedChooserScreen::SeedChooserScreen_ViewLawn);
	mViewLawnButton->mLabel = _S("[VIEW_LAWN]");
	mViewLawnButton->mButtonImage = aBtnImage;
	mViewLawnButton->mOverImage = aOverImage;
	mViewLawnButton->mDownImage = nullptr;
	mViewLawnButton->SetFont(Sexy::FONT_BRIANNETOD12);
	mViewLawnButton->mColors[0] = aBtnColor;
	mViewLawnButton->mColors[1] = aBtnColor;
	mViewLawnButton->Resize(22, 561 + SEED_CHOOSER_EXTRA_HEIGHT, aImageWidth, aImageHeight);
	mViewLawnButton->mTextOffsetY = 1;
	mViewLawnButton->mParentWidget = this;
	if (!mBoard->mCutScene->IsSurvivalRepick())
	{
		mViewLawnButton->mBtnNoDraw = true;
		mViewLawnButton->mDisabled = true;
	}

	mAlmanacButton = new GameButton(SeedChooserScreen::SeedChooserScreen_Almanac);
	mAlmanacButton->mLabel = _S("[ALMANAC_BUTTON]");
	mAlmanacButton->mButtonImage = aBtnImage;
	mAlmanacButton->mOverImage = aOverImage;
	mAlmanacButton->mDownImage = nullptr;
	mAlmanacButton->SetFont(Sexy::FONT_BRIANNETOD12);
	mAlmanacButton->mColors[0] = aBtnColor;
	mAlmanacButton->mColors[1] = aBtnColor;
	mAlmanacButton->Resize(560 + aButtonOffsetX, 572 + SEED_CHOOSER_EXTRA_HEIGHT, aImageWidth, aImageHeight);
	mAlmanacButton->mTextOffsetY = 1;
	mAlmanacButton->mParentWidget = this;

	mStoreButton = new GameButton(SeedChooserScreen::SeedChooserScreen_Store);
	mStoreButton->mLabel = _S("[SHOP_BUTTON]");
	mStoreButton->mButtonImage = aBtnImage;
	mStoreButton->mOverImage = aOverImage;
	mStoreButton->mDownImage = nullptr;
	mStoreButton->SetFont(Sexy::FONT_BRIANNETOD12);
	mStoreButton->mColors[0] = aBtnColor;
	mStoreButton->mColors[1] = aBtnColor;
	mStoreButton->Resize(680 + aButtonOffsetX, 572 + SEED_CHOOSER_EXTRA_HEIGHT, aImageWidth, aImageHeight);
	mStoreButton->mTextOffsetY = 1;
	mStoreButton->mParentWidget = this;

	if (!mApp->CanShowAlmanac())
	{
		mAlmanacButton->mBtnNoDraw = true;
		mAlmanacButton->mDisabled = true;
	}
	if (!mApp->CanShowStore())
	{
		mStoreButton->mBtnNoDraw = true;
		mStoreButton->mDisabled = true;
	}

	DBG_ASSERT(mApp->GetSeedsAvailable() < NUM_SEED_TYPES);
	memset(mChosenSeeds, 0, sizeof(mChosenSeeds));
	for (SeedType aSeedType = SEED_PEASHOOTER; aSeedType < NUM_SEEDS_IN_CHOOSER; aSeedType = (SeedType)(aSeedType + 1))
	{
		ChosenSeed& aChosenSeed = mChosenSeeds[aSeedType];
		aChosenSeed.mSeedType = aSeedType;
		GetSeedPositionInChooser(aSeedType, aChosenSeed.mX, aChosenSeed.mY);
		aChosenSeed.mTimeStartMotion = 0;
		aChosenSeed.mTimeEndMotion = 0;
		aChosenSeed.mStartX = aChosenSeed.mX;
		aChosenSeed.mStartY = aChosenSeed.mY;
		aChosenSeed.mEndX = aChosenSeed.mX;
		aChosenSeed.mEndY = aChosenSeed.mY;
		aChosenSeed.mSeedState = SEED_IN_CHOOSER;
		aChosenSeed.mSeedIndexInBank = 0;
		aChosenSeed.mRefreshCounter = 0;
		aChosenSeed.mRefreshing = false;
		aChosenSeed.mImitaterType = SEED_NONE;
		aChosenSeed.mCrazyDavePicked = false;
	}
	if (mBoard->mCutScene->IsSurvivalRepick())
	{
		for (int anIdx = 0; anIdx < mBoard->mSeedBank->mNumPackets; anIdx++)
		{
			SeedPacket* aSeedPacket = &mBoard->mSeedBank->mSeedPackets[anIdx];
			ChosenSeed& aChosenSeed = mChosenSeeds[aSeedPacket->mPacketType];
			aChosenSeed.mRefreshing = aSeedPacket->mRefreshing;
			aChosenSeed.mRefreshCounter = aSeedPacket->mRefreshCounter;
		}
		mBoard->mSeedBank->mNumPackets = 0;
	}
	if (mApp->mGameMode == GAMEMODE_CHALLENGE_SEEING_STARS)
	{
		ChosenSeed& aStarFruit = mChosenSeeds[SEED_STARFRUIT];
		int aX = mBoard->GetSeedPacketPositionX(0);
		aStarFruit.mX = aX, aStarFruit.mY = 8;
		aStarFruit.mStartX = aX, aStarFruit.mStartY = 8;
		aStarFruit.mEndX = aX, aStarFruit.mEndY = 8;
		aStarFruit.mSeedState = SEED_IN_BANK;
		aStarFruit.mSeedIndexInBank = 0;
		mSeedsInBank++;
	}
	if ((mApp->mCrazySeeds && mApp->mPlayingQuickplay) || (mApp->IsAdventureMode() && !mApp->IsFirstTimeAdventureMode() && !mApp->mPlayingQuickplay))
			CrazyDavePickSeeds();

	mSlider = new Sexy::Slider(IMAGE_OPTIONS_SLIDERSLOT_PLANT, IMAGE_OPTIONS_SLIDERKNOB_PLANT, 0, this);
	mSlider->SetValue(max(0.0, min(mMaxScrollPosition, mScrollPosition)));
	mSlider->mHorizontal = false;
	mSlider->mThumbOffsetX = -14;
	mSlider->mNoDraw = true;
	ResizeSlider();
}

int SeedChooserScreen::PickFromWeightedArrayUsingSpecialRandSeed(TodWeightedArray* theArray, int theCount, MTRand& theLevelRNG)
{
	int aTotalWeight = 0;
	for (int i = 0; i < theCount; i++)
		aTotalWeight += theArray[i].mWeight;
	DBG_ASSERT(aTotalWeight > 0);

	int aRndResult = theLevelRNG.Next((unsigned long)aTotalWeight);
	int aWeight = 0;
	for (int j = 0; j < theCount; j++)
	{
		aWeight += theArray[j].mWeight;
		if (aWeight > aRndResult) return theArray[j].mItem;
	}
	DBG_ASSERT(false);
}

void SeedChooserScreen::CrazyDavePickSeeds()
{
	TodWeightedArray aSeedArray[NUM_SEED_TYPES];
	for (SeedType aSeedType = SEED_PEASHOOTER; aSeedType < NUM_SEEDS_IN_CHOOSER; aSeedType = (SeedType)(aSeedType + 1))
	{
		aSeedArray[aSeedType].mItem = aSeedType;
		uint aRecFlags = SeedNotRecommendedToPick(aSeedType);
		if ((aSeedType == SEED_GATLINGPEA && !mApp->mPlayerInfo->mPurchases[STORE_ITEM_PLANT_GATLINGPEA]) || !mApp->SeedTypeAvailable(aSeedType) ||
			SeedNotAllowedToPick(aSeedType) || Plant::IsUpgrade(aSeedType) || aSeedType == SEED_IMITATER || aSeedType == SEED_UMBRELLA || aSeedType == SEED_BLOVER)
		{
			aSeedArray[aSeedType].mWeight = 0;
		}
		else
		{
			aSeedArray[aSeedType].mWeight = 1;
		}
	}
	if (mBoard->mZombieAllowed[ZOMBIE_BUNGEE] || mBoard->mZombieAllowed[ZOMBIE_CATAPULT])
	{
		aSeedArray[SEED_UMBRELLA].mWeight = 1;
	}
	if (mBoard->mZombieAllowed[ZOMBIE_BALLOON] || mBoard->StageHasFog())
	{
		aSeedArray[SEED_BLOVER].mWeight = 1;
	}
	if (mBoard->StageHasRoof())
	{
		aSeedArray[SEED_TORCHWOOD].mWeight = 0;
	}

	MTRand aLevelRNG = MTRand(mBoard->GetLevelRandSeed());
	for (int i = 0; i < 3; i++)
	{
		SeedType aPickedSeed = (SeedType)PickFromWeightedArrayUsingSpecialRandSeed(aSeedArray, NUM_SEEDS_IN_CHOOSER, aLevelRNG);
		aSeedArray[aPickedSeed].mWeight = 0;
		ChosenSeed& aChosenSeed = mChosenSeeds[aPickedSeed];

		int aPosX = mBoard->GetSeedPacketPositionX(i);
		aChosenSeed.mX = aPosX;
		aChosenSeed.mY = 8;
		aChosenSeed.mStartX = aPosX;
		aChosenSeed.mStartY = 8;
		aChosenSeed.mEndX = aPosX;
		aChosenSeed.mEndY = 8;
		aChosenSeed.mSeedState = SEED_IN_BANK;
		aChosenSeed.mSeedIndexInBank = i;
		aChosenSeed.mCrazyDavePicked = true;
		mSeedsInBank++;
	}
}

bool SeedChooserScreen::Has7Rows()
{
	PlayerInfo* aPlayer = mApp->mPlayerInfo;
	if (mApp->HasFinishedAdventure() || mApp->mPlayerInfo->mPurchases[STORE_ITEM_PLANT_GATLINGPEA]) return true;
	for (SeedType aSeedType = SEED_TWINSUNFLOWER; aSeedType < SEED_COBCANNON; aSeedType = (SeedType)(aSeedType + 1))
		if (aSeedType != SEED_SPIKEROCK && mApp->SeedTypeAvailable(aSeedType)) return true;
	return false;
}

void SeedChooserScreen::GetSeedPositionInChooser(int theIndex, int& x, int& y)
{
	if (theIndex == SEED_IMITATER)
	{
		x = IMITATER_POS_X + 5;
		y = IMITATER_POS_Y + 12 + SEED_CHOOSER_EXTRA_HEIGHT;
	}
	else
	{
		x = theIndex % cSeedPacketRows * 53 + 22;
		y = theIndex / cSeedPacketRows * (SEED_PACKET_HEIGHT + cSeedPacketYOffset) + (SEED_PACKET_HEIGHT + 53) - mScrollPosition;
	}
	if (mBoard->IsControllerChooserSelected((SeedType)theIndex))
		y += cControllerOffset;
}

void SeedChooserScreen::GetSeedPositionInBank(int theIndex, int& x, int& y)
{
	x = mBoard->mSeedBank->mX - mX + mBoard->GetSeedPacketPositionX(theIndex);
	y = mBoard->mSeedBank->mY - mY + 8;
}

SeedChooserScreen::~SeedChooserScreen()
{
	if (mStartButton) delete mStartButton;
	if (mRandomButton) delete mRandomButton;
	if (mViewLawnButton) delete mViewLawnButton;
	if (mAlmanacButton) delete mAlmanacButton;
	if (mStoreButton) delete mStoreButton;
	if (mSlider) delete mSlider;
	if (mToolTip) delete mToolTip;
	if (mMenuButton) delete mMenuButton;
}

void SeedChooserScreen::RemovedFromManager(WidgetManager* theWidgetManager)
{
	Widget::RemovedFromManager(theWidgetManager);
	RemoveWidget(mSlider);
}

void SeedChooserScreen::AddedToManager(WidgetManager* theWidgetManager)
{
	Widget::AddedToManager(theWidgetManager);
	AddWidget(mSlider);
}


unsigned int SeedChooserScreen::SeedNotRecommendedToPick(SeedType theSeedType)
{
	uint aRecFlags = mBoard->SeedNotRecommendedForLevel(theSeedType);
	if (TestBit(aRecFlags, NOT_RECOMMENDED_NOCTURNAL) && PickedPlantType(SEED_INSTANT_COFFEE))
		SetBit(aRecFlags, NOT_RECOMMENDED_NOCTURNAL, false);
	return aRecFlags;
}

bool SeedChooserScreen::SeedNotAllowedToPick(SeedType theSeedType)
{
	return mApp->mGameMode == GAMEMODE_CHALLENGE_LAST_STAND && (theSeedType == SEED_SUNFLOWER || theSeedType == SEED_SUNSHROOM ||
		theSeedType == SEED_TWINSUNFLOWER || theSeedType == SEED_SEASHROOM || theSeedType == SEED_PUFFSHROOM);
}

bool SeedChooserScreen::SeedNotAllowedDuringTrial(SeedType theSeedType)
{
	return mApp->IsTrialStageLocked() && (theSeedType == SEED_SQUASH || theSeedType == SEED_THREEPEATER);
}

void SeedChooserScreen::Draw(Graphics* g)
{
	if (mApp->GetDialog(DIALOG_STORE) || mApp->GetDialog(DIALOG_ALMANAC))
		return;

	g->SetLinearBlend(true);
	if (!mBoard->ChooseSeedsOnCurrentLevel() || (mBoard->mCutScene && mBoard->mCutScene->IsBeforePreloading()))
		return;

	g->DrawImage(Sexy::IMAGE_SEEDCHOOSER_BACKGROUND, 0, 87);
	if (mApp->SeedTypeAvailable(SEED_IMITATER))
		g->DrawImage(Sexy::IMAGE_SEEDCHOOSER_IMITATERADDON, IMITATER_POS_X, IMITATER_POS_Y + SEED_CHOOSER_EXTRA_HEIGHT);

	TodDrawString(g, _S("[CHOOSE_YOUR_PLANTS]"), 229, 110, Sexy::FONT_DWARVENTODCRAFT18YELLOW, Color::White, DS_ALIGN_CENTER);
	mSlider->SliderDraw(g);
	for (SeedType aSeedType = SEED_PEASHOOTER; aSeedType < NUM_SEEDS_IN_CHOOSER; aSeedType = (SeedType)(aSeedType + 1))
	{
		if (aSeedType != SEED_IMITATER)
			g->SetClipRect(cSeedClipRect);

		int x, y;
		GetSeedPositionInChooser(aSeedType, x, y);

		if (mApp->SeedTypeAvailable(aSeedType))
		{
			ChosenSeed& aChosenSeed = mChosenSeeds[aSeedType];
			if (aChosenSeed.mSeedState != SEED_IN_CHOOSER)
			{
				int aGrayness = 55;
				bool aDrawCost = aSeedType != SEED_IMITATER;
				if (mBoard->IsControllerChooserSelected(aSeedType))
					mControllerDrawSeeds.emplace_back(x, y, aSeedType, SEED_NONE, aGrayness, aDrawCost);
				else
					DrawSeedPacket(g, x, y, aSeedType, SEED_NONE, 0, aGrayness, aDrawCost, false);
			}
		}
		else if (aSeedType != SEED_IMITATER)
		{
			if (mBoard->IsControllerChooserSelected(aSeedType))
				mControllerDrawSeeds.emplace_back(x, y, SEED_NONE, SEED_NONE, 0, false);
			else
				g->DrawImage(Sexy::IMAGE_SEEDPACKETSILHOUETTE, x, y);
		}

		ChosenSeed& aChosenSeed = mChosenSeeds[aSeedType];
		ChosenSeedState aSeedState = aChosenSeed.mSeedState;
		if (mApp->SeedTypeAvailable(aSeedType) &&
			aSeedState != SEED_FLYING_TO_BANK &&
			aSeedState != SEED_FLYING_TO_CHOOSER &&
			aSeedState != SEED_PACKET_HIDDEN &&
			(aSeedState == SEED_IN_CHOOSER || mBoard->mCutScene->mSeedChoosing))
		{
			bool aGrayed = false;
			if (((SeedNotRecommendedToPick(aSeedType) || SeedNotAllowedToPick(aSeedType) || IsImitaterUnselectable(aSeedType)) && aSeedState == SEED_IN_CHOOSER) || SeedNotAllowedDuringTrial(aSeedType))
				aGrayed = true;

			int aPosX, aPosY;
			if (aSeedState == SEED_IN_CHOOSER)
			{
				GetSeedPositionInChooser((int)aSeedType, aPosX, aPosY);
			}
			else if (aSeedState == SEED_IN_BANK)
			{
				g->ClearClipRect();
				aPosX = aChosenSeed.mX;
				aPosY = aChosenSeed.mY;
			}
			else
			{
				aPosX = aChosenSeed.mX;
				aPosY = aChosenSeed.mY;
			}
			if (mChooseState != CHOOSE_VIEW_LAWN || (mChooseState == CHOOSE_VIEW_LAWN && aSeedState == SEED_IN_CHOOSER))
			{
				int aGrayness = aGrayed ? 115 : 255;
				bool aDrawCost = aSeedType != SEED_IMITATER || aSeedState != SEED_IN_CHOOSER;
				if (mBoard->IsControllerChooserSelected(aSeedType) && aSeedState == SEED_IN_CHOOSER)
					mControllerDrawSeeds.emplace_back(aPosX, aPosY, aChosenSeed.mSeedType, aChosenSeed.mImitaterType, aGrayness, aDrawCost);
				else
					DrawSeedPacket(g, aPosX, aPosY, aChosenSeed.mSeedType, aChosenSeed.mImitaterType, 0, aGrayness, aDrawCost, false);
			}
		}
		g->ClearClipRect();
	}

	float aSeedSelectorScale = 1.05;
	int aSeedSelectorOffset = 7;

	for (int i = 0; i < MAX_GAMEPADS; i++)
	{
		GamepadPlayer* aGamepadPlayer = mBoard->mGamepadPlayerList[i];
		SeedType aSeedType = aGamepadPlayer->mSeedChooserSeed;
		if (aSeedType == SEED_NONE)
			continue;
		int aSeedSelectorWidth = IMAGE_SEED_SELECTOR->mWidth * aSeedSelectorScale;
		int aSeedSelectorHeight = IMAGE_SEED_SELECTOR->mHeight * aSeedSelectorScale;
		int aPosX, aPosY;
		GetSeedPositionInChooser(aSeedType, aPosX, aPosY);
		aPosX -= aSeedSelectorOffset;
		aPosY -= aSeedSelectorOffset;
		Color aOldColor = g->mColor;
		g->SetColorizeImages(true);
		g->SetColor(aGamepadPlayer->mColor);
		TodDrawImageScaledF(g, IMAGE_SEED_SELECTOR, aPosX, aPosY, aSeedSelectorScale, aSeedSelectorScale);
		g->SetColor(aOldColor);
		g->SetColorizeImages(false);
	}

	for (const auto& aSeedData : mControllerDrawSeeds)
	{
		int aPosX, aPosY, aGrayness;
		SeedType aSeedType, aImitaterType;
		bool aDrawCost;
		std::tie(aPosX, aPosY, aSeedType, aImitaterType, aGrayness, aDrawCost) = aSeedData;
		if (aSeedType != SEED_IMITATER)
			g->SetClipRect(cSeedClipRect);
		if (aSeedType != SEED_NONE)
			DrawSeedPacket(g, aPosX, aPosY, aSeedType, aImitaterType, 0, aGrayness, aDrawCost, false);
		else
			g->DrawImage(Sexy::IMAGE_SEEDPACKETSILHOUETTE, aPosX, aPosY);
		g->ClearClipRect();
	}
	mControllerDrawSeeds.clear();

	for (int i = 0; i < MAX_GAMEPADS; i++)
	{
		GamepadPlayer* aGamepadPlayer = mBoard->mGamepadPlayerList[i];
		SeedType aSeedType = aGamepadPlayer->mSeedChooserSeed;
		if (aSeedType == SEED_NONE)
			continue;
		int aSeedSelectorWidth = IMAGE_SEED_SELECTOR->mWidth * aSeedSelectorScale;
		int aSeedSelectorHeight = IMAGE_SEED_SELECTOR->mHeight * aSeedSelectorScale;
		int aPosX, aPosY;
		GetSeedPositionInChooser(aSeedType, aPosX, aPosY);
		aPosX -= aSeedSelectorOffset;
		aPosY -= aSeedSelectorOffset;
		bool aIsOverlapping = false;
		for (int j = i + 1; j < MAX_GAMEPADS; j++)
		{
			aIsOverlapping = mBoard->mGamepadPlayerList[j]->mSeedChooserSeed == aSeedType;
			if (aIsOverlapping)
				break;
		}
		if (!aIsOverlapping)
		{
			SexyString aText = "P" + to_string(i + 1);
			int aTextPosX = aPosX + 34;
			int aTextPosY = aPosY - 4;
			TodDrawString(g, aText, aTextPosX, aTextPosY, FONT_DWARVENTODCRAFT24, Color(0, 0, 0), DrawStringJustification::DS_ALIGN_CENTER_VERTICAL_MIDDLE);
			TodDrawString(g, aText, aTextPosX - 2, aTextPosY - 2, FONT_DWARVENTODCRAFT24, aGamepadPlayer->mColor, DrawStringJustification::DS_ALIGN_CENTER_VERTICAL_MIDDLE);
		}
		if (!mApp->SeedTypeAvailable(aSeedType))
			continue;
		Color aOldColor = g->mColor;
		g->SetColorizeImages(true);
		g->SetColor(aGamepadPlayer->mColor);

		float aOffsetY = TodAnimateCurveFloat(mArrowTimeStartMotion, mArrowTimeEndMotion, mSeedChooserAge, 0, 2.5, TodCurves::CURVE_SIN_WAVE);
		g->DrawImageF(IMAGE_BOARD_ARROW, aPosX + aSeedSelectorWidth / 2 - IMAGE_BOARD_ARROW->mWidth / 2, aPosY - 2 + aOffsetY);
		g->SetColor(aOldColor);
		g->SetColorizeImages(false);
	}

	// Draw flying seeds
	for (SeedType aSeedType = SEED_PEASHOOTER; aSeedType < NUM_SEEDS_IN_CHOOSER; aSeedType = (SeedType)(aSeedType + 1))
	{
		ChosenSeed& aChosenSeed = mChosenSeeds[aSeedType];
		ChosenSeedState aSeedState = aChosenSeed.mSeedState;
		if (mApp->SeedTypeAvailable(aSeedType) && (aSeedState == SEED_FLYING_TO_BANK || aSeedState == SEED_FLYING_TO_CHOOSER))
		{
			DrawSeedPacket(g, aChosenSeed.mX, aChosenSeed.mY, aChosenSeed.mSeedType, aChosenSeed.mImitaterType, 0, 255, aChosenSeed.mSeedType != SEED_IMITATER, false);
		}
	}

	int aNumSeedsInBank = mBoard->mSeedBank->mNumPackets;
	for (int anIndex = 0; anIndex < aNumSeedsInBank; anIndex++)
	{
		if (FindSeedInBank(anIndex) == SEED_NONE)
		{
			int aX, aY;
			GetSeedPositionInBank(anIndex, aX, aY);
			g->DrawImage(Sexy::IMAGE_SEEDPACKETSILHOUETTE, aX, aY);
		}
	}

	mStartButton->Draw(g);
	mRandomButton->Draw(g);
	mViewLawnButton->Draw(g);
	mAlmanacButton->Draw(g);
	mStoreButton->Draw(g);
	Graphics aBoardFrameG = Graphics(*g);
	aBoardFrameG.mTransX -= mX;
	aBoardFrameG.mTransY -= mY;
	mMenuButton->Draw(&aBoardFrameG);
	for (int i = 0; i < MAX_GAMEPADS; i++)
		mBoard->mGamepadPlayerList[i]->mSeedChooserToolTip->Draw(g);
	mToolTip->Draw(g);
}

void SeedChooserScreen::UpdateViewLawn()
{
	if (mChooseState != CHOOSE_VIEW_LAWN) return;
	mViewLawnTime++;
	if (mViewLawnTime == 100) mBoard->DisplayAdviceAgain("[CLICK_TO_CONTINUE]", MESSAGE_STYLE_HINT_STAY, ADVICE_CLICK_TO_CONTINUE);
	else if (mViewLawnTime == 251) mViewLawnTime = 250;

	for (int anIndex = 0; anIndex < mBoard->mSeedBank->mNumPackets; anIndex++)
	{
		SeedType aSeedType = FindSeedInBank(anIndex);
		if (aSeedType == SEED_NONE) break;
		ChosenSeed& aChosenSeed = mChosenSeeds[aSeedType];
		SeedPacket& aSeedPacket = mBoard->mSeedBank->mSeedPackets[anIndex];
		aSeedPacket.SetPacketType(aSeedType, aChosenSeed.mImitaterType);
	}

	int aSeedChooserY = SEED_CHOOSER_OFFSET_Y - Sexy::IMAGE_SEEDCHOOSER_BACKGROUND->GetHeight() - 87;
	int aStreetOffset = BOARD_IMAGE_WIDTH_OFFSET + BOARD_ADDITIONAL_WIDTH - mApp->mWidth;
	if (mViewLawnTime <= 100)
	{
		mBoard->mRoofPoleOffset = TodAnimateCurve(0, 100, mViewLawnTime, ROOF_POLE_END, ROOF_POLE_START, TodCurves::CURVE_EASE_IN_OUT);
		mBoard->mRoofTreeOffset = TodAnimateCurve(0, 100, mViewLawnTime, ROOF_TREE_END, ROOF_TREE_START, TodCurves::CURVE_EASE_IN_OUT);
		mBoard->Move(-TodAnimateCurve(0, 100, mViewLawnTime, aStreetOffset, 0, CURVE_EASE_IN_OUT), 0);
		Move(0, TodAnimateCurve(0, 40, mViewLawnTime, aSeedChooserY, SEED_CHOOSER_OFFSET_Y, CURVE_EASE_IN_OUT));
	}
	else if (mViewLawnTime <= 250)
	{
		mBoard->Move(0, 0);
		Move(0, SEED_CHOOSER_OFFSET_Y);
	}
	else if (mViewLawnTime <= 350)
	{
		mBoard->ClearAdvice(ADVICE_CLICK_TO_CONTINUE);
		mBoard->mRoofPoleOffset = TodAnimateCurve(250, 350, mViewLawnTime, ROOF_POLE_START, ROOF_POLE_END, TodCurves::CURVE_EASE_IN_OUT);
		mBoard->mRoofTreeOffset = TodAnimateCurve(250, 350, mViewLawnTime, ROOF_TREE_START, ROOF_TREE_END, TodCurves::CURVE_EASE_IN_OUT);
		mBoard->Move(-TodAnimateCurve(250, 350, mViewLawnTime, 0, aStreetOffset, CURVE_EASE_IN_OUT), 0);
		Move(0, TodAnimateCurve(310, 350, mViewLawnTime, SEED_CHOOSER_OFFSET_Y, aSeedChooserY, CURVE_EASE_IN_OUT));
	}
	else
	{
		mChooseState = CHOOSE_NORMAL;
		mViewLawnTime = 0;
		mMenuButton->mDisabled = false;

		for (int anIndex = 0; anIndex < mBoard->mSeedBank->mNumPackets; anIndex++)
		{
			SeedPacket& aSeedPacket = mBoard->mSeedBank->mSeedPackets[anIndex];
			aSeedPacket.SetPacketType(SEED_NONE, SEED_NONE);
		}
	}
}

void SeedChooserScreen::LandFlyingSeed(ChosenSeed& theChosenSeed)
{
	if (theChosenSeed.mSeedState == SEED_FLYING_TO_BANK)
	{
		theChosenSeed.mX = theChosenSeed.mEndX;
		theChosenSeed.mY = theChosenSeed.mEndY;
		theChosenSeed.mTimeStartMotion = 0;
		theChosenSeed.mTimeEndMotion = 0;
		theChosenSeed.mSeedState = SEED_IN_BANK;
		mSeedsInFlight--;
	}
	else if (theChosenSeed.mSeedState == SEED_FLYING_TO_CHOOSER)
	{
		theChosenSeed.mX = theChosenSeed.mEndX;
		theChosenSeed.mY = theChosenSeed.mEndY;
		theChosenSeed.mTimeStartMotion = 0;
		theChosenSeed.mTimeEndMotion = 0;
		theChosenSeed.mSeedState = SEED_IN_CHOOSER;
		mSeedsInFlight--;
		if (theChosenSeed.mSeedType == SEED_IMITATER)
		{
			theChosenSeed.mImitaterType = SEED_NONE;
		}
	}
}

void SeedChooserScreen::UpdateCursor()
{
	if (mApp->GetDialogCount() || mBoard->mCutScene->IsInShovelTutorial() || mApp->mGameMode == GAMEMODE_UPSELL || mSlider->mIsOver || mSlider->mDragging) return;
	SeedType aMouseSeedType = SeedHitTest(mApp->mWidgetManager->mLastMouseX, mApp->mWidgetManager->mLastMouseY);
	if (aMouseSeedType != SEED_NONE)
	{
		if (IsImitaterUnselectable(aMouseSeedType))
			aMouseSeedType = SEED_NONE;
		ChosenSeed& aMouseChosenSeed = mChosenSeeds[aMouseSeedType];
		if (aMouseChosenSeed.mSeedState == SEED_IN_BANK && aMouseChosenSeed.mCrazyDavePicked)
			aMouseSeedType = SEED_NONE;
	}
	if (mMouseVisible && mChooseState != CHOOSE_VIEW_LAWN && ((ZombieHitTest(mApp->mWidgetManager->mLastMouseX, mApp->mWidgetManager->mLastMouseY) && mApp->CanShowAlmanac() && !IsOverImitater(mApp->mWidgetManager->mLastMouseX, mApp->mWidgetManager->mLastMouseY)) || (aMouseSeedType != SEED_NONE && !SeedNotAllowedToPick(aMouseSeedType)) ||
		mRandomButton->IsMouseOver() || mViewLawnButton->IsMouseOver() || mAlmanacButton->IsMouseOver() ||
		mStoreButton->IsMouseOver() || mMenuButton->IsMouseOver() || mStartButton->IsMouseOver()))
		mApp->SetCursor(CURSOR_HAND);
	else
		mApp->SetCursor(CURSOR_POINTER);
}

void SeedChooserScreen::Update()
{
	Widget::Update();

	mRandomButton->mBtnNoDraw = !mApp->mTodCheatKeys;
	mRandomButton->mDisabled = !mApp->mTodCheatKeys;
	mMaxScrollPosition = max(0, (((NUM_SEEDS_IN_CHOOSER - 2) / cSeedPacketRows) * (SEED_PACKET_HEIGHT + cSeedPacketYOffset)) + SEED_PACKET_HEIGHT - cSeedClipRect.mHeight);
	mSlider->mVisible = mMaxScrollPosition != 0;
	if (mSlider->mVisible)
	{
		mScrollPosition = ClampFloat(mScrollPosition += mScrollAmount * (mBaseScrollSpeed + abs(mScrollAmount) * mScrollAccel), 0, mMaxScrollPosition);
		mScrollAmount *= (1.0f - mScrollAccel);
		mSlider->SetValue(max(0.0, min(mMaxScrollPosition, mScrollPosition)) / mMaxScrollPosition);
	}
	else
	{
		mScrollPosition = 0;
		mScrollAmount = 0;
	}

	if (!mBoard->mPaused)
		mSeedChooserAge++;

	if ((mArrowTimeStartMotion == -1 && mArrowTimeEndMotion == -1) || mSeedChooserAge >= mArrowTimeEndMotion)
	{
		mArrowTimeStartMotion = mSeedChooserAge;
		mArrowTimeEndMotion = mSeedChooserAge + 125;
	}

	for (SeedType aSeedType = SEED_PEASHOOTER; aSeedType < NUM_SEEDS_IN_CHOOSER; aSeedType = (SeedType)(aSeedType + 1))
	{
		if (mApp->SeedTypeAvailable(aSeedType))
		{
			ChosenSeed& aChosenSeed = mChosenSeeds[aSeedType];
			if (aChosenSeed.mSeedState == SEED_FLYING_TO_BANK || aChosenSeed.mSeedState == SEED_FLYING_TO_CHOOSER)
			{
				int aTimeStart = aChosenSeed.mTimeStartMotion;
				int aTimeEnd = aChosenSeed.mTimeEndMotion;
				aChosenSeed.mX = TodAnimateCurve(aTimeStart, aTimeEnd, mSeedChooserAge, aChosenSeed.mStartX, aChosenSeed.mEndX, CURVE_EASE_IN_OUT);
				aChosenSeed.mY = TodAnimateCurve(aTimeStart, aTimeEnd, mSeedChooserAge, aChosenSeed.mStartY, aChosenSeed.mEndY, CURVE_EASE_IN_OUT);
				if (mSeedChooserAge >= aChosenSeed.mTimeEndMotion)
				{
					LandFlyingSeed(aChosenSeed);
				}
			}
		}
	}
	
	mToolTip->Update();
	for (int i = 0; i < MAX_GAMEPADS; i++)
		mBoard->mGamepadPlayerList[i]->mSeedChooserToolTip->Update();
	ShowToolTip();
	mStartButton->Update();
	mRandomButton->Update();
	mViewLawnButton->Update();
	mAlmanacButton->Update();
	mStoreButton->Update();
	mMenuButton->Update();
	UpdateViewLawn();
	UpdateCursor();
	MarkDirty();
}

void SeedChooserScreen::MouseWheel(int theDelta)
{
	if (mChooseState != CHOOSE_NORMAL) return;

	mScrollAmount -= mBaseScrollSpeed * theDelta;
	mScrollAmount -= mScrollAmount * mScrollAccel;
}

bool SeedChooserScreen::DisplayRepickWarningDialog(const SexyChar* theMessage)
{
	return mApp->LawnMessageBox(
		Dialogs::DIALOG_CHOOSER_WARNING,
		_S("[DIALOG_WARNING]"),
		theMessage,
		_S("[DIALOG_BUTTON_YES]"),
		_S("[REPICK_BUTTON]"),
		Dialog::BUTTONS_YES_NO
	) == Dialog::ID_YES;
}

bool SeedChooserScreen::FlyersAreComming()
{
	for (int aWave = 0; aWave < mBoard->mNumWaves; aWave++)
	{
		for (int anIndex = 0; anIndex < MAX_ZOMBIES_IN_WAVE; anIndex++)
		{
			ZombieType aZombieType = mBoard->mZombiesInWave[aWave][anIndex];
			if (aZombieType == ZOMBIE_INVALID)
				break;

			if (aZombieType == ZOMBIE_BALLOON)
				return true;
		}
	}
	return false;
}

bool SeedChooserScreen::FlyProtectionCurrentlyPlanted()
{
	Plant* aPlant = nullptr;
	while (mBoard->IteratePlants(aPlant))
	{
		if (aPlant->mSeedType == SEED_CATTAIL || aPlant->mSeedType == SEED_CACTUS)
		{
			return true;
		}
	}
	return false;
}

bool SeedChooserScreen::CheckSeedUpgrade(SeedType theSeedTypeTo, SeedType theSeedTypeFrom)
{
	if (mApp->IsSurvivalMode() || !PickedPlantType(theSeedTypeTo) || PickedPlantType(theSeedTypeFrom))
		return true;

	SexyString aWarning = TodStringTranslate(_S("[SEED_CHOOSER_UPGRADE_WARNING]"));
	aWarning = TodReplaceString(aWarning, _S("{UPGRADE_TO}"), Plant::GetNameString(theSeedTypeTo));
	aWarning = TodReplaceString(aWarning, _S("{UPGRADE_FROM}"), Plant::GetNameString(theSeedTypeFrom));
	return DisplayRepickWarningDialog(aWarning.c_str());
}

void SeedChooserScreen::OnStartButton()
{
	if (mApp->mGameMode == GAMEMODE_CHALLENGE_SEEING_STARS && !PickedPlantType(SEED_STARFRUIT))
	{
		if (!DisplayRepickWarningDialog(_S("[SEED_CHOOSER_SEEING_STARS_WARNING]")))
		{
			return;
		}
	}

	if (mApp->IsFirstTimeAdventureMode() && mBoard->mLevel == 11 && !PickedPlantType(SEED_PUFFSHROOM))
	{
		if (!DisplayRepickWarningDialog(_S("[SEED_CHOOSER_PUFFSHROOM_WARNING]")))
		{
			return;
		}
	}
	if (!PickedPlantType(SEED_SUNFLOWER) && !PickedPlantType(SEED_TWINSUNFLOWER) && !PickedPlantType(SEED_SUNSHROOM) &&
		!mBoard->mCutScene->IsSurvivalRepick() && mApp->mGameMode != GAMEMODE_CHALLENGE_LAST_STAND)
	{
		if (mApp->IsFirstTimeAdventureMode() && mBoard->mLevel == 11)
		{
			if (!DisplayRepickWarningDialog(_S("[SEED_CHOOSER_NIGHT_SUN_WARNING]")))
			{
				return;
			}
		}
		else if (!DisplayRepickWarningDialog(_S("[SEED_CHOOSER_SUN_WARNING]")))
		{
			return;
		}
	}
	if (mBoard->StageHasPool() && !PickedPlantType(SEED_LILYPAD) && !PickedPlantType(SEED_SEASHROOM) && !PickedPlantType(SEED_TANGLEKELP) && !mBoard->mCutScene->IsSurvivalRepick())
	{
		if (mApp->IsFirstTimeAdventureMode() && mBoard->mLevel == 21)
		{
			if (!DisplayRepickWarningDialog(_S("[SEED_CHOOSER_LILY_WARNING]")))
			{
				return;
			}
		}
		else if (!DisplayRepickWarningDialog(_S("[SEED_CHOOSER_POOL_WARNING]")))
		{
			return;
		}
	}
	if (mBoard->StageHasRoof() && !PickedPlantType(SEED_FLOWERPOT) && mApp->SeedTypeAvailable(SEED_FLOWERPOT))
	{
		if (!DisplayRepickWarningDialog(_S("[SEED_CHOOSER_ROOF_WARNING]")))
		{
			return;
		}
	}

	if (mApp->mGameMode == GAMEMODE_CHALLENGE_ART_CHALLENGE_WALLNUT && !PickedPlantType(SEED_WALLNUT))
	{
		if (!DisplayRepickWarningDialog(_S("[SEED_CHOOSER_ART_WALLNUT_WARNING]")))
		{
			return;
		}
	}
	if (mApp->mGameMode == GAMEMODE_CHALLENGE_ART_CHALLENGE_SUNFLOWER &&
		(!PickedPlantType(SEED_STARFRUIT) || !PickedPlantType(SEED_UMBRELLA) || !PickedPlantType(SEED_WALLNUT)))
	{
		if (!DisplayRepickWarningDialog(_S("[SEED_CHOOSER_ART_2_WARNING]")))
		{
			return;
		}
	}

	if (FlyersAreComming() && !FlyProtectionCurrentlyPlanted() && !PickedPlantType(SEED_CATTAIL) && !PickedPlantType(SEED_CACTUS) && !PickedPlantType(SEED_BLOVER))
	{
		if (!DisplayRepickWarningDialog(_S("[SEED_CHOOSER_FLYER_WARNING]")))
		{
			return;
		}
	}

	if (!CheckSeedUpgrade(SEED_GATLINGPEA, SEED_REPEATER) ||
		!CheckSeedUpgrade(SEED_WINTERMELON, SEED_MELONPULT) ||
		!CheckSeedUpgrade(SEED_TWINSUNFLOWER, SEED_SUNFLOWER) ||
		!CheckSeedUpgrade(SEED_SPIKEROCK, SEED_SPIKEWEED) ||
		!CheckSeedUpgrade(SEED_COBCANNON, SEED_KERNELPULT) ||
		!CheckSeedUpgrade(SEED_GOLD_MAGNET, SEED_MAGNETSHROOM) ||
		!CheckSeedUpgrade(SEED_GLOOMSHROOM, SEED_FUMESHROOM) ||
		!CheckSeedUpgrade(SEED_CATTAIL, SEED_LILYPAD))
		return;

	CloseSeedChooser();
}

void SeedChooserScreen::PickRandomSeeds()
{
	for (int anIndex = mSeedsInBank; anIndex < mBoard->mSeedBank->mNumPackets; anIndex++)
	{
		SeedType aSeedType;
		do aSeedType = (SeedType)Rand(mApp->GetSeedsAvailable());
		while (!mApp->SeedTypeAvailable(aSeedType) || aSeedType == SEED_IMITATER || mChosenSeeds[aSeedType].mSeedState != SEED_IN_CHOOSER);
		ChosenSeed& aChosenSeed = mChosenSeeds[aSeedType];
		aChosenSeed.mTimeStartMotion = 0;
		aChosenSeed.mTimeEndMotion = 0;
		aChosenSeed.mStartX = aChosenSeed.mX;
		aChosenSeed.mStartY = aChosenSeed.mY;
		GetSeedPositionInBank(anIndex, aChosenSeed.mEndX, aChosenSeed.mEndY);
		aChosenSeed.mSeedState = SEED_IN_BANK;
		aChosenSeed.mSeedIndexInBank = anIndex;
		mSeedsInBank++;
	}
	for (SeedType aSeedFlying = SEED_PEASHOOTER; aSeedFlying < NUM_SEEDS_IN_CHOOSER; aSeedFlying = (SeedType)(aSeedFlying + 1))
		LandFlyingSeed(mChosenSeeds[aSeedFlying]);
	CloseSeedChooser();
}

void SeedChooserScreen::ButtonDepress(int theId)
{
	if (mSeedsInFlight > 0 || mChooseState == CHOOSE_VIEW_LAWN || !mMouseVisible)
		return;

	if (theId == SeedChooserScreen::SeedChooserScreen_ViewLawn)
	{
		mChooseState = CHOOSE_VIEW_LAWN;
		mMenuButton->mDisabled = true;
		mViewLawnTime = 0;
	}
	else if (theId == SeedChooserScreen::SeedChooserScreen_Almanac)
	{
		mApp->DoAlmanacDialog()->WaitForResult(true);
		mApp->mWidgetManager->SetFocus(this);
	}
	else if (theId == SeedChooserScreen::SeedChooserScreen_Store)
	{
		StoreScreen* aStore = mApp->ShowStoreScreen();
		aStore->mBackButton->mLabel = _S("[STORE_BACK_TO_GAME]");
		aStore->WaitForResult();
		if (aStore->mGoToTreeNow)
		{
			mApp->KillBoard();
			mApp->PreNewGame(GAMEMODE_TREE_OF_WISDOM, false);
		}
		else
		{
			mApp->mMusic->MakeSureMusicIsPlaying(MUSIC_TUNE_CHOOSE_YOUR_SEEDS);
			mApp->mWidgetManager->SetFocus(this);
		}
	}
	else if (theId == SeedChooserScreen::SeedChooserScreen_Menu)
	{
		mMenuButton->mIsOver = false;
		mMenuButton->mIsDown = false;
		UpdateCursor();
		mApp->DoNewOptions(false);
	}
	else if (mApp->GetSeedsAvailable() >= mBoard->mSeedBank->mNumPackets)
	{
		if (theId == SeedChooserScreen::SeedChooserScreen_Start)
			OnStartButton();
		else if (theId == SeedChooserScreen::SeedChooserScreen_Random)
			PickRandomSeeds();
	}
}

SeedType SeedChooserScreen::SeedHitTest(int x, int y)
{
	if (mMouseVisible)
	{
		for (SeedType aSeedType = SEED_PEASHOOTER; aSeedType < NUM_SEEDS_IN_CHOOSER; aSeedType = (SeedType)(aSeedType + 1))
		{
			ChosenSeed& aChosenSeed = mChosenSeeds[aSeedType];
			if (!mApp->SeedTypeAvailable(aSeedType) || aChosenSeed.mSeedState == SEED_PACKET_HIDDEN) continue;
			if (aChosenSeed.mSeedState == SEED_IN_CHOOSER)
			{
				Rect aChosenSeedRect = Rect(aChosenSeed.mX, aChosenSeed.mY + (aChosenSeed.mSeedType != SEED_IMITATER ? -mScrollPosition : 0), SEED_PACKET_WIDTH, SEED_PACKET_HEIGHT);
				if ((aChosenSeed.mSeedType != SEED_IMITATER ? cSeedClipRect.Contains(x, y) : true) && aChosenSeedRect.Contains(x, y))
				{
					return aSeedType;
				}
			}
			else
			{
				if (Rect(aChosenSeed.mX, aChosenSeed.mY, SEED_PACKET_WIDTH, SEED_PACKET_HEIGHT).Contains(x, y)) return aSeedType;
			}
		}
	}
	return SEED_NONE;
}

SeedType SeedChooserScreen::FindSeedInBank(int theIndexInBank)
{
	for (SeedType aSeedType = SEED_PEASHOOTER; aSeedType < NUM_SEEDS_IN_CHOOSER; aSeedType = (SeedType)(aSeedType + 1))
	{
		if (mApp->SeedTypeAvailable(aSeedType))
		{
			ChosenSeed& aChosenSeed = mChosenSeeds[aSeedType];
			if (aChosenSeed.mSeedState == SEED_IN_BANK && aChosenSeed.mSeedIndexInBank == theIndexInBank) return aSeedType;
		}
	}
	return SEED_NONE;
}

void SeedChooserScreen::EnableStartButton(bool theEnabled)
{
	mStartButton->SetDisabled(!theEnabled);
	if (theEnabled) mStartButton->mColors[GameButton::COLOR_LABEL] = Color::White;
	else mStartButton->mColors[GameButton::COLOR_LABEL] = Color(64, 64, 64);
}

void SeedChooserScreen::ClickedSeedInBank(ChosenSeed& theChosenSeed)
{
	mPreviousType = FindSeedInBank(mSeedsInBank - (theChosenSeed.mSeedIndexInBank == mSeedsInBank - 1 ? 2 : 1));
	for (int anIndex = theChosenSeed.mSeedIndexInBank + 1; anIndex < mBoard->mSeedBank->mNumPackets; anIndex++)
	{
		SeedType aSeedType = FindSeedInBank(anIndex);
		if (aSeedType != SEED_NONE)
		{
			ChosenSeed& aChosenSeed = mChosenSeeds[aSeedType];
			aChosenSeed.mTimeStartMotion = mSeedChooserAge;
			aChosenSeed.mTimeEndMotion = mSeedChooserAge + 15;
			aChosenSeed.mStartX = aChosenSeed.mX;
			aChosenSeed.mStartY = aChosenSeed.mY;
			GetSeedPositionInBank(anIndex - 1, aChosenSeed.mEndX, aChosenSeed.mEndY);
			aChosenSeed.mSeedState = SEED_FLYING_TO_BANK;
			aChosenSeed.mSeedIndexInBank = anIndex - 1;
			mSeedsInFlight++;
		}
	}

	theChosenSeed.mTimeStartMotion = mSeedChooserAge;
	theChosenSeed.mTimeEndMotion = mSeedChooserAge + 25;
	theChosenSeed.mStartX = theChosenSeed.mX;
	theChosenSeed.mStartY = theChosenSeed.mY;
	GetSeedPositionInChooser(theChosenSeed.mSeedType, theChosenSeed.mEndX, theChosenSeed.mEndY);
	theChosenSeed.mY += theChosenSeed.mSeedType == SEED_IMITATER ? 0 : mScrollPosition;
	theChosenSeed.mSeedState = SEED_FLYING_TO_CHOOSER;
	theChosenSeed.mImitaterType = SEED_NONE;
	theChosenSeed.mSeedIndexInBank = 0;
	mSeedsInFlight++;
	mSeedsInBank--;
	EnableStartButton(false);
	mApp->PlaySample(Sexy::SOUND_TAP);

}

void SeedChooserScreen::ClickedSeedInChooser(ChosenSeed& theChosenSeed)
{
	if (mSeedsInBank == mBoard->mSeedBank->mNumPackets)
		return;

	if (IsImitaterUnselectable(theChosenSeed.mSeedType))
		return;

	if (theChosenSeed.mSeedType == SEED_IMITATER)
		theChosenSeed.mImitaterType = FindSeedInBank(mSeedsInBank - 1);
	else
		mPreviousType = theChosenSeed.mSeedType;
	
	theChosenSeed.mTimeStartMotion = mSeedChooserAge;
	theChosenSeed.mTimeEndMotion = mSeedChooserAge + 25;
	theChosenSeed.mStartX = theChosenSeed.mX;
	theChosenSeed.mStartY = theChosenSeed.mY - (theChosenSeed.mSeedType == SEED_IMITATER ? 0 : mScrollPosition);
	GetSeedPositionInBank(mSeedsInBank, theChosenSeed.mEndX, theChosenSeed.mEndY);
	theChosenSeed.mSeedState = SEED_FLYING_TO_BANK;
	theChosenSeed.mSeedIndexInBank = mSeedsInBank;
	mSeedsInFlight++;
	mSeedsInBank++;

	mApp->PlaySample(Sexy::SOUND_TAP);
	if (mSeedsInBank == mBoard->mSeedBank->mNumPackets)
		EnableStartButton(true);
}

void SeedChooserScreen::ShowToolTip()
{
	if (!mBoard->mCutScene->mSeedChoosing || mChooseState == CHOOSE_VIEW_LAWN)
	{
		RemoveToolTip();
		for (int i = 0; i < MAX_GAMEPADS; i++)
			RemoveToolTip(i);
		return;
	}

	bool aIsToolTipInUse = mBoard->mPaused;
	Zombie* aHitZombie = ZombieHitTest(mApp->mWidgetManager->mLastMouseX, mApp->mWidgetManager->mLastMouseY);
	if (aHitZombie != nullptr && aHitZombie->mFromWave == Zombie::ZOMBIE_WAVE_CUTSCENE && !IsOverImitater(mApp->mWidgetManager->mLastMouseX, mApp->mWidgetManager->mLastMouseY) && !aIsToolTipInUse)
	{
		mToolTip->SetTitle(StrFormat(_S("[%s]"), GetZombieDefinition(aHitZombie->mZombieType).mZombieName));
		if (mApp->CanShowAlmanac())
			mToolTip->SetLabel(_S("[CLICK_TO_VIEW]"));
		else
			mToolTip->SetLabel(_S(""));
		mToolTip->SetWarningText(_S(""));

		Rect aRect = aHitZombie->GetZombieRect();
		mToolTip->mX = aRect.mWidth / 2 + aRect.mX + 5 + mBoard->mX;
		mToolTip->mY = aRect.mHeight + aRect.mY - 10 - mBoard->mY;
		if (aHitZombie->mZombieType == ZombieType::ZOMBIE_BUNGEE)
			mToolTip->mY = aHitZombie->mY;
		mToolTip->mCenter = true;
		mToolTip->mVisible = true;
		if (mAlmanacButton->mBtnNoDraw && mStoreButton->mBtnNoDraw)
			mToolTip->mMaxBottom = BOARD_HEIGHT;
		else
			mToolTip->mMaxBottom = BOARD_HEIGHT - 30;
		aIsToolTipInUse = true;
	}
	
	SeedType aHitSeed = SeedHitTest(mApp->mWidgetManager->mLastMouseX, mApp->mWidgetManager->mLastMouseY);
	if (aHitSeed != SEED_NONE && !aIsToolTipInUse)
	{
		ChosenSeed& aChosenSeed = mChosenSeeds[aHitSeed];
		if (aChosenSeed.mSeedState != SEED_FLYING_TO_BANK && aChosenSeed.mSeedState != SEED_FLYING_TO_CHOOSER)
		{
			SetToolTipSeedContents(aHitSeed);
			int aSeedX, aSeedY;
			if (aChosenSeed.mSeedState == SEED_IN_BANK)
				GetSeedPositionInBank(aChosenSeed.mSeedIndexInBank, aSeedX, aSeedY);
			else
				GetSeedPositionInChooser(aHitSeed, aSeedX, aSeedY);
			int aX, aY;
			GetToolTipPosition(-1, aSeedX, aSeedY, aHitSeed, &aX, &aY);
			mToolTip->mX = aX;
			mToolTip->mY = aY;
			mToolTip->mVisible = true;
			aIsToolTipInUse = true;
		}
	}

	if (!aIsToolTipInUse)
		RemoveToolTip();

	for (int i = 0; i < MAX_GAMEPADS; i++)
	{
		GamepadPlayer* aGamepadPlayer = mBoard->mGamepadPlayerList[i];
		SeedType aSeedType = aGamepadPlayer->mSeedChooserSeed;
		if (aSeedType == SEED_NONE)
		{
			RemoveToolTip(i);
		}
		else
		{
			ChosenSeed& aChosenSeed = mChosenSeeds[aSeedType];
			if (!mApp->SeedTypeAvailable(aSeedType) || aChosenSeed.mSeedState == SEED_PACKET_HIDDEN)
			{
				RemoveToolTip(i);
				continue;
			}
			bool aIsOverlapping = aHitSeed == aSeedType && aChosenSeed.mSeedState == SEED_IN_CHOOSER;
			if (!aIsOverlapping && i != 0)
			{
				for (int i = 0; i < MAX_GAMEPADS; i++)
				{
					aIsOverlapping = aSeedType == aGamepadPlayer->mSeedChooserSeed && i != i;
					if (aIsOverlapping)
						break;
				}
			}
			if (aIsOverlapping || aChosenSeed.mSeedState == SEED_FLYING_TO_BANK || aChosenSeed.mSeedState == SEED_FLYING_TO_CHOOSER)
			{
				RemoveToolTip(i);
				continue;
			}
			SetToolTipSeedContents(aSeedType, i);
			int aSeedX, aSeedY;
			GetSeedPositionInChooser(aSeedType, aSeedX, aSeedY);
			int aX, aY;
			GetToolTipPosition(i, aSeedX, aSeedY, aSeedType, &aX, &aY);
			ToolTipWidget* aToolTip = aGamepadPlayer->mSeedChooserToolTip;
			aToolTip->mX = aX;
			aToolTip->mY = aY;
			aToolTip->mVisible = true;
		}
	}
}

void SeedChooserScreen::RemoveToolTip(int theIndex)
{
	ToolTipWidget* aToolTip = nullptr;
	if (theIndex == -1)
		aToolTip = mToolTip;
	else
		aToolTip = mBoard->mGamepadPlayerList[theIndex]->mSeedChooserToolTip;
	if (aToolTip == nullptr)
		return;
	aToolTip->mVisible = false;
	aToolTip->mMaxBottom = BOARD_HEIGHT;
	aToolTip->mCenter = false;
}

void SeedChooserScreen::CancelLawnView()
{
	if (mChooseState == CHOOSE_VIEW_LAWN && mViewLawnTime > 100 && mViewLawnTime <= 250) mViewLawnTime = 251;
}

bool SeedChooserScreen::IsOverImitater(int x, int y)
{
	return mApp->SeedTypeAvailable(SEED_IMITATER) && Rect(IMITATER_POS_X, IMITATER_POS_Y + SEED_CHOOSER_EXTRA_HEIGHT, IMAGE_SEEDCHOOSER_IMITATERADDON->mWidth, IMAGE_SEEDCHOOSER_IMITATERADDON->mHeight).Contains(x, y);
}

void SeedChooserScreen::ResizeSlider()
{
	mSlider->Resize(472, 92, 40, IMAGE_SEEDCHOOSER_BACKGROUND->mHeight - (mApp->SeedTypeAvailable(SEED_IMITATER) ? IMAGE_SEEDCHOOSER_IMITATERADDON->mHeight + 4 : 0) - 11);
}

void SeedChooserScreen::SetToolTipSeedContents(SeedType theSeedType, int theIndex)
{
	RemoveToolTip(theIndex);
	ChosenSeed& aChosenSeed = mChosenSeeds[theSeedType];
	ToolTipWidget* aToolTip = nullptr;
	if (theIndex == -1)
		aToolTip = mToolTip;
	else
		aToolTip = mBoard->mGamepadPlayerList[theIndex]->mSeedChooserToolTip;
	if (aToolTip == nullptr)
		return;
	uint aRecFlags = SeedNotRecommendedToPick(theSeedType);
	if (SeedNotAllowedToPick(theSeedType))
	{
		aToolTip->SetWarningText(_S("[NOT_ALLOWED_ON_THIS_LEVEL]"));
	}
	else if (SeedNotAllowedDuringTrial(theSeedType))
	{
		aToolTip->SetWarningText(_S("[FULL_VERSION_ONLY]"));
	}
	else if (aChosenSeed.mSeedState == SEED_IN_BANK && aChosenSeed.mCrazyDavePicked)
	{
		aToolTip->SetWarningText(_S("[CRAZY_DAVE_WANTS]"));
	}
	else if (aRecFlags != 0U)
	{
		if (TestBit(aRecFlags, NOT_RECOMMENDED_NOCTURNAL))
		{
			aToolTip->SetWarningText(_S("[NOCTURNAL_WARNING]"));
		}
		else
		{
			aToolTip->SetWarningText(_S("[NOT_RECOMMENDED_FOR_LEVEL]"));
		}
	}
	else
	{
		aToolTip->SetWarningText(_S(""));
	}

	if (theSeedType == SEED_IMITATER) // && theIndex == -1
	{
		aToolTip->SetTitle(Plant::GetNameString(theSeedType, aChosenSeed.mImitaterType));
		aToolTip->SetLabel(Plant::GetToolTip(aChosenSeed.mImitaterType == SEED_NONE ? SEED_IMITATER : aChosenSeed.mImitaterType));
	}
	else
	{
		aToolTip->SetTitle(Plant::GetNameString(theSeedType, SEED_NONE));
		aToolTip->SetLabel(Plant::GetToolTip(theSeedType));
	}
}

void SeedChooserScreen::GetToolTipPosition(int theIndex, int theSeedX, int theSeedY, SeedType theSeedType, int* theX, int* theY)
{
	ChosenSeed& aChosenSeed = mChosenSeeds[theSeedType];
	ToolTipWidget* aToolTip = nullptr;
	if (theIndex == -1)
		aToolTip = mToolTip;
	else
		aToolTip = mBoard->mGamepadPlayerList[theIndex]->mSeedChooserToolTip;
	if (aToolTip == nullptr)
		return;
	if (theX != nullptr)
		*theX = ClampInt((SEED_PACKET_WIDTH - aToolTip->mWidth) / 2 + theSeedX, 0, BOARD_WIDTH - aToolTip->mWidth);
	if (theY != nullptr)
		*theY = theSeedY + (theSeedType == SEED_IMITATER && (aChosenSeed.mSeedState == SEED_IN_CHOOSER || theIndex != -1) ? -aToolTip->mHeight : SEED_PACKET_HEIGHT);
}

void SeedChooserScreen::SelectSeedType(SeedType theSeedType)
{
	if (theSeedType != SEED_NONE && !SeedNotAllowedToPick(theSeedType) && mApp->SeedTypeAvailable(theSeedType))
	{
		if (SeedNotAllowedDuringTrial(theSeedType))
		{
			mApp->PlaySample(Sexy::SOUND_TAP);
			if (mApp->LawnMessageBox(
				DIALOG_MESSAGE,
				_S("[GET_FULL_VERSION_TITLE]"),
				_S("[GET_FULL_VERSION_BODY]"),
				_S("[GET_FULL_VERSION_YES_BUTTON]"),
				_S("[GET_FULL_VERSION_NO_BUTTON]"),
				Dialog::BUTTONS_YES_NO
			) == Dialog::ID_YES)
			{
				if (mApp->mDRM)
				{
					mApp->mDRM->BuyGame();
				}
				mApp->DoBackToMain();
			}
		}
		else
		{
			ChosenSeed& aChosenSeed = mChosenSeeds[theSeedType];
			if (aChosenSeed.mSeedState == SEED_PACKET_HIDDEN) 
				return;
			if (aChosenSeed.mSeedState == SEED_FLYING_TO_BANK || aChosenSeed.mSeedState == SEED_FLYING_TO_CHOOSER)
			{
				aChosenSeed.mX = aChosenSeed.mEndX;
				aChosenSeed.mY = aChosenSeed.mEndY;
				aChosenSeed.mSeedState = aChosenSeed.mSeedState == SEED_FLYING_TO_BANK ? SEED_IN_BANK : SEED_IN_CHOOSER;
				mSeedsInFlight--;
			}
			if (mSeedsInBank != 0)
			{
				for (SeedType aSeedType = SEED_PEASHOOTER; aSeedType < NUM_SEEDS_IN_CHOOSER; aSeedType = (SeedType)(aSeedType + 1))
				{
					if (mApp->SeedTypeAvailable(aSeedType))
					{
						ChosenSeed& aPrevChosenSeed = mChosenSeeds[aSeedType];
						if (aPrevChosenSeed.mSeedIndexInBank == mSeedsInBank - 1 && (aPrevChosenSeed.mSeedState == SEED_FLYING_TO_BANK || aPrevChosenSeed.mSeedState == SEED_FLYING_TO_CHOOSER))
						{
							aPrevChosenSeed.mX = aPrevChosenSeed.mEndX;
							aPrevChosenSeed.mY = aPrevChosenSeed.mEndY;
							aPrevChosenSeed.mSeedState = aPrevChosenSeed.mSeedState == SEED_FLYING_TO_BANK ? SEED_IN_BANK : SEED_IN_CHOOSER;
							mSeedsInFlight--;
						}
					}
				}
			}
			if (aChosenSeed.mSeedState == SEED_IN_BANK)
			{
				if (aChosenSeed.mCrazyDavePicked)
				{
					mApp->PlaySample(Sexy::SOUND_BUZZER);
					mToolTip->FlashWarning();
					for (int i = 0; i < MAX_GAMEPADS; i++)
					{
						GamepadPlayer* aGamepadPlayer = mBoard->mGamepadPlayerList[i];
						ToolTipWidget* aToolTip = aGamepadPlayer->mSeedChooserToolTip;
						if (aToolTip->mVisible && aGamepadPlayer->mSeedChooserSeed == theSeedType)
							aToolTip->FlashWarning();
					}
				}
				else ClickedSeedInBank(aChosenSeed);
			}
			else if (aChosenSeed.mSeedState == SEED_IN_CHOOSER)
				ClickedSeedInChooser(aChosenSeed);
		}
	}
}

int SeedChooserScreen::GetRows()
{
	return cSeedPacketRows;
}

void SeedChooserScreen::MouseUp(int x, int y, int theClickCount)
{
	if (theClickCount == 1)
	{
		if (mMenuButton->IsMouseOver()) ButtonDepress(SeedChooserScreen::SeedChooserScreen_Menu);
		else if (mStartButton->IsMouseOver()) ButtonDepress(SeedChooserScreen::SeedChooserScreen_Start);
		else if (mAlmanacButton->IsMouseOver()) ButtonDepress(SeedChooserScreen::SeedChooserScreen_Almanac);
		else if (mStoreButton->IsMouseOver()) ButtonDepress(SeedChooserScreen::SeedChooserScreen_Store);
	}
}

bool SeedChooserScreen::IsImitaterUnselectable(SeedType seedType)
{
	return seedType == SEED_IMITATER && (mSeedsInBank == 0 || mSeedsInBank == mBoard->mSeedBank->mNumPackets || Plant::IsUpgrade(mPreviousType) || SeedNotAllowedToPick(mPreviousType));
}

void SeedChooserScreen::MouseDown(int x, int y, int theClickCount)
{
	Widget::MouseDown(x, y, theClickCount);

	if (mSeedsInFlight > 0)
	{
		for (int i = 0; i < NUM_SEEDS_IN_CHOOSER; i++)
		{
			LandFlyingSeed(mChosenSeeds[i]);
		}
	}

	if (mChooseState == CHOOSE_VIEW_LAWN)
	{
		CancelLawnView();
	}
	else if (mRandomButton->IsMouseOver())
	{
		mApp->PlaySample(Sexy::SOUND_TAP);
		ButtonDepress(SeedChooserScreen::SeedChooserScreen_Random);
	}
	else if (mViewLawnButton->IsMouseOver())
	{
		mApp->PlaySample(Sexy::SOUND_TAP);
		ButtonDepress(SeedChooserScreen::SeedChooserScreen_ViewLawn);
	}
	else if (mMenuButton->IsMouseOver())
	{
		mApp->PlaySample(Sexy::SOUND_GRAVEBUTTON);
	}
	else if (mStartButton->IsMouseOver() || mAlmanacButton->IsMouseOver() || mStoreButton->IsMouseOver())
	{
		mApp->PlaySample(Sexy::SOUND_TAP);
	}
	else
	{
		if (!IsOverImitater(x, y) && !mAlmanacButton->IsMouseOver() && !mStoreButton->IsMouseOver() && mApp->CanShowAlmanac())
		{
			Zombie* aZombie = ZombieHitTest(x, y);
			if (aZombie && aZombie->mFromWave == Zombie::ZOMBIE_WAVE_CUTSCENE && aZombie->mZombieType != ZOMBIE_REDEYE_GARGANTUAR)
			{
				mApp->PlaySample(Sexy::SOUND_TAP);
				mApp->DoAlmanacDialog(SEED_NONE, aZombie->mZombieType)->WaitForResult(true);
				mApp->mWidgetManager->SetFocus(this);
				return;
			}
		}

		SelectSeedType(SeedHitTest(x, y));
	}
}

bool SeedChooserScreen::PickedPlantType(SeedType theSeedType)
{
	for (SeedType aSeedType = SEED_PEASHOOTER; aSeedType < NUM_SEEDS_IN_CHOOSER; aSeedType = (SeedType)(aSeedType + 1))
	{
		ChosenSeed& aChosenSeed = mChosenSeeds[aSeedType];
		if (aChosenSeed.mSeedState == SEED_IN_BANK || aChosenSeed.mSeedState == SEED_FLYING_TO_BANK)
		{
			if (aChosenSeed.mSeedType == theSeedType || (aChosenSeed.mSeedType == SEED_IMITATER && aChosenSeed.mImitaterType == theSeedType))
			{
				return true;
			}
		}
	}
	return false;
}

void SeedChooserScreen::CloseSeedChooser()
{
	DBG_ASSERT(mBoard->mSeedBank->mNumPackets == mBoard->GetNumSeedsInBank());
	for (int anIndex = 0; anIndex < mBoard->mSeedBank->mNumPackets; anIndex++)
	{
		SeedType aSeedType = FindSeedInBank(anIndex);
		ChosenSeed& aChosenSeed = mChosenSeeds[aSeedType];
		SeedPacket& aSeedPacket = mBoard->mSeedBank->mSeedPackets[anIndex];
		aSeedPacket.SetPacketType(aSeedType, aChosenSeed.mImitaterType);
		if (aChosenSeed.mRefreshing)
		{
			aSeedPacket.mRefreshCounter = aChosenSeed.mRefreshCounter;
			aSeedPacket.mRefreshTime = Plant::GetRefreshTime(aSeedPacket.mPacketType, aSeedPacket.mImitaterType);
			aSeedPacket.mRefreshing = true;
			aSeedPacket.mActive = false;
		}
	}
	mBoard->mCutScene->EndSeedChooser();
}

void SeedChooserScreen::KeyDown(KeyCode theKey)
{
	mBoard->DoTypingCheck(theKey);
}

void SeedChooserScreen::KeyChar(SexyChar theChar)
{
	if (mChooseState == CHOOSE_VIEW_LAWN && (theChar == ' ' || theChar == '\r' || theChar == '\u001B'))
		CancelLawnView();
	else if (mApp->mTodCheatKeys && theChar == '\u001B')
		PickRandomSeeds();
	else mBoard->KeyChar(theChar);
}

void SeedChooserScreen::UpdateAfterPurchase()
{
	for (SeedType aSeedType = SEED_PEASHOOTER; aSeedType < NUM_SEEDS_IN_CHOOSER; aSeedType = (SeedType)(aSeedType + 1))
	{
		ChosenSeed& aChosenSeed = mChosenSeeds[aSeedType];
		if (aChosenSeed.mSeedState == SEED_IN_BANK)
			GetSeedPositionInBank(aChosenSeed.mSeedIndexInBank, aChosenSeed.mX, aChosenSeed.mY);
		else if (aChosenSeed.mSeedState == SEED_IN_CHOOSER)
			GetSeedPositionInChooser(aSeedType, aChosenSeed.mX, aChosenSeed.mY);
		else continue;
		aChosenSeed.mStartX = aChosenSeed.mX;
		aChosenSeed.mStartY = aChosenSeed.mY;
		aChosenSeed.mEndX = aChosenSeed.mX;
		aChosenSeed.mEndY = aChosenSeed.mY;
	}
	EnableStartButton(mSeedsInBank == mBoard->mSeedBank->mNumPackets);
	ResizeSlider();
}

Zombie* SeedChooserScreen::ZombieHitTest(int x, int y)
{
	Zombie* aRecord = nullptr;
	if (mMouseVisible && !Rect(mBoard->mSeedBank->mX - mX, mBoard->mSeedBank->mY - mY, mBoard->mSeedBank->mWidth, mBoard->mSeedBank->mHeight).Contains(x, y))
	{
		Zombie* aZombie = nullptr;
		while (mBoard->IterateZombies(aZombie))
		{
			if (aZombie->mDead || aZombie->IsDeadOrDying() || aZombie->mZombieType >= NUM_ZOMBIES_IN_ALMANAC)
				continue;

			if (aZombie->GetZombieRect().Contains(x - mBoard->mX, y - mBoard->mY))
			{
				if (aRecord == nullptr || aZombie->mY > aRecord->mY)
				{
					aRecord = aZombie;
				}
			}
		}
	}
	return aRecord;
}


void SeedChooserScreen::SliderVal(int theId, double theVal)
{
	switch (theId)
	{
	case 0:
		mScrollPosition = theVal * mMaxScrollPosition;
		break;
	}
}