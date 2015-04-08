#include "AnnEngine.hpp"

using namespace Annwvyn;

AnnEngine::AnnEngine(const char title[])
{
    m_Camera = NULL;
	x11LayoutAtStartup = "unknown";
    //block ressources loading for now
    readyForLoadingRessources = false;

    //This structure handle player's body parameters
    player = new AnnPlayer;
    eventManager = new AnnEventManager;
    defaultEventListener = NULL;

    //here we set all the defaults parameters for the body.

    //Launching initialisation routines : 
    setUpOgre(title);
    setUpOIS();
    setUpTime();
    setUpBullet();
    setUpAudio();
    setUpGUI();

    //Setting up reference quaternion for the camera coordinate system.
    QuatReference = Ogre::Quaternion::IDENTITY;

    //Setting up the Visual Body management 
    VisualBody = NULL;
    VisualBodyAnimation = NULL;
    VisualBodyAnchor = NULL;
    //VisualBodyAnchor = m_SceneManager->getRootSceneNode()->createChildSceneNode();

    refVisualBody = Ogre::Quaternion::IDENTITY;
    log("Annwvyn Game Engine - Step into the Other World", false);
    log("Desinged for Virtual Reality", false);
}


AnnEngine::~AnnEngine()
{
    delete eventManager;

    //All AnnGameObject
    for(size_t i(0); i < objects.size(); i++)
    {
        destroyGameObject(objects[i]);
        objects[i] = NULL;  //don't change the size of the vector while iterating throug it
    }

    objects.clear();

    //Bullet
    delete m_DynamicsWorld;
    delete m_Broadphase;
    delete player;
    delete m_CollisionConfiguration;
    delete m_Dispatcher;
    delete m_Solver;

    //OIS
    delete m_debugDrawer;
    delete m_Keyboard;
    delete m_Mouse;

    //Audio
    delete AudioEngine;

#ifdef __gnu_linux__
    log("setting back the keyboard to " + x11LayoutAtStartup);
	if(x11LayoutAtStartup != "unknown")
	{
		system(std::string("setxkbmap " + x11LayoutAtStartup).c_str());
		log("Done system call to setxkbmap");
	}
#endif

	log("Game engine sucessfully destroyed.");
	log("Good luck with the real world now! :3");
}

AnnEventManager* AnnEngine::getEventManager()
{
    return eventManager;
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
}

void AnnEngine::emergency(void)
{
    log("FATAL : It is imposible to keep the engine running. Plese check engine and object initialization");
    abort();
}

////////////////////////////////////// INITALIZATION
///////////// Graphics
void AnnEngine::setUpOgre(const char title[])
{
	//All Ogre related critical component is done inside the OgreOculusRenderer class. 
    oor = new OgreOculusRender(title);
    oor->initLibraries();
    oor->getOgreConfig();
    oor->createWindow();
    oor->initScene();
    oor->initCameras();
    oor->setCamerasNearClippingDistance(0.15f);
    oor->initRttRendering();
    m_SceneManager = oor->getSceneManager();
    m_Window = oor->getWindow();

    readyForLoadingRessources = true;
}

///////////// Physics
void AnnEngine::setUpBullet()
{
    log("Init Bullet physics");

    m_Broadphase = new btDbvtBroadphase();
    m_CollisionConfiguration = new btDefaultCollisionConfiguration();
    m_Dispatcher = new btCollisionDispatcher(m_CollisionConfiguration);
    m_Solver = new btSequentialImpulseConstraintSolver();
    m_ghostPairCallback = new btGhostPairCallback();

    m_DynamicsWorld = new btDiscreteDynamicsWorld(m_Dispatcher, m_Broadphase, m_Solver, m_CollisionConfiguration);

    log("Gravity vector = (0,-10,0)");
    m_DynamicsWorld->setGravity(btVector3(0,-10,0));
    m_DynamicsWorld->getPairCache()->setInternalGhostPairCallback(m_ghostPairCallback);

    debugPhysics = false;//by default
    m_debugDrawer = new BtOgre::DebugDrawer(m_SceneManager->getRootSceneNode(), m_DynamicsWorld);
    m_DynamicsWorld->setDebugDrawer(m_debugDrawer);

    //colision with this object will allow the player to jump
    m_Ground = NULL;
}

///////////// Inputs
void AnnEngine::setUpOIS()
{
    //We use OIS to catch all user inputs
#ifdef __gnu_linux__
	//Here's a little hack to save the X11 keyboard layout on Linux, then set it to a standard QWERTY
	//Under windows the keycode match the standard US QWERTY layout. Under linux they are converted to whatever you're using.
	//I use a French AZERTY keyboard layout so it's not that bad. If you have a greek keyboard you're out of luck...
	//So, assuming that the only program that has the focus is the Annwvyn powered application, we can just switch the layout to US 
	//then switch back to the original layout.

    log("we are running on linux. getting x11 keyboard layout from the system");
    FILE* layout = popen("echo $(setxkbmap -query | grep layout | cut -d : -f 2 )","r");
    char* buffer = static_cast<char *>(malloc(128*sizeof(char)));
    
    if(!layout)
        log("cannot get the layout");
    if(!buffer)
        log("cannot create a 128 byte text buffer (weird...)");

    if(layout && buffer)
    {
        fscanf(layout, "%s", buffer);
        x11LayoutAtStartup = std::string(buffer);

        log("Saving keyboard layout for shutdown.");
        log("saved layout="+x11LayoutAtStartup, false);
    }
    free(buffer);
    buffer = NULL;
    system("setxkbmap us");
#endif

    //init OIS
    log("Initialize OIS");
    m_InputManager = NULL;
    m_Keyboard = NULL;
    m_Mouse = NULL;
    m_Joystick = NULL;
    windowHnd = 0; 

    m_Window->getCustomAttribute("WINDOW",&windowHnd);
    windowHndStr << windowHnd;

    pl.insert(std::make_pair(
                std::string("WINDOW"), windowHndStr.str()));

    m_InputManager = OIS::InputManager::createInputSystem(pl);


    m_Keyboard = static_cast<OIS::Keyboard*>(m_InputManager->createInputObject(OIS::OISKeyboard, true));
    m_Mouse = static_cast<OIS::Mouse*>(m_InputManager->createInputObject(OIS::OISMouse, true));

    if(m_InputManager->getNumberOfDevices(OIS::OISJoyStick) > 0)
        m_Joystick = static_cast<OIS::JoyStick*>(m_InputManager->createInputObject(OIS::OISJoyStick, true));

  

    if(eventManager)
    {
        eventManager->setKeyboard(m_Keyboard);
        eventManager->setMouse(m_Mouse);
        eventManager->setJoystick(m_Joystick);
    }
}

///////////// Time system
void AnnEngine::setUpTime()
{
    log("Setup time system");
    last = oor->getTimer()->getMilliseconds();
    now = last;
}

///////////// Audio System
void AnnEngine::setUpAudio()
{
    AudioEngine = new AnnAudioEngine;
    log("Audio Engine started");
}

///////////// Interface 
void AnnEngine::setUpGUI()
{
	//TODO initialize Gorilla here
    return;
}

//Convinient method to the user to call : do it and let go !
void AnnEngine::initPlayerPhysics()
{
    createVirtualBodyShape();
    createPlayerPhysicalVirtualBody();
    addPlayerPhysicalBodyToDynamicsWorld();
}

//will be private 
void AnnEngine::createVirtualBodyShape()
{
    assert(player);
    float radius(0.25f);
    player->setShape(new btCapsuleShape(radius,player->getEyesHeight()-2*radius));
}

void AnnEngine::createPlayerPhysicalVirtualBody()
{
	//Player need to have a shape (capsule)
    assert(player->getShape());

	//Create a rigid body state through BtOgre
    BtOgre::RigidBodyState *state = new BtOgre::RigidBodyState
        (m_Camera);

	//Get inertia vector
    btVector3 inertia;
    player->getShape()->calculateLocalInertia(player->getMass(), inertia);

	//Set the body to the player
    player->setBody(new btRigidBody(player->getMass(), 
                state,
                player->getShape(), 
                inertia));	
}

void AnnEngine::addPlayerPhysicalBodyToDynamicsWorld()
{
    assert(player->getBody());

    float height(player->getEyesHeight());
    //player->getBody()->translate(btVector3(0,height,0));

	//TODO define name for the bullet's collision masks
    m_DynamicsWorld->addRigidBody(player->getBody(), BIT(0), BIT(1));
}

//move player's body IGNORING COLLISION !
void AnnEngine::translatePhysicBody(Ogre::Vector3 translation)
{
    player->getBody()->translate(btVector3(translation.x, translation.y, translation.z));
}

//the most convinient for controling the player : set the linear velocity
void AnnEngine::setPhysicBodyLinearSpeed(Ogre::Vector3 V)
{
    player->getBody()->setLinearVelocity(btVector3(V.x, V.y, V.z));
}

//loading ressources
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
    loadDir("media");
    loadZip("media/CORE.zip");
}

//initalize oculus rendering
void AnnEngine::oculusInit(bool fullscreen)
{   
    oor->initOculus(fullscreen);

    m_Camera = oor->getCameraInformationNode();

    m_Camera->setPosition(player->getPosition() + 
            Ogre::Vector3(0.0f, player->getEyesHeight(), 0.0f));
}

AnnGameObject* AnnEngine::createGameObject(const char entityName[], AnnGameObject* obj)
{
    try
    {
        if(std::string(entityName).empty())
            throw std::string("Hey! what are you trying to do here? Please specify a non empty string for entityName !");

        Ogre::Entity* ent = m_SceneManager->createEntity(entityName);

        Ogre::SceneNode* node = 
            m_SceneManager->getRootSceneNode()->createChildSceneNode();

        node->attachObject(ent);

        obj->setNode(node);
        obj->setEntity(ent);
        obj->setAudioEngine(AudioEngine);
        obj->setTimePtr(&deltaT);//Ok, ok, that's bad...

        obj->setBulletDynamicsWorld(m_DynamicsWorld);

        obj->postInit(); //Run post init directives

        objects.push_back(obj); //keep address in list

        std::stringstream ss;
        ss << "The object " << entityName << "has been created. Annwvyn memory address " << obj;  
        log(ss.str());
    }
    catch (std::string const& e)
    {
        log(e,false);
        delete obj;
        return NULL;
    }
    return obj;
}

bool AnnEngine::destroyGameObject(Annwvyn::AnnGameObject* object)
{
    std::stringstream ss;
    bool returnCode(false);
    for(size_t i(0); i < objects.size(); i++)
    {

        ss << "Object " << static_cast<void*>(objects[i]) << " stop collision test" << std::endl;
        log(ss.str());

        objects[i]->stopGettingCollisionWith(object);

        if(objects[i] == object)
        {
            ss.clear();
            ss << "Object found" << std::endl;
            log(ss.str());

            objects.erase(objects.begin() + i);
            Ogre::SceneNode* node = object->node();

            node->getParent()->removeChild(node);

            btRigidBody* body = object->getBody();
            m_DynamicsWorld->removeRigidBody(body);

            m_SceneManager->destroySceneNode(node);
            delete object;

            returnCode = true; // found
        }
    }
    return returnCode;
}


void AnnEngine::renderOneFrame()
{
    oor->RenderOneFrame();
}

Annwvyn::AnnLightObject* AnnEngine::addLight()
{
    //Actualy here i'm cheating, the AnnLightObjet is a simple typdef to Ogre LightSceneNode
    //I'll add a proper class to do it later
    AnnLightObject* Light = m_SceneManager->createLight();
    Light->setType(Ogre::Light::LT_POINT);
    return Light;
}

bool AnnEngine::requestStop()
{
    //pres ESC to quit. Stupid but efficient. I like that.
    if(m_Keyboard->isKeyDown(OIS::KC_ESCAPE))
        return true;
    return false;
}

void AnnEngine::updateCamera()
{
    m_Camera->setPosition(player->getPosition());
    m_Camera->setOrientation(QuatReference * player->getOrientation().toQuaternion());
}

void AnnEngine::doRender()
{
    updateCamera(); //update camera opsition from GameLogic
    renderOneFrame();
}

void AnnEngine::updateAudioSystemState()
{
    AudioEngine->updateListenerPos(oor->returnPose.position);
    AudioEngine->updateListenerOrient(oor->returnPose.orientation);
    for(unsigned int i = 0; i < objects.size(); i++)
        objects[i]->updateOpenAlPos();
}

void AnnEngine::applyMouseYaw()
{
    player->applyRelativeBodyYaw
        (Ogre::Radian(-m_Mouse->getMouseState().X.rel*player->getTurnSpeed()));
}

void AnnEngine::runBasicGameplay()
{
	//Test if there is a collision with the ground
    collisionWithGround();
    player->engineUpdate();

    //Dissmiss health and safety warning
    if(!oor->IsHsDissmissed()) //If not already dissmissed
        for(unsigned char kc = 0x00; kc <= 0xED; kc++) //For each keycode available (= every keyboard button)
            if(m_Keyboard->isKeyDown(static_cast<OIS::KeyCode>(kc))) //if tte selected keycode is available
                oor->dissmissHS();	//dissmiss the Health and Safety warning.
}

void AnnEngine::captureEvents()
{
    m_Keyboard->capture();
    m_Mouse->capture();
    if(m_Joystick)
        m_Joystick->capture();
}

float AnnEngine::updateTime()
{
    last = now;
    now = oor->getTimer()->getMilliseconds();
    return (now - last)/1000.0f;
}

void AnnEngine::refresh()
{
	//OIS Events
	captureEvents();
	//animations playing :
	deltaT = updateTime();
	playObjectsAnnimation();
	m_DynamicsWorld->stepSimulation(deltaT,2);

	runBasicGameplay();
	eventManager->update();

	processCollisionTesting();
	processTriggersContacts();

	if(debugPhysics)
		m_debugDrawer->step();

	//Call of refresh method
	for(AnnGameObjectVect::iterator it = objects.begin(); it != objects.end(); ++it)
		(*it)->atRefresh();

	if(VisualBodyAnimation)
		VisualBodyAnimation->addTime(getTime());
	updateAudioSystemState();
	doRender();
}

bool AnnEngine::isKeyDown(OIS::KeyCode key)
{
    return m_Keyboard->isKeyDown(key);
}

bool AnnEngine::collisionWithGround()
{
    //If collision isn't computable : 
    if(m_Ground == NULL || player->getBody() == NULL)
        return false;

    //Getting rid of differences of types. There is polymorphism we don't care of, we are just comparing memory addresses here!
    void* pplayer = (void*) player->getBody();
    void* ground = (void*) m_Ground->getBody();

    int numManifolds = m_Dispatcher->getNumManifolds();

    for (int i=0;i<numManifolds;i++)
    {
        btPersistentManifold* contactManifold =
            m_DynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);

        const btCollisionObject* obA = (btCollisionObject*) contactManifold->getBody0();
        const btCollisionObject* obB = (btCollisionObject*) contactManifold->getBody1();

        void* pair1 = (void*) obA;
        void* pair2 = (void*) obB;

        if((pair1 == pplayer && pair2 == ground) || (pair2 == pplayer && pair1 == ground))
        {
            int numContacts = contactManifold->getNumContacts();
            if(numContacts > 0)
            {
                player->contactWithGround = true;
                return true;
            }
            else
            {
                player->contactWithGround = false;
                return false;
            }
        }
    }
    return false;
}

void AnnEngine::processCollisionTesting()
{
    //TODO make a typedeff for getting off the uglyness here 
    std::vector<struct collisionTest*> pairs;

    //get all collision mask
    for(size_t i = 0; i < objects.size(); i++)
    {
        std::vector<struct collisionTest*> onThisObject = 
            objects[i]->getCollisionMask();

        for(size_t j = 0; j < onThisObject.size(); j++)
            pairs.push_back(onThisObject[j]);
    }

    //process for each maniflod

    int numManifolds = m_Dispatcher->getNumManifolds();
    //m is manifold identifier
    for (int m = 0;m <numManifolds;m++)
    {
        btPersistentManifold* contactManifold =
            m_DynamicsWorld->getDispatcher()->getManifoldByIndexInternal(m);

        const btCollisionObject* obA = (btCollisionObject*) contactManifold->getBody0();
        const btCollisionObject* obB = (btCollisionObject*) contactManifold->getBody1();

        //CAUTION HERE !
        //
        //we deliberatly lost track of objects types to just test THE ADDRESS of the pointer
        //Comparaison between pointer of differents types aren't permited in C++.

        void* pair1 = (void*) obA;
        void* pair2 = (void*) obB;

        for(size_t p = 0; p < pairs.size(); p++)
        {
            void* body1 = (void*) pairs[p]->Object->getBody();
            void* body2 = (void*) pairs[p]->Receiver->getBody();

            if((pair1 == body1 && pair2 == body2) || 
                    (pair2 == body1 && pair1 == body2))
            {
                int numContacts = contactManifold->getNumContacts();
                if(numContacts > 0)
                {
                    pairs[p]->collisionState = true;
                }
                else
                {
                    pairs[p]->collisionState = false;
                }
                break;
            }
        }
    }
}

AnnTriggerObject* AnnEngine::createTriggerObject(AnnTriggerObject* object)
{
    assert(object);
    triggers.push_back(object);
    object->postInit();
    return object;
}

void AnnEngine::processTriggersContacts()
{
    for(size_t i = 0; i < triggers.size(); i++)
    {
        if(Tools::Geometry::distance(player->getPosition(),
                    triggers[i]->getPosition()) <= triggers[i]->getThreshold())
        {
            triggers[i]->setContactInformation(true);
            //Call overloaded 
            triggers[i]->atContact();
        }
        else
        {
            triggers[i]->setContactInformation(false);
        }
    }
}

void AnnEngine::playObjectsAnnimation()
{
    for(size_t i = 0; i < objects.size(); i++)
        objects[i]->addTime(oor->getUpdateTime());
}

AnnGameObject* AnnEngine::playerLookingAt()
{
    //Origin vector
    Ogre::Vector3 Orig(oor->lastOculusPosition);

    //Direction Vector
    Ogre::Quaternion Orient = oor->lastOculusOrientation;
    Ogre::Vector3 LookAt = Orient * Ogre::Vector3::NEGATIVE_UNIT_Z;

    //create ray
    Ogre::Ray ray(Orig,LookAt);

    //create query
    Ogre::RaySceneQuery* raySceneQuery = m_SceneManager->createRayQuery(ray);
    raySceneQuery->setSortByDistance(true);

    //execute and get the results
    Ogre::RaySceneQueryResult& result = raySceneQuery->execute();

    //read the result list
    Ogre::RaySceneQueryResult::iterator it;
    bool found(false);Ogre::SceneNode* node;

    for(it = result.begin(); it != result.end(); it++)
    {
        if(it->movable && it->movable->getMovableType() == "Entity")
        {
            node = it->movable->getParentSceneNode();
            found = true;
            break;
        }	
    }

    if(found)
        for(size_t i = 0; i < objects.size(); i++)
            if((void*)objects[i]->node() == (void*)node)
                return objects[i];

    return NULL; //means that we don't know what the player is looking at.
}

void AnnEngine::attachVisualBody(const std::string entityName, float z_offset, bool flip, bool animated , Ogre::Vector3 scale)
{
    log("Visual Body");
    log(entityName);

    Ogre::Entity* ent = m_SceneManager->createEntity(entityName);
    VisualBodyAnchor = m_Camera->createChildSceneNode();
    VisualBodyAnchor->attachObject(ent);

    if(flip)
        refVisualBody = Ogre::Quaternion(Ogre::Degree(180),Ogre::Vector3::UNIT_Y);
    else
        refVisualBody = Ogre::Quaternion::IDENTITY;

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
    oor->recenter();
}

Annwvyn::AnnGameObject* AnnEngine::getFromNode(Ogre::SceneNode* node)
{
    try 
    { 
        if(!node) 
            throw 0; 
    } 
    catch (int e) 
    { 
        if(e==0)
            log("Plese do not try to identify a NULL node.");
        return NULL;
    }

    //This methods only test memory address
    for(size_t i(0); i < objects.size(); i++)
        if((void*)objects[i]->node() == (void*)node)
            return objects[i];
    return NULL;
}

////////////////////////////////////////////////////////// GETTERS

Annwvyn::bodyParams* AnnEngine::getBodyParams()
{
    //BAD!
    return player->getLowLevelBodyParams();
}

Ogre::SceneNode* AnnEngine::getCamera()
{
    return m_Camera;
}

Ogre::Quaternion AnnEngine::getReferenceQuaternion()
{
    return QuatReference;
}

float AnnEngine::getTime()
{
    return deltaT;
}

AnnAudioEngine* AnnEngine::getAudioEngine()
{
    return AudioEngine;
}

OIS::Mouse* AnnEngine::getOISMouse()
{
    return m_Mouse;
}

OIS::Keyboard* AnnEngine::getOISKeyboard()
{
    return m_Keyboard;
}

OIS::JoyStick* AnnEngine::getOISJoyStick()
{
    return m_Joystick;
}

btDiscreteDynamicsWorld* AnnEngine::getDynamicsWorld()
{
    return m_DynamicsWorld;
}

Ogre::SceneManager* AnnEngine::getSceneManager()
{
    return m_SceneManager;
}

float AnnEngine::getTimeFromStartUp()
{
    return static_cast<float>(oor->getTimer()->getMilliseconds());//Why ?? O.O 
}

////////////////////////////////////////////////////////// SETTERS

void AnnEngine::setReferenceQuaternion(Ogre::Quaternion q)
{
    QuatReference = q;
}

void AnnEngine::setDebugPhysicState(bool state)
{
    debugPhysics = state;
}

void AnnEngine::setAmbiantLight(Ogre::ColourValue v)
{
    m_SceneManager->setAmbientLight(v);
}

void AnnEngine::setGround(AnnGameObject* Ground)
{
    m_Ground = Ground;
}

void AnnEngine::setSkyDomeMaterial(bool activate, const char materialName[], float curvature, float tiling)
{
    m_SceneManager->setSkyDome(activate, materialName, curvature, tiling);
}

void AnnEngine::setNearClippingDistance(Ogre::Real nearClippingDistance)
{
    if(oor)
        oor->setCamerasNearClippingDistance(nearClippingDistance);
}

void AnnEngine::useDefaultEventListener()
{
    assert(eventManager);

    log("Reconfiguring the engine to use the default event listener");

	//Remove the current event listener (if any)
    eventManager->removeListener();

	//If the event listenre isn't allready initialized, allocate one
    if(!defaultEventListener)
        defaultEventListener = new AnnDefaultEventListener(getPlayer());

	//Set the default event listener to the event manager
    eventManager->setListener(defaultEventListener);
}

AnnDefaultEventListener* AnnEngine::getInEngineDefaultListener()
{
    return defaultEventListener;
}
