#include "ActorManager.h"

ActorManager::ActorManager(PxPhysics* physics, PxScene* scene, PxMaterial* material)
{
	g_Physics = physics;
	g_PhysicsScene = scene;
	g_PhysicsMaterial = material;
}

void ActorManager::reset()
{
	for (int i = 0; i < dynamicActors.size(); i++)
	{
		dynamicActors[i]->release();
	}

	dynamicActors.clear();
	dynamicActorColors.clear();

	for (int i = 0; i < staticActors.size(); i++)
	{
		staticActors[i]->release();
	}

	staticActors.clear();
	staticActorColors.clear();
}


void ActorManager::createSphere(bool dynamic, vec4 color, vec3 position, float radius, float density,bool trigger)
{
	physx::PxSphereGeometry shape(radius);

	physx::PxTransform Lpose = physx::PxTransform(physx::PxVec3(position.x, position.y, position.z));

	if (trigger)
	{
		physx::PxRigidStatic* sphere = PxCreateStatic(*g_Physics, Lpose, shape, *g_PhysicsMaterial);
		g_PhysicsScene->addActor(*sphere);
		staticActors.push_back(sphere);
		staticActorColors.push_back(color);

		PxShape* pxShape;
		staticActors[staticActors.size() - 1]->getShapes(&pxShape, 1);

		pxShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
		pxShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
	}
	else if (dynamic)
	{
		physx::PxRigidDynamic* sphere = PxCreateDynamic(*g_Physics, Lpose, shape, *g_PhysicsMaterial, density);

		g_PhysicsScene->addActor(*sphere);
		dynamicActors.push_back(sphere);
		dynamicActorColors.push_back(color);


	}
	else
	{
		physx::PxRigidStatic* sphere = PxCreateStatic(*g_Physics, Lpose, shape, *g_PhysicsMaterial);
		g_PhysicsScene->addActor(*sphere);
		staticActors.push_back(sphere);
		staticActorColors.push_back(color);
	}
}

void ActorManager::createBox(bool dynamic, vec4 color, vec3 position, vec3 constraints, float density,bool trigger)
{
	
	physx::PxBoxGeometry shape(constraints.x, constraints.y, constraints.z);

	physx::PxTransform Lpose = physx::PxTransform(physx::PxVec3(position.x, position.y, position.z));

	if (trigger)
	{
		physx::PxRigidStatic* box = PxCreateStatic(*g_Physics, Lpose, shape, *g_PhysicsMaterial);
		g_PhysicsScene->addActor(*box);
		staticActors.push_back(box);
		staticActorColors.push_back(color);

		PxShape* pxShape;
		staticActors[staticActors.size() - 1]->getShapes(&pxShape, 1);

		pxShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
		pxShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);

	}
	else if (dynamic)
	{
		physx::PxRigidDynamic* box = PxCreateDynamic(*g_Physics, Lpose, shape, *g_PhysicsMaterial, density);

		g_PhysicsScene->addActor(*box);
		dynamicActors.push_back(box);
		dynamicActorColors.push_back(color);
	}
	else
	{
		physx::PxRigidStatic* box = PxCreateStatic(*g_Physics, Lpose, shape, *g_PhysicsMaterial);
		g_PhysicsScene->addActor(*box);
		staticActors.push_back(box);
		staticActorColors.push_back(color);
	}

}

void ActorManager::createCapsule(bool dynamic, vec4 color, vec3 position, float radius, float height, float density, bool trigger)
{

	physx::PxCapsuleGeometry shape(radius,height/2);

	physx::PxTransform Lpose = physx::PxTransform(physx::PxVec3(position.x, position.y, position.z));

	if (trigger)
	{
		physx::PxRigidStatic* capsule = PxCreateStatic(*g_Physics, Lpose, shape, *g_PhysicsMaterial);
		g_PhysicsScene->addActor(*capsule);
		staticActors.push_back(capsule);
		staticActorColors.push_back(color);

		PxShape* pxShape;
		staticActors[staticActors.size() - 1]->getShapes(&pxShape, 1);

		pxShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE,false);
		pxShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE,true);



	}
	else if (dynamic)
	{
		physx::PxRigidDynamic* capsule = PxCreateDynamic(*g_Physics, Lpose, shape, *g_PhysicsMaterial, density);

		g_PhysicsScene->addActor(*capsule);
		dynamicActors.push_back(capsule);
		dynamicActorColors.push_back(color);
	}
	else
	{
		physx::PxRigidStatic* capsule = PxCreateStatic(*g_Physics, Lpose, shape, *g_PhysicsMaterial);
		g_PhysicsScene->addActor(*capsule);
		staticActors.push_back(capsule);
		staticActorColors.push_back(color);
	}
}

void ActorManager::addDynamicActor(PxRigidDynamic* actor,vec4 color)
{
	dynamicActors.push_back(actor);
	dynamicActorColors.push_back(color);
}

void ActorManager::Render()
{
	

	for (int i = 0; i < staticActors.size(); i++)
	{
		RenderShape(staticActors[i], staticActorColors, i);
	}

	for (int i = 0; i < dynamicActors.size(); i++)
	{
		RenderShape(dynamicActors[i], dynamicActorColors, i);	

	}


}


void ActorManager::RenderShape(PxRigidActor* actor, vector<vec4> colorVector, int index)
{
	physx::PxTransform transform = actor->getGlobalPose();
	glm::vec3 pos(transform.p.x, transform.p.y, transform.p.z);
	glm::quat q;

	q.x = transform.q.x;
	q.y = transform.q.y;
	q.z = transform.q.z;
	q.w = transform.q.w;

	glm::mat4 rot = glm::mat4(q);

	PxShape* shapes[16];
	int shapeCount = actor->getShapes(shapes, 16);


	PxSphereGeometry sphereGeo;
	PxBoxGeometry boxGeo;
	PxCapsuleGeometry capGeo;

	for (int i = 0; i < shapeCount; ++i)
	{
		PxShape* shape = shapes[i];

		vec4 targetColor=colorVector[index];

		bool isTrigger = false;

		
		if (shape->getFlags().isSet(PxShapeFlag::eTRIGGER_SHAPE))
		{
			isTrigger = true;
		}
		

		if (shape->getSphereGeometry(sphereGeo))
		{

			if (isTrigger == true)
			{
				Gizmos::addBlackSphereFilled(pos, sphereGeo.radius, 15, 15, targetColor, &rot);
			}
			else
			{
				Gizmos::addSphereFilled(pos, sphereGeo.radius, 15, 15, targetColor, &rot);
			}
			
		}
		else if (shape->getBoxGeometry(boxGeo))
		{
			if (isTrigger == true)
			{
				Gizmos::addBlackAABBFilled(pos, vec3(boxGeo.halfExtents.x, boxGeo.halfExtents.y, boxGeo.halfExtents.z), targetColor, &rot);
			}
			else
			{
				Gizmos::addAABBFilled(pos, vec3(boxGeo.halfExtents.x, boxGeo.halfExtents.y, boxGeo.halfExtents.z), targetColor, &rot);
			}

			
		}
		else if (shape->getCapsuleGeometry(capGeo))
		{
			Gizmos::addCapsule(pos, capGeo.halfHeight * 2, capGeo.radius, 15, 15, targetColor, &rot);
		}
	}

}




