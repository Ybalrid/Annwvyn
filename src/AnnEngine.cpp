#include "AnnEngine.hpp"

using namespace Annwvyn;

AnnEngine::AnnEngine(const char title[])
{
    m_Camera = NULL;

    //block ressources loading for now
    readyForLoadingRessources = false;

    //This structure handle player's body parameters
    player = new AnnPlayer;
	eventManager = new AnnEventManager;
    
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
        objects.erase(objects.begin()+i);
    }

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
    //Only keyboard/mouse is suported for now but in the future we will try joystick and this kind of controller.

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

    //basic gameplay that you can use out of the box
    activateWASD = true; 
    //move around with WASD keys
    //run with SHIFT pressed
    activateJump = true; 
    //jump with space if your feet touch the ground (m_Groudn object)
    jumpForce = 100.0f;

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
    log("Setup time");
    last = oor->getTimer()->getMilliseconds();
    now = last;
    log("Timer is setup");
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
	//There is no GUI for now...
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
    assert(player != NULL);
    player->setShape(new btCapsuleShape(0.25,player->getEyesHeight()/2));
}

void AnnEngine::createPlayerPhysicalVirtualBody()
{
	assert(player->getShape());

    BtOgre::RigidBodyState *state = new BtOgre::RigidBodyState
        (m_Camera);

    btVector3 inertia;
	player->getShape()->calculateLocalInertia(player->getMass(), inertia);

    player->setBody(new btRigidBody(player->getMass(), 
            state,
            player->getShape(), 
            inertia));	
}

void AnnEngine::addPlayerPhysicalBodyToDynamicsWorld()
{
    assert(player->getBody());

	float height(player->getEyesHeight());
    m_DynamicsWorld->addRigidBody(player->getBody());
}

void AnnEngine::updatePlayerFromPhysics()
{
    if(!player->getBody())
        return;

    //Get pos from bullet
    btVector3 phyPos = player->getBody()->getCenterOfMassPosition();

    player->setPosition(Ogre::Vector3
		(phyPos.getX(),
         phyPos.getY(),
         phyPos.getZ()));

    //Get orientation from bullet
    btQuaternion phyOrient = player->getBody()->getOrientation();

    Ogre::Euler GraphicOrient;
    GraphicOrient.fromQuaternion
        (Ogre::Quaternion(phyOrient.getW(),
                          phyOrient.getX(),
                          phyOrient.getY(),
                          phyOrient.getZ()));


    player->setOrientation(GraphicOrient);
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
    bool returnCode(false);
    for(size_t i(0); i < objects.size(); i++)
    {
        std::cerr << "Object " << static_cast<void*>(objects[i]) << " stop collision test" << std::endl;

        objects[i]->stopGettingCollisionWith(object);

        if(objects[i] == object)
        {
            std::cout << "Object found" << std::endl;
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
    AudioEngine->updateListenerPos(oor->lastOculusPosition);
    AudioEngine->updateListenerOrient(oor->lastOculusOrientation);
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
  

	player->engineUpdate();

	//Dissmiss health and safety warning
	if(!oor->IsHsDissmissed()) //If not already dissmissed
		for(unsigned char kc = 0x00; kc <= 0xED; kc++) //For each keycode available (= every keyboard button)
			if(m_Keyboard->isKeyDown(static_cast<OIS::KeyCode>(kc))) //if tte selected keycode is available
				oor->dissmissHS();	//dissmiss the Health and Safety warning.


    if(activateWASD && player->getBody() != NULL) //classic fps control
    {
        //TODO extract this piece of code and make it accesible with a method !!
        player->getBody()->activate(); //don't sleep !

        btVector3 curVel = player->getBody()->getLinearVelocity(); //get current velocity

        Ogre::Vector3 translate(Ogre::Vector3::ZERO);
        if(processWASD(&translate)) //If player want to move w/ WASD
        {
            Ogre::Vector3 velocity = player->getOrientation()*(translate);
            player->getBody()->setLinearVelocity(
                    btVector3(velocity.x, curVel.y(), velocity.z));
        }
        else
        {	
            //Just apply effect of gravity.
            player->getBody()->setLinearVelocity(curVel * btVector3(0,1,0)); //we keep the original vertical velocity only
        }
    }//body & WASD

    if(player->getBody() != NULL) //if physic
    {
        btTransform Transform = player->getBody()->getCenterOfMassTransform();
        Transform.setRotation(btQuaternion(0,0,0,1));
        player->getBody()->setCenterOfMassTransform(Transform);
    }

    
	applyMouseYaw();

    if(player->getBody() != NULL)
        player->setPosition(
            Ogre::Vector3( 
                    player->getBody()->getCenterOfMassPosition().x(),
                    player->getBody()->getCenterOfMassPosition().y() + player->getEyesHeight()/2,
                    player->getBody()->getCenterOfMassPosition().z()
					));


    if(m_Ground != NULL && activateJump && collisionWithGround() && m_Keyboard->isKeyDown(OIS::KC_SPACE))
                    player->getBody()->applyCentralImpulse(btVector3(0,jumpForce,0));
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
    {
        m_debugDrawer->step();
        //TODO display triggers position, influence zone and state... 
    }

    //Call of refresh method
    for(size_t i(0); i < objects.size(); i++)
        objects[i]->atRefresh();

    if(VisualBodyAnimation)
        VisualBodyAnimation->addTime(getTime());

	doRender();
}

bool AnnEngine::processWASD(Ogre::Vector3* translate)
{
    bool move(false);
    if(m_Keyboard->isKeyDown(OIS::KC_W))
    {
        move = true;
        translate->z = -player->getWalkSpeed();
    }
    if(m_Keyboard->isKeyDown(OIS::KC_S))
    {	
        move = true;
        translate->z = player->getWalkSpeed();
    }
    if(m_Keyboard->isKeyDown(OIS::KC_A))
    {
        move = true;
        translate->x = -player->getWalkSpeed();
    }
    if(m_Keyboard->isKeyDown(OIS::KC_D))
    {
        move = true;
        translate->x = player->getWalkSpeed();
    }
    if(m_Keyboard->isModifierDown(OIS::Keyboard::Shift))
    {
        move = true;
        *translate *= 5;
    }
    return move;
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

bool AnnEngine::isKeyDown(OIS::KeyCode key)
{
    assert(m_Keyboard != NULL);
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
                return true;
            return false;
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
    assert(object != NULL);
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
        Ogre::AnimationState* as = ent->getAnimationState("IDLE");
        as->setLoop(true);
        as->setEnabled(true);

        VisualBodyAnimation = as;
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