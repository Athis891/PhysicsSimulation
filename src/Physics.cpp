#include "Physics.h"

#include "gl_core_4_4.h"
#include "GLFW/glfw3.h"
#include "Gizmos.h"

#include "glm/gtc/quaternion.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <time.h>

#define Assert(val) if (val){}else{ *((char*)0) = 0;}
#define ArrayCount(val) (sizeof(val)/sizeof(val[0]))

using namespace physx;

void OnMouseButton(GLFWwindow*, int b, int a, int m) {
	TwEventMouseButtonGLFW(b, a);
}
void OnMousePosition(GLFWwindow*, double x, double y) {
	TwEventMousePosGLFW((int)x, (int)y);
}
void OnMouseScroll(GLFWwindow*, double x, double y) {
	TwEventMouseWheelGLFW((int)y);
}
void OnKey(GLFWwindow*, int k, int s, int a, int m) {
	TwEventKeyGLFW(k, a);
}
void OnChar(GLFWwindow*, unsigned int c) {
	TwEventCharGLFW(c, GLFW_PRESS);
}
void OnWindowResize(GLFWwindow*, int w, int h) {
	TwWindowSize(w, h);
	glViewport(0, 0, w, h);
}

class MyAllocator : public physx::PxAllocatorCallback
{
    virtual void* allocate(size_t size, const char* typeName, const char* filename, int line)
    {
        void* new_mem = malloc(size + 32);
        void* result = (char*)new_mem + (16 - ((size_t)new_mem % 16));
        Assert(((size_t)result % 16) == 0);
        *(void**)result = new_mem;
        return (char*)result + 16;
    }

    virtual void deallocate(void* ptr)
    {
        if (ptr)
        {
            void* real_ptr = *(void**)((char*)ptr - 16);
            free(real_ptr);
        }
    }
};

bool Physics::startup()
{
    if (Application::startup() == false)
    {
        return false;
    }

    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glEnable(GL_DEPTH_TEST);
	Gizmos::create(1 << 20, 1 << 20);

	srand(time(NULL));


    int screen_width, screen_height;
    glfwGetWindowSize(m_window, &screen_width, &screen_height);

    screen_size = glm::vec2(screen_width, screen_height);


    m_camera = FlyCamera(screen_size.x / screen_size.y, 10.0f);
    m_camera.setLookAt(vec3(10, 10, 10), vec3(0), vec3(0, 1, 0));
    m_camera.sensitivity = 3;

    setupPhysx();

	//add a plane
	physx::PxTransform pose =
		physx::PxTransform(physx::PxVec3(0.0f, 0, 0.0f),
		physx::PxQuat(physx::PxHalfPi * 1.0f,
		physx::PxVec3(0.0f, 0.0f, 1.0f)));


	physx::PxRigidStatic* plane =
		physx::PxCreateStatic(*g_Physics,
		pose,
		physx::PxPlaneGeometry(),
		*g_PhysicsMaterial);

	//add it to the physX scene
	g_PhysicsScene->addActor(*plane);


    setupTutorial1();

	resetFields = false;

	m_shapeSpawnWidget = new ShapeSpawnWidget(m_actorManager);

	TwInit(TW_OPENGL_CORE, nullptr);
	TwWindowSize(1280, 720);


	glfwSetMouseButtonCallback(m_window, OnMouseButton);
	glfwSetCursorPosCallback(m_window, OnMousePosition);
	glfwSetScrollCallback(m_window, OnMouseScroll);
	glfwSetKeyCallback(m_window, OnKey);
	glfwSetCharCallback(m_window, OnChar);
	glfwSetWindowSizeCallback(m_window, OnWindowResize);


	TwBar* bar = TwNewBar("Physx Simulation");

	TwAddButton(bar, "General", NULL, NULL, "label='General'");
	TwAddVarRO(bar, "FPS", TW_TYPE_FLOAT, &fps, "label='FPS'");
	TwAddVarRW(bar, "Reset Simulation", TW_TYPE_BOOL8, &resetFields, "label='Reset Simulation'");
	TwAddButton(bar, "", NULL, NULL, "label=' '");


	TwAddButton(bar, "Ragdoll Spawner", NULL, NULL, "label='Ragdoll Spawner'");
	TwAddVarRW(bar, "Spawn Position", TW_TYPE_DIR3F, &m_ragdollSpawner->spawnPos, "label='Spawn Position'");
	TwAddVarRW(bar, "Spawn Range", TW_TYPE_FLOAT, &m_ragdollSpawner->spawnRange, "label='Spawn Range'");
	TwAddVarRW(bar, "Spawn Ragdoll", TW_TYPE_BOOL8, &ragdollSpawn, "label='Spawn Ragdoll'");
	TwAddButton(bar, "", NULL, NULL, "label=' '");


	TwAddButton(bar, "Liquid Emitter", NULL, NULL, "label='Liquid Emitter'");
	TwAddVarRW(bar, "Particle Sp-Position", TW_TYPE_DIR3F, &m_fluidEmitter->position, "label='[R]Spawn Position'");
	TwAddVarRW(bar, "Emit Rate", TW_TYPE_FLOAT, &m_fluidEmitter->emitRate, "label='[R]Emit Rate'");
	TwAddVarRW(bar, "Liquid Color", TW_TYPE_COLOR4F, &m_fluidEmitter->m_particleColor, "label='Liquid Color'");
	TwAddVarRW(bar, "Liquid Borderless", TW_TYPE_BOOL8, &m_fluidEmitter->borderless, "label='Liquid Borderless'");
	TwAddVarRW(bar, "Pause Emission", TW_TYPE_BOOL8, &m_fluidEmitter->pauseEmission, "label='Pause Emission'");
	TwAddButton(bar, "", NULL, NULL, "label=' '");


	TwAddButton(bar, "Shape Spawning", NULL, NULL, "label='Shape Spawning'");
	TwAddVarRW(bar, "Shape Target", TW_TYPE_INT32, &m_shapeSpawnWidget->shapeTarget, "label='Shape Target'");
	TwAddVarRW(bar, "Is Dynamic", TW_TYPE_BOOL8, &m_shapeSpawnWidget->isDynamic, "label='Is Dynamic'");
	TwAddVarRW(bar, "Is Trigger", TW_TYPE_BOOL8, &m_shapeSpawnWidget->isTrigger, "label='Is Trigger'");
	TwAddVarRW(bar, "Spawn Position Shape", TW_TYPE_DIR3F, &m_shapeSpawnWidget->spawnPos, "label='Spawn Position'");
	TwAddVarRW(bar, "Box Constraints", TW_TYPE_DIR3F, &m_shapeSpawnWidget->boxConstraints, "label='Box Constraints'");
	TwAddVarRW(bar, "Radius", TW_TYPE_FLOAT, &m_shapeSpawnWidget->radius, "label='Radius'");
	TwAddVarRW(bar, "Capsule Height", TW_TYPE_FLOAT, &m_shapeSpawnWidget->capsuleHeight, "label='Capsule Height'");
	TwAddVarRW(bar, "Density", TW_TYPE_FLOAT, &m_shapeSpawnWidget->density, "label='Density'");
	TwAddVarRW(bar, "Spawn Shape", TW_TYPE_BOOL8, &m_shapeSpawnWidget->pendingCreate, "label='Spawn Shape'");

	TwAddButton(bar, "", NULL, NULL, "label=' '");





    return true;
}

void Physics::setupTutorial1()
{
	//

	if (m_actorManager != nullptr)
	{
		m_actorManager->reset();
	}
	else
	{
		m_actorManager = new ActorManager(g_Physics, g_PhysicsScene, g_PhysicsMaterial);
	}

	
	
	float density = 10;

	m_actorManager->createBox(false, vec4(0, 0, 1, 1), vec3(0, 0.5, 5), vec3(5, 0.5, 0.5), density,false);
	m_actorManager->createBox(false, vec4(0, 0, 1, 1), vec3(0, 0.5, -5), vec3(5, 0.5, 0.5), density,false);
	m_actorManager->createBox(false, vec4(0, 0, 1, 1), vec3(5, 0.5, 0), vec3(0.5, 0.5, 5), density,false);
	m_actorManager->createBox(false, vec4(0, 0, 1, 1), vec3(-5, 0.5, 0), vec3(0.5, 0.5, 5), density,false);


	if (m_fluidEmitter != nullptr)
	{
		m_fluidEmitter->reset();
	}
	else
	{
		m_fluidEmitter = new FluidEmitter(g_Physics, g_PhysicsScene, PxVec3(2, 0.5f, 3.2f), 100000, 0.006f, glm::vec4(143.0f / 255.0f, 179.0f / 255.0f, 178.0f / 255.0f, 1), true);
	}	
	m_fluidEmitter->pauseEmission = true;



	if (m_ragdollSpawner != nullptr)
	{
		m_ragdollSpawner->reset();
	}
	else
	{
		m_ragdollSpawner = new RagdollSpawner(g_Physics, g_PhysicsScene, g_PhysicsMaterial, vec3(0, 8, 0), 6);
	}


	if (m_playerController != nullptr)
	{
		m_playerController->reset();
	}
	else
	{
		m_playerController = new PlayerController(g_PhysicsScene, g_PhysicsMaterial, m_actorManager, PxExtendedVec3(0, 2, 0));
	}


	
	

}


void Physics::setupPhysx()
{
    physx::PxAllocatorCallback *myCallback = new MyAllocator();

    g_PhysicsFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, *myCallback, gDefaultErrorCallback);
    g_Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *g_PhysicsFoundation, physx::PxTolerancesScale());
    PxInitExtensions(*g_Physics);
    //create physics material  
    g_PhysicsMaterial = g_Physics->createMaterial(0.9f, 0.9f,.1f);
    physx::PxSceneDesc sceneDesc(g_Physics->getTolerancesScale());
    sceneDesc.gravity = physx::PxVec3(0, -10.0f, 0);
    sceneDesc.filterShader = &physx::PxDefaultSimulationFilterShader;
    sceneDesc.cpuDispatcher = physx::PxDefaultCpuDispatcherCreate(1);
    g_PhysicsScene = g_Physics->createScene(sceneDesc);





	/*

	// check if PvdConnection manager is available on this platform
	if (g_Physics->getPvdConnectionManager() == NULL)
		return;
	// setup connection parameters
	const char* pvd_host_ip = "127.0.0.1";
	// IP of the PC which is running PVD
	int port = 5425;
	// TCP port to connect to, where PVD is listening
	unsigned int timeout = 100;
	// timeout in milliseconds to wait for PVD to respond,
	//consoles and remote PCs need a higher timeout.
	PxVisualDebuggerConnectionFlags connectionFlags = PxVisualDebuggerExt::getAllConnectionFlags();
	// and now try to connectPxVisualDebuggerExt
	auto theConnection = PxVisualDebuggerExt::createConnection(
	g_Physics->getPvdConnectionManager(), pvd_host_ip, port, timeout, connectionFlags);


	*/


	PxSimulationEventCallback* mycollisionCallBack = new MycollisionCallBack();
	g_PhysicsScene->setSimulationEventCallback(mycollisionCallBack);
}

void Physics::shutdown()
{

    g_PhysicsScene->release();
    g_Physics->release();
    g_PhysicsFoundation->release();


    Gizmos::destroy();
    Application::shutdown();
}

bool Physics::update()
{
    if (Application::update() == false)
    {
        return false;
    }

    int screen_width, screen_height;
    glfwGetWindowSize(m_window, &screen_width, &screen_height);
    if (screen_width != screen_size.x || screen_height != screen_size.y)
    {
        glViewport(0, 0, screen_width, screen_height);
        m_camera.proj = 
            glm::perspective(glm::radians(60.0f), screen_size.x / screen_size.y, 0.1f, 1000.0f);
    }




    Gizmos::clear();

    float dt = (float)glfwGetTime();
    glfwSetTime(0.0);

	fps = 1.0f / dt;




	GLFWwindow* curr_window = glfwGetCurrentContext();


	if (ragdollSpawn == true)
	{
		m_ragdollSpawner->SpawnRagdoll(vec4(((rand() % 255) / 255.0f), ((rand() % 255) / 255.0f), ((rand() % 255) / 255.0f), 1), 0.1f);
		ragdollSpawn = false;
	}

		
	if (resetFields == true)
	{
		resetFields = false;
		setupTutorial1();
	}


	m_shapeSpawnWidget->UpdateAndRender();

	m_fluidEmitter->UpdateAndRender(dt);
	m_ragdollSpawner->Render();
	
	m_playerController->Update(dt);

    vec4 white(1);
    vec4 black(0, 0, 0, 1);

    for (int i = 0; i <= 20; ++i)
    {
        Gizmos::addLine(vec3(-10 + i, -0.01, -10), vec3(-10 + i, -0.01, 10),
            i == 10 ? white : black);
        Gizmos::addLine(vec3(-10, -0.01, -10 + i), vec3(10, -0.01, -10 + i),
            i == 10 ? white : black);
    }

    float phys_dt = dt;// *0.01f;
    if (phys_dt > 1.0f / 30.f)
    {
        phys_dt = 1.0f / 30.0f;
    }

    g_PhysicsScene->simulate(phys_dt);
    while (g_PhysicsScene->fetchResults() == false)
    {

    }

	m_actorManager->Render();
		

    m_camera.update(dt);
    
    return true;
}

void Physics::draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Gizmos::draw(m_camera.proj, m_camera.view);
    Gizmos::draw2D(glm::ortho(-1280/2.0f,1280.0f/2.0f,-720.0f/2.0f,720.0f/2.0f));

	TwDraw();

    glfwSwapBuffers(m_window);
    glfwPollEvents();
}