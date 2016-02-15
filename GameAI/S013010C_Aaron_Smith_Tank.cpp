#include "S013010C_Aaron_Smith_Tank.h"
#include "TankManager.h"
#include "Commons.h"
#include "C2DMatrix.h"

//--------------------------------------------------------------------------------------------------

S013010C_Aaron_Smith_Tank::S013010C_Aaron_Smith_Tank(SDL_Renderer* renderer, TankSetupDetails details,bool demoTank)
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
	mSteering = new S013010C_Aaron_Smith_Steering(this,demoTank);
	mTargetPos = Vector2D(0, 0);//Vector2D(479.5f,100.0f);
	

	mTargetSprite = new Texture2D(renderer);
	mTargetSprite->LoadFromFile("Images\\Target.png");
	mDrawTarget = false;
	mHasTarget = false;
	mRenderer = renderer;
	mIsTestTank = demoTank;

	mShowingDebug = false;
	mLeftFeelers = vector<Vector2D>();
	mRightFeelers = vector<Vector2D>();
	mStraightFeelers = vector<Vector2D>();
	mLeftSideFeelers = vector<Vector2D>();
	mRightSideFeelers = vector<Vector2D>();
	/*CreateLeftFeelers(mLeftFeelers);
	CreateRightFeelers(mRightFeelers);
	CreateStraightFeelers(mStraightFeelers);*/
	CreateFeelers(mRightSideFeelers, 90,	60, GetCentralPosition());
	CreateFeelers(mLeftSideFeelers, -90,	60, GetCentralPosition());
	CreateFeelers(mLeftFeelers,		-30,	80, mTopLeftCorner);
	CreateFeelers(mRightFeelers,	 30,	80, mTopRightCorner);
	CreateFeelers(mStraightFeelers,  0,		70, mTopLeftCorner);


	demoPursuit = false;
	cout << "1. Arrive" << endl;
	cout << "2. Seek" << endl;
	cout << "3. Flee" << endl;
	cout << "4. Pursuit" << endl;
}

//--------------------------------------------------------------------------------------------------

S013010C_Aaron_Smith_Tank::~S013010C_Aaron_Smith_Tank()
{
}

//--------------------------------------------------------------------------------------------------

void S013010C_Aaron_Smith_Tank::ChangeState(BASE_TANK_STATE newState)
{
	BaseTank::ChangeState(newState);
}

//--------------------------------------------------------------------------------------------------

void S013010C_Aaron_Smith_Tank::Update(float deltaTime, SDL_Event e)
{
	//TODO: Adjust mHeading.
	switch (e.type)
	{
	case SDL_MOUSEMOTION:
		mMouseX = e.motion.x;
		mMouseY = e.motion.y;

		if (demoPursuit)
		{
			Vector2D prevPosition;
			prevPosition = mTargetPos;
			mTargetPos = Vector2D(e.motion.x , e.motion.y);

			Vector2D enemySteering = (Vec2DNormalize(mTargetPos - prevPosition )* 75) - mEnemyVelocity ;
			//mEnemyVelocity = mEnemyHeading;

			//Acceleration = Force/Mass
			//mSteeringForce.Truncate(GetMaxForce());
			Vector2D acceleration = enemySteering / GetMass();

			//Update velocity.
			mEnemyVelocity += acceleration * deltaTime;
			mEnemySpeed = mEnemyVelocity.Length();
			//Don't allow the tank does not go faster than max speed.
			mEnemyVelocity.Truncate(GetMaxSpeed());
			mEnemyHeading = Vec2DNormalize(mEnemyVelocity);
			cout << "enemySteering " << enemySteering.x << ":" << enemySteering.y << endl;

		}
		//cout << mMouseX << ":" << mMouseY << endl;
		break;
	case SDL_MOUSEBUTTONDOWN:
		if (e.button.button == SDL_BUTTON_LEFT)
		{
			mTargetPos = Vector2D(e.motion.x, e.motion.y);
			//SetHasTarget(true);
		}
		else
		{
			cout << "Mouse Down: " << e.motion.x << ":" << e.motion.y << endl;
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
		else if (e.key.keysym.sym == SDLK_1)
		{
			demoPursuit = false;
			cout << "Arrive is on" << endl;
			mSteering->TurnArriveOn();
			mSteering->TurnSeekOff();
			mSteering->TurnFleeOff();
			mSteering->TurnPursueOff();
		}
		else if (e.key.keysym.sym == SDLK_2)
		{
			demoPursuit = false;
			cout << "Seek is on" << endl;
			mSteering->TurnSeekOn();
			mSteering->TurnArriveOff();
			mSteering->TurnFleeOff();
			mSteering->TurnPursueOff();
		}
		else if (e.key.keysym.sym == SDLK_3)
		{
			demoPursuit = false;
			cout << "Flee is on" << endl;
			mSteering->TurnFleeOn();
			mSteering->TurnArriveOff();
			mSteering->TurnSeekOff();
			mSteering->TurnPursueOff();
		}
		else if (e.key.keysym.sym == SDLK_4)
		{
			cout << "Pursuit is on" << endl;
			demoPursuit = true;
			mSteering->TurnPursueOn();
			mSteering->TurnArriveOff();
			mSteering->TurnFleeOff();
			mSteering->TurnSeekOff();
		}
		break;
		
	}

	
	if (mTanksICanSee.size() >= 1)
	{
		bool isSafe = true;
		for (BaseTank* enemy : mTanksICanSee)
		{
			
			Vector2D targetDir = enemy->GetPosition() - GetPosition();
			float distance = targetDir.Length();

			if (distance < 100)
			{
				cout << "START FLEEING!!!!!" << endl;
				mTargetPos = enemy->GetCentrePosition();
				mEnemyHeading = enemy->GetHeading();
				mEnemySpeed = enemy->GetCurrentSpeed();
				mEnemyVelocity = enemy->GetVelocity();
				mSteering->TurnEvadeOn();
				
				isSafe = false;
			}

			if (!mIsTestTank)
			{
				mTargetPos = enemy->GetCentrePosition();
				mEnemyHeading = enemy->GetHeading();
				mEnemySpeed = enemy->GetCurrentSpeed();
				mEnemyVelocity = enemy->GetVelocity();
				mSteering->TurnPursueOn();
			}
		}
	

		/*if (isSafe)
			mSteering->TurnArriveOn();*/
		
	}
	else
	{
		//cout << "NO LONGER IN PURSUIT" << endl;
		//if(mSteering->mIsPursuing)mSteering->TurnEvadeOff();
	}
	//Call parent update.
	BaseTank::Update(deltaTime, e);
}

//--------------------------------------------------------------------------------------------------

void S013010C_Aaron_Smith_Tank::MoveInHeadingDirection(float deltaTime)
{
	
	mSteeringForce = mSteering->Calculate(deltaTime);
	//Acceleration = Force/Mass
	//mSteeringForce.Truncate(GetMaxForce());
	Vector2D acceleration = mSteeringForce / GetMass();
	
	//Update velocity.
	mVelocity += acceleration * deltaTime;

	//Don't allow the tank does not go faster than max speed.
	mVelocity.Truncate(GetMaxSpeed());
	//cout << "mVelocity " << mVelocity.x << ":" << mVelocity.y << endl;
	//cout << "VELOCITY " << mVelocity.Length() << endl;

	
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
		Vector2D aheadDistance = cenPos + ahead * 10 ;
		
		RotateHeadingToTarget(aheadDistance);

	//	double dynamicLength = mVelocity.Length() / GetMaxSpeed();
		

		Vector2D left = (mHeading - mSide) * 0.5;
		Vector2D right = (mHeading + mSide) * 0.5;

		mTopLeftCorner.x = left.x;
		mTopLeftCorner.y = left.y;
		mTopLeftCorner = Vec2DNormalize(mTopLeftCorner) * -10;
		mTopLeftCorner += GetCentralPosition();

		mTopRightCorner.x = right.x;
		mTopRightCorner.y = right.y;
		mTopRightCorner = Vec2DNormalize(mTopRightCorner) * -10;
		mTopRightCorner += GetCentralPosition();



		/*double angle = 70;
		int ahead2Amount = 40;
		int ahead1Amount = ahead2Amount / 2;
		int side2Amount = 30;
		int side1Amount = side2Amount / 2;

		Vector2D fannedLeftAhead;
		fannedLeftAhead.x = ahead.x * cos(DegsToRads(-angle)) - ahead.y * sin(DegsToRads(-angle));
		fannedLeftAhead.y = ahead.x * sin(DegsToRads(-angle)) + ahead.y * cos(DegsToRads(-angle));
		Vector2D fannedRightAhead;
		fannedRightAhead.x = ahead.x * cos(DegsToRads(angle)) - ahead.y * sin(DegsToRads(angle));
		fannedRightAhead.y = ahead.x * sin(DegsToRads(angle)) + ahead.y * cos(DegsToRads(angle));

		mLeftAhead2Distance = (mTopLeftCorner) + (fannedLeftAhead *  dynamicLength * side2Amount);
		mLeftAhead1Distance = (mTopLeftCorner) + (fannedLeftAhead *  dynamicLength * side1Amount);
		
		mRightAhead2Distance = (mTopRightCorner)+(fannedRightAhead  *  dynamicLength * side2Amount);
		mRightAhead1Distance = (mTopRightCorner)+(fannedRightAhead  *  dynamicLength * side1Amount);

		mAhead1Distance = GetCentrePosition() + (ahead * dynamicLength * ahead1Amount );
		mAhead2Distance = GetCentrePosition() + (ahead * dynamicLength * (ahead2Amount + 50));*/
		//cout << "AHEAD " << mLeftAhead2Distance.x << ":" << mLeftAhead2Distance.y << endl;
		//mAhead1Distance = mAhead2Distance * 0.5;
	}

	CreateFeelers(mRightSideFeelers, 90, 30, GetCentralPosition());
	CreateFeelers(mLeftSideFeelers, -90, 30, GetCentralPosition());
	CreateFeelers(mLeftFeelers, -40, 30, mTopLeftCorner);
	CreateFeelers(mRightFeelers, 40, 30, mTopRightCorner); // 70
	CreateFeelers(mStraightFeelers, 0, 40, mTopLeftCorner);
}



//--------------------------------------------------------------------------------------------------

bool S013010C_Aaron_Smith_Tank::RotateHeadingToTarget(Vector2D targetPos)
{
	Vector2D toTarget = Vec2DNormalize(GetCentralPosition() - targetPos);

	//Determine the angle between the heading vector and the target.

	double angle = atan2(toTarget.y - mHeading.y,toTarget.x - mHeading.x);

	
	//cout << angle << endl;
	angle *= (180 / Pi);
	//Return true if the player is facing the target.
	/*if (angle < -0.007)
		return true;*/

	if (angle < 0)
	{
		angle = 360 - (-angle);
	}

	RotateHeadingByRadian(angle, mHeading.Sign(toTarget));

	return true;
}

void S013010C_Aaron_Smith_Tank::RotateHeadingByRadian(double radian, int sign)
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



void S013010C_Aaron_Smith_Tank::Flee(float deltaTime)
{
	Vector2D targetDir = mHeading - GetPosition() ;
	mHeading = Vec2DNormalize(targetDir);

}

void S013010C_Aaron_Smith_Tank::Render()
{
	

	//Call parent render.
	BaseTank::Render();

	//Draw the left cannon.
	
	if(mDrawTarget)
		mTargetSprite->Render(Vector2D(mTargetPos.x + mTargetSprite->GetWidth()/2  , mTargetPos.y + mTargetSprite->GetHeight() / 2)); //+ 16
	
																
	//DrawDebugCircle(Vector2D(GetCentrePosition().x - mHeading.x * 20, GetCentrePosition().y - mHeading.y* 20), 10);

	Vector2D normVel = Vec2DNormalize(mVelocity);
	
	//DrawDebugCircle(Vector2D(GetCentrePosition().x + normVel.x *20 , GetCentrePosition().y + normVel.y * 20 ), 10);
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


	SDL_Rect* boundingRect = new SDL_Rect();
	boundingRect->h = GetAdjustedBoundingBox().height;
	boundingRect->w = GetAdjustedBoundingBox().width;
	boundingRect->x = GetAdjustedBoundingBox().x;
	boundingRect->y = GetAdjustedBoundingBox().y;
	SDL_RenderDrawRect(mRenderer, boundingRect);
	
	

	SDL_SetRenderDrawColor(mRenderer, 0, 255, 0, 255);

	for (Vector2D feelerPoint : mLeftFeelers)
	{
		DrawDebugCircle(feelerPoint, 5);
	}
	for (Vector2D feelerPoint : mRightFeelers)
	{
		DrawDebugCircle(feelerPoint, 5);
	}
	for (Vector2D feelerPoint : mStraightFeelers)
	{
		DrawDebugCircle(feelerPoint, 5);
	}
	for (Vector2D feelerPoint : mLeftSideFeelers)
	{
		DrawDebugCircle(feelerPoint, 5);
	}

	SDL_SetRenderDrawColor(mRenderer, 255, 0, 0, 255);
	for (Vector2D feelerPoint : mRightSideFeelers)
	{
		DrawDebugCircle(feelerPoint, 5);
	}
	/*DrawDebugCircle(mLeftAhead2Distance, 5);
	DrawDebugCircle(mAhead2Distance, 5);
	DrawDebugCircle(mRightAhead2Distance, 5);


	SDL_SetRenderDrawColor(mRenderer, 255, 0, 0, 255);
	DrawDebugCircle(mLeftAhead1Distance, 5);
	DrawDebugCircle(mAhead1Distance, 5);
	DrawDebugCircle(mRightAhead1Distance, 5);*/


	vector<GameObject * > obs = ObstacleManager::Instance()->GetObstacles();
	for (GameObject* obj : obs)
	{
		double radius = obj->GetCollisionRadius();

		for (auto i = radius; i > radius - 5; i--)
		{
			SDL_Rect* rect = new SDL_Rect();
			rect->h = obj->GetAdjustedBoundingBox().height;
			rect->w = obj->GetAdjustedBoundingBox().width;
			rect->x = obj->GetAdjustedBoundingBox().x;
			rect->y = obj->GetAdjustedBoundingBox().y;

			//DrawDebugCircle(obj->GetCentralPosition(), i);
			SDL_RenderDrawRect(mRenderer, rect);
		}
		
	}

	
	
}

void S013010C_Aaron_Smith_Tank::DrawDebugCircle(Vector2D centrePoint, float radius)
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

void S013010C_Aaron_Smith_Tank::CreateStraightFeelers(vector<Vector2D> &feelers)
{
	feelers.clear();

	Vector2D ahead = Vec2DNormalize(mVelocity);
	if (ahead.Length() == 0)
		ahead = mHeading;

	double dynamicLength = mVelocity.Length() / GetMaxSpeed();
	double angle = 70;
	int ahead2Amount = 50;
	int ahead1Amount = ahead2Amount / 2;

	mAhead1Distance = GetCentrePosition() + (ahead * dynamicLength * ahead1Amount);
	mAhead2Distance = GetCentrePosition() + (ahead * dynamicLength * (ahead2Amount + 50));

	feelers.push_back(mAhead1Distance);
	feelers.push_back(mAhead2Distance);
}

void S013010C_Aaron_Smith_Tank::CreateRightFeelers(vector<Vector2D> &feelers)
{
	feelers.clear();

	Vector2D ahead = Vec2DNormalize(mVelocity);
	if (ahead.Length() == 0)
		ahead = mHeading;

	double dynamicLength = mVelocity.Length() / GetMaxSpeed();
	double angle = 40;
	int side2Amount = 30;
	int side1Amount = side2Amount / 2;

	Vector2D fannedRightAhead;
	fannedRightAhead.x = ahead.x * cos(DegsToRads(angle)) - ahead.y * sin(DegsToRads(angle));
	fannedRightAhead.y = ahead.x * sin(DegsToRads(angle)) + ahead.y * cos(DegsToRads(angle));

	mRightAhead2Distance = (mTopRightCorner)+(fannedRightAhead  *  dynamicLength * side2Amount);
	mRightAhead1Distance = (mTopRightCorner)+(fannedRightAhead  *  dynamicLength * side1Amount);

	feelers.push_back(mRightAhead1Distance);
	feelers.push_back(mRightAhead2Distance);

}

void S013010C_Aaron_Smith_Tank::CreateLeftFeelers(vector<Vector2D> &feelers)
{
	feelers.clear();

	Vector2D ahead = Vec2DNormalize(mVelocity);
	if (ahead.Length() == 0)
		ahead = mHeading;

	double dynamicLength = mVelocity.Length() / GetMaxSpeed();
	double angle = 40;
	int side2Amount = 30;
	int side1Amount = side2Amount / 2;

	Vector2D fannedLeftAhead;
	fannedLeftAhead.x = ahead.x * cos(DegsToRads(-angle)) - ahead.y * sin(DegsToRads(-angle));
	fannedLeftAhead.y = ahead.x * sin(DegsToRads(-angle)) + ahead.y * cos(DegsToRads(-angle));


	mLeftAhead2Distance = (mTopLeftCorner)+(fannedLeftAhead *  dynamicLength * side2Amount);
	mLeftAhead1Distance = (mTopLeftCorner)+(fannedLeftAhead *  dynamicLength * side1Amount);

	feelers.push_back(mLeftAhead1Distance);
	feelers.push_back(mLeftAhead2Distance);

}

void S013010C_Aaron_Smith_Tank::CreateFeelers(vector<Vector2D> &feelers, double angle, int distance,Vector2D startPos)
{
	feelers.clear();

	Vector2D ahead = Vec2DNormalize(mVelocity);
	if (ahead.Length() == 0)
		ahead = mHeading;

	double dynamicLength = mVelocity.Length() / GetMaxSpeed();
	if (dynamicLength < 0.6) dynamicLength = 0.6;
	cout << dynamicLength << endl;

	int dist2Amount = distance;
	int dist1Amount = dist2Amount / 2;
	Vector2D feelerVect1;
	Vector2D feelerVect2;
	if (angle != 0)
	{
		Vector2D fannedAhead;
		fannedAhead.x = ahead.x * cos(DegsToRads(angle)) - ahead.y * sin(DegsToRads(angle));
		fannedAhead.y = ahead.x * sin(DegsToRads(angle)) + ahead.y * cos(DegsToRads(angle));

		feelerVect2 = (startPos)+(fannedAhead *  dynamicLength * dist2Amount);
		feelerVect1 = (startPos)+(fannedAhead *  dynamicLength * dist1Amount);
	}
	else
	{
		feelerVect1 = GetCentrePosition() + (ahead * dynamicLength * dist1Amount);
		feelerVect2 = GetCentrePosition() + (ahead * dynamicLength * (dist2Amount + 50));
	}
	feelers.push_back(feelerVect1);
	feelers.push_back(feelerVect2);

}

Rect2D S013010C_Aaron_Smith_Tank::GetAdjustedBoundingBox()
{
	return BaseTank::GetAdjustedBoundingBox();
}