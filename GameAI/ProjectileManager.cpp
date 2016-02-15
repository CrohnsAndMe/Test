#include "ProjectileManager.h"
#include <SDL.h>
#include <iostream>
#include "Projectile.h"
#include "BaseTank.h"
#include "Collisions.h"
#include <algorithm>
#include "Commons.h"
#include "ObstacleManager.h"

//Initialise the instance to null.
ProjectileManager* ProjectileManager::mInstance = NULL;

//--------------------------------------------------------------------------------------------------

ProjectileManager* ProjectileManager::Instance()
{
	if(!mInstance)
	{
		mInstance = new ProjectileManager;
	}

	return mInstance;
}

//--------------------------------------------------------------------------------------------------

void ProjectileManager::Init(SDL_Renderer* renderer)
{
	mInstance->mRenderer = renderer;
	mInstance->mProjectileIndexToDelete = -1;
	mInstance->SetUpIndestructibleMines();
}

//--------------------------------------------------------------------------------------------------

ProjectileManager::ProjectileManager()
{
}

//--------------------------------------------------------------------------------------------------

ProjectileManager::~ProjectileManager()
{
	mInstance = NULL;

	for(unsigned int i = 0; i < mInstance->mProjectiles.size(); i++)
		delete mInstance->mProjectiles[i];
	mInstance->mProjectiles.clear();
}

//--------------------------------------------------------------------------------------------------

void ProjectileManager::UpdateProjectiles(float deltaTime)
{
	//Update the indestructbile mines.
	for(unsigned int i = 0; i < mInstance->mIndestructibleMines.size(); i++)
		mInstance->mIndestructibleMines[i]->Update(deltaTime);

	//Update the projectiles.
	for(unsigned int i = 0; i < mInstance->mProjectiles.size(); i++)
		mInstance->mProjectiles[i]->Update(deltaTime);

	//Check if any projectile have left the screen.
	Rect2D screenBox(0.0f, 0.0f, kScreenWidth, kScreenHeight);
	for(unsigned int i = 0; i < mInstance->mProjectiles.size(); i++)
	{
		if(!Collisions::Instance()->PointInBox(mInstance->mProjectiles[i]->GetCentralPosition(), screenBox))
		{
			//Prepare this projectile for deletion.
			mInstance->mProjectileIndexToDelete = i;
			break;
		}
	}

	//Remove one projectile a frame.
	if(mInstance->mProjectileIndexToDelete != -1)
	{
		Projectile* projectileToDelete = mInstance->mProjectiles.at(mInstance->mProjectileIndexToDelete);
		mInstance->mProjectiles.erase(mInstance->mProjectiles.begin()+mInstance->mProjectileIndexToDelete);
		delete projectileToDelete;
		mProjectileIndexToDelete = -1;
	}
}

//--------------------------------------------------------------------------------------------------

void ProjectileManager::RenderProjectiles()
{
	for(unsigned int i = 0; i < mIndestructibleMines.size(); i++)
		mInstance->mIndestructibleMines[i]->Render();

	for(unsigned int i = 0; i < mProjectiles.size(); i++)
		mInstance->mProjectiles[i]->Render();
}

//--------------------------------------------------------------------------------------------------

void ProjectileManager::CheckForCollisions(vector<BaseTank*> listOfObjects)
{
	for(unsigned int i = 0; i < listOfObjects.size(); i++)
	{
		GameObject* gameObject = (GameObject*)listOfObjects[i];
		CheckForACollision(gameObject);
	}
}

//--------------------------------------------------------------------------------------------------

void ProjectileManager::CheckForCollisions(vector<GameObject*> listOfObjects)
{
	for(unsigned int i = 0; i < listOfObjects.size(); i++)
	{
		CheckForACollision(listOfObjects[i]);
	}
}

//--------------------------------------------------------------------------------------------------

void ProjectileManager::CheckForACollision(GameObject* gameObject)
{
	if(gameObject != NULL)
	{
		Rect2D rect = gameObject->GetAdjustedBoundingBox();
		for(unsigned int i = 0; i < mInstance->mProjectiles.size(); i++)
		{
			if(Collisions::Instance()->PointInBox(mInstance->mProjectiles[i]->GetPosition(), rect))
			{
				if(gameObject->GetGameObjectType() == GAMEOBJECT_TANK)
				{
					//Inform bullet that is has hit a target if it was a tank.
					if((GameObject*)mInstance->mProjectiles[i]->GetFirer() == gameObject)
					{
						//Do nothing if this is a bullet from the firing tank.
						return;
					}
					else
					{
						//Damage Tank.
						BaseTank* tank = (BaseTank*)gameObject;
						tank->TakeDamage(mInstance->mProjectiles[i]->GetGameObjectType());

						//If the hit tank has no health then it is destroyed and a bonus score is awared to the firing tank.
						if(tank->GetHealth() <= 0)
							mInstance->mProjectiles[i]->GetFirer()->AddToScore(SCORE_DESTROYEDTANK);

						//Add score to firing tank.
						switch(mInstance->mProjectiles[i]->GetGameObjectType())
						{
							case GAMEOBJECT_BULLET:
								mInstance->mProjectiles[i]->GetFirer()->AddToScore(SCORE_BULLETHIT);
							break;

							case GAMEOBJECT_ROCKET:
								mInstance->mProjectiles[i]->GetFirer()->AddToScore(SCORE_ROCKETHIT);
							break;

							case GAMEOBJECT_MINE:
								mInstance->mProjectiles[i]->GetFirer()->AddToScore(SCORE_MINEHIT);
							break;
						}
					}

					//Remove this projectile.
					mInstance->mProjectiles[i]->RegisterHit();
				}
			
				//Prepare this bullet for deletion.
				mInstance->mProjectileIndexToDelete = i;
			}
		}

		//Check against the indestructible mines.
		for(unsigned int i = 0; i < mInstance->mIndestructibleMines.size(); i++)
		{
			if(Collisions::Instance()->PointInBox(mInstance->mIndestructibleMines[i]->GetPosition(), rect))
			{
				if(gameObject->GetGameObjectType() == GAMEOBJECT_TANK)
				{
					//Damage Tank.
					BaseTank* tank = (BaseTank*)gameObject;
					tank->TakeDamage(mInstance->mIndestructibleMines[i]->GetGameObjectType());
				}
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------

void ProjectileManager::CreateProjectile(SDL_Renderer* renderer, ProjectileSetupDetails details, BaseTank* firer)
{
	switch(details.GameObjectType)
	{
		case GAMEOBJECT_BULLET:
			if(firer->GetBullets() > 0)
			{
				mInstance->mProjectiles.push_back(new Projectile(renderer, details, firer));
				firer->DeductABullet();
			}
		break;

		case GAMEOBJECT_ROCKET:
			if(firer->GetRockets() > 0)
			{
				mInstance->mProjectiles.push_back(new Projectile(renderer, details, firer));
				firer->DeductARocket();
			}
		break;

		case GAMEOBJECT_MINE:
			if(firer->GetMines() > 0)
			{
				mInstance->mProjectiles.push_back(new Projectile(renderer, details, firer));
				firer->DeductAMine();
			}
		break;
	}
}

//--------------------------------------------------------------------------------------------------

void ProjectileManager::SetUpIndestructibleMines()
{
	//Set the projectile setup details.
	ProjectileSetupDetails details;
	details.Direction		= Vector2D();
	details.GameObjectType	= GAMEOBJECT_MINE;
	details.ImagePath		= kMinePath;
	details.RotationAngle	= 0.0f;

	for(unsigned int i = 0; i < ObstacleManager::Instance()->GetObstacles().size(); i++)
	{
		GameObject* currentObstacle = ObstacleManager::Instance()->GetObstacles().at(i);
		Rect2D boundingBox = currentObstacle->GetAdjustedBoundingBox();

		//Create some offsets for spawn positions.
		Vector2D spawnPos[] = { Vector2D(-(boundingBox.width*0.45f),-(boundingBox.height*0.4f)),
								Vector2D(boundingBox.width*0.45f,-(boundingBox.height*0.4f)),
								Vector2D(),
								Vector2D(-(boundingBox.width*0.45f),boundingBox.height*0.4f),
								Vector2D(boundingBox.width*0.45f,boundingBox.height*0.4f)
							  };

		//Create 5 mines upon each obstacle.
		for(unsigned int j = 0; j < 5; j++)
		{
			details.StartPosition	= Vector2D( currentObstacle->GetCentralPosition().x + spawnPos[j].x,
												currentObstacle->GetCentralPosition().y + spawnPos[j].y);

			mInstance->mIndestructibleMines.push_back(new Projectile(mRenderer, details, NULL));
		}
	}
}

//--------------------------------------------------------------------------------------------------
