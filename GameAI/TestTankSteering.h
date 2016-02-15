#ifndef TestTankSteering_H
#define TestTankSteering_H



/*

accessing position from inside steering class?

the arrival values higher than code example

rotatetowardsfaceposition


*/

//#include "S013010C_Aaron_Smith_Tank.h"
#include "Commons.h"
#include <vector>
class TestTank;

class TestTankSteering
{
	enum Deceleration
	{
		SLOW = 3,
		NORMAL = 2,
		FAST = 1
	};

private:

	vector<Vector2D> mSteeringForces;
	TestTank* mTank;

	bool mIsSeeking, mIsArriving;
	bool mIsFleeing;
	bool mIsWandering;
	bool mIsPursuing;

	double RandFloat() { return ((rand()) / (RAND_MAX + 1.0)); }
	float mCurWanderTime;
	float mTimeToChangeWander;
	float mWanderAngle = 0;
	int mCircleRadius = 10;
	Vector2D mWanderTarget;
public:
	TestTankSteering();
	TestTankSteering(TestTank* tank, bool demoTank);
	~TestTankSteering();
	void Seek(Vector2D targetPos);
	void Flee(Vector2D targetPos);
	void Arrive(Vector2D targetPos, float acceleration);
	void Wander(float deltaTime);
	void Pursuing();
	Vector2D Update(float deltaTime);
	Vector2D Calculate(float deltaTime);


public:
	void TurnSeekOn() { mIsSeeking = true; }
	void TurnSeekOff() { mIsSeeking = false; }
	void TurnArriveOn() { mIsArriving = true; }
	void TurnArriveOff() { mIsArriving = false; }
	void TurnFleeOff() { mIsFleeing = false; }
	void TurnFleeOn() { mIsFleeing = true; }
	void TurnWanderOff() { mIsWandering = false; }
	void TurnWanderOn() { mIsWandering = true; }
	void TurnPursueOff() { mIsPursuing = false; }
	void TurnPursueOn() { mIsPursuing = true; }
};

#endif // !1