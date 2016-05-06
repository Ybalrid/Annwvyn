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
	eventManager(nullptr),
	levelManager(nullptr),
	povNode(nullptr),
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
	SubSystemList.push_back(levelManager = new AnnLevelManager);
	SubSystemList.push_back(gameObjectManager = new AnnGameObjectManager);
	SubSystemList.push_back(physicsEngine = new AnnPhysicsEngine(getSceneManager()->getRootSceneNode(), player, gameObjectManager->Objects, gameObjectManager->Triggers));
	SubSystemList.push_back(eventManager = new AnnEventManager(renderer->getWindow()));
	SubSystemList.push_back(audioEngine = new AnnAudioEngine);
	SubSystemList.push_back(filesystemManager = new AnnFilesystemManager(title));
	SubSystemList.push_back(resourceManager = new AnnResourceManager);
	SubSystemList.push_back(sceneryManager = new AnnSceneryManager(renderer));

	log("Init VR rendering system");
	renderer->initClientHmdRendering();
	povNode = renderer->getCameraInformationNode();
	povNode->setPosition(player->getPosition() +
		AnnVect3(0.0f, player->getEyesHeight(), 0.0f));

	//This subsystem need the povNode object to be initialized.
	SubSystemList.push_back(onScreenConsole = new AnnConsole());

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

AnnSceneryManager * Annwvyn::AnnEngine::getSceneryManager()
{
	if (!canAccessSubSystems) return nullptr;
	return sceneryManager;
}

OgreVRRender * Annwvyn::AnnEngine::getVRRenderer()
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


void AnnEngine::initPlayerPhysics()
{
	physicsEngine->initPlayerPhysics(player, povNode);
}

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

	//Update view
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

OgrePose AnnEngine::getHmdPose()
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

	SetConsoleTitle(L"Annwyn Debug Console");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);

#endif
}

void AnnEngine::toogleOnScreenConsole()
{
	if (onScreenConsole) onScreenConsole->toogle();
}

void AnnEngine::toogleOculusPerfHUD()
{
	if (renderer) renderer->cycleDebugHud();
}

bool AnnEngine::appVisibleInHMD()
{
	if (renderer->isVisibleInHmd() == true)
		return true;
	return false;
}
