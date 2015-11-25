#include "PlayerController.h"


void ControllerHitReport::onShapeHit(const PxControllerShapeHit &hit)
{
	//gets a reference to a structure which tells us what has been hit and where
	//get the acter from the shape we hit
	PxRigidActor* actor = hit.shape->getActor();
	//get the normal of the thing we hit and store it so the player controller can	respond correctly
	_playerContactNormal = hit.worldNormal;
	//try to cast to a dynamic actor
	PxRigidDynamic* myActor = actor->is<PxRigidDynamic>();
	if (myActor)
	{

		PxShape* shape;

		myActor->getShapes(&shape, 1);

		//this is where we can apply forces to things we hit
		if (shape->getSphereGeometry(PxSphereGeometry()))
		{
			myActor->addForce(_playerContactNormal * 1000);
		}
		else
		{
			myActor->addForce(_playerContactNormal * -1000);
		}
		
		
	}
}


PlayerController::PlayerController(PxScene* PhysicsScene,PxMaterial* PhysicsMaterial,ActorManager* a_actorManager,PxExtendedVec3 a_startingPosition)
{
	g_PhysicsScene = PhysicsScene;
	g_PhysicsMaterial = PhysicsMaterial;
	actorManager = a_actorManager;
	startingPosition = a_startingPosition;

	jumpCooldown = 0.3f;
	currentJumpCooldown = 0;

	initController();
}

void PlayerController::reset()
{
	initController();
}
void PlayerController::initController()
{
	m_hitReport = new ControllerHitReport();
	gCharacterManager = PxCreateControllerManager(*g_PhysicsScene);

	//describe our controller...
	PxCapsuleControllerDesc desc;
	desc.height = 1.6f;
	desc.radius = 0.4f;
	desc.position.set(0, 0, 0);
	desc.material = g_PhysicsMaterial;
	desc.reportCallback = m_hitReport; //connect it to our collision detection routine
	desc.density = 10;

	//create the layer controller
	gPlayerController = gCharacterManager->createController(desc);
	gPlayerController->setPosition(startingPosition);

	//set up some variables to control our player with
	characterYVelocity = 0; //initialize character velocity
	characterRotation = 0; //and rotation
	playerGravity = -0.07f; //set up the player gravity

	m_hitReport->clearPlayerContactNormal(); //initialize the contact normal (what we are in contact with)


	actorManager->addDynamicActor(gPlayerController->getActor(), vec4(((rand() % 255) / 255.0f), ((rand() % 255) / 255.0f), ((rand() % 255) / 255.0f), 1));
}

void PlayerController::Update(float a_deltaTime)
{
	bool onGround; //set to true if we are on the ground
	float movementSpeed = 10.0f; //forward and back movement speed
	float rotationSpeed = 3.0f; //turn speed
	//check if we have a contact normal. if y is greater than .3 we assume this is	solid ground.This is a rather primitive way to do this.Can you do better ?
	if (m_hitReport->getPlayerContactNormal().y > 0.3f)
	{
		characterYVelocity = -0.1f;
		onGround = true;
	}
	else
	{
		characterYVelocity += playerGravity * a_deltaTime;
		onGround = false;
	}
	m_hitReport->clearPlayerContactNormal();
	const PxVec3 up(0, 1, 0);
	//scan the keys and set up our intended velocity based on a global transform
	PxVec3 velocity(0, characterYVelocity, 0);

	GLFWwindow* curr_window = glfwGetCurrentContext();

	currentJumpCooldown -= a_deltaTime;

	if (glfwGetKey(curr_window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		velocity.x -= movementSpeed*a_deltaTime;
	}
	if (glfwGetKey(curr_window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		velocity.x += movementSpeed*a_deltaTime;
	}

	if (glfwGetKey(curr_window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		characterRotation -= rotationSpeed*a_deltaTime;
	}
	if (glfwGetKey(curr_window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		characterRotation += rotationSpeed*a_deltaTime;
	}

	float jumpSpeed = 0.25f;

	if (persistanceCd > 0)
	{
		PxExtendedVec3  pos = gPlayerController->getPosition();

		pos.y += jumpSpeed;
		gPlayerController->setPosition(pos);

		persistanceCd -= a_deltaTime;

	}

	if (glfwGetKey(curr_window, GLFW_KEY_SPACE) == GLFW_PRESS&&currentJumpCooldown<=0)
	{
		PxExtendedVec3  pos = gPlayerController->getPosition();

		pos.y += jumpSpeed;
		gPlayerController->setPosition(pos);

		currentJumpCooldown = jumpCooldown;
		persistanceCd = 0.12f;
	}



	//To do.. add code to control z movement and jumping
	float minDistance = 0.001f;
	PxControllerFilters filter;
	//make controls relative to player facing
	PxQuat rotation(characterRotation, PxVec3(0, 1, 0));
	//move the controller
	gPlayerController->move(rotation.rotate(velocity), minDistance, a_deltaTime,
		filter);
}