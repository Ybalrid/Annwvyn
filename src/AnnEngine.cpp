#include "stdafx.h"
#include "AnnEngine.hpp"
#include "AnnLogger.hpp"
#include "AnnException.hpp"

using namespace Annwvyn;

AnnEngine* AnnEngine::singleton(nullptr);

bool AnnEngine::autosetProcessPriorityHigh(true);
bool AnnEngine::noConsoleColor(false);
bool AnnEngine::manualConsole(false);
std::string AnnEngine::logFileName{ "Annwvyn.log" };

#ifdef _WIN32
WORD AnnEngine::consoleGreen(0);
WORD AnnEngine::consoleYellow(0);
WORD AnnEngine::consoleWhite(0);
#endif

bool AnnEngine::consoleReady(false);
AnnEngineSingletonReseter::AnnEngineSingletonReseter(AnnEngine* address)
{
	engine = address;
}

AnnEngineSingletonReseter::~AnnEngineSingletonReseter()
{
	//Reset static members of friend class AnnEnigine that will outlive the object itself.
	engine->singleton = nullptr;
	engine->consoleReady = false;
}

void AnnEngine::setNoConsoleColor()
{
	noConsoleColor = true;
}

AnnEngine* AnnEngine::Instance()
{
	return singleton;
}

std::string AnnEngine::getAnnwvynVersion(long long int padding)
{
	std::stringstream version;
	version << ANN_MAJOR << "." << ANN_MINOR << "." << ANN_PATCH;
	if (ANN_EXPERIMENTAL)
		version << "-experimental";
	padding -= version.str().length();
	for (long long int i(0); i < padding; i++)
		version << " ";
	return version.str();
}

void AnnEngine::startGameplayLoop()
{
	while (refresh());
}

void AnnEngine::selectAndCreateRenderer(const std::string& hmdCommand, const std::string& title)
{
	std::cerr << "HMD selection from command line routine returned : "
		<< hmdCommand << std::endl;

	//Select the correct AnnOgreVRRenderer class to use :
#ifdef _WIN32
	//TODO ISSUE make the default switchable by the client user
	if (hmdCommand == "OgreOculusRender"
		|| hmdCommand == "OgreDefaultRender")
		renderer = std::make_shared<AnnOgreOculusRenderer>(title);
	else if (hmdCommand == "OgreOpenVRRender")
		renderer = std::make_shared<AnnOgreOpenVRRenderer>(title);
	else if (hmdCommand == "OgreNoVRRender")
		renderer = std::make_shared<AnnOgreNoVRRenderer>(title);
	else
	{
		displayWin32ErrorMessage(
			L"Error: Cannot understand VR System you want to use!",
			L"This program can be used with multiple VR solution.\n"
			L"The executable should be launched via a dedicated launcher.\n"
			L"If you're trying to launch it by hand, please check if your"
			L"command line parameter is correct!\n\n"
			L"Available command line parameter : \n"
			L"\t-rift\n"
			L"\t-vive\n"
			L"\nIf you don't specify anything, the default system will be used"
			L"(here it's the Oculus Rift)\n"
			L"If you don't have (or can't use) VR Hardware, you can launch with"
			L"-noVR.\n"
			L"This will display the image on a simple window without attempting"
			L"to talk to VR hardware"
		);
		throw AnnInitializationError(ANN_ERR_CANTHMD, "Can't find an HMD to use");
	}
#endif

#ifdef __linux__
	//No VR support currently
	renderer = std::make_shared<AnnOgreNoVRRenderer>(title);
#endif
}

AnnEngine::AnnEngine(const char title[], std::string hmdCommand) :
	resetGuard(this),
	applicationQuitRequested(false),
	renderer(nullptr),
	resourceManager(nullptr),
	sceneryManager(nullptr),
	filesystemManager(nullptr),
	audioEngine(nullptr),
	eventManager(nullptr),
	physicsEngine(nullptr),
	gameObjectManager(nullptr),
	levelManager(nullptr),
	player(nullptr),
	SceneManager(nullptr),
	vrRendererPovGameplayPlacement(nullptr),
	updateTime(-1)
{
	consoleReady = false;
#ifdef _WIN32

	//Windows specific setup

	//Set current process to high priority.
	//Looks like the scheduler of Windows sometimes don't give use the time we need to be consistent.
	//This seems to fixes the problem.
	if (autosetProcessPriorityHigh)
		setProcessPriorityHigh();

	consoleGreen = FOREGROUND_GREEN |
		FOREGROUND_INTENSITY;
	consoleYellow = FOREGROUND_GREEN |
		FOREGROUND_RED |
		FOREGROUND_INTENSITY;
	consoleWhite = FOREGROUND_RED |
		FOREGROUND_GREEN |
		FOREGROUND_BLUE |
		FOREGROUND_INTENSITY;
#endif

	if (singleton)
	{
		log("Can't create 2 instances of the engine!");
		throw AnnInitializationError(ANN_ERR_MEMORY, "Can't create 2 instances of AnnEngine");
	}
	singleton = this;

	stringUtility = std::make_shared<AnnStringUility>();

	selectAndCreateRenderer(hmdCommand, title);

	renderer->initOgreRoot(logFileName);

	player = std::make_shared< AnnPlayerBody>();
	renderer->initVrHmd();
	renderer->initPipeline();
	SceneManager = renderer->getSceneManager();

	renderer->showDebug(AnnOgreVRRenderer::DebugMode::MONOSCOPIC);

	log("Setup Annwvyn's subsystems");

	//Element on this list will be updated in order by the engine each frame
	SubSystemList.push_back(levelManager =
		std::make_shared<AnnLevelManager>());
	SubSystemList.push_back(gameObjectManager =
		std::make_shared<AnnGameObjectManager>());

	//Physics engine needs to be declared before the event manager. But we want the physics engine to be updated after the event manager.

	/*The wanted order is
	- physics is ticked (stuff move)
	- event happens (player input, timers...)
	- audio is synced (sounds comes form where they should)
	- then the game can redraw*/

	SubSystemList.push_back
	(physicsEngine = std::make_shared<AnnPhysicsEngine>(
		getSceneManager()->getRootSceneNode(),
		player));

	SubSystemList.push_back
	(eventManager = std::make_shared< AnnEventManager>
		(renderer->getWindow()));

	SubSystemList.push_back
	(audioEngine = std::make_shared< AnnAudioEngine>());

	//These could be anywhere
	SubSystemList.push_back
	(filesystemManager = std::make_shared<AnnFilesystemManager>
		(title));

	SubSystemList.push_back
	(resourceManager = std::make_shared<AnnResourceManager>());

	SubSystemList.push_back
	(sceneryManager = std::make_shared<AnnSceneryManager>
		(renderer));

	SubSystemList.push_back
	(scriptManager = std::make_shared<AnnScriptManager>());

	renderer->initClientHmdRendering();
	vrRendererPovGameplayPlacement = renderer->getCameraInformationNode();
	vrRendererPovGameplayPlacement->setPosition(player->getPosition() +
		AnnVect3(0.0f,
			player->getEyesHeight(),
			0.0f));

	//This subsystem need the vrRendererPovGameplayPlacement object to be
	//initialized. And the Resource manager because it wants a font file and an
	//image background
	SubSystemList.push_back(onScreenConsole = std::make_shared<AnnConsole>());

	consoleReady = true;
	//Display start banner
	log("============================================================", false);
	log("| Annwvyn Game Engine - Step into the Other World          |", false);
	log("| Free/Libre C++ Game Engine designed for Virtual Reality  |", false);
	log("|                                                          |", false);
	log("| Copyright Arthur Brainville (a.k.a. Ybalrid) 2013-2017   |", false);
	log("| Distributed under the terms of the MIT license agreement |", false);
	log("|                                                          |", false);
	log("| Visit http://annwvyn.org/ for more informations!         |", false);
	log("| Version : " + getAnnwvynVersion(61 - 13 - 1) + "|", false);
	log("============================================================", false);
}

AnnEngine::~AnnEngine()
{
	//Some cute log messages
	log("Game engine stopped. Subsystem are shutting down...");
	log("Good luck with the real world now! :3");
	consoleReady = false;
#ifdef _WIN32
	if (manualConsole) FreeConsole();
#endif
}

//All theses getter are for encapsulation purpose. Calling them directly would
//make very long lines of code. You can, but you should use the functions in
//AnnGetter.hpp
AnnEventManagerPtr AnnEngine::getEventManager() const
{
	return eventManager;
}

AnnResourceManagerPtr AnnEngine::getResourceManager() const
{
	return resourceManager;
}

AnnGameObjectManagerPtr AnnEngine::getGameObjectManager() const
{
	return gameObjectManager;
}

std::shared_ptr<AnnSceneryManager> AnnEngine::getSceneryManager() const
{
	return sceneryManager;
}

AnnScriptManagerPtr AnnEngine::getScriptManager() const
{
	return scriptManager;
}

AnnOgreVRRendererPtr AnnEngine::getVRRenderer() const
{
	return renderer;
}

AnnLevelManagerPtr AnnEngine::getLevelManager() const
{
	return levelManager;
}

AnnPlayerBodyPtr AnnEngine::getPlayer() const
{
	return player;
}

AnnFilesystemManagerPtr AnnEngine::getFileSystemManager() const
{
	return filesystemManager;
}

AnnAudioEnginePtr AnnEngine::getAudioEngine() const
{
	return audioEngine;
}

AnnPhysicsEnginePtr AnnEngine::getPhysicsEngine() const
{
	return physicsEngine;
}

//This is static, but actually needs Ogre to be running. So be careful
void AnnEngine::log(std::string message, bool flag)
{
	Ogre::String messageForLog;

	if (flag)
	{
#ifdef _WIN32
		if (!noConsoleColor)
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), consoleYellow);
#endif
		messageForLog += "Annwvyn - ";
	}
#ifdef _WIN32
	else
	{
		if (!noConsoleColor)
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), consoleGreen);
	}
#endif

	messageForLog += message;
	if (Ogre::LogManager::getSingletonPtr())
		Ogre::LogManager::getSingleton().logMessage(messageForLog);

	if (consoleReady)
		singleton->onScreenConsole->append(message);
#ifdef _WIN32
	if (!noConsoleColor)
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), consoleGreen);
#endif
}

//Need to be redone.
bool AnnEngine::requestStop() const
{
	//pres ESC to quit. Stupid but efficient. I like that.
	if (isKeyDown(OIS::KC_ESCAPE))
		return true;
	//If the user quit the App from the Oculus Home
	if (renderer->shouldQuit())
		return true;

	if (applicationQuitRequested)
		return true;

	return false;
}

// This is the most important function of the whole project. It's the heartbeat
// of the game or app using this engine.
bool AnnEngine::refresh()
{
	// Get the rendering delta time (should be roughly equals to
	// 1/desiredFramerate in seconds)
	//Update VR form real world
	syncPov();
	renderer->updateTracking();

	updateTime = renderer->getUpdateTime();
	player->engineUpdate(float(getFrameTime()));

	for (auto& SubSystem : SubSystemList)
		if (SubSystem->needUpdate())
			SubSystem->update();

	//Update view
	renderer->renderAndSubmitFrame();

	//Don't laugh
	return !requestStop();
}

// This just move a node where the other node is. Yes I know about parenting.
// I had reasons to do it that way, but I forgot.
void AnnEngine::syncPov() const
{
	vrRendererPovGameplayPlacement->setPosition(
		player->getPosition());
	vrRendererPovGameplayPlacement->setOrientation(
		player->getOrientation().toQuaternion());
}

//Bad. Don't use. Register an event listener and use the KeyEvent callback.
inline bool AnnEngine::isKeyDown(OIS::KeyCode key) const
{
	if (!eventManager) return false;
	return eventManager->Keyboard->isKeyDown(key);
}

Ogre::SceneNode* AnnEngine::getPlayerPovNode() const
{
	return vrRendererPovGameplayPlacement;
}

Ogre::SceneManager* AnnEngine::getSceneManager() const
{
	return SceneManager;
}

unsigned long AnnEngine::getTimeFromStartUp() const
{
	return renderer->getTimer()->getMilliseconds();
}

double AnnEngine::getTimeFromStartupSeconds() const
{
	return double(getTimeFromStartUp()) / 1000.0;
}

//the delta time of the last frame, not the current one
double AnnEngine::getFrameTime() const
{
	return updateTime;
}

// Raw position and orientation of the head in world space. This is useful if
// you want to mess around with weird stuff. This has been bodged when I
// integrated a LEAP motion in that mess.
AnnPose AnnEngine::getHmdPose() const
{
	if (renderer)
		return renderer->trackedHeadPose;
	return AnnPose();
}

std::shared_ptr<AnnUserSubSystem>
AnnEngine::registerUserSubSystem(std::shared_ptr<AnnUserSubSystem> userSystem)
{
	for (auto system : SubSystemList)
		if (userSystem->name == system->name)
		{
			AnnDebug() << "A subsystem with the name "
				<< userSystem->name
				<< "is already registered.";

			return nullptr;
		}
	SubSystemList.push_back(userSystem);
	return userSystem;
}

std::shared_ptr<AnnSubSystem> AnnEngine::getSubSystemByName(std::string name)
{
	for (auto subsystem : SubSystemList)
		if (subsystem->name == name)
			return subsystem;
	return nullptr;
}

bool AnnEngine::isUserSubSystem(std::shared_ptr<AnnSubSystem> subsystem)
{
	auto nakedSubSystem(subsystem.get());

	auto result = dynamic_cast<AnnUserSubSystem*>(nakedSubSystem);
	if (result != nullptr) return true;
	return false;
}

void AnnEngine::removeUserSubSystem(std::shared_ptr<AnnUserSubSystem> subsystem)
{
	SubSystemList.remove(subsystem);
}

//Because Windows and the Win32 platform sucks.
bool AnnEngine::openConsole()
{
	auto state{ true };
#ifdef _WIN32

	//Allocate a console for this app
	if (AllocConsole())
	{
		//put stdout on this console;
		FILE* f; freopen_s(&f, "CONOUT$", "w", stdout);
		if (!f) state = false;
		manualConsole = true;
	}

	//Redirect cerr to cout
	std::cerr.rdbuf(std::cout.rdbuf());

	SetConsoleTitle(L"Annwvyn Debug Console");
	if (!noConsoleColor)
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
			consoleWhite);

#endif
	return state;
}

bool AnnEngine::appVisibleInHMD() const
{
	if (renderer->isVisibleInHmd())
		return true;
	return false;
}

void AnnEngine::initPlayerStandingPhysics() const
{
	physicsEngine->initPlayerStandingPhysics(vrRendererPovGameplayPlacement);
}

void AnnEngine::initPlayerRoomscalePhysics() const
{
	physicsEngine->initPlayerRoomscalePhysics(vrRendererPovGameplayPlacement);
}

AnnConsolePtr AnnEngine::getOnScreenConsole() const
{
	return onScreenConsole;
}

AnnStringUtilityPtr AnnEngine::getStringUtility() const
{
	return stringUtility;
}

void AnnEngine::setProcessPriorityNormal()
{
#ifdef _WIN32
	SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);
#endif
}

void AnnEngine::setProcessPriorityHigh()
{
#ifdef _WIN32
	SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
#endif
}

void AnnEngine::requestQuit()
{
	applicationQuitRequested = true;
}