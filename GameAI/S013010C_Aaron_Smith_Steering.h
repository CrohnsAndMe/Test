#ifndef S013010C_STEERING_H
#define S013010C_STEERING_H


//#include "S013010C_Aaron_Smith_Tank.h"
#include "Commons.h"
#include <vector>
#include "ObstacleManager.h"
#include "Collisions.h"


class S013010C_Aaron_Smith_Tank;

class S013010C_Aaron_Smith_Steering
{
	struct Walls {
		Vector2D mFrom;
		Vector2D mTo;

		Walls::Walls(Vector2D from, Vector2D to)
		{
			mFrom = from;
			mTo = to;
		}
	};

	enum Deceleration
	{
		SLOW = 3,
		NORMAL = 2,
		FAST = 1
	};

private:

	vector<Vector2D> mSteeringForces;
	S013010C_Aaron_Smith_Tank* mTank;
	vector<Walls> walls;

	bool mIsSeeking, mIsArriving;
	bool mIsFleeing;
	bool mIsWandering;
	bool mIsEvading;
	bool mIsObstacleAvoiding;
	bool mIsWallAvoided;
	bool mIsPursuing;

	double RandFloat() { return ((rand()) / (RAND_MAX + 1.0)); }
	float mCurWanderTime;
	float mTimeToChangeWander;
	float mWanderAngle = 0;
	int mCircleRadius = 10;

	Vector2D mWanderTarget;

	vector<GameObject*> obstacles;
	bool CheckLineInstersect(Vector2D ahead1,Vector2D ahead2,GameObject* obj);



	//Weighted Values
	double mObstacleAvoidanceWeight = 20.0;
	double mArriveWeight = 1.0;
	double mSeekWeight = 1.0;
	double mWanderWeight = 0.2;
	double mWallAvoidWeight = 10.0;
	double mFleeWeight = 1.0;
	double mPursuitWeight = 1.0;
public:
	S013010C_Aaron_Smith_Steering();
	S013010C_Aaron_Smith_Steering(S013010C_Aaron_Smith_Tank* tank,bool demoTank);
	~S013010C_Aaron_Smith_Steering();
	Vector2D Seek(Vector2D targetPos);
	Vector2D Flee(Vector2D targetPos);
	Vector2D Arrive(Vector2D targetPos,float acceleration);
	Vector2D Wander(float deltaTime);
	Vector2D Pursuing();
	void Evade();
	Vector2D ObstacleAvoidance();
	Vector2D WallAvoidance();
	Vector2D Update(float deltaTime);
	Vector2D Calculate(float deltaTime);
	bool AccumulateForce(Vector2D& totalForce, Vector2D newForce);
	GameObject* GetClosestObject(Vector2D ahead1, Vector2D ahead2);
	double GetDistance(Vector2D obj1, Vector2D obj2);
public:
	void TurnSeekOn() {  mIsSeeking = true; }
	void TurnSeekOff() { mIsSeeking = false; }
	void TurnArriveOn() { mIsArriving = true; }
	void TurnArriveOff() {  mIsArriving = false; }
	void TurnFleeOff() {  mIsFleeing = false; }
	void TurnFleeOn() {  mIsFleeing = true; }
	void TurnWanderOff() {  mIsWandering = false; }
	void TurnWanderOn() {  mIsWandering = true; }
	void TurnPursueOff() { mIsPursuing = false; }
	void TurnPursueOn() { mIsPursuing = true; }
	void TurnEvadeOff() { mIsEvading = false; }
	void TurnEvadeOn() { mIsEvading = true; }
	
};

#endif // !1