#pragma once

#include <PxPhysicsAPI.h>
#include <PxScene.h>
#include <vector>
#include <iostream>
#include "gl_core_4_4.h"
#include "GLFW/glfw3.h"
#include "Gizmos.h"

#include "glm/gtc/quaternion.hpp"
#include "glm/gtc/matrix_transform.hpp"

using namespace std;
using namespace physx;
using namespace glm;

class ActorManager
{
public:
	ActorManager(PxPhysics* physics, PxScene* scene, PxMaterial* material);
	~ActorManager();

	void reset();


	void createSphere(bool dynamic, vec4 color, vec3 position, float radius, float density,bool trigger);
	void createBox(bool dynamic, vec4 color, vec3 position, vec3 constraints,float density,bool trigger);
	void createCapsule(bool dynamic, vec4 color, vec3 position, float radius, float height, float density,bool trigger);

	void addDynamicActor(PxRigidDynamic* actor, vec4 color);


	void Render();

private:

	void RenderShape(PxRigidActor* actor, vector<vec4> colorVector, int index);

	vector<PxRigidDynamic*> dynamicActors;
	vector<vec4> dynamicActorColors;
	vector<PxRigidStatic*> staticActors;	
	vector<vec4> staticActorColors;

	PxPhysics* g_Physics;
	PxScene* g_PhysicsScene;
	PxMaterial* g_PhysicsMaterial;
};


