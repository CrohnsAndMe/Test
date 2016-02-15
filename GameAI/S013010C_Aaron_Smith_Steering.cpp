#include "S013010C_Aaron_Smith_Steering.h"
#include "S013010C_Aaron_Smith_Tank.h"

S013010C_Aaron_Smith_Steering::S013010C_Aaron_Smith_Steering()
{
	mTank = nullptr;
	cout << "No Tank set In Steering!!!!" << endl;
}

S013010C_Aaron_Smith_Steering::S013010C_Aaron_Smith_Steering(S013010C_Aaron_Smith_Tank* tank,bool demoTank)
{
	mTank = tank;

	mIsArriving = false;
	mIsPursuing = false;
	mIsEvading = false;
	mIsFleeing = false;
	mIsWandering = false;
	mIsSeeking = false;
	mIsPursuing = false;
	mIsObstacleAvoiding = true;
	mIsWallAvoided = true;

	//add walls
	walls = vector<Walls>();
	Walls leftWall = Walls(Vector2D(0, 0), Vector2D(0, kScreenHeight));
	Walls rightWall = Walls(Vector2D(kScreenWidth, 0), Vector2D(kScreenWidth, kScreenHeight));
	Walls bottomWall = Walls(Vector2D(0, kScreenHeight), Vector2D(kScreenWidth, kScreenHeight));
	Walls topWall = Walls(Vector2D(0, 0), Vector2D(kScreenWidth, 0));
	walls.push_back(leftWall);
	walls.push_back(rightWall);
	walls.push_back(bottomWall);
	walls.push_back(topWall);
}



S013010C_Aaron_Smith_Steering::~S013010C_Aaron_Smith_Steering()
{

}

Vector2D S013010C_Aaron_Smith_Steering::Seek( Vector2D targetPos)
{
	//cout << "SEEKING" << endl;
	Vector2D reqVelocity = Vec2DNormalize(targetPos - mTank->GetCentrePosition() ) * mTank->GetMaxSpeed();
	reqVelocity -= mTank->GetVelocity();
	/*mSteeringForces.push_back(reqVelocity);*/
	return reqVelocity;
}

Vector2D S013010C_Aaron_Smith_Steering::Flee(Vector2D targetPos)
{
	Vector2D reqVelocity = Vec2DNormalize(mTank->GetCentrePosition()- targetPos ) * mTank->GetMaxSpeed();
	reqVelocity -= mTank->GetVelocity();
	return reqVelocity;
}

Vector2D S013010C_Aaron_Smith_Steering::Arrive(Vector2D targetPos, float deceleration)
{
	Vector2D targetDir = targetPos - mTank->GetCentrePosition();
	double distance = targetDir.Length();
	//cout << "Distance: " << distance << endl;
	if (distance > 50)
		TurnSeekOn();
		/*Seek(targetPos);*/
	else 
	{
		if (mIsSeeking) TurnSeekOff();
		//cout << "MOVING" << endl;
		//because Deceleration is enumerated as an int, this value is required
		//to provide fine tweaking of the deceleration..
		const double DecelerationTweaker = .3;

		//calculate the speed required to reach the target given the desired
		//deceleration
		double speed = (distance) / (double)deceleration * ( DecelerationTweaker);
		//
		//make sure the velocity does not exceed the max
		speed = min(speed, mTank->GetMaxSpeed());

		//from here proceed just like Seek except we don't need to normalize 
		//the ToTarget vector because we have already gone to the trouble
		//of calculating its length: dist. 
		//Vector2D reqVelocity = targetDir /  distance;
		Vector2D reqVelocity = targetDir / distance;
		//Vec2DNormalize(targetDir * speed / distance);
	
		return (reqVelocity - mTank->GetVelocity());
		//mSteeringForces.push_back(reqVelocity - mTank->GetVelocity());
	}
	//else
	//	cout << "STOPPED" << endl;
	
}



Vector2D S013010C_Aaron_Smith_Steering::Wander(float deltaTime)
{
	
	float wanderCircleRadius = 11;

	Vector2D normVel = Vec2DNormalize( mTank->GetVelocity());
	Vector2D circleInFront = Vector2D(mTank->GetCentrePosition().x + normVel.x * wanderCircleRadius,
		mTank->GetCentrePosition().y + normVel.y * wanderCircleRadius);
	
	Vector2D randPos = Vector2D((RandFloat() - RandFloat())  , (RandFloat() - RandFloat()) );
	mWanderTarget += randPos;
	mWanderTarget = Vec2DNormalize(mWanderTarget);
	mWanderTarget *= wanderCircleRadius;
	Vector2D newForce = circleInFront + mWanderTarget;

	Vector2D reqVelocity = Vec2DNormalize(newForce - mTank->GetCentrePosition()) * mTank->GetMaxSpeed();
	return reqVelocity -= mTank->GetVelocity();

}

Vector2D S013010C_Aaron_Smith_Steering::Pursuing()
{
	
	Vector2D enemyTankPos = mTank->GetTarget();

	double relativeHeading = mTank->GetHeading().Dot(mTank->GetEnemyHeading());

	if (enemyTankPos.Dot(mTank->GetHeading()) > 0 &&
		(relativeHeading < -0.95))
		return Seek(enemyTankPos);
	else
	{
		//not ahead of player so need to predict enemy position
		
		double lookAheadTime = enemyTankPos.Length() / (mTank->GetMaxSpeed() + mTank->GetEnemySpeed());
		return Seek(enemyTankPos + mTank->GetEnemyVelocity() * lookAheadTime);
	}

}

void S013010C_Aaron_Smith_Steering::Evade()
{
	Vector2D TargetToEvade = mTank->GetTarget() - mTank->GetCentrePosition();

	double lookAheadTime = TargetToEvade.Length() / (mTank->GetMaxSpeed() + mTank->GetEnemySpeed());
	Flee(mTank->GetTarget() + mTank->GetEnemyVelocity() * lookAheadTime);

}

Vector2D S013010C_Aaron_Smith_Steering::ObstacleAvoidance()
{
	bool safe = true;
	//Vector2D leftAhead1 = mTank->GetLeftAhead1();
	//Vector2D leftAhead2 = mTank->GetLeftAhead2();

	//Vector2D RightAhead1 = mTank->GetRightAhead1();
	//Vector2D RightAhead2 = mTank->GetRightAhead2();

	//Vector2D Ahead1 = mTank->GetAhead1();
	//Vector2D Ahead2 = mTank->GetAhead2();

	Vector2D leftAhead1 = mTank->mLeftFeelers[0];
	Vector2D leftAhead2 = mTank->mLeftFeelers[1];

	Vector2D RightAhead1 = mTank->mRightFeelers[0];
	Vector2D RightAhead2 = mTank->mRightFeelers[1];

	Vector2D Ahead1 = mTank->mStraightFeelers[0];
	Vector2D Ahead2 = mTank->mStraightFeelers[1];

	Vector2D rightSide1 = mTank->mRightSideFeelers[0];
	Vector2D rightSide2 = mTank->mRightSideFeelers[1];

	Vector2D leftSide1 = mTank->mLeftSideFeelers[0];
	Vector2D leftSide2 = mTank->mLeftSideFeelers[1];

	double maxAvoidForce = 500;
	//now find most threatening obstacle
	/*GameObject* closestObj = GetClosestObject(leftAhead1, leftAhead2);*/
	
	//check left feeler
	//check straight feeler
	//check right feeler
	GameObject* closestLeftObj = GetClosestObject(leftAhead1, leftAhead2);
	GameObject* closestStraightObj = GetClosestObject(Ahead1, Ahead2);
	GameObject* closestRightObj = GetClosestObject(RightAhead1, RightAhead2);
	GameObject* closestLeftSideObj = GetClosestObject(leftSide1, leftSide2);
	GameObject* closestRightSideObj = GetClosestObject(rightSide1, rightSide2);
	/*
	then compare the distance of them to find closest one
	*/

	double leftDistance = 999;
	double straightDistance = 999;
	double rightDistance = 999;
	double leftSideDistance = 999;
	double rightSideDistance = 999;
	
	if(closestLeftObj != nullptr)
		leftDistance = GetDistance(closestLeftObj->GetCentralPosition(), mTank->GetCentrePosition());
	if (closestStraightObj != nullptr)
		straightDistance = GetDistance(closestStraightObj->GetCentralPosition(), mTank->GetCentrePosition());
	if (closestRightObj != nullptr)
		rightDistance = GetDistance(closestRightObj->GetCentralPosition(), mTank->GetCentrePosition());
	if (closestLeftSideObj != nullptr)
		leftSideDistance = GetDistance(closestLeftSideObj->GetCentralPosition(), mTank->GetCentrePosition());
	if (closestRightSideObj != nullptr)
		rightSideDistance = GetDistance(closestRightSideObj->GetCentralPosition(), mTank->GetCentrePosition());

	Vector2D avoidForce;
	Vector2D totalForce = Vector2D(0,0);
	if (closestLeftObj != nullptr)
	{
		
		avoidForce = leftAhead1 - closestLeftObj->GetCentralPosition();
		avoidForce = Vec2DNormalize(avoidForce);
		avoidForce *= maxAvoidForce;
		totalForce += avoidForce;
	}

	if (closestRightObj != nullptr)
	{
		avoidForce = RightAhead1 - closestRightObj->GetCentralPosition();
		avoidForce = Vec2DNormalize(avoidForce);
		avoidForce *= maxAvoidForce;
		totalForce += avoidForce;
	}

	if (closestStraightObj != nullptr)
	{
		avoidForce = Ahead1 - closestStraightObj->GetCentralPosition();
		avoidForce = Vec2DNormalize(avoidForce);
		avoidForce *= maxAvoidForce;
		totalForce += avoidForce;
	}
	if (closestLeftSideObj != nullptr)
	{
		avoidForce = leftSide1 - closestLeftSideObj->GetCentralPosition();
		avoidForce = Vec2DNormalize(avoidForce);
		avoidForce *= maxAvoidForce;
		totalForce += avoidForce;
	}
	if (closestRightSideObj != nullptr)
	{
		avoidForce = rightSide1 - closestRightSideObj->GetCentralPosition();
		avoidForce = Vec2DNormalize(avoidForce);
		avoidForce *= maxAvoidForce;
		totalForce += avoidForce;
	}

	return totalForce;



}

Vector2D S013010C_Aaron_Smith_Steering::WallAvoidance()
{
	Vector2D leftAhead1 = mTank->mLeftFeelers[0];
	Vector2D leftAhead2 = mTank->mLeftFeelers[1];
	Vector2D RightAhead1 = mTank->mRightFeelers[0];
	Vector2D RightAhead2 = mTank->mRightFeelers[1];
	Vector2D Ahead1 = mTank->mStraightFeelers[0];
	Vector2D Ahead2 = mTank->mStraightFeelers[1];

	vector<Vector2D> feelers = vector<Vector2D>();
	feelers.push_back(leftAhead1);
	feelers.push_back(leftAhead2);
	feelers.push_back(RightAhead1);
	feelers.push_back(RightAhead1);
	feelers.push_back(Ahead1);
	feelers.push_back(Ahead2);

	double maxAvoidForce = 500;
	Vector2D totalForce = Vector2D(0, 0);
	Vector2D avoidForce;

	//check if any feelers pass through left of screen
	
	for (int i = 0; i < feelers.size(); i++)
	{
		//Check if feeler passes left wall
		if (feelers[i].x < 0)
		{
			
			avoidForce = Vector2D(0, feelers[i].y) - feelers[i];
			avoidForce = Vec2DNormalize(avoidForce);
			avoidForce *= maxAvoidForce;
			totalForce += avoidForce;
		}
		if (feelers[i].x > kScreenWidth)
		{
			
			avoidForce = Vector2D(kScreenWidth, feelers[i].y) - feelers[i];
			avoidForce = Vec2DNormalize(avoidForce);
			avoidForce *= maxAvoidForce;
			totalForce += avoidForce;
		}
		if (feelers[i].y < 0)
		{
			
			avoidForce = Vector2D(feelers[i].x, 0) - feelers[i];
			avoidForce = Vec2DNormalize(avoidForce);
			avoidForce *= maxAvoidForce;
			totalForce += avoidForce;

		}
		if (feelers[i].y > kScreenHeight)
		{
			
			avoidForce = Vector2D(feelers[i].x,kScreenHeight) - feelers[i];
			avoidForce = Vec2DNormalize(avoidForce);
			avoidForce *= maxAvoidForce;
			totalForce += avoidForce;
		}
	
	}
	
	

	return totalForce;
}

GameObject* S013010C_Aaron_Smith_Steering::GetClosestObject(Vector2D ahead1, Vector2D ahead2)
{
	GameObject* closestObj = nullptr;

	obstacles = ObstacleManager::Instance()->GetObstacles();
	for (GameObject* obj : obstacles)
	{
		//bool collision = CheckLineInstersect(ahead1, ahead2, obj);
		
		bool collision = Collisions::Instance()->PointInBox(ahead2, obj->GetAdjustedBoundingBox());

		if (collision)
		{
		
			if (closestObj == NULL)
				closestObj = obj;
			else
			{
				if (GetDistance(mTank->GetCentrePosition(), obj->GetCentralPosition()) < GetDistance(mTank->GetCentrePosition(), closestObj->GetCentralPosition()))
					closestObj = obj;
			}
		}
		
	}

	return closestObj;

}
bool S013010C_Aaron_Smith_Steering::CheckLineInstersect(Vector2D ahead1, Vector2D ahead2, GameObject* obj)
{
	return GetDistance(obj->GetCentralPosition(), ahead1) <= obj->GetCollisionRadius()
		||
		GetDistance(obj->GetCentralPosition(), ahead2) <= obj->GetCollisionRadius();
}

double S013010C_Aaron_Smith_Steering::GetDistance(Vector2D obj1, Vector2D obj2)
{
	return  sqrt((obj1.x - obj2.x) * (obj1.x - obj2.x)
		+ (obj1.y - obj2.y) * (obj1.y - obj2.y));
}


bool S013010C_Aaron_Smith_Steering::AccumulateForce(Vector2D& totalForce, Vector2D newForce)
{
	//calculate how much steering force the vehicle has used so far
	double MagnitudeSoFar = totalForce.Length();

	//calculate how much steering force remains to be used by this vehicle
	double MagnitudeRemaining = mTank->GetMaxSpeed() - MagnitudeSoFar;

	//return false if there is no more force left to use
	if (MagnitudeRemaining <= 0.0) return false;

	//calculate the magnitude of the force we want to add
	double MagnitudeToAdd = newForce.Length();

	//if the magnitude of the sum of ForceToAdd and the running total
	//does not exceed the maximum force available to this vehicle, just
	//add together. Otherwise add as much of the ForceToAdd vector is
	//possible without going over the max.
	if (MagnitudeToAdd < MagnitudeRemaining)
	{
		totalForce += newForce;
	}

	else
	{
		//add it to the steering force
		totalForce += (Vec2DNormalize(newForce) * MagnitudeRemaining);
	}

	return true;
}
Vector2D S013010C_Aaron_Smith_Steering::Calculate(float deltaTime)
{
	Vector2D force;
	Vector2D totalForce;
	if (mIsObstacleAvoiding)
	{
		force = ObstacleAvoidance() * mObstacleAvoidanceWeight;
		if (!AccumulateForce(totalForce, force)) return totalForce;
	}

	if (mIsWallAvoided)
	{
		force = WallAvoidance() * mWallAvoidWeight;
		if (!AccumulateForce(totalForce, force)) return totalForce;
	}

	if (mIsWandering)
	{
		force = Wander(deltaTime) * mSeekWeight;
		if (!AccumulateForce(totalForce, force)) return totalForce;
	}

	if (mIsPursuing)
	{
		force = Pursuing() * mPursuitWeight;
		if (!AccumulateForce(totalForce, force)) return totalForce;

	}
	if (mIsSeeking)
	{
		force = Seek(mTank->GetTarget()) * mSeekWeight;
		if (!AccumulateForce(totalForce, force)) return totalForce;

	}
	if (mIsArriving)
	{
		force = Arrive(mTank->GetTarget(), Deceleration::NORMAL) * mArriveWeight;
		if (!AccumulateForce(totalForce, force)) return totalForce;
	}
	if (mIsFleeing)
	{
		force = Flee(mTank->GetTarget()) * mFleeWeight;
		if (!AccumulateForce(totalForce, force)) return totalForce;
	}

	return totalForce;

	//if (isFleeing) Flee(mTank->GetTarget());
	
	

	//Vector2D totalForce;


	

	/*for (Vector2D vec : mSteeringForces)
	{
		totalForce += vec;
	}
	mSteeringForces.clear();
	
	return totalForce;*/
}

Vector2D S013010C_Aaron_Smith_Steering::Update(float deltaTime)
{

	Vector2D totalForce;

	for (Vector2D vec : mSteeringForces)
	{
		totalForce += vec;
	}
	mSteeringForces.clear();

	return totalForce;

}


