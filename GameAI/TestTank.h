#ifndef TestTank_H
#define TestTank_H

#include "BaseTank.h"
#include <SDL.h>
#include "Commons.h"
#include "TestTankSteering.h"

//---------------------------------------------------------------

class TestTank : public BaseTank
{


	//---------------------------------------------------------------
public:
	TestTank(SDL_Renderer* renderer, TankSetupDetails details, bool demoTank);
	~TestTank();

	void ChangeState(BASE_TANK_STATE newState);

	void Update(float deltaTime, SDL_Event e);
	void Render();

	void RotateHeadingByRadian(double radian, int sign);

	//Vector2D GetPosition() { return GetCentrePosition(); }
	Vector2D GetTarget() { return mTargetPos; }
	double	 GetMaxSpeed() { return mMaxSpeed; }
	Vector2D GetVelocity() { return mVelocity; }
	void	 SetHasTarget(bool val) { mHasTarget = val; }
	Vector2D	GetEnemyHeading() { return mEnemyHeading; }
	double	GetEnemySpeed() { return mEnemySpeed; }
	Vector2D GetEnemyVelocity() { return mEnemyVelocity; }
	//---------------------------------------------------------------
protected:
	void	MoveInHeadingDirection(float deltaTime);
	void	Seek(float deltaTime);
	void	Flee(float deltaTime);
private:
	TURN_DIRECTION  mTankTurnDirection;
	bool			mTankTurnKeyDown;
	MOVE_DIRECTION  mTankMoveDirection;
	bool			mTankMoveKeyDown;
	TURN_DIRECTION  mManTurnDirection;
	bool			mManKeyDown;
	bool			mFireKeyDown;
	bool			mIsSeeking;
	TestTankSteering *mSteering;
	Vector2D		mSteeringForce;
	Vector2D		mTargetPos;
	Texture2D*			mTargetSprite;					//The target.
	bool			mDrawTarget;
	bool			mHasTarget;
	SDL_Renderer*	mRenderer;
	float			mMouseX, mMouseY;
	vector<Vector2D>mMouseRadiusPoints;
	bool			mShowingDebug;
	Vector2D		mEnemyHeading;
	double			mEnemySpeed;
	Vector2D		mEnemyVelocity;

	bool			mIsTestTank;

	void DrawDebugCircle(Vector2D centrePoint, float radius);
};

//---------------------------------------------------------------

#endif //S013010C_AARON_SMITH_TANK_H
