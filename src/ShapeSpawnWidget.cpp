#include "ShapeSpawnWidget.h"

ShapeSpawnWidget::ShapeSpawnWidget(ActorManager* actorManager)
{
	shapeTarget = 1;
	isDynamic = true;
	spawnPos = vec3(1, 8, 1);
	boxConstraints = vec3(1);
	radius = 1;
	capsuleHeight = 2;
	density = 10;
	pendingCreate = false;
	isTrigger = false;

	m_actorManager = actorManager;

	srand(time(NULL));
}

void ShapeSpawnWidget::UpdateAndRender()
{
	if (pendingCreate == true)
	{
		pendingCreate = false;
		SpawnActor();
	}

	if (shapeTarget == 1)
	{
		Gizmos::addAABBFilled(spawnPos, boxConstraints, vec4(1));
	}
	else if (shapeTarget == 2)
	{
		Gizmos::addSphereFilled(spawnPos, radius, 15, 15, vec4(1));
	}
	else if (shapeTarget == 3)
	{
		Gizmos::addCapsule(spawnPos, capsuleHeight, radius, 15, 15, vec4(1));
	}


}

void ShapeSpawnWidget::SpawnActor()
{
	vec4 color = vec4(((rand() % 255) / 255.0f), ((rand() % 255) / 255.0f), ((rand() % 255) / 255.0f), 1);

	if (shapeTarget == 1)
	{
		m_actorManager->createBox(isDynamic, color, spawnPos, boxConstraints, density,isTrigger);
	}
	else if (shapeTarget == 2)
	{
		m_actorManager->createSphere(isDynamic, color, spawnPos, radius, density,isTrigger);
	}
	else if (shapeTarget == 3)
	{
		m_actorManager->createCapsule(isDynamic, color, spawnPos, radius, capsuleHeight, density,isTrigger);
	}
}

