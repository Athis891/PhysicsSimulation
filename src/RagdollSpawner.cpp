#include "RagdollSpawner.h"

#include "gl_core_4_4.h"
#include "GLFW/glfw3.h"
#include "Gizmos.h"

#include "glm/gtc/quaternion.hpp"
#include "glm/gtc/matrix_transform.hpp"

RagdollNode::RagdollNode(PxQuat _globalRotation, int _parentNodeIdx, float _halfLength, float
	_radius, float _parentLinkPos, float _childLinkPos, char* _name){
	globalRotation =
		_globalRotation, parentNodeIdx = _parentNodeIdx; halfLength = _halfLength; radius = _radius;
	parentLinkPos = _parentLinkPos; childLinkPos = _childLinkPos; name = _name;
};

PxArticulation* Ragdoll::makeRagdoll(PxPhysics* g_Physics, RagdollNode** nodeArray,
	PxTransform worldPos, float scaleFactor, PxMaterial* ragdollMaterial)
{
	//create the articulation for our ragdoll
	PxArticulation *articulation = g_Physics->createArticulation();
	RagdollNode** currentNode = nodeArray;
	//while there are more nodes to process...
	while (*currentNode != NULL)
	{
		//get a pointer to the current node
		RagdollNode* currentNodePtr = *currentNode;
		//create a pointer ready to hold the parent node pointer if there is one
		RagdollNode* parentNode = nullptr;
		//get scaled values for capsule
		float radius = currentNodePtr->radius*scaleFactor;
		float halfLength = currentNodePtr->halfLength*scaleFactor;
		float childHalfLength = radius + halfLength;
		float parentHalfLength = 0; //will be set later if there is a parent
		//get a pointer to the parent
		PxArticulationLink* parentLinkPtr = NULL;
		currentNodePtr->scaledGobalPos = worldPos.p;

		if (currentNodePtr->parentNodeIdx != -1)
		{
			//if there is a parent then we need to work out our local position for the link
			//get a pointer to the parent node
			parentNode = *(nodeArray + currentNodePtr->parentNodeIdx);

			//get a pointer to the link for the parent
			parentLinkPtr = parentNode->linkPtr;
			parentHalfLength = (parentNode->radius + parentNode->halfLength) *scaleFactor;

			//work out the local position of the node
			PxVec3 currentRelative = currentNodePtr->childLinkPos * currentNodePtr ->globalRotation.rotate(PxVec3(childHalfLength, 0, 0));
			PxVec3 parentRelative = -currentNodePtr->parentLinkPos * parentNode ->globalRotation.rotate(PxVec3(parentHalfLength, 0, 0));
			currentNodePtr->scaledGobalPos = parentNode->scaledGobalPos - (parentRelative +	currentRelative);
		}

		//build the transform for the link
		PxTransform linkTransform = PxTransform(currentNodePtr->scaledGobalPos, currentNodePtr ->globalRotation);
		//create the link in the articulation
		PxArticulationLink* link = articulation->createLink(parentLinkPtr, linkTransform);

		currentNodePtr->linkPtr = link;
		float jointSpace = .01f; //gap between joints
		float capsuleHalfLength = (halfLength > jointSpace ? halfLength - jointSpace : 0) + .01f;
		PxCapsuleGeometry capsule(radius, capsuleHalfLength);
		link->createShape(capsule, *ragdollMaterial); //adds a capsule collider to the link
		PxRigidBodyExt::updateMassAndInertia(*link, 50.0f);

		if (currentNodePtr->parentNodeIdx != -1)
		{
			//get the pointer to the joint from the link
			PxArticulationJoint *joint = link->getInboundJoint();
			//get the relative rotation of this link
			PxQuat frameRotation = parentNode->globalRotation.getConjugate() *	currentNodePtr->globalRotation;
			//set the parent contraint frame

			PxTransform parentConstraintFrame = PxTransform(PxVec3(currentNodePtr ->parentLinkPos * parentHalfLength, 0, 0), frameRotation);
			PxTransform thisConstraintFrame = PxTransform(PxVec3(currentNodePtr ->childLinkPos * childHalfLength, 0, 0));

			//set up the poses for the joint so it is in the correct place
			joint->setParentPose(parentConstraintFrame);
			joint->setChildPose(thisConstraintFrame);
			//set up some constraints to stop it flopping around
			joint->setStiffness(20);
			joint->setDamping(20);
			joint->setSwingLimit(0.4f, 0.4f);
			joint->setSwingLimitEnabled(true);
			joint->setTwistLimit(-0.1f, 0.1f);
			joint->setTwistLimitEnabled(true);
		}


		//get a pointer to the current node
		currentNode++;
	}
	return articulation;
}



RagdollSpawner::RagdollSpawner(PxPhysics* physics, PxScene* scene, PxMaterial* material, vec3 a_spawnPos, float a_spawnRange)
{
	g_Physics = physics;
	g_PhysicsScene = scene;
	g_PhysicsMaterial = material;



	spawnPos = a_spawnPos;
	spawnRange = a_spawnRange;
}

void RagdollSpawner::SpawnRagdoll(vec4 color,float scale)
{
	PxArticulation* ragdoll;
	int multipleRange = spawnRange * 10;

	ragdoll = Ragdoll::makeRagdoll(g_Physics, ragdollData, 
		PxTransform(PxVec3(spawnPos.x + (((rand() % multipleRange) / 10.0f) - (spawnRange / 2.0f)), spawnPos.y, spawnPos.z + (((rand() % multipleRange) / 10.0f) - (spawnRange / 2.0f)))),
		scale, g_PhysicsMaterial);
	g_PhysicsScene->addArticulation(*ragdoll);

	ragdolls.push_back(ragdoll);
	colors.push_back(color);
}

void RagdollSpawner::Render()
{
	for (int i = 0; i < ragdolls.size(); i++)
	{
		PxArticulationLink* links [64];
		int linkCount=ragdolls[i]->getLinks(links,64);

		for (int j = 0; j < linkCount; j++)
		{
			physx::PxTransform cap_transform = links[j]->getGlobalPose();


			glm::vec3 pos(cap_transform.p.x, cap_transform.p.y, cap_transform.p.z);
			glm::quat q;

			q.x = cap_transform.q.x;
			q.y = cap_transform.q.y;
			q.z = cap_transform.q.z;
			q.w = cap_transform.q.w;

			glm::mat4 rot = glm::mat4(q);

			glm::mat4 model_matrix;
			model_matrix = rot * glm::translate(model_matrix, pos);


			PxShape* shape;
			links[j]->getShapes(&shape, 1);

			PxCapsuleGeometry capShape;
			shape->getCapsuleGeometry(capShape);

			Gizmos::addCapsule(pos, (capShape.halfHeight * 2), capShape.radius, 10, 10, colors[i], &rot);
		}


	}
}

void RagdollSpawner::reset()
{
	for (int i = 0; i < ragdolls.size(); i++)
	{
		ragdolls[i]->release();
	}

	ragdolls.clear();
	colors.clear();
}