#include "stdafx.h"
#include "AnnEngine.hpp"
#include "AnnLogger.hpp"
using namespace Annwvyn;

AnnEngine* AnnEngine::singleton(NULL);
AnnConsole* AnnEngine::onScreenConsole(NULL);

AnnEngine* AnnEngine::Instance()
{
	return singleton;
}

std::string AnnEngine::getAnnwvynVersion()
{
	std::stringstream version;
	version << ANN_MAJOR << "." << ANN_MINOR << "." << ANN_PATCH;
	if (ANN_EXPERIMENTAL)
		version << "-experimental";
	return version.str();
}

void AnnEngine::startGameplayLoop()
{
	while (refresh());
}

AnnEngine::AnnEngine(const char title[]) :
	player(nullptr),
	audioEngine(nullptr),
	eventManager(nullptr),
	levelManager(nullptr),
	filesystemManager(nullptr),
	resourceManager(nullptr),
	gameObjectManager(nullptr),
	sceneryManager(nullptr),
	renderer(nullptr),
	povNode(nullptr),
	updateTime(-1),
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
	renderer->initOgreRoot("Annwvyn.log");

	player = new AnnPlayer;
	renderer->initVrHmd();
	renderer->initPipeline();
	SceneManager = renderer->getSceneManager();

	renderer->showDebug(OgreVRRender::DebugMode::MONOSCOPIC);

	log("Setup Annwvyn's subsystems");

	//Element on this list will be updated in order by the engine each frame
	SubSystemList.push_back(levelManager = new AnnLevelManager);
	SubSystemList.push_back(gameObjectManager = new AnnGameObjectManager);

	//Physics engine needs to be declared before the event manager. But we want the physics engine to be updated after the event manager.


	/*The wanted order is 
	- event happens (player input, timers...)
	- physics is ticked (stuff move)
	- audio is synced (sonds comes form where they should)
	- then the game can redraw*/

	physicsEngine = new AnnPhysicsEngine(getSceneManager()->getRootSceneNode(), player, gameObjectManager->Objects, gameObjectManager->Triggers);
	SubSystemList.push_back(eventManager = new AnnEventManager(renderer->getWindow()));
	SubSystemList.push_back(physicsEngine);
	SubSystemList.push_back(audioEngine = new AnnAudioEngine);

	
	//These could be anywere
	SubSystemList.push_back(filesystemManager = new AnnFilesystemManager(title));
	SubSystemList.push_back(resourceManager = new AnnResourceManager);
	SubSystemList.push_back(sceneryManager = new AnnSceneryManager(renderer));


	renderer->initClientHmdRendering();
	povNode = renderer->getCameraInformationNode();
	povNode->setPosition(player->getPosition() +
		AnnVect3(0.0f, player->getEyesHeight(), 0.0f));

	//This subsystem need the povNode object to be initialized. And the Resource manager because it wants a font file and an image background 
	SubSystemList.push_back(onScreenConsole = new AnnConsole());

	log("===================================================", false);
	log("Annwvyn Game Engine - Step into the Other World    ", false);
	log("Free/Libre Game Engine designed for Virtual Reality", false);
	log("Version : " + getAnnwvynVersion(), false);
	log("===================================================", false);
}

void AnnEngine::destroyAllSubSystem()
{
	//Destroy all SubSystems
	for (auto SubSystem : SubSystemList)
	{
		/*Some part of the engine will check for theses pointers to be null or not. This can cause segfaults if we delete them without putting them at null, even in the "destroyAllSubsystems" context
		TODO:Find a way for subsystemps to be destroyed without calling others subsystems (non critical)*/
		if (SubSystem == physicsEngine) physicsEngine = nullptr;
		if (SubSystem == audioEngine) audioEngine = nullptr;
		delete SubSystem;
	}
	//This is another safegard. Sorry for the spaghettiness of this.
	canAccessSubSystems = false;
}

AnnEngine::~AnnEngine()
{
	//Destroy all parts of the engine
	destroyAllSubSystem();

	//Some cute log messsages
	log("Game engine sucessfully destroyed.");
	log("Good luck with the real world now! :3");
	
	//Destroy the VR renderer
	delete renderer;

	//Forget AnnEngine static address
	singleton = nullptr;

	//At this point, you *could* recreate an instance of the engine. But I don't know why you would want that.
}

//All theses getter are for encapsulation purpose. Calling them directly would make verry long lines of code. Note that there's a whole bunch of macro in AnnEngine.hpp to help with that
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

AnnSceneryManager * AnnEngine::getSceneryManager()
{
	if (!canAccessSubSystems) return nullptr;
	return sceneryManager;
}

OgreVRRender * AnnEngine::getVRRenderer()
{
	return renderer;
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

//This is static, but actually needs Ogre to be running. So be carefull
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

//Don't ask me why this is not part of the Physics engine. Actually it just calls something on the physics engine. Will be probably deleted in the future.
void AnnEngine::initPlayerPhysics()
{
	physicsEngine->initPlayerPhysics(player, povNode);
}

//Need to be redone. 
bool AnnEngine::requestStop()
{
	//pres ESC to quit. Stupid but efficient. I like that.
	if (isKeyDown(OIS::KC_ESCAPE))
		return true;
	//If the user quit the App from the Oculus Home
	if (renderer->shouldQuit())
		return true;
	return false;
}

//This is the most important function of the whole project. It's the heartbeat of the game or app using this engine.
bool AnnEngine::refresh()
{
	//Get the rendering delta time (should be roughly equals to 1/desiredFramerate in seconds)
	updateTime = renderer->getUpdateTime();
	player->engineUpdate(getFrameTime());

	for (auto SubSystem : SubSystemList) 
		if (!SubSystem->needUpdate()) continue; //If doen't need update, swith to the next
		else SubSystem->update();				//The "else" keyword is used to not put curly braces, by lazyness and by code style.

	//Update camera from player
	syncPov();

	//Update VR form real world
	renderer->updateTracking();

	//Update view
	renderer->renderAndSubmitFrame();

	//Don't laugh
	return !requestStop();
}

//This just move a node where the other node is. Yes I know about parenting. I had reasons to do it that way, but I forgot. 
inline void AnnEngine::syncPov()
{
	povNode->setPosition(player->getPosition());
	povNode->setOrientation(player->getOrientation().toQuaternion());
}

//Bad. Don't use. Register an event listener and use the KeyEvent callback. 
inline bool AnnEngine::isKeyDown(OIS::KeyCode key)
{
	if (!eventManager) return false;
	return eventManager->Keyboard->isKeyDown(key);
}

Ogre::SceneNode* AnnEngine::getCamera()
{
	return povNode;
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

//the delta time of the last frame, not the current one
double AnnEngine::getFrameTime()
{
	return updateTime;
}

//Raw position and orientaiton of the head in world space. This is usefull if you want to mess around with weird stuff. This has been bodged when I integrated a LEAP motion in that mess. 
OgrePose AnnEngine::getHmdPose()
{
	if (renderer)
		return renderer->returnPose;
	return OgrePose();
}

//Because Windows and the Win32 platform sucks. 
void AnnEngine::openConsole()
{
#ifdef _WIN32

	//Allocate a console for this app
	if (AllocConsole())
	{
		//put stdout on this console;
#pragma warning(disable:4996) //Okay, so for some reason, freopen is "bad" because potentially dangerous. However, since I'm passing static strings here, unless you go hack the DLL, I don't know what harm you can do.
		freopen("CONOUT$", "w", stdout); 
#pragma warning(default:4996)
	}

	//Redirect cerr to cout
	std::cerr.rdbuf(std::cout.rdbuf());

	SetConsoleTitle(L"Annwyn Debug Console");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);

#endif
}

//Well, I may make the pointer to the onScreenConsole more accessible. 
void AnnEngine::toogleOnScreenConsole()
{
	if (onScreenConsole) onScreenConsole->toogle();
}

bool AnnEngine::appVisibleInHMD()
{
	if (renderer->isVisibleInHmd() == true) // why "== true" ? Because at some point it was returning an ovrBool, wich is the boolean type of the oculus SDK that doesn't cast correctly to a C++ bool. YES. I DON'T KNOW HOW THEY MANAGED TO FFFF TAHT UP. 
		return true;
	return false;
}
