#include "stdafx.h"
#include "AnnEngine.hpp"
#include "AnnLogger.hpp"
using namespace Annwvyn;

AnnEngine* AnnEngine::singleton(NULL);
std::shared_ptr<AnnConsole> AnnEngine::onScreenConsole(nullptr);

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

AnnEngine::AnnEngine(const char title[], std::string hmdCommand) :
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

	std::cerr << "HMD selection from command line routine retuned : " << hmdCommand << std::endl;

	//Launching initialisation routines : 
	if (hmdCommand == "OgreOculusRender"
		|| hmdCommand == "OgreDefaultRender")
	{
		renderer = std::make_shared<OgreOculusRender>(title);
	}
	///else if vive
	else if (hmdCommand == "OgreOpenVRRender")
	{
		MessageBox(NULL,
				   L"The Vive rendering is not implemented yet.\n"
				   L"Sorry for that. ^^\"",
				   L"Error: Vive not implemented. Yet ;-)",
				   MB_ICONERROR);
		exit(ANN_ERR_CANTHMD);
	}
	///else if osvr
	///else if ...
	else
	{
		MessageBox(NULL,
				   L"This program can be used with multiple VR sollution.\n"
				   L"The executable should be launched from the intended launcher.\n"
				   L"If you're trying to launch it by hand, please check if your command line parameter is correct!\n\n"
				   L"Availaible command line parameter : \n\t-oculus\n",
				   L"Error: Cannot understand what HMD you want to use!",
				   MB_ICONERROR);
		exit(ANN_ERR_CANTHMD);
	}

	renderer->initOgreRoot("Annwvyn.log");


	srand(time(nullptr));
	singleton = this;

	player = std::make_shared< AnnPlayer>();
	renderer->initVrHmd();
	renderer->initPipeline();
	SceneManager = renderer->getSceneManager();

	renderer->showDebug(OgreVRRender::DebugMode::MONOSCOPIC);

	log("Setup Annwvyn's subsystems");

	//Element on this list will be updated in order by the engine each frame
	SubSystemList.push_back(levelManager = std::make_shared<AnnLevelManager>());
	SubSystemList.push_back(gameObjectManager = std::make_shared<AnnGameObjectManager>());

	//Physics engine needs to be declared before the event manager. But we want the physics engine to be updated after the event manager.


	/*The wanted order is 
	- event happens (player input, timers...)
	- physics is ticked (stuff move)
	- audio is synced (sonds comes form where they should)
	- then the game can redraw*/

	physicsEngine = std::make_shared<AnnPhysicsEngine>(getSceneManager()->getRootSceneNode(), player, gameObjectManager->Objects, gameObjectManager->Triggers);
	SubSystemList.push_back(eventManager = std::make_shared< AnnEventManager>(renderer->getWindow()));
	SubSystemList.push_back(physicsEngine);
	SubSystemList.push_back(audioEngine = std::make_shared< AnnAudioEngine>());

	
	//These could be anywere
	SubSystemList.push_back(filesystemManager = std::make_shared<AnnFilesystemManager>(title));
	SubSystemList.push_back(resourceManager = std::make_shared<AnnResourceManager>());
	SubSystemList.push_back(sceneryManager = std::make_shared<AnnSceneryManager>(renderer));


	renderer->initClientHmdRendering();
	povNode = renderer->getCameraInformationNode();
	povNode->setPosition(player->getPosition() +
		AnnVect3(0.0f, player->getEyesHeight(), 0.0f));

	//This subsystem need the povNode object to be initialized. And the Resource manager because it wants a font file and an image background 
	SubSystemList.push_back(onScreenConsole = std::make_shared<AnnConsole>());

	log("===================================================", false);
	log("Annwvyn Game Engine - Step into the Other World    ", false);
	log("Free/Libre Game Engine designed for Virtual Reality", false);
	log("Version : " + getAnnwvynVersion(), false);
	log("===================================================", false);
}

AnnEngine::~AnnEngine()
{
	//Some cute log messsages
	log("Game engine stopped. Subsystem are shutting down...");
	log("Good luck with the real world now! :3");
}

//All theses getter are for encapsulation purpose. Calling them directly would make verry long lines of code. Note that there's a whole bunch of macro in AnnEngine.hpp to help with that
std::shared_ptr<AnnEventManager> AnnEngine::getEventManager()
{
	return eventManager;
}

std::shared_ptr<AnnResourceManager> AnnEngine::getResourceManager()
{
	return resourceManager;
}

std::shared_ptr<AnnGameObjectManager> AnnEngine::getGameObjectManager()
{
	return gameObjectManager;
}

std::shared_ptr<AnnSceneryManager> AnnEngine::getSceneryManager()
{
	return sceneryManager;
}

std::shared_ptr<OgreVRRender> AnnEngine::getVRRenderer()
{
	return renderer;
}

std::shared_ptr<AnnLevelManager> AnnEngine::getLevelManager()
{
	return levelManager;
}

std::shared_ptr<AnnPlayer> AnnEngine::getPlayer()
{
	return player;
}

std::shared_ptr<AnnFilesystemManager> AnnEngine::getFileSystemManager()
{
	return filesystemManager;
}


std::shared_ptr<AnnAudioEngine> AnnEngine::getAudioEngine()
{
	return audioEngine;
}

std::shared_ptr<AnnPhysicsEngine> AnnEngine::getPhysicsEngine()
{
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
	if (onScreenConsole != nullptr)
		onScreenConsole->append(message);

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);
}

//Don't ask me why this is not part of the Physics engine. Actually it just calls something on the physics engine. Will be probably deleted in the future.
void AnnEngine::initPlayerPhysics()
{
	physicsEngine->initPlayerPhysics(povNode);
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
