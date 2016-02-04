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
	return version.str();
}

void AnnEngine::startGameplayLoop()
{
	while(refresh());
}
	
AnnEngine::AnnEngine(const char title[], bool fs) :
	eventManager(NULL),
	levelManager(NULL),
	fullscreen(fs),
	lastFrameTimeCode(0),
	currentFrameTimeCode(0),
	m_CameraReference(NULL),	
	readyForLoadingRessources(false),
	defaultEventListener(NULL),
	VisualBody(NULL),
	VisualBodyAnimation(NULL),
	VisualBodyAnchor(NULL),
	refVisualBody(AnnQuaternion::IDENTITY)
{
	if(singleton) 
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
	m_SceneManager = renderer->getSceneManager();

	readyForLoadingRessources = true;
	log("OGRE Object-oriented Graphical Rendering Engine initialized", true);

#ifdef __gnu_linux__
	x11LayoutAtStartup = "unknown";
	//Here's a little hack to save the X11 keyboard layout on Linux, then set it to a standard QWERTY
	//Under windows the keycode match the standard US QWERTY layout. Under linux they are converted to whatever you're using.
	//I use a French AZERTY keyboard layout so it's not that bad. If you have a greek keyboard you're out of luck...
	//So, assuming that the only program that has the focus is the Annwvyn powered application, we can just switch the layout to US 
	//then switch back to the original layout.

	log("we are running on linux. getting x11 keyboard layout from the system");
	FILE* layout = popen("echo $(setxkbmap -query | grep layout | cut -d : -f 2 )","r");
	char* buffer = static_cast<char *>(malloc(64*sizeof(char)));

	if(layout && buffer)
	{
		fscanf(layout, "%s", buffer);
		x11LayoutAtStartup = std::string(buffer);

		log("Saving keyboard layout for shutdown.");
		log("saved layout="+x11LayoutAtStartup, false);
	}

	free(buffer);
	fclose(layout);

	buffer = NULL;
	layout = NULL;
	system("setxkbmap us");
#endif

	log("Setup event system");
	eventManager = new AnnEventManager(renderer->getWindow());
	log("Setup physics engine");
	physicsEngine = new AnnPhysicsEngine(getSceneManager()->getRootSceneNode());
	log("Setup audio engine");
	AudioEngine = new AnnAudioEngine;
	log("Setup Level system");
	levelManager = new AnnLevelManager;
	log("Setup Filesystem Manager");
	filesystemManager = new AnnFilesystemManager;
	filesystemManager->setSaveDirectoryName(title);

	log("==================================================", false);
	log("Annwvyn Game Engine - Step into the Other World   ", false);
	log("Designed for Virtual Reality                      ", false);
	log("Version : " + getAnnwvynVersion()                  , false);
	log("==================================================", false);
}

AnnEngine::~AnnEngine()
{
#ifdef __gnu_linux__
	log("setting back the keyboard to " + x11LayoutAtStartup);
	if(x11LayoutAtStartup != "unknown")
	{
		system(std::string("setxkbmap " + x11LayoutAtStartup).c_str());
		log("Done system call to setxkbmap");
	}
#endif

	log("Destroying the event manager");
	delete eventManager;
	eventManager = nullptr;

	log("Destroy the levelManager");
	delete levelManager;
	levelManager = nullptr;

	log("Destroying every objects remaining in engine");
	
	log(" Creating the destroing queue;");
	AnnDebug() << " Will destroy " << objects.size() << " remaining objects";
	AnnDebug() << " Will destroy " << triggers.size() << " remaining triggers";
	AnnDebug() << " Will destroy " << lights.size() << " remaining lights";

	AnnGameObject** tmpArrayObj = static_cast<AnnGameObject**>(malloc(sizeof(AnnGameObject*)*objects.size()));
	AnnTriggerObject** tmpArrayTrig = static_cast<AnnTriggerObject**>(malloc(sizeof(AnnTriggerObject*)*triggers.size()));
	AnnLightObject** tmpArrayLight = static_cast<AnnLightObject**>(malloc(sizeof(AnnLightObject*)*lights.size()));
	
	auto objIt = objects.begin();
	auto trigIt = triggers.begin();
	auto lightIt = lights.begin();

	for(size_t i(0); i < objects.size(); i++) tmpArrayObj[i] = *objIt++;;
	for(size_t i(0); i < triggers.size(); i++) tmpArrayTrig[i] = *trigIt++;
	for(size_t i(0); i < lights.size(); i++) tmpArrayLight[i] = *lightIt++;

	log("Content of the destroing queue :");
	log("Game Object");
	for(size_t i(0); i < objects.size(); i++)
		AnnDebug() << (void*)tmpArrayObj[i];
	log("Trigger Object");
	for(size_t i(0); i < triggers.size(); i++)
		AnnDebug() << (void*)tmpArrayTrig[i];
	log("Light object");
	for(size_t i(0); i < lights.size(); i++)
		AnnDebug() << (void*)tmpArrayLight[i];

	size_t queueSize;
	queueSize = objects.size();
	for(size_t i(0); i < queueSize; i++)
		destroyGameObject(tmpArrayObj[i]);
	queueSize = triggers.size();
	for(size_t i(0); i < queueSize; i++)
		destroyTriggerObject(tmpArrayTrig[i]);
	queueSize = lights.size();
	for(size_t i(0); i < queueSize; i++)
		destroyLightObject(tmpArrayLight[i]);


	log("Destroing the deletion queues");
	free(tmpArrayObj);
	free(tmpArrayTrig);
	free(tmpArrayLight);

	log("Clearing object lists");
	objects.clear();
	triggers.clear();
	lights.clear();

	log("Destroying physics engine");
	delete physicsEngine;
	physicsEngine = nullptr;
	log("Destroying Player");
	delete player;
	player = nullptr;
	log("Destroying AudioEngine");
	delete AudioEngine;
	AudioEngine = nullptr;

	log("Game engine sucessfully destroyed.");
	log("Good luck with the real world now! :3");
	delete onScreenConsole;
	onScreenConsole = NULL;
	singleton = NULL;
	delete renderer;
	renderer = nullptr;
}

AnnEventManager* AnnEngine::getEventManager()
{
	return eventManager;
}

AnnLevelManager* AnnEngine::getLevelManager()
{
	return levelManager;
}

AnnPlayer* AnnEngine::getPlayer()
{
	return player;
}

AnnFilesystemManager* AnnEngine::getFileSystemManager()
{
	return filesystemManager;
}

////////////////////////////////////////////////////////// UTILITY
void AnnEngine::log(std::string message, bool flag)
{
	Ogre::String messageForLog;

	if(flag)
		messageForLog += "Annwvyn - ";

	messageForLog += message;
	Ogre::LogManager::getSingleton().logMessage(messageForLog);
	if(onScreenConsole)
		onScreenConsole->append(message);
}

void AnnEngine::useDefaultEventListener()
{
	if(!eventManager) return; 
	log("Reconfiguring the engine to use the default event listener");
	log("This unregister any current listener in use!");

	//Remove all event listeners
	eventManager->removeListener();

	//If the event listenre isn't allready initialized, allocate one
	if(!defaultEventListener)
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
	physicsEngine->initPlayerPhysics(player, m_CameraReference);
}

void AnnEngine::loadZip(const char path[], const char resourceGroupName[])
{
	log("Loading resources from Zip archive :");
	log(path, false);
	if(readyForLoadingRessources)
		Ogre::ResourceGroupManager::getSingleton().addResourceLocation(path, "Zip", resourceGroupName);
}

void AnnEngine::loadDir(const char path[], const char resourceGroupName[])
{
	log("Loading resources from Filesystem directory :");
	log(path, false);
	if(readyForLoadingRessources)
		Ogre::ResourceGroupManager::getSingleton().addResourceLocation(path, "FileSystem", resourceGroupName);
}

void AnnEngine::loadResFile(const char path[])
{
	AnnDebug() << "Loading resource file : " << path;
	renderer->loadReseourceFile(path);
}

void AnnEngine::initResources()
{
	addDefaultResourceLocaton();
	renderer->initAllResources();
	log("Resources initialized");
}

void AnnEngine::addDefaultResourceLocaton()
{
	log("Adding Annwvyn CORE resources");
	loadDir("media");
	loadZip("media/CORE.zip");
}

void AnnEngine::oculusInit()
{   
	log("Init Oculus rendering system");
	renderer->initOculus();
	m_CameraReference = renderer->getCameraInformationNode();
	m_CameraReference->setPosition(player->getPosition() + 
		AnnVect3(0.0f, player->getEyesHeight(), 0.0f));
	onScreenConsole = new AnnConsole();
	//This will populate swap texture and turn on the rift display earlier
	renderer->updateTracking();
	renderer->renderAndSubmitFrame();
}

AnnGameObject* AnnEngine::createGameObject(const char entityName[], AnnGameObject* obj)
{
	log("Creatig a game object from the entity " + std::string(entityName));

	if(std::string(entityName).empty())
	{
		log("Hey! what are you trying to do here? Please specify a non empty string for entityName !");
		delete obj;
		return NULL;
	}

	Ogre::Entity* ent = m_SceneManager->createEntity(entityName);
	Ogre::SceneNode* node = m_SceneManager->getRootSceneNode()->createChildSceneNode();

	node->attachObject(ent);
	obj->setNode(node);
	obj->setEntity(ent);
	obj->setBulletDynamicsWorld(physicsEngine->getWorld());
	obj->postInit(); //Run post init directives

	objects.push_back(obj); //keep addreAnnDebug() in list

	AnnDebug() << "The object " << entityName << " has been created. Annwvyn memory address " << obj;  

	return obj;
}

void AnnEngine::destroyTriggerObject(AnnTriggerObject* object)
{
	//Discard destroying to end of frame processing if in the middle of the renderCallback
	if(lockForCallback)
	{
		triggerClearingQueue.push_back(object);
		return;
	}

	triggers.remove(object);
	AnnDebug() << "Destroy trigger : " << (void*)object;
	delete object;
}

void AnnEngine::clearTriggers()
{
	if(lockForCallback)return;
	for(auto trigger : triggerClearingQueue)
		destroyTriggerObject(trigger);
	triggerClearingQueue.clear();
}

bool AnnEngine::destroyGameObject(Annwvyn::AnnGameObject* object)
{
	//If that boolean is true, it means that we are iterating the object list during
	//render callback. We cannot modify this list. However, we sore the querry to 
	//destroy an object in a queue that will be cleared as soon as it's safe to do.
	if(lockForCallback) 
	{
		clearingQueue.push_back(object); 
		return false;
	}

	bool returnCode(false);

	//TODO: remove the need to mark objects as NULL in this array before being able to clear them.
	for(auto it = objects.begin(); it != objects.end(); it++)
    {
		if(!*it) continue;
		(*it)->stopGettingCollisionWith(object); 
		if(*it == object)
		{
			returnCode = true;
			*it = NULL;

			Ogre::SceneNode* node = object->node();
			node->getParent()->removeChild(node);
			size_t nbObject(node->numAttachedObjects());
			std::vector<Ogre::MovableObject*> attachedObject;

			for(unsigned short i(0); i < nbObject; i++)
				attachedObject.push_back(node->getAttachedObject(i));

			node->detachAllObjects();

			auto attachedIterator(attachedObject.begin());
			while(attachedIterator!= attachedObject.end())
				m_SceneManager->destroyMovableObject(*attachedIterator++);

			physicsEngine->removeRigidBody(object->getBody());
			m_SceneManager->destroySceneNode(node);
			delete object;
		}
	}

	//Clear everything equals to "NULL" on the vector
	objects.remove(NULL);

	return returnCode;
}

AnnLightObject* AnnEngine::createLightObject()
{
	log("Creating a light");
	AnnLightObject* Light = m_SceneManager->createLight();
	Light->setType(Ogre::Light::LT_POINT);
	lights.push_back(Light);
	return Light;
}

void AnnEngine::destroyLightObject(AnnLightObject* object)
{
	if(object)
		m_SceneManager->destroyLight(object);

	//Forget that this light existed
	lights.remove(object);
}

bool AnnEngine::requestStop()
{
	//pres ESC to quit. Stupid but efficient. I like that.
	if(isKeyDown(OIS::KC_ESCAPE))
		return true;
	return false;
}



bool AnnEngine::refresh()
{
	//Get the rendering delta time (should be roughly equals to 1/desiredFramerate)
	deltaT = renderer->getUpdateTime();
	//Step the simulation
	physicsEngine->step(deltaT);
	//Update player logic code 
	player->engineUpdate(deltaT);

	//Run animations and update OpenAL sources position
	for(auto gameObject : objects)
	{
		gameObject->addTime(deltaT);
		gameObject->updateOpenAlPos();
	}

	lockForCallback = true;
	//Process some logic to extract basic informations (theses should be done within the eventManager).
	physicsEngine->processCollisionTesting(objects);
	physicsEngine->processTriggersContacts(player, triggers);
		//Update the event system
	if(eventManager)
		eventManager->update(); 
	lockForCallback = false;
	clearTriggers();

	//Audio
	AudioEngine->updateListenerPos(renderer->returnPose.position);
	AudioEngine->updateListenerOrient(renderer->returnPose.orientation);
	
	//Update camera
	m_CameraReference->setPosition(player->getPosition());
	m_CameraReference->setOrientation(/*QuatReference* */ player->getOrientation().toQuaternion());

	physicsEngine->stepDebugDrawer();

	if(onScreenConsole->needUpdate()) onScreenConsole->update();
	
	//renderer->RenderOneFrame();
	renderer->updateTracking();
	
	///Start of old render callbak call
	//This will lock the removal of object during the refresh call.
	lockForCallback = true;
	levelManager->step();
	//Create a copy of the object list and call the atRefresh object from it. This will prevent using a potentially invalidated iterator
	size_t queueSize = objects.size();
	AnnGameObject** refreshQueue = static_cast<AnnGameObject**>(malloc(sizeof(AnnGameObject*)*queueSize));
	auto objectIterator(objects.begin());
	for(size_t i(0); i < queueSize; i++) refreshQueue[i] = *objectIterator++;
	for(size_t i(0); i < queueSize; i++) refreshQueue[i]->atRefresh();
	
	//Get rid of the refresh queue
	free(refreshQueue); 
	refreshQueue = NULL;

	//Now it's safe to remove the objects
	lockForCallback = false;
	clearTriggers();

	//Destroy the objects in the queue
	for(size_t i(0); i < clearingQueue.size(); i++)
		destroyGameObject(clearingQueue[i]);

	//Clear the queue
	clearingQueue.clear();	
	
	///End of old render callback 

	renderer->renderAndSubmitFrame();

	return !requestStop();
}

bool AnnEngine::isKeyDown(OIS::KeyCode key)
{
	if(!eventManager) return false;
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
	Ogre::RaySceneQuery* raySceneQuery(m_SceneManager->createRayQuery(ray));
	raySceneQuery->setSortByDistance(true);

	//execute and get the results
	Ogre::RaySceneQueryResult& result(raySceneQuery->execute());

	//read the result list
	for(auto it(result.begin()); it != result.end(); it++)
		if(it->movable && it->movable->getMovableType() == "Entity")
			return getFromNode(it->movable->getParentSceneNode());//Get the AnnGameObject that is attached to this SceneNode	

	return NULL; //means that we don't know what the player is looking at.
}

void AnnEngine::attachVisualBody(const std::string entityName, float z_offset, bool flip, bool animated , Ogre::Vector3 scale)
{
	log("Attaching visual body :");
	log(entityName);

	//Could actually be an AnnGameObject without the physics ? So it will be cleaned by the AnnEngine destructor
	Ogre::Entity* ent = m_SceneManager->createEntity(entityName);
	VisualBodyAnchor = m_CameraReference->createChildSceneNode();
	VisualBodyAnchor->attachObject(ent);

	if(flip)
		refVisualBody = AnnQuaternion(Ogre::Degree(180), AnnVect3::UNIT_Y);
	else
		refVisualBody = AnnQuaternion::IDENTITY;

	visualBody_Zoffset = z_offset;
	VisualBody = ent;

	VisualBodyAnchor->setPosition(0, -player->getEyesHeight(), -visualBody_Zoffset);
	VisualBodyAnchor->setOrientation(refVisualBody);
}

void AnnEngine::resetOculusOrientation()
{
	log("Reseting the base direction of player's head");
	renderer->recenter();
}

Annwvyn::AnnGameObject* AnnEngine::getFromNode(Ogre::SceneNode* node)
{
	if(!node)
	{
		log("Plese do not try to identify a NULL");
		return NULL;
	}
	AnnDebug() << "Trying to identify object at address " << (void*)node;

	//This methods only test memory address
	for(auto object : objects)
		if((void*)object->node() == (void*)node)
			return object;
	AnnDebug() << "The object " << (void*)node << " doesn't belong to any AnnGameObject";

	return NULL;
}

////////////////////////////////////////////////////////// GETTERS
Ogre::SceneNode* AnnEngine::getCamera()
{
	return m_CameraReference;
}

AnnAudioEngine* AnnEngine::getAudioEngine()
{
	return AudioEngine;
}

AnnPhysicsEngine* AnnEngine::getPhysicsEngine()
{
	return physicsEngine;
}


Ogre::SceneManager* AnnEngine::getSceneManager()
{
	return m_SceneManager;
}

double AnnEngine::getTimeFromStartUp()
{
	return static_cast<double>(renderer->getTimer()->getMilliseconds());
}

////////////////////////////////////////////////////////// SETTERS
void AnnEngine::setDebugPhysicState(bool state)
{
	assert(physicsEngine);
	log("Activating debug drawing for physics engine");
	physicsEngine->setDebugPhysics(state);
}

void AnnEngine::setAmbiantLight(Ogre::ColourValue v)
{
 AnnDebug() << "Setting the ambiant light to color " << v; 
	m_SceneManager->setAmbientLight(v);
}

void AnnEngine::setSkyDomeMaterial(bool activate, const char materialName[], float curvature, float tiling)
{
	log("Setting skydome from material"); log(materialName, false);
	m_SceneManager->setSkyDome(activate, materialName, curvature, tiling);
}

void AnnEngine::setSkyBoxMaterial(bool activate, const char materialName[], float distance, bool renderedFirst)
{
	log("Setting skybox from material"); log(materialName, false);
	m_SceneManager->setSkyBox(activate, materialName, distance, renderedFirst);
}

void AnnEngine::setWorldBackgroudColor(Ogre::ColourValue v)
{
	AnnDebug() << "Setting the backgroud world color " << v;
	renderer->changeViewportBackgroundColor(v); 
}
void AnnEngine::removeSkyDome()
{
	log("Disabeling skydome");
	m_SceneManager->setSkyDomeEnabled(false);
}

void AnnEngine::removeSkyBox()
{
	log("Disabeling skybox");
	m_SceneManager->setSkyBoxEnabled(false);
}

void AnnEngine::setNearClippingDistance(Ogre::Real nearClippingDistance)
{
	AnnDebug() << "Setting the near clipping distance to " << nearClippingDistance;

	if(renderer)
		renderer->setCamerasNearClippingDistance(nearClippingDistance);
}

void AnnEngine::resetPlayerPhysics()
{
	if(!player->hasPhysics()) return;
	log("Reset player's physics");

	//Remove the player's rigidbody from the world
	physicsEngine->getWorld()->removeRigidBody(player->getBody());
	
	//We don't need that body anymore...
	delete player->getBody();
	//prevent memory access to unallocated address
	player->setBody(NULL);

	//Put everything back in order
	m_CameraReference->setPosition(player->getPosition());
	physicsEngine->createPlayerPhysicalVirtualBody(player, m_CameraReference);
	physicsEngine->addPlayerPhysicalBodyToDynamicsWorld(player);
}

OgrePose AnnEngine::getPoseFromOOR()
{
	if(renderer)
		return renderer->returnPose;
	return OgrePose();
}

void AnnEngine::openConsole()
{
#ifdef _WIN32
	int outHandle, errHandle, inHandle;
    FILE *outFile, *errFile, *inFile;
    AllocConsole();
    CONSOLE_SCREEN_BUFFER_INFO coninfo;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
    coninfo.dwSize.Y = 9999;
    SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);

    outHandle = _open_osfhandle((long)GetStdHandle(STD_OUTPUT_HANDLE), _O_TEXT);
    errHandle = _open_osfhandle((long)GetStdHandle(STD_ERROR_HANDLE), _O_TEXT);
    inHandle = _open_osfhandle((long)GetStdHandle(STD_INPUT_HANDLE), _O_TEXT );

    outFile = _fdopen(outHandle, "w" );
    errFile = _fdopen(errHandle, "w");
    inFile =  _fdopen(inHandle, "r");

    *stdout = *outFile;
    *stderr = *errFile;
    *stdin = *inFile;

    setvbuf(stdout, NULL, _IONBF, 0 );
    setvbuf(stderr, NULL, _IONBF, 0 );
    setvbuf(stdin, NULL, _IONBF, 0 );

    std::ios::sync_with_stdio();
#endif
}

void AnnEngine::toogleOnScreenConsole()
{
	if(onScreenConsole) onScreenConsole->toogle();
}

void AnnEngine::toogleOculusPerfHUD()
{
	if(renderer) renderer->cycleOculusHUD();
}