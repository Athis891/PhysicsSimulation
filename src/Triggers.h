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


class MycollisionCallBack : public PxSimulationEventCallback
{
	virtual void onContact(const PxContactPairHeader& pairHeader, const
		PxContactPair* pairs, PxU32 nbPairs)
	{
		for (PxU32 i = 0; i < nbPairs; i++)
		{
			const PxContactPair& cp = pairs[i];
			//only interested in touches found and lost
			if (cp.events & PxPairFlag::eNOTIFY_TOUCH_FOUND)
			{
				cout << "Collision Detected between: ";
				//cout << pairHeader.actors[0]->getName();
				//cout << pairHeader.actors[1]->getName() << endl;
			}
		}
	};
	virtual void onTrigger(PxTriggerPair* pairs, PxU32 nbPairs)
	{
		for (PxU32 i = 0; i < nbPairs; i++)
		{
			PxTriggerPair* pair = pairs + i;
			PxActor* triggerActor = pair->triggerActor;
			PxActor* otherActor = pair->otherActor;
			//cout << otherActor->getName();
			cout << "Triggered"<<endl;
			//cout << triggerActor->getName() << endl;
		}
	}
	virtual void onConstraintBreak(PxConstraintInfo*, PxU32){};
	virtual void onWake(PxActor**, PxU32){};
	virtual void onSleep(PxActor**, PxU32){};
};

