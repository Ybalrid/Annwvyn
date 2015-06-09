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

void AnnEngine::toogleOnScreenConsole()
{
	if(onScreenConsole) onScreenConsole->toogle();
}

AnnEngine::AnnEngine(const char title[], bool fs)
{
	eventManager = NULL;
	levelManager = NULL;
	fullscreen = fs;
	lastFrameTimeCode = 0;
	currentFrameTimeCode = 0;

	//Make the necessary singleton initialization. 
	if(singleton) abort();
	singleton = this;

	m_CameraReference = NULL;

	//block ressources loading for now
	readyForLoadingRessources = false;

	//This structure handle player's body parameters
	defaultEventListener = NULL;

	//Launching initialisation routines : 
	//All Ogre related critical component is done inside the OgreOculusRenderer class. 
	oor = new OgreOculusRender(title);
	oor->setFullScreen(fullscreen);
	oor->setRenderCallback(this);
	oor->initLibraries("Annwvyn.log");
	player = new AnnPlayer;
	oor->getOgreConfig();
	oor->createWindow();
	oor->initScene();
	oor->initCameras();
	oor->setCamerasNearClippingDistance(0.15f);
	oor->initRttRendering();
	m_SceneManager = oor->getSceneManager();
	m_Window = oor->getWindow();

	readyForLoadingRessources = true;
	log("OGRE Object-oriented Graphical Rendering Engine initialized", true);

//We use OIS to catch all user inputs
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
	eventManager = new AnnEventManager(m_Window);

	log("Setup physics engine");
	physicsEngine = new AnnPhysicsEngine(getSceneManager()->getRootSceneNode());
		
	log("Setup audio engine");
	AudioEngine = new AnnAudioEngine;

	//Setting up the Visual Body management 
	VisualBody = NULL;
	VisualBodyAnimation = NULL;
	VisualBodyAnchor = NULL;

	refVisualBody = AnnQuaternion::IDENTITY;

	levelManager = new AnnLevelManager;

	log("---------------------------------------------------", false);
	log("Annwvyn Game Engine - Step into the Other World   ", false);
	log("Designed for Virtual Reality                      ", false);
	log("---------------------------------------------------", false);
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

	log("Destroy the levelManager");
	delete levelManager;

	//All AnnGameObject
	log("Destroying every objects remaining in engine");
	
	log(" Creating the destroing queue;");
	AnnDebug() << " Will destroy " << objects.size() << " objects";

	AnnGameObject** tmpArray = static_cast<AnnGameObject**>(malloc(sizeof(AnnGameObject*)*objects.size()));
	for(size_t i(0); i < objects.size(); i++)
		tmpArray[i] = objects[i];

	log("Content of the destroing queue :");
	for(size_t i(0); i < objects.size(); i++)
		AnnDebug() << (void*)tmpArray[i];

	size_t queueSize(objects.size());
	for(size_t i(0); i < queueSize; i++)
		destroyGameObject(tmpArray[i]);

	log("Destroing the deletion queue");
	free(tmpArray);
	log("Clearing the object list");
	objects.clear();

	log("Destroying physics engine");
	delete physicsEngine;
	log("Destroying Player");
	delete player;
	log("Destroying AudioEngine");
	delete AudioEngine;

	log("Game engine sucessfully destroyed.");
	log("Good luck with the real world now! :3");
	delete onScreenConsole;
	onScreenConsole = NULL;
	delete oor;
	singleton = NULL;
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

	//Remove all event listeners
	eventManager->removeListener();

	//If the event listenre isn't allready initialized, allocate one
	if(!defaultEventListener)
		defaultEventListener = new AnnDefaultEventListener(getPlayer());

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
	oor->loadReseourceFile(path);
}

void AnnEngine::initResources()
{
	addDefaultResourceLocaton();
	oor->initAllResources();
	log("Resources initialized");
}

void AnnEngine::addDefaultResourceLocaton()
{
	log("Adding Annwvyn CORE resources");
	loadDir("media");
	loadZip("media/CORE.zip");
}

void AnnEngine::oculusInit(bool fullscreen)
{   
	log("Init Oculus rendering system");
	oor->initOculus(fullscreen);
	m_CameraReference = oor->getCameraInformationNode();
	m_CameraReference->setPosition(player->getPosition() + 
		AnnVect3(0.0f, player->getEyesHeight(), 0.0f));
	onScreenConsole = new AnnConsole();
	///This will populate swap texture and turn on the rift display earlier
	oor->RenderOneFrame();
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
	Ogre::SceneNode* node = 
	m_SceneManager->getRootSceneNode()->createChildSceneNode();

	node->attachObject(ent);
	obj->setNode(node);
	obj->setEntity(ent);
	obj->setBulletDynamicsWorld(physicsEngine->getWorld());
	obj->postInit(); //Run post init directives

	objects.push_back(obj); //keep addreAnnDebug() in list

	AnnDebug() << "The object " << entityName << " has been created. Annwvyn memory address " << obj;  

	return obj;
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

	for(AnnGameObjectVect::iterator it = objects.begin(); it != objects.end(); it++)
    {
		//ss << "Object " << static_cast<void*>(*it) << " stop collision test";log(ss.str());ss.str("");
		
		if(!*it)
		{
			log("NULL object found. jump to next one");
			continue;
		}
		
		//This system needs to be redone
		(*it)->stopGettingCollisionWith(object);
		if(*it == object)
		{
			returnCode = true; // found
			//log("Object found");
			*it = NULL;
	
			Ogre::SceneNode* node = object->node();

			node->getParent()->removeChild(node);
			size_t nbObject(node->numAttachedObjects());
			std::vector<Ogre::MovableObject*> attachedObject;

			for(size_t i(0); i < nbObject; i++)
				attachedObject.push_back(node->getAttachedObject(i));

			node->detachAllObjects();

			std::vector<Ogre::MovableObject*>::iterator attachedIterator(attachedObject.begin());
			while(attachedIterator!= attachedObject.end())
				m_SceneManager->destroyMovableObject(*attachedIterator++);

			physicsEngine->removeRigidBody(object->getBody());
			//node->removeAndDestroyAllChildren();
			m_SceneManager->destroySceneNode(node);
			delete object;
		}
	}

	AnnGameObjectVect::iterator it = objects.begin();
	while(it != objects.end())
	if(!(*it))
		it = objects.erase(it);
	else
		it++;

	return returnCode;
}

void AnnEngine::destroyLight(AnnLightObject* object)
{
	if(object)
		m_SceneManager->destroyLight(object);
}

AnnLightObject* AnnEngine::addLight()
{
	log("Creating a light");
	AnnLightObject* Light = m_SceneManager->createLight();
	Light->setType(Ogre::Light::LT_POINT);
	return Light;
}

bool AnnEngine::requestStop()
{
	//pres ESC to quit. Stupid but efficient. I like that.
	if(isKeyDown(OIS::KC_ESCAPE))
		return true;
	return false;
}

//This is called by the OgreOculusRender object just before updating the frame
void AnnEngine::renderCallback()
{
	//This will lock the removal of object during the refresh call.
	lockForCallback = true;
	levelManager->step();
	//Create a copy of the object list and call the atRefresh object from it. This will prevent using a potentially invalidated iterator
	size_t queueSize = objects.size();
	AnnGameObject** refreshQueue = static_cast<AnnGameObject**>(malloc(sizeof(AnnGameObject*)*queueSize));
	for(size_t i(0); i < queueSize; i++) refreshQueue[i] = objects[i];
	for(size_t i(0); i < queueSize; i++) refreshQueue[i]->atRefresh();
	
	//Get rid of the refresh queue
	free(static_cast<void*>(refreshQueue)); 
	refreshQueue = NULL;

	//Now it's safe to remove the objects
	lockForCallback = false;

	//If there is nothing to do, don't waist time
	if(clearingQueue.empty())
		return;

	//Destroy the objects in the queue
	for(size_t i(0); i < clearingQueue.size(); i++)
		destroyGameObject(clearingQueue[i]);

	//Clear the queue
	clearingQueue.clear();
}

bool AnnEngine::refresh()
{
	deltaT = oor->getUpdateTime();
	physicsEngine->step(deltaT);
	player->engineUpdate(deltaT);
	for(AnnGameObjectVect::iterator it = objects.begin(); it != objects.end(); ++it)
	{
		(*it)->addTime(deltaT);
		(*it)->updateOpenAlPos();
	}
	if(eventManager)
		eventManager->update(); 

	physicsEngine->processCollisionTesting(objects);
	physicsEngine->processTriggersContacts(player, triggers);

	//Animation
	if(VisualBodyAnimation)
		VisualBodyAnimation->addTime(deltaT);

	//Audio
	AudioEngine->updateListenerPos(oor->returnPose.position);
	AudioEngine->updateListenerOrient(oor->returnPose.orientation);
	
	//Update camera
	m_CameraReference->setPosition(player->getPosition());
	m_CameraReference->setOrientation(/*QuatReference* */ player->getOrientation().toQuaternion());

	physicsEngine->stepDebugDrawer();
	if(onScreenConsole->needUpdate())onScreenConsole->update();
	oor->RenderOneFrame();

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
	AnnVect3 LookAt(getPoseFromOOR().orientation * AnnVect3::NEGATIVE_UNIT_Z);

	//create ray
	Ogre::Ray ray(Orig, LookAt);

	//create query
	Ogre::RaySceneQuery* raySceneQuery(m_SceneManager->createRayQuery(ray));
	raySceneQuery->setSortByDistance(true);

	//execute and get the results
	Ogre::RaySceneQueryResult& result(raySceneQuery->execute());

	//read the result list
	for(Ogre::RaySceneQueryResult::iterator it(result.begin()); it != result.end(); it++)
		if(it->movable && it->movable->getMovableType() == "Entity")
			return getFromNode(it->movable->getParentSceneNode());//Get the AnnGameObject that is attached to this SceneNode	

	return NULL; //means that we don't know what the player is looking at.
}

void AnnEngine::attachVisualBody(const std::string entityName, float z_offset, bool flip, bool animated , Ogre::Vector3 scale)
{
	log("Attaching visual body :");
	log(entityName);

	Ogre::Entity* ent = m_SceneManager->createEntity(entityName);
	VisualBodyAnchor = m_CameraReference->createChildSceneNode();
	VisualBodyAnchor->attachObject(ent);

	if(flip)
		refVisualBody = AnnQuaternion(Ogre::Degree(180), AnnVect3::UNIT_Y);
	else
		refVisualBody = AnnQuaternion::IDENTITY;

	visualBody_Zoffset = z_offset;
	VisualBody = ent;

	VisualBodyAnchor->setPosition(0,-player->getEyesHeight(),-visualBody_Zoffset);
	VisualBodyAnchor->setOrientation(refVisualBody);

	if(animated)
	{
		//TODO play idle animation
	}
}

void AnnEngine::resetOculusOrientation()
{
	log("Reseting the base direction of player's head");
	oor->recenter();
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
	for(size_t i(0); i < objects.size(); i++)
		if((void*)objects[i]->node() == (void*)node)
			return objects[i];
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

Ogre::SceneManager* AnnEngine::getSceneManager()
{
	return m_SceneManager;
}

double AnnEngine::getTimeFromStartUp()
{
	return static_cast<double>(oor->getTimer()->getMilliseconds());
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

void AnnEngine::removeSkyDome()
{
	log("Disabeling skydome");
	m_SceneManager->setSkyDomeEnabled(false);
}

void AnnEngine::setNearClippingDistance(Ogre::Real nearClippingDistance)
{
	AnnDebug() << "Setting the near clipping distance to " << nearClippingDistance;

	if(oor)
		oor->setCamerasNearClippingDistance(nearClippingDistance);
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
	if(oor)
		return oor->returnPose;
	OgrePose p; return p;
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
    errHandle = _open_osfhandle((long)GetStdHandle(STD_ERROR_HANDLE),_O_TEXT);
    inHandle = _open_osfhandle((long)GetStdHandle(STD_INPUT_HANDLE),_O_TEXT );

    outFile = _fdopen(outHandle, "w" );
    errFile = _fdopen(errHandle, "w");
    inFile =  _fdopen(inHandle, "r");

    *stdout = *outFile;
    *stderr = *errFile;
    *stdin = *inFile;

    setvbuf( stdout, NULL, _IONBF, 0 );
    setvbuf( stderr, NULL, _IONBF, 0 );
    setvbuf( stdin, NULL, _IONBF, 0 );

    std::ios::sync_with_stdio();
#endif
}

void AnnEngine::openDebugWindow()
{
	log("Open a debug render window on the main screen");
	oor->openDebugWindow();
}
