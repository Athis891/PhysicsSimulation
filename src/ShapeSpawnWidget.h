#pragma once

#include "ActorManager.h"
#include <time.h>
#include "Gizmos.h"

class ShapeSpawnWidget
{
public:
	ShapeSpawnWidget(ActorManager* actorManager);
	~ShapeSpawnWidget();

	void UpdateAndRender();

	//shape specifications
	int shapeTarget;
	bool isDynamic;
	vec3 spawnPos;
	vec3 boxConstraints;
	float radius;
	float capsuleHeight;
	float density;
	bool isTrigger;

	bool pendingCreate;
private:
	void SpawnActor();

	ActorManager* m_actorManager;

};