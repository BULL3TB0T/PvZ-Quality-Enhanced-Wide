#ifndef __ALMANACDIALOG_H__
#define __ALMANACDIALOG_H__

#include "LawnDialog.h"
#include "../../ConstEnums.h"
#include "../../SexyAppFramework/SliderListener.h"
#include "../../SexyAppFramework/Slider.h"

constexpr const float			ALMANAC_PLANT_POSITION_X		= 578.0f + BOARD_ADDITIONAL_WIDTH;
constexpr const float			ALMANAC_PLANT_POSITION_Y		= 140.0f + BOARD_OFFSET_Y;
constexpr const float			ALMANAC_ZOMBIE_POSITION_X		= 559.0f + BOARD_ADDITIONAL_WIDTH;
constexpr const float			ALMANAC_ZOMBIE_POSITION_Y		= 175.0f + BOARD_OFFSET_Y;
constexpr const int				ALMANAC_INDEXPLANT_POSITION_X	= 167 + BOARD_ADDITIONAL_WIDTH;
constexpr const int				ALMANAC_INDEXPLANT_POSITION_Y	= 255 + BOARD_OFFSET_Y;
constexpr const float			ALMANAC_INDEXZOMBIE_POSITION_X	= 535.0f + BOARD_ADDITIONAL_WIDTH;
constexpr const float			ALMANAC_INDEXZOMBIE_POSITION_Y	= 215.0f + BOARD_OFFSET_Y;
constexpr const int				ALMANAC_LEVEL_BUTTON_OFFSET_Y = 3 + BOARD_OFFSET_Y;
constexpr const int				ALMANAC_LEVEL_BUTTON_HEIGHT = 48 + BOARD_ADDITIONAL_WIDTH;

class Plant;
class Zombie;
class LawnApp;
class GameButton;
class Reanimation;
class Slider;
class AlmanacDialog : public LawnDialog, public Sexy::SliderListener
{
private:
	enum
	{
		ALMANAC_BUTTON_CLOSE,
		ALMANAC_BUTTON_PLANT,
		ALMANAC_BUTTON_ZOMBIE,
		ALMANAC_BUTTON_INDEX,
		ALMANAC_SLIDER,
		ALMANAC_BUTTON_NEXT,
		ALMANAC_BUTTON_LAST
	};

public:
	LawnApp*					mApp;					//+0x16C
	GameButton*					mCloseButton;			//+0x170
	GameButton*					mIndexButton;			//+0x174
	GameButton*					mPlantButton;			//+0x178
	GameButton*					mZombieButton;			//+0x17C
	Sexy::Slider* mSlider;
	AlmanacPage					mOpenPage;				//+0x180
	Reanimation*				mReanim[4];				//+0x184
	SeedType					mSelectedSeed;			//+0x194
	ZombieType					mSelectedZombie;		//+0x198
	Plant*						mPlant;					//+0x19C
	Zombie*						mZombie;				//+0x1A0
	Zombie*						mZombiePerfTest[400];	//+0x1A4
	float						mIncrement;
	float						mScrollPosition;
	float						mScrollAmount;
	const float					mBaseScrollSpeed = 1.0f;
	const float					mScrollAccel = 0.1f;
	float						mMaxScrollPosition;
	int							mLastMouseX;
	int							mLastMouseY;
	
public:
	AlmanacDialog(LawnApp* theApp);
	virtual ~AlmanacDialog();

	void						ClearObjects();
	virtual void				RemovedFromManager(WidgetManager* theWidgetManager);
	virtual void				AddedToManager(WidgetManager* theWidgetManager);
	void                        SliderVal(int theId, double theVal);
	void						SetupPlant();
	void						SetupZombie();
	void						SetPage(AlmanacPage thePage);
	virtual void				Update();
	void						DrawIndex(Graphics* g);
	void						DrawPlants(Graphics* g);
	void						DrawZombies(Graphics* g);
	virtual void				Draw(Graphics* g);
	void						GetSeedPosition(SeedType theSeedType, int& x, int& y, bool specialSpot);
	SeedType					SeedHitTest(int x, int y);
	/*inline*/ int				ZombieHasSilhouette(ZombieType theZombieType);
	int						ZombieIsShown(ZombieType theZombieType);
	int						ZombieHasDescription(ZombieType theZombieType);
	void						GetZombiePosition(ZombieType theZombieType, int& x, int& y);
	ZombieType					ZombieHitTest(int x, int y);
	virtual void				MouseUp(int x, int y, int theClickCount);
	virtual void				MouseDown(int x, int y, int theClickCount);
	virtual void				KeyChar(char theChar) {  }

	static ZombieType			GetZombieType(int theIndex);
	/*inline*/ void				ShowPlant(SeedType theSeedType);
	/*inline*/ void				ShowZombie(ZombieType theZombieType);
	virtual void				MouseWheel(int theDelta);
};
extern int gZombieDefeated[NUM_ZOMBIE_TYPES];

/*inline*/ void					AlmanacInitForPlayer();
/*inline*/ void					AlmanacPlayerDefeatedZombie(ZombieType theZombieType);

#endif
