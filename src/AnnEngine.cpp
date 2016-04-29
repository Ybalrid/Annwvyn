#include "stdafx.h"
#include "AnnEngine.hpp"
#include "AnnLogger.hpp"
using namespace Annwvyn;

AnnEngine* AnnEngine::singleton(NULL);
AnnConsole* AnnEngine::onScreenConsole(NULL);
std::stringstream oss;
std::streambuf* old = nullptr;

AnnEngine* AnnEngine::Instance()
{
	return singleton;
}

std::string AnnEngine::getAnnwvynVersion()
{
	std::stringstream version;
	version << ANN_MAJOR << "." << ANN_MINOR << "." << ANN_PATCH;
	return version.str();
}

void AnnEngine::startGameplayLoop()
{
	while (refresh());
}

AnnEngine::AnnEngine(const char title[]) :
	eventManager(NULL),
	levelManager(NULL),
	povNode(NULL),
	defaultEventListener(NULL),
	canAccessSubSystems(true)
{
	if (singleton)
	{
		log("Can't create 2 instances of the engine!");
		exit(ANN_ERR_MEMORY);
	}
	srand(time(nullptr));
	singleton = this;

	//Launching initialisation routines : 
	//All Ogre related critical component is done inside the OgreOculusRenderer class. 
	renderer = new OgreOculusRender(title);
	renderer->initLibraries("Annwvyn.log");
	player = new AnnPlayer;
	renderer->getOgreConfig();
	renderer->createWindow();
	renderer->initScene();
	renderer->initCameras();
	renderer->setCamerasNearClippingDistance(0.15f);
	renderer->initRttRendering();
	SceneManager = renderer->getSceneManager();

	log("OGRE Object-oriented Graphical Rendering Engine initialized", true);

	renderer->showMonscopicView();

	log("Setup Annwvyn's subsystems");
	SubSystemList.push_back(levelManager = new AnnLevelManager);
	SubSystemList.push_back(gameObjectManager = new AnnGameObjectManager);
	SubSystemList.push_back(physicsEngine = new AnnPhysicsEngine(getSceneManager()->getRootSceneNode(), player, gameObjectManager->Objects, triggers));
	SubSystemList.push_back(eventManager = new AnnEventManager(renderer->getWindow()));
	SubSystemList.push_back(audioEngine = new AnnAudioEngine);
	SubSystemList.push_back(filesystemManager = new AnnFilesystemManager(title));
	SubSystemList.push_back(resourceManager = new AnnResourceManager);

	log("===================================================", false);
	log("Annwvyn Game Engine - Step into the Other World    ", false);
	log("Free/Libre Game Engine designed for Virtual Reality", false);
	log("Version : " + getAnnwvynVersion(), false);
	log("===================================================", false);
}

void Annwvyn::AnnEngine::destroyAllSubSystem()
{
	//Destroy all SubSystems
	for (auto SubSystem : SubSystemList)
	{
		if (SubSystem == physicsEngine) physicsEngine = nullptr;
		if (SubSystem == audioEngine) audioEngine = nullptr;
		delete SubSystem;
	}
	canAccessSubSystems = false;
}

AnnEngine::~AnnEngine()
{
	destroyAllSubSystem();

	//In case of orphan objects, do their cleanup here. 
	log("Destroying every objects remaining orphan object in engine");

	if (triggers.size() > 0) for (auto object : triggers) destroyTriggerObject(object);
	else log("Trigger list allready clean");
	if (lights.size() > 0) for (auto object : lights) destroyLightObject(object);
	else log("Light list allready clean");

	log("Game engine sucessfully destroyed.");
	log("Good luck with the real world now! :3");
	
	//Destroy the VR renderer
	delete renderer;

	//Forget AnnEngine static address
	singleton = nullptr;
}

AnnEventManager* AnnEngine::getEventManager()
{
	if (!canAccessSubSystems) return nullptr;
	return eventManager;
}


AnnResourceManager* AnnEngine::getResourceManager()
{
	if (!canAccessSubSystems) return nullptr;
	return resourceManager;
}

AnnGameObjectManager * AnnEngine::getGameObjectManager()
{
	if (!canAccessSubSystems) return nullptr;
	return gameObjectManager;
}



AnnLevelManager* AnnEngine::getLevelManager()
{
	if (!canAccessSubSystems) return nullptr;
	return levelManager;
}

AnnPlayer* AnnEngine::getPlayer()
{
	return player;
}

AnnFilesystemManager* AnnEngine::getFileSystemManager()
{
	if (!canAccessSubSystems) return nullptr;
	return filesystemManager;
}

////////////////////////////////////////////////////////// UTILITY
void AnnEngine::log(std::string message, bool flag)
{
	Ogre::String messageForLog;

	if (flag)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
		messageForLog += "Annwvyn - ";
	}
	else
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);

	messageForLog += message;
	Ogre::LogManager::getSingleton().logMessage(messageForLog);
	if (onScreenConsole)
		onScreenConsole->append(message);

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);
}

void AnnEngine::useDefaultEventListener()
{
	if (!eventManager) return;
	log("Reconfiguring the engine to use the default event listener");
	log("This unregister any current listener in use!");

	//Remove all event listeners
	eventManager->removeListener();

	//If the event listenre isn't allready initialized, allocate one
	if (!defaultEventListener)
		defaultEventListener = new AnnDefaultEventListener;

	//Set the default event listener to the event manager
	eventManager->addListener(defaultEventListener);
}

AnnDefaultEventListener* AnnEngine::getInEngineDefaultListener()
{
	return defaultEventListener;
}

void AnnEngine::initPlayerPhysics()
{
	physicsEngine->initPlayerPhysics(player, povNode);
}

void AnnEngine::oculusInit()
{
	log("Init Oculus rendering system");
	renderer->initOculus();
	povNode = renderer->getCameraInformationNode();
	povNode->setPosition(player->getPosition() +
		AnnVect3(0.0f, player->getEyesHeight(), 0.0f));
	SubSystemList.push_back(onScreenConsole = new AnnConsole());
}

void AnnEngine::destroyTriggerObject(AnnTriggerObject* object)
{
	triggers.remove(object);
	AnnDebug() << "Destroy trigger : " << (void*)object;
	delete object;
}

AnnLightObject* AnnEngine::createLightObject()
{
	log("Creating a light");
	AnnLightObject* Light = new AnnLightObject(SceneManager->createLight());
	Light->setType(AnnLightObject::LightTypes::ANN_LIGHT_POINT);
	lights.push_back(Light);
	return Light;
}

void AnnEngine::destroyLightObject(AnnLightObject* object)
{
	if (object)
		SceneManager->destroyLight(object->light);

	//Forget that this light existed
	lights.remove(object);
	delete object;
}

bool AnnEngine::requestStop()
{
	//pres ESC to quit. Stupid but efficient. I like that.
	if (isKeyDown(OIS::KC_ESCAPE))
		return true;
	//If the user quit the App from the Oculus Home
	if (renderer->getSessionStatus().ShouldQuit)
		return true;
	return false;
}

bool AnnEngine::refresh()
{
	//Get the rendering delta time (should be roughly equals to 1/desiredFramerate in seconds)
	updateTime = renderer->getUpdateTime();
	player->engineUpdate(getFrameTime());

	for (auto SubSystem : SubSystemList) 
		if (!SubSystem->needUpdate()) continue; 
		else SubSystem->update();

	//Update camera from player
	syncPov();

	//Update VR form real world
	renderer->updateTracking();


	renderer->renderAndSubmitFrame();

	return !requestStop();
}

inline void AnnEngine::syncPov()
{
	povNode->setPosition(player->getPosition());
	povNode->setOrientation(player->getOrientation().toQuaternion());
}


inline bool AnnEngine::isKeyDown(OIS::KeyCode key)
{
	if (!eventManager) return false;
	return eventManager->Keyboard->isKeyDown(key);
}

AnnTriggerObject* AnnEngine::createTriggerObject(AnnTriggerObject* object)
{
	assert(object);
	log("Creating a trigger object");
	triggers.push_back(object);
	object->postInit();
	return object;
}

AnnGameObject* AnnEngine::playerLookingAt()
{
	//Origin vector of the ray
	AnnVect3 Orig(getPoseFromOOR().position);

	//Caltulate direction Vector of the ray to be the midpont camera optical axis
	AnnVect3 LookAt(AnnQuaternion(getPoseFromOOR().orientation).getAtVector());

	//create ray
	Ogre::Ray ray(Orig, LookAt);

	//create query
	Ogre::RaySceneQuery* raySceneQuery(SceneManager->createRayQuery(ray));
	raySceneQuery->setSortByDistance(true);

	//execute and get the results
	Ogre::RaySceneQueryResult& result(raySceneQuery->execute());

	//read the result list
	for (auto it(result.begin()); it != result.end(); it++)
		if (it->movable && it->movable->getMovableType() == "Entity")
			return AnnGetGameObjectManager()->getFromNode(it->movable->getParentSceneNode());//Get the AnnGameObject that is attached to this SceneNode	

	return nullptr; //means that we don't know what the player is looking at.
}

void AnnEngine::resetOculusOrientation()
{
	log("Reseting the base direction of player's head");
	renderer->recenter();
}



Ogre::SceneNode* AnnEngine::getCamera()
{
	return povNode;
}

AnnAudioEngine* AnnEngine::getAudioEngine()
{
	if (!canAccessSubSystems) return nullptr;
	return audioEngine;
}

AnnPhysicsEngine* AnnEngine::getPhysicsEngine()
{
	if (!canAccessSubSystems) return nullptr;
	return physicsEngine;
}

Ogre::SceneManager* AnnEngine::getSceneManager()
{
	return SceneManager;
}

unsigned long AnnEngine::getTimeFromStartUp()
{
	return renderer->getTimer()->getMilliseconds();
}

double AnnEngine::getTimeFromStartupSeconds()
{
	return static_cast<double>(getTimeFromStartUp()) / 1000.0;
}

double AnnEngine::getFrameTime()
{
	return updateTime;
}

void AnnEngine::setDebugPhysicState(bool state)
{
	log("Activating debug drawing for physics engine");
	physicsEngine->setDebugPhysics(state);
}

void AnnEngine::setAmbiantLight(AnnColor color)
{
	AnnDebug() << "Setting the ambiant light to color " << color;
	SceneManager->setAmbientLight(color.getOgreColor());
}

void AnnEngine::setSkyDomeMaterial(bool activate, const char materialName[], float curvature, float tiling)
{
	log("Setting skydome from material"); log(materialName, false);
	SceneManager->setSkyDome(activate, materialName, curvature, tiling);
}

void AnnEngine::setSkyBoxMaterial(bool activate, const char materialName[], float distance, bool renderedFirst)
{
	log("Setting skybox from material"); log(materialName, false);
	SceneManager->setSkyBox(activate, materialName, distance, renderedFirst);
}

void AnnEngine::setWorldBackgroundColor(AnnColor v)
{
	AnnDebug() << "Setting the backgroud world color " << v;
	renderer->changeViewportBackgroundColor(v.getOgreColor());
}

void AnnEngine::removeSkyDome()
{
	log("Disabeling skydome");
	SceneManager->setSkyDomeEnabled(false);
}

void AnnEngine::removeSkyBox()
{
	log("Disabeling skybox");
	SceneManager->setSkyBoxEnabled(false);
}

void AnnEngine::setNearClippingDistance(Ogre::Real nearClippingDistance)
{
	AnnDebug() << "Setting the near clipping distance to " << nearClippingDistance;

	if (renderer)
		renderer->setCamerasNearClippingDistance(nearClippingDistance);
}

void AnnEngine::resetPlayerPhysics()
{
	if (!player->hasPhysics()) return;
	log("Reset player's physics");

	//Remove the player's rigidbody from the world
	physicsEngine->getWorld()->removeRigidBody(player->getBody());

	//We don't need that body anymore...
	delete player->getBody();
	//prevent memory access to unallocated address
	player->setBody(NULL);

	//Put everything back in order
	povNode->setPosition(player->getPosition());
	physicsEngine->createPlayerPhysicalVirtualBody(player, povNode);
	physicsEngine->addPlayerPhysicalBodyToDynamicsWorld(player);
}

OgrePose AnnEngine::getPoseFromOOR()
{
	if (renderer)
		return renderer->returnPose;
	return OgrePose();
}

void AnnEngine::openConsole()
{
#ifdef _WIN32
#if _MSC_VER == 1900

	//Allocate a console for this app
	if (AllocConsole())
	{
		//put stdout on this console;
#pragma warning(disable:4996)
		freopen("CONOUT$", "w", stdout);
#pragma warning(default:4996)

		SetConsoleTitle(L"Annwyn Debug Console");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
	}
	//Redirect cerr to cout
	std::cerr.rdbuf(std::cout.rdbuf());

#else
	int outHandle, errHandle, inHandle;
	FILE *outFile, *errFile, *inFile;
	AllocConsole();
	CONSOLE_SCREEN_BUFFER_INFO coninfo;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
	coninfo.dwSize.Y = 9999;
	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);

	outHandle = _open_osfhandle((long)GetStdHandle(STD_OUTPUT_HANDLE), _O_TEXT);
	errHandle = _open_osfhandle((long)GetStdHandle(STD_ERROR_HANDLE), _O_TEXT);
	inHandle = _open_osfhandle((long)GetStdHandle(STD_INPUT_HANDLE), _O_TEXT);

	outFile = _fdopen(outHandle, "w");
	errFile = _fdopen(errHandle, "w");
	inFile = _fdopen(inHandle, "r");

	*stdout = *outFile;
	*stderr = *errFile;
	*stdin = *inFile;

	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);
	setvbuf(stdin, NULL, _IONBF, 0);

	std::ios::sync_with_stdio();
#endif
#endif
}

void AnnEngine::toogleOnScreenConsole()
{
	if (onScreenConsole) onScreenConsole->toogle();
}

void AnnEngine::toogleOculusPerfHUD()
{
	if (renderer) renderer->cycleOculusHUD();
}

bool AnnEngine::appVisibleInHMD()
{
	if (renderer->getSessionStatus().IsVisible == ovrTrue)
		return true;
	return false;
}
