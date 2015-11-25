#ifndef SOUND_PROGRAMMING_H_
#define SOUND_PROGRAMMING_H_

#include "Application.h"
#include "Camera.h"

#include <PxPhysicsAPI.h>
#include <PxScene.h>

#include <vector>

#include "FluidEmitter.h"
#include "RagdollSpawner.h"
#include "ActorManager.h"
#include "PlayerController.h"
#include "AntTweakBar.h"
#include "ShapeSpawnWidget.h"
#include "Triggers.h"

class Physics : public Application
{
public:
    virtual bool startup();
    virtual void shutdown();
    virtual bool update();
    virtual void draw();

    void setupPhysx();
    void setupTutorial1();

	void ResetField();

    FlyCamera m_camera;

    glm::vec2 screen_size;

    physx::PxFoundation* g_PhysicsFoundation;
    physx::PxPhysics* g_Physics;
    physx::PxScene* g_PhysicsScene;
    physx::PxDefaultErrorCallback gDefaultErrorCallback;
    physx::PxDefaultAllocator gDefaultAllocatorCallback;
    physx::PxSimulationFilterShader gDefaultFilterShader = physx::PxDefaultSimulationFilterShader;
    physx::PxMaterial* g_PhysicsMaterial;
    physx::PxMaterial* g_boxMaterial;
    physx::PxCooking* g_PhysicsCooker;

	FluidEmitter* m_fluidEmitter;
	RagdollSpawner* m_ragdollSpawner;
	ActorManager* m_actorManager;
	PlayerController* m_playerController;
	ShapeSpawnWidget* m_shapeSpawnWidget;

	bool ragdollSpawn;

	float fps;
	bool resetFields;
};

#endif //CAM_PROJ_H_