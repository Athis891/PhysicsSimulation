#pragma once

#include <PxPhysicsAPI.h>
#include <PxScene.h>
#include <characterkinematic/PxController.h>

#include "ActorManager.h"

#include <vector>
#include <time.h>

using namespace physx;

class ControllerHitReport : public PxUserControllerHitReport
{
public:
	//overload the onShapeHit function
	virtual void onShapeHit(const PxControllerShapeHit &hit);
	//other collision functions which we must overload //these handle collision with other controllers and hitting obstacles
	virtual void onControllerHit(const PxControllersHit &hit){};
	//Called when current controller hits another controller. More...
	virtual void onObstacleHit(const PxControllerObstacleHit &hit){};
	//Called when current controller hits a user-defined obstacl
	ControllerHitReport() :PxUserControllerHitReport(){};
	PxVec3 getPlayerContactNormal(){ return _playerContactNormal; };
	void clearPlayerContactNormal(){ _playerContactNormal = PxVec3(0, 0, 0); };
	PxVec3 _playerContactNormal;
};


class PlayerController
{
public:
	PlayerController(PxScene* g_PhysicsScene,PxMaterial* g_PhysicsMaterial, ActorManager* actorManager, PxExtendedVec3 startingPosition);
	~PlayerController();

	void reset();

	void Update(float a_deltaTime);

private:
	void initController();

	PxScene* g_PhysicsScene;
	PxMaterial* g_PhysicsMaterial;
	ActorManager* actorManager;
	PxExtendedVec3 startingPosition;

	ControllerHitReport* m_hitReport;
	PxControllerManager* gCharacterManager;
	PxController* gPlayerController;

	float jumpCooldown;
	float currentJumpCooldown;
	float persistanceCd;

	float characterYVelocity;
	float characterRotation;
	float playerGravity;


};