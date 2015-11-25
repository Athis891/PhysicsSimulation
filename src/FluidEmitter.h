#pragma once

#include "ParticleFluidEmitter.h"
#include <PxPhysicsAPI.h>
#include <PxScene.h>
#include <vector>

#include "Gizmos.h"

using namespace physx;

class FluidEmitter
{
public:
	FluidEmitter(PxPhysics* g_Physics, PxScene* g_PhysicsScene, PxVec3 position, PxU32 maxParticles, float emitRate, glm::vec4 color, bool a_borderless);
	~FluidEmitter(){}

	
	void reset();

	void UpdateAndRender(float dt);

	glm::vec4 m_particleColor;
	bool borderless;

	bool pauseEmission;

	PxVec3 position;
	PxU32 maxParticles;
	float emitRate;

private:
	void initEmitter();

	ParticleFluidEmitter* m_particleFluidEmitter;
	PxPhysics* g_Physics;
	PxScene* g_PhysicsScene;

	


	

};