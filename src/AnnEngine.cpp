// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "stdafx.h"
#include "AnnEngine.hpp"
#include "AnnLogger.hpp"
#include "AnnException.hpp"

#include "AnnOgreOculusRenderer.hpp"

//Graphic rendering system for the vive
#include "AnnOgreOpenVRRenderer.hpp"

//Graphic rendering system for NO FREAKING VR SYSTEM
#include "AnnOgreNoVRRenderer.hpp"

using namespace Annwvyn;

AnnEngine* AnnEngine::singleton{ nullptr };

bool AnnEngine::autosetProcessPriorityHigh{ true };
bool AnnEngine::noConsoleColor{ false };
bool AnnEngine::consoleReady{ false };
bool AnnEngine::manualConsole{ false };
std::string AnnEngine::logFileName{ "Annwvyn.log" };
std::string AnnEngine::defaultRenderer{ "NoVRRender" };

#ifdef _WIN32
WORD AnnEngine::consoleGreen{ 0 };
WORD AnnEngine::consoleYellow{ 0 };
WORD AnnEngine::consoleWhite{ 0 };
#endif

AnnEngineSingletonReseter::AnnEngineSingletonReseter(AnnEngine* address)
{
	engine = address;
}

AnnEngineSingletonReseter::~AnnEngineSingletonReseter()
{
	//Reset static members of friend class AnnEnigine that will outlive the object itself.
	engine->singleton	= nullptr;
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

std::string AnnEngine::getAnnwvynVersion(size_t padding)
{
	static std::string versionString{ "" };
	if(!versionString.empty() && !padding) return versionString;

	if(versionString.empty())
	{
		std::stringstream version;
		version << ANN_MAJOR << "." << ANN_MINOR << "." << ANN_PATCH;
		if(ANN_EXPERIMENTAL)
			version << "-experimental";
		versionString = version.str();
	}

	if(padding)
	{
		auto padded = versionString;

		padding -= padded.length();
		for(auto i(0); i < padding; i++)
			padded.push_back(' ');

		return padded;
	}

	return versionString;
}

void AnnEngine::startGameplayLoop()
{
	while(refresh()) {}
}

void AnnEngine::selectAndCreateRenderer(const std::string& hmdCommand, const std::string& title)
{
	std::cerr << "HMD selection from command line routine returned : "
			  << hmdCommand << std::endl;

	//Select the correct AnnOgreVRRenderer class to use :

	if(hmdCommand == "DefaultRender" && (!defaultRenderer.empty() && (defaultRenderer != "DefaultRender")))
	{
		std::cerr << "Using the default renderer " << defaultRenderer << " as HMD selector\n";
		std::cerr << "Re-running the renderer selection test..\n";
		selectAndCreateRenderer(defaultRenderer, title);
		return;
	}

	auto set{ false };
#ifdef _WIN32
	if(hmdCommand == "OculusRender")
	{
		std::cerr << "Using Oculus...\n";
		renderer = std::make_shared<AnnOgreOculusRenderer>(title);
		set		 = true;
	}
#endif
	if(hmdCommand == "OpenVRRender")
	{
		std::cerr << "Using OpenVR...\n";
		renderer = std::make_shared<AnnOgreOpenVRRenderer>(title);
		set		 = true;
	}
	if(hmdCommand == "NoVRRender")
	{
		std::cerr << "Not rendering in VR...\n";
		renderer = std::make_shared<AnnOgreNoVRRenderer>(title);
		set		 = true;
	}
	if(!set)
	{
#ifdef _WIN32
		displayWin32ErrorMessage(
			"Error: Cannot understand VR System you want to use!",
			"This program can be used with multiple VR solution.\n"
			"The executable should be launched via a dedicated launcher.\n"
			"If you're trying to launch it by hand, please check if your"
			"command line parameter is correct!\n\n"
			"Available command line parameter : \n"
			"\t-rift\n"
			"\t-vive\n"
			"\nIf you don't specify anything, the default system will be used"
			"(here it's the Oculus Rift)\n"
			"If you don't have (or can't use) VR Hardware, you can launch with"
			"-noVR.\n"
			"This will display the image on a simple window without attempting"
			"to talk to VR hardware");
#endif
		std::cerr << "It looks like we can't start the VR renderer. The engine is going to crash\n."
				  << "Dumping in standard error the current configuration : \n"
				  << "The default renderer is:" << defaultRenderer << '\n'
				  << "The hmdCommand is: " << hmdCommand << '\n';
		if(renderer == nullptr) std::cerr << "The renderer is currently nullptr\n";
		throw AnnInitializationError(ANN_ERR_CANTHMD, "Can't find an HMD to use");
	}
}

AnnEngine::AnnEngine(const char title[], const std::string& hmdCommand) :
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
	if(singleton)
	{
		writeToLog("Can't create 2 instances of the engine!");
		throw AnnInitializationError(ANN_ERR_MEMORY, "Can't create 2 instances of AnnEngine");
	}

	singleton	= this;
	consoleReady = false;

#ifdef _WIN32 //Windows specific setup

	//Set current process to high priority.
	//Looks like the scheduler of Windows sometimes don't give use the time we need to be consistent.
	//This seems to fixes the problem.
	if(autosetProcessPriorityHigh)
		setProcessPriorityHigh();

	consoleGreen  = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
	consoleYellow = FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY;
	consoleWhite  = FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY;

#endif //WIN32

	stringUtility = std::make_shared<AnnStringUility>();

	selectAndCreateRenderer(hmdCommand, title);
	renderer->initOgreRoot(logFileName);
	player = std::make_shared<AnnPlayerBody>();
	renderer->initVrHmd();
	renderer->initPipeline();
	SceneManager = renderer->getSceneManager();
	renderer->showDebug(AnnOgreVRRenderer::DebugMode::MONOSCOPIC);

	writeToLog("Setup Annwvyn's subsystems");

	// Subsystems are updated in their creation order :
	// - level management is handeled
	// - object management is handeled
	// - physics is ticked (stuff move)
	// - event happens (player input, timers...)
	// - audio is synced (sounds comes form where they should)
	// - other less important operation are done
	// then the game can redraw

	subsystems.push_back(levelManager = std::make_shared<AnnLevelManager>());
	subsystems.push_back(gameObjectManager = std::make_shared<AnnGameObjectManager>());
	subsystems.push_back(physicsEngine = std::make_shared<AnnPhysicsEngine>(getSceneManager()->getRootSceneNode(), player));
	subsystems.push_back(eventManager = std::make_shared<AnnEventManager>(renderer->getWindow()));
	subsystems.push_back(audioEngine = std::make_shared<AnnAudioEngine>());
	subsystems.push_back(filesystemManager = std::make_shared<AnnFilesystemManager>(title));
	subsystems.push_back(resourceManager = std::make_shared<AnnResourceManager>());
	subsystems.push_back(sceneryManager = std::make_shared<AnnSceneryManager>(renderer));
	subsystems.push_back(scriptManager = std::make_shared<AnnScriptManager>());
	renderer->initClientHmdRendering();

	vrRendererPovGameplayPlacement = renderer->getCameraInformationNode();
	vrRendererPovGameplayPlacement->setPosition(player->getPosition() + AnnVect3(0.0f, player->getEyesHeight(), 0.0f));

	//This subsystem need the vrRendererPovGameplayPlacement object to be
	//initialized. And the Resource manager because it wants a font file and an
	//image background
	subsystems.push_back(onScreenConsole = std::make_shared<AnnConsole>());

	consoleReady = true;
	//Display start banner
	writeToLog("============================================================", false);
	writeToLog("| Annwvyn Game Engine - Step into the Other World          |", false);
	writeToLog("| Free/Libre C++ Game Engine designed for Virtual Reality  |", false);
	writeToLog("|                                                          |", false);
	writeToLog("| Copyright Arthur Brainville (a.k.a. Ybalrid) 2013-2018   |", false);
	writeToLog("| Distributed under the terms of the MIT license agreement |", false);
	writeToLog("|                                                          |", false);
	writeToLog("| Visit https://wwwannwvyn.org/ for more informations!     |", false);
	writeToLog("| Version : " + getAnnwvynVersion(61 - 13 - 1) + "|", false);
	writeToLog("============================================================", false);
}

AnnEngine::~AnnEngine()
{
	//Some cute log messages
	writeToLog("Game engine stopped. Subsystem are shutting down...");
	writeToLog("Good luck with the real world now! :3");
	consoleReady = false;
#ifdef _WIN32
	if(manualConsole) FreeConsole();
#endif
}

//All theses getter are for encapsulation purpose. Calling them directly would
//make very long lines of code. You can, but you should use the functions in
//AnnGetter.hpp
AnnEventManagerPtr AnnEngine::getEventManager() const { return eventManager; }
AnnResourceManagerPtr AnnEngine::getResourceManager() const { return resourceManager; }
AnnGameObjectManagerPtr AnnEngine::getGameObjectManager() const { return gameObjectManager; }
AnnSceneryManagerPtr AnnEngine::getSceneryManager() const { return sceneryManager; }
AnnScriptManagerPtr AnnEngine::getScriptManager() const { return scriptManager; }
AnnOgreVRRendererPtr AnnEngine::getVRRenderer() const { return renderer; }
AnnLevelManagerPtr AnnEngine::getLevelManager() const { return levelManager; }
AnnPlayerBodyPtr AnnEngine::getPlayer() const { return player; }
AnnFilesystemManagerPtr AnnEngine::getFileSystemManager() const { return filesystemManager; }
AnnAudioEnginePtr AnnEngine::getAudioEngine() const { return audioEngine; }
AnnPhysicsEnginePtr AnnEngine::getPhysicsEngine() const { return physicsEngine; }

Ogre::SceneNode* AnnEngine::getPlayerPovNode() const { return vrRendererPovGameplayPlacement; }
Ogre::SceneManager* AnnEngine::getSceneManager() const { return SceneManager; }

unsigned long AnnEngine::getTimeFromStartUp() const { return renderer->getTimer()->getMilliseconds(); }
double AnnEngine::getTimeFromStartupSeconds() const { return double(getTimeFromStartUp()) / 1000.0; }

void AnnEngine::initPlayerStandingPhysics() const { physicsEngine->initPlayerStandingPhysics(vrRendererPovGameplayPlacement); }
void AnnEngine::initPlayerRoomscalePhysics() const { physicsEngine->initPlayerRoomscalePhysics(vrRendererPovGameplayPlacement); }
AnnConsolePtr AnnEngine::getOnScreenConsole() const { return onScreenConsole; }
AnnStringUtilityPtr AnnEngine::getStringUtility() const { return stringUtility; }

void AnnEngine::setConsoleGreen()
{
#ifdef _WIN32
	if(!noConsoleColor)
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), consoleGreen);
#endif
}

void AnnEngine::setConsoleYellow()
{
#ifdef _WIN32
	if(!noConsoleColor)
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), consoleYellow);
#endif
}

//This is static, but actually needs Ogre to be running. So be careful
void AnnEngine::writeToLog(std::string message, bool flag)
{
	if(consoleReady)
		singleton->onScreenConsole->append(message);

	if(flag)
	{
		setConsoleYellow();
		message = "Annwvyn - " + message;
	}
	else
	{
		setConsoleGreen();
	}

	if(Ogre::LogManager::getSingletonPtr())
		Ogre::LogManager::getSingleton().logMessage(message);

	setConsoleGreen();
}

//Need to be redone.
bool AnnEngine::requestStop() const
{
	//If the user quit the App from the Oculus Home
	if(renderer->shouldQuit())
		return true;
	if(applicationQuitRequested)
		return true;

	return false;
}

// This is the most important function of the whole project. It's the heartbeat
// of the game or app using this engine.
bool AnnEngine::refresh()
{
	//Set player position from gameplay to the rendering code
	syncPalyerPov();
	//Update VR form real world
	renderer->updateTracking();

	updateTime = renderer->getUpdateTime();
	player->engineUpdate(float(getFrameTime()));

	for(size_t i{ 0 }; i < subsystems.size(); ++i)
		if(subsystems[i]->needUpdate())
			subsystems[i]->update();

	//Update view
	renderer->renderAndSubmitFrame();
	return !requestStop();
}

void AnnEngine::syncPalyerPov() const
{
	vrRendererPovGameplayPlacement->setPosition(player->getPosition());
	vrRendererPovGameplayPlacement->setOrientation(player->getOrientation().toQuaternion());
}

//Bad. Don't use. Register an event listener and use the KeyEvent callback.
inline bool AnnEngine::isKeyDown(OIS::KeyCode key) const
{
	if(!eventManager) return false;
	return eventManager->Keyboard->isKeyDown(key);
}

//the delta time of the last frame, not the current one
double AnnEngine::getFrameTime() const { return updateTime; }

// Raw position and orientation of the head in world space. This is useful if
// you want to mess around with weird stuff. This has been bodged when I
// integrated a LEAP motion in that mess.
AnnPose AnnEngine::getHmdPose() const
{
	if(renderer)
		return renderer->trackedHeadPose;
	return AnnPose();
}

AnnUserSubSystemPtr AnnEngine::registerUserSubSystem(AnnUserSubSystemPtr userSystem)
{
	for(const auto system : subsystems)
		if(userSystem->name == system->name)
		{
			AnnDebug() << "A subsystem with the name "
					   << userSystem->name
					   << "is already registered.";

			return nullptr;
		}
	subsystems.push_back(userSystem);
	return userSystem;
}

void AnnEngine::loadUserSubSystemFromPlugin(const std::string& pluginName, bool local)
{
	AnnDebug() << "Attempting to load an user subsystem from" << pluginName;
	std::string bootstrapName = "AnnBootPlugin_";
	bootstrapName += pluginName;
#ifdef _WIN32
	if(const auto handle = LoadLibraryA(pluginName.c_str()))
	{
		AnnDebug() << "Sucessully loadded dynamic libray";
		if(const auto bootstrapPlugin = GetProcAddress(handle, bootstrapName.c_str()))
		{
			AnnDebug() << "Found address of bootstrap funciton for " << pluginName;
			AnnDebug() << "Create and register subsystem...";
			registerUserSubSystem(AnnUserSubSystemPtr{ reinterpret_cast<AnnUserSubSystem*>(bootstrapPlugin()) });
		}
		else
		{
			AnnDebug() << "Wasn't able to get boostrap function for " << pluginName;
		}
	}
	else
	{
		AnnDebug() << "Wasn't able to load dynamic library " << pluginName;
	}

#else
	//convert "PluginName" into a "libPluginName.so" format as dlopen need the name of the actual file
	std::string pluginSoFile = (local ? "./lib" : "lib");
	pluginSoFile += pluginName;
	pluginSoFile += ".so";

	if(auto handle = dlopen(pluginSoFile.c_str(), RTLD_NOW))
	{
		AnnDebug() << "Sucessully loadded dynamic libray";
		if(auto bootstrapPlugin = (void* (*)())dlsym(handle, bootstrapName.c_str())) //We need to cast the pointer to a functor of the "void* boostrap(void)" format
		{
			AnnDebug() << "Found address of bootstrap funciton for " << pluginName;
			AnnDebug() << "Create and register subsystem...";
			registerUserSubSystem(AnnUserSubSystemPtr(reinterpret_cast<AnnUserSubSystem*>(bootstrapPlugin())));
		}
		else
		{
			AnnDebug() << "Wasn't able to get bootsrap function for " << pluginSoFile;
		}
	}
	else
	{
		AnnDebug() << "Wasn't able to load dynamic library " << pluginName;
	}

#endif
}

AnnSubSystemPtr AnnEngine::getSubSystemByName(const std::string& name)
{
	auto result = std::find_if(std::begin(subsystems),
							   std::end(subsystems),
							   [&](AnnSubSystemPtr s) {
								   return s->name == name;
							   });

	if(result == std::end(subsystems)) return nullptr;
	return *result;
}

bool AnnEngine::isUserSubSystem(AnnSubSystemPtr subsystem)
{
	const auto nakedSubSystem(subsystem.get());
	const auto result = dynamic_cast<AnnUserSubSystem*>(nakedSubSystem);
	return result != nullptr;
}

void AnnEngine::removeUserSubSystem(AnnUserSubSystemPtr subsystem)
{
	subsystems.erase(std::remove(
						 std::begin(subsystems),
						 std::end(subsystems),
						 subsystem),
					 std::end(subsystems));
}

//Because Windows and the Win32 platform sucks.
bool AnnEngine::openConsole()
{
	auto state{ true };
#ifdef _WIN32

	//Allocate a console for this app
	if(AllocConsole())
	{
		//put stdout on this console;
		FILE* f;
		const auto err = freopen_s(&f, "CONOUT$", "w", stdout);
		if(!f) state = false;
		if(err != 0) state = false;
		manualConsole = true;
	}

	//Redirect cerr to cout
	std::cerr.rdbuf(std::cout.rdbuf());

	SetConsoleTitleA("Annwvyn Debug Console");
	if(!noConsoleColor)
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
								consoleWhite);

#endif
	return state;
}

bool AnnEngine::appVisibleInHMD() const
{
	if(renderer->isVisibleInHmd())
		return true;
	return false;
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

void AnnEngine::requestQuit() { applicationQuitRequested = true; }
