#ifndef S013010C_AARON_SMITH_TANK_H
#define S013010C_AARON_SMITH_TANK_H

#include "BaseTank.h"
#include <SDL.h>
#include "Commons.h"
#include "S013010C_Aaron_Smith_Steering.h"

//---------------------------------------------------------------

class S013010C_Aaron_Smith_Tank : public BaseTank
{

	
	//---------------------------------------------------------------
public:
	S013010C_Aaron_Smith_Tank(SDL_Renderer* renderer, TankSetupDetails details, bool demoTank);
	~S013010C_Aaron_Smith_Tank();

	void ChangeState(BASE_TANK_STATE newState);

	void Update(float deltaTime, SDL_Event e);
	void Render();

	void RotateHeadingByRadian(double radian, int sign);
	bool RotateHeadingToTarget(Vector2D targetPos);
	//Vector2D GetPosition() { return GetCentrePosition(); }
	Vector2D GetTarget() { return mTargetPos; }
	double	 GetMaxSpeed() { return mMaxSpeed; }
	Vector2D GetVelocity() { return mVelocity; }
	void	 SetHasTarget(bool val) { mHasTarget = val; }
	Vector2D	GetEnemyHeading() { return mEnemyHeading; }
	double	GetEnemySpeed() { return mEnemySpeed; }
	Vector2D GetEnemyVelocity() { return mEnemyVelocity; }
	Vector2D GetLeftAhead1() { return mLeftAhead1Distance; }
	Vector2D GetLeftAhead2() { return mLeftAhead2Distance; }
	Vector2D GetRightAhead1() { return mRightAhead1Distance; }
	Vector2D GetRightAhead2() { return mRightAhead2Distance; }
	Vector2D GetAhead1() { return mAhead1Distance; }
	Vector2D GetAhead2() { return mAhead2Distance; }

	vector<Vector2D> mLeftFeelers;
	vector<Vector2D> mRightFeelers;
	vector<Vector2D> mStraightFeelers;
	vector<Vector2D> mRightSideFeelers;
	vector<Vector2D> mLeftSideFeelers;
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
	S013010C_Aaron_Smith_Steering *mSteering;
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
	Vector2D		mLeftAhead2Distance;
	Vector2D		mLeftAhead1Distance;
	Vector2D		mRightAhead2Distance;
	Vector2D		mRightAhead1Distance;
	Vector2D		mAhead1Distance;
	Vector2D		mAhead2Distance;

	Vector2D		mTopLeftCorner;
	Vector2D		mTopRightCorner;
	
	void DrawDebugCircle(Vector2D centrePoint, float radius);
	void CreateLeftFeelers(vector<Vector2D> &leftFeelers);
	void CreateRightFeelers(vector<Vector2D> &rightFeelers);
	void CreateStraightFeelers(vector<Vector2D> &straightFeelers);
	void CreateFeelers(vector<Vector2D> &sideFeelers, double angle, int distance, Vector2D startPos);

	bool demoPursuit;
	Rect2D		GetAdjustedBoundingBox();
};

//---------------------------------------------------------------

#endif //S013010C_AARON_SMITH_TANK_H
