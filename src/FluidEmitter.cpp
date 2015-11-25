#include "FluidEmitter.h"

FluidEmitter::FluidEmitter(PxPhysics* Physics,PxScene* PhysicsScene,PxVec3 a_position,PxU32 a_maxParticles,float a_emitRate,glm::vec4 color,bool a_borderless)
{
	m_particleColor = color;
	borderless = a_borderless;

	g_Physics = Physics;
	g_PhysicsScene = PhysicsScene;

	position = a_position;
	maxParticles = a_maxParticles;
	emitRate = a_emitRate;

	initEmitter();
}

void FluidEmitter::initEmitter()
{
	//create our particle system
	PxParticleFluid* pf;
	// create particle system in PhysX SDK
	// set immutable properties.

	bool perParticleRestOffset = false;
	pf = g_Physics->createParticleFluid(maxParticles, perParticleRestOffset);
	pf->setRestParticleDistance(.3f);
	pf->setDynamicFriction(0.1);
	pf->setStaticFriction(0.1);
	pf->setDamping(0.1);
	pf->setParticleMass(.1);
	pf->setRestitution(0);
	//pf->setParticleReadDataFlag(PxParticleReadDataFlag::eDENSITY_BUFFER,
	// true);
	pf->setParticleBaseFlag(PxParticleBaseFlag::eCOLLISION_TWOWAY, true);
	pf->setStiffness(100);


	pauseEmission = false;

	if (pf)
	{
		g_PhysicsScene->addActor(*pf);
		m_particleFluidEmitter = new ParticleFluidEmitter(maxParticles,
			position, pf, emitRate);
		m_particleFluidEmitter->setStartVelocityRange(-0.001f, -250.0f, -0.001f,
			0.001f, -250.0f, 0.001f);
	}
}

void FluidEmitter::reset()
{
	m_particleFluidEmitter->releaseAll();
	initEmitter();
}



void FluidEmitter::UpdateAndRender(float dt)
{
	if (m_particleFluidEmitter)
	{
		if (pauseEmission == false)
		{
			m_particleFluidEmitter->update(dt);
		}
		

		m_particleFluidEmitter->renderParticles(m_particleColor,borderless);
	}
}

