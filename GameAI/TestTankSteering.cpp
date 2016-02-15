#include "TestTankSteering.h"
#include "TestTank.h"

TestTankSteering::TestTankSteering()
{
	mTank = nullptr;
	cout << "No Tank set In Steering!!!!" << endl;
}

TestTankSteering::TestTankSteering(TestTank* tank, bool demoTank)
{
	mTank = tank;
	if (demoTank)
	{
		mIsWandering = false;
		mIsPursuing = false;
		mIsArriving = false;
	}
	else
	{
		mIsWandering = false;
		mIsPursuing = false;
		mIsArriving = false;
	}
	//mIsSeeking = true;

	mIsFleeing = false;


}



TestTankSteering::~TestTankSteering()
{

}

void TestTankSteering::Seek(Vector2D targetPos)
{
	//cout << "SEEKING" << endl;
	Vector2D reqVelocity = Vec2DNormalize(targetPos - mTank->GetCentrePosition()) * mTank->GetMaxSpeed();
	reqVelocity -= mTank->GetVelocity();
	mSteeringForces.push_back(reqVelocity);
}

void TestTankSteering::Flee(Vector2D targetPos)
{
	Vector2D reqVelocity = Vec2DNormalize(mTank->GetPosition() - targetPos) * mTank->GetMaxSpeed();
	reqVelocity -= mTank->GetVelocity();
	mSteeringForces.push_back(reqVelocity);
}

void TestTankSteering::Arrive(Vector2D targetPos, float deceleration)
{
	Vector2D targetDir = targetPos - mTank->GetCentrePosition();
	double distance = targetDir.Length();
	//cout << "Distance: " << distance << endl;
	if (distance > 50)
		Seek(targetPos);
	else if (distance < 2)
	{
		cout << "ARRIVED" << endl;
		//mTank->SetHasTarget(false);
		mIsArriving = false;
	}
	else
	{
		//cout << "MOVING" << endl;
		//because Deceleration is enumerated as an int, this value is required
		//to provide fine tweaking of the deceleration..
		const double DecelerationTweaker = .3;

		//calculate the speed required to reach the target given the desired
		//deceleration
		double speed = (distance) / (double)deceleration * (DecelerationTweaker);
		//
		//make sure the velocity does not exceed the max
		speed = min(speed, mTank->GetMaxSpeed());

		//from here proceed just like Seek except we don't need to normalize 
		//the ToTarget vector because we have already gone to the trouble
		//of calculating its length: dist. 
		//Vector2D reqVelocity = targetDir /  distance;
		Vector2D reqVelocity =
			Vec2DNormalize(targetDir / distance);


		mSteeringForces.push_back(reqVelocity - mTank->GetVelocity());
	}
	//else
	//	cout << "STOPPED" << endl;

}



void TestTankSteering::Wander(float deltaTime)
{

	float wanderCircleRadius = 11;

	Vector2D normVel = Vec2DNormalize(mTank->GetVelocity());
	Vector2D circleInFront = Vector2D(mTank->GetCentrePosition().x + normVel.x * wanderCircleRadius,
		mTank->GetCentrePosition().y + normVel.y * wanderCircleRadius);

	Vector2D randPos = Vector2D((RandFloat() - RandFloat()), (RandFloat() - RandFloat()));
	//Vector2D randPos = Vector2D(RandFloat(), RandFloat());
	mWanderTarget += randPos;
	mWanderTarget = Vec2DNormalize(mWanderTarget);
	mWanderTarget *= wanderCircleRadius;
	Vector2D newForce = circleInFront + mWanderTarget;
	Seek(newForce);

	//- mTank->GetVelocity()
	//mSteeringForces.push_back(newForce);
}

void TestTankSteering::Pursuing()
{

	Vector2D enemyTankPos = mTank->GetTarget();
	Seek(enemyTankPos);

	double relativeHeading = mTank->GetHeading().Dot(mTank->GetEnemyHeading());

	if (enemyTankPos.Dot(mTank->GetHeading()) > 0 &&
		(relativeHeading < -0.95))
		Seek(enemyTankPos);
	else
	{
		//not ahead of player so need to predict enemy position

		double lookAheadTime = enemyTankPos.Length() / (mTank->GetMaxSpeed() + mTank->GetEnemySpeed());
		Seek(enemyTankPos + mTank->GetEnemyVelocity() * lookAheadTime);
	}

}
Vector2D TestTankSteering::Calculate(float deltaTime)
{

	//if (isFleeing) Flee(mTank->GetTarget());
	if (mIsArriving) Arrive(mTank->GetTarget(), Deceleration::NORMAL);
	//if (mIsWandering) Wander(deltaTime);
	//if (mIsPursuing) Pursuing();


	Vector2D totalForce;

	//totalForce = SeekR(mTank->GetTarget());



	for (Vector2D vec : mSteeringForces)
	{
		totalForce += vec;
	}
	mSteeringForces.clear();

	return totalForce;
}

Vector2D TestTankSteering::Update(float deltaTime)
{

	Vector2D totalForce;

	for (Vector2D vec : mSteeringForces)
	{
		totalForce += vec;
	}
	mSteeringForces.clear();

	return totalForce;

}


