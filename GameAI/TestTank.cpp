#include "TestTank.h"
#include "TankManager.h"
#include "Commons.h"
#include "C2DMatrix.h"

//--------------------------------------------------------------------------------------------------

TestTank::TestTank(SDL_Renderer* renderer, TankSetupDetails details, bool demoTank)
	: BaseTank(renderer, details)
{
	mTankTurnDirection = DIRECTION_UNKNOWN;
	mTankTurnKeyDown = false;
	mTankMoveDirection = DIRECTION_NONE;
	mTankMoveKeyDown = false;
	mManTurnDirection = DIRECTION_UNKNOWN;
	mManKeyDown = false;
	mFireKeyDown = false;
	mIsSeeking = false;
	mSteering = new TestTankSteering(this, demoTank);
	mTargetPos = Vector2D(0, 0);//Vector2D(479.5f,100.0f);
	cout << "INSIDE NEW CLASS";

	mTargetSprite = new Texture2D(renderer);
	mTargetSprite->LoadFromFile("Images\\Target.png");
	mDrawTarget = false;
	mHasTarget = false;
	mRenderer = renderer;
	mIsTestTank = demoTank;

	mShowingDebug = false;
}

//--------------------------------------------------------------------------------------------------

TestTank::~TestTank()
{
}

//--------------------------------------------------------------------------------------------------

void TestTank::ChangeState(BASE_TANK_STATE newState)
{
	BaseTank::ChangeState(newState);
}

//--------------------------------------------------------------------------------------------------

void TestTank::Update(float deltaTime, SDL_Event e)
{
	//TODO: Adjust mHeading.
	switch (e.type)
	{
	case SDL_MOUSEMOTION:
		mMouseX = e.motion.x;
		mMouseY = e.motion.y;

		//cout << mMouseX << ":" << mMouseY << endl;
		break;
	case SDL_MOUSEBUTTONDOWN:
		if (e.button.button == SDL_BUTTON_LEFT)
		{
				mTargetPos = Vector2D(e.motion.x, e.motion.y);
				SetHasTarget(true);
				mSteering->TurnArriveOn();
		}
		else
		{
			//cout << "Mouse Down: " << e.motion.x << ":" << e.motion.y << endl;
		}
		break;
	case SDL_KEYDOWN:
		if (e.key.keysym.sym == SDLK_SPACE)
		{
			mDrawTarget = !mDrawTarget;
		}
		else if (e.key.keysym.sym == SDLK_RETURN)
		{
			mShowingDebug = !mShowingDebug;
		}
		else if (e.key.keysym.sym == SDLK_RIGHT)
		{
			RotateHeadingByRadian(0.05, 1);
		}
		break;

	}


	//if (mTanksICanSee.size() >= 1)
	//{
	//	bool isSafe = true;
	//	for (BaseTank* enemy : mTanksICanSee)
	//	{

	//		Vector2D targetDir = enemy->GetPosition() - GetPosition();
	//		float distance = targetDir.Length();

	//		if (distance < 100)
	//		{
	//			cout << "START FLEEING!!!!!" << endl;
	//			mTargetPos = enemy->GetCentrePosition();
	//			mEnemyHeading = enemy->GetHeading();
	//			mEnemySpeed = enemy->GetCurrentSpeed();
	//			mEnemyVelocity = enemy->GetVelocity();
	//			mSteering->TurnFleeOn();
	//			mSteering->TurnArriveOff();

	//			isSafe = false;
	//		}

	//		if (!mIsTestTank)
	//		{
	//			mSteering->TurnPursueOn();
	//		}
	//	}

	//	/*if (isSafe)
	//	mSteering->TurnArriveOn();*/

	//}
	//Call parent update.
	BaseTank::Update(deltaTime, e);
}

//--------------------------------------------------------------------------------------------------

void TestTank::MoveInHeadingDirection(float deltaTime)
{

	mSteeringForce = mSteering->Calculate(deltaTime);


	//cout << mSteeringForce.x << ":" << mSteeringForce.y << endl;
	//Acceleration = Force/Mass
	Vector2D acceleration = mSteeringForce / GetMass();

	//Update velocity.
	mVelocity += acceleration * deltaTime;

	//Don't allow the tank does not go faster than max speed.
	mVelocity.Truncate(GetMaxSpeed());



	if (mVelocity.Length() > 0.0001)
	{
		if (mVelocity.x != mVelocity.x || mVelocity.y != mVelocity.y) return;
		Vector2D newPosition = GetPosition();
		newPosition.x += mVelocity.x*deltaTime;
		newPosition.y += (mVelocity.y)*deltaTime;	//Y flipped as adding to Y moves down screen.
		SetPosition(newPosition);

		Vector2D ahead = Vec2DNormalize(mVelocity);
		if (ahead.Length() == 0)
			ahead = mHeading;

		Vector2D cenPos = GetCentrePosition();
		Vector2D aheadDistance = cenPos + ahead * 30;
		RotateHeadingToFacePosition(aheadDistance);
	}

}



//--------------------------------------------------------------------------------------------------

void TestTank::RotateHeadingByRadian(double radian, int sign)
{

	//Clamp the amount to turn to the max turn rate.
	if (radian > mMaxTurnRate)
		radian = mMaxTurnRate;
	else if (radian < -mMaxTurnRate)
		radian = -mMaxTurnRate;
	//IncrementTankRotationAngle(RadsToDegs(radian));
	mRotationAngle += RadsToDegs(radian*sign);//

											  //Usee a rotation matrix to rotate the player's heading
	C2DMatrix RotationMatrix;

	//Calculate the direction of rotation.
	RotationMatrix.Rotate(radian * sign);
	//Get the new heading.
	//cout << "Before RotateHeadingByRadian -- Heading x = " << mHeading.x << " y = " << mHeading.y << endl;
	RotationMatrix.TransformVector2Ds(mHeading);

	//cout << "After RotateHeadingByRadian -- Heading x = " << mHeading.x << " y = " << mHeading.y << endl;

	//Get the new velocity.
	//RotationMatrix.TransformVector2Ds(mVelocity);

	//Side vector must always be perpendicular to the heading.
	mSide = mHeading.Perp();



}



void TestTank::Flee(float deltaTime)
{
	Vector2D targetDir = mHeading - GetPosition();
	mHeading = Vec2DNormalize(targetDir);

}

void TestTank::Render()
{


	//Call parent render.
	BaseTank::Render();

	//Draw the left cannon.

	if (mDrawTarget)
		mTargetSprite->Render(Vector2D(mTargetPos.x + mTargetSprite->GetWidth() / 2, mTargetPos.y + mTargetSprite->GetHeight() / 2)); //+ 16


																																	  //DrawDebugCircle(Vector2D(GetCentrePosition().x - mHeading.x * 20, GetCentrePosition().y - mHeading.y* 20), 10);

	Vector2D normVel = Vec2DNormalize(mVelocity);

	DrawDebugCircle(Vector2D(GetCentrePosition().x + normVel.x * 20, GetCentrePosition().y + normVel.y * 20), 10);
	if (mShowingDebug)
	{

		for (int i = 50; i > 44; i--)
		{
			DrawDebugCircle(Vector2D(mMouseX, mMouseY), i);
		}
		// drawing the heading vector line
		//Start position will be the front of the tank
		//the End position will be the tanks heading vector 
		SDL_SetRenderDrawColor(mRenderer, 255, 0, 0, 255);
		//heading end = centreposition - mHeading * by some length
		SDL_RenderDrawLine(mRenderer, GetCentrePosition().x, GetCentrePosition().y, GetCentrePosition().x - mHeading.x * 50, GetCentrePosition().y - mHeading.y * 50);

		//Render the Steering Force
		SDL_SetRenderDrawColor(mRenderer, 0, 255, 0, 255);
		SDL_RenderDrawLine(mRenderer, GetCentrePosition().x, GetCentrePosition().y, GetCentrePosition().x + mSteeringForce.x * 5, GetCentrePosition().y + mSteeringForce.y * 5);
	}

	Vector2D ahead = Vec2DNormalize(mVelocity);
	if (ahead.Length() == 0)
		ahead = mHeading;

	Vector2D cenPos = GetCentralPosition();
	Vector2D aheadDistance = cenPos + ahead * 30;
	DrawDebugCircle(Vector2D(aheadDistance.x, aheadDistance.y), 5);


}

void TestTank::DrawDebugCircle(Vector2D centrePoint, float radius)
{
	Vector2D polarVec;
	polarVec.x = 0.0f;
	polarVec.y = radius;

	float stepSize = 0.05f;

	float _360DegAsRads = DegsToRads(360.0);

	while (polarVec.x < _360DegAsRads)
	{
		Vector2D polarAsCart;
		polarAsCart.x = polarVec.y * cosf(polarVec.x);
		polarAsCart.y = polarVec.y * sinf(polarVec.x);

		Vector2D drawPoint;
		drawPoint.x = centrePoint.x + polarAsCart.x;
		drawPoint.y = centrePoint.y + polarAsCart.y;

		SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 255);
		SDL_RenderDrawPoint(mRenderer, drawPoint.x, drawPoint.y);

		polarVec.x += stepSize;
	}

}
//--------------------------------------------------------------------------------------------------