#include "AnnEngine.hpp"

using namespace Annwvyn;

AnnEngine::AnnEngine(const char title[])
{
	//-----------------TODO : factorise this method into multiples ones-----------------------\\

	m_Camera = NULL;

	log("Annwvyn Game Engine - Step into the Other World",false);
	log("Desinged for Virtual Reality",false);

	//at this point, loading ressources will cause a segfault
	readyForLoadingRessources = false;
	
	//This structure handle player's body parameters
	m_bodyParams = new bodyParams;
	log("Virtual body declared");
	
	//here we set all the defaults parameters for the body.
	initBodyParams(m_bodyParams);


	log("Setting up Ogre");
	
	//this method open the Ogre config pop-up, it's good for now but in the future the program will be able to directly load parameters
	m_Root = askSetUpOgre();					//root
	//	GOOD TO KNOW : 'askSetUpOgre()' will close the program if the player hit the "Cancel" button
	
	log("Create window");
	m_Window = m_Root->initialise(true,title);

	log("Create Ogre OctreeSceneManager");
	m_SceneManager = m_Root->createSceneManager("OctreeSceneManager"); //smgr


	if(m_Window != NULL)
	{
		//this boolean forbid segfault when you try to load ressources
		readyForLoadingRessources = true;
	}
	else
	{
		log("Error creating window");
		exit(-1); //goodbye cruel world...
	}

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
    

    m_Keyboard = static_cast<OIS::Keyboard*>(m_InputManager->createInputObject(OIS::OISKeyboard,true));
	m_Mouse = static_cast<OIS::Mouse*>(m_InputManager->createInputObject(OIS::OISMouse,true));
	
    if(m_InputManager->getNumberOfDevices(OIS::OISJoyStick) > 0)
        m_Joystick = static_cast<OIS::JoyStick*>(m_InputManager->createInputObject(OIS::OISJoyStick,true));
	
    //THESE are the basic gameplay that you can use out of the box. set these variables to false to use costum one.
	activateWASD = true; 
	// * move around with WASD keys (or, if you are running windows, local equivalent. I've an AZERTY keyboard and it handle ZQSD keys without doing anything special)
	// * run with SHIFT pressed
	activateJump = true; 
	// * jump with space if your feet touch the ground (m_groudn object)
	jumpForce = 250.0f;
	
	//animation are time-based
	
	//time initializer
	log("Setup time");
	last = m_Root->getTimer()->getMilliseconds();
	now = last;

	//Bullet is a great physic motor, we init a dynamicsworld 

	//Create the physics world
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
  	
	log("Engine ready");

	//we can draw physic debuging while rendering
	debugPhysics = false;//by default
	m_debugDrawer = new BtOgre::DebugDrawer(m_SceneManager->getRootSceneNode(), m_DynamicsWorld);
	m_DynamicsWorld->setDebugDrawer(m_debugDrawer);

	//This quaternion lock the body orientation to be stricly vertical
	Ogre::Quaternion Orient(Ogre::Radian(0),Ogre::Vector3(0,0,1));
	fixedBodyOrient = btQuaternion(Orient.x, Orient.y,Orient.z,Orient.w);

	//colision with this object will allow the player to jump
	m_Ground = NULL;

	//OpenAl is handeled thanks to this class
	AudioEngine = new AnnAudioEngine;
    
    m_CEGUI_Renderer = NULL;
    initCEGUI();

    QuatReference = Ogre::Quaternion::IDENTITY;
}

void AnnEngine::setReferenceQuaternion(Ogre::Quaternion q)
{
    QuatReference = q;
}

Ogre::Quaternion AnnEngine::getReferenceQuaternion()
{
    return QuatReference;
}


AnnEngine::~AnnEngine()
{
	//delete all stuf created on the engine 
	for(unsigned int i(0); i < objects.size(); i++)
	{
		delete objects[i];
		objects.erase(objects.begin()+i);
	}
	delete m_DynamicsWorld;
	delete m_Broadphase;
	delete m_bodyParams;
	delete m_CollisionConfiguration;
	delete m_Dispatcher;
	delete m_Solver;
	
	delete m_debugDrawer;
	delete m_Keyboard;
	delete m_Mouse;

	delete AudioEngine;
}

void AnnEngine::initCEGUI()
{
    m_CEGUI_Renderer = &CEGUI::OgreRenderer::bootstrapSystem();
}

Ogre::Root* AnnEngine::askSetUpOgre(Ogre::Root* root)
{
	if(!root->showConfigDialog()) 
		exit(-1); //If you hit cancel or close the window
	return root;
}

//see the .hpp file for the defaults values
void AnnEngine::initBodyParams(Annwvyn::bodyParams* bodyP,
	float eyesHeight,
	float walkSpeed,
	float turnSpeed,
	float mass,
	Ogre::Vector3 Position,
	Ogre::Quaternion HeadOrientation,
	btCollisionShape* Shape,
	btRigidBody* Body)
{
	bodyP->eyeHeight = eyesHeight;
	bodyP->walkSpeed = walkSpeed;
	bodyP->turnSpeed = turnSpeed;
	bodyP->mass = mass;
	bodyP->Position = Position;
	bodyP->HeadOrientation = HeadOrientation;
	bodyP->Shape = Shape;
	bodyP->Body = Body;
}

//Convinient method to the user to call : do it and let go !
void AnnEngine::initPlayerPhysics()
{
	createVirtualBodyShape();
	createPlayerPhysicalVirtualBody();
	addPlayerPhysicalBodyToDynamicsWorld();
}


void AnnEngine::createVirtualBodyShape()
{
	float height = m_bodyParams->eyeHeight + 0.2f;
	//m_bodyParams->Shape = new btBoxShape(btVector3(1,height,1));
	m_bodyParams->Shape = new btCapsuleShape(0.5,height); //We block rotation
}

void AnnEngine::createPlayerPhysicalVirtualBody()
{
	//the VirtualBody is a bullet shape used to detect collison and to apply gravity on the player
	if(m_bodyParams->Shape == NULL)
		return;

	BtOgre::RigidBodyState *state = new BtOgre::RigidBodyState(oculus.getCameraNode());
	btVector3 inertia;
	
	m_bodyParams->Shape->calculateLocalInertia(m_bodyParams->mass,inertia);

	m_bodyParams->Body = new btRigidBody(m_bodyParams->mass, 
		state,
		m_bodyParams->Shape, 
		inertia);	
}


void AnnEngine::addPlayerPhysicalBodyToDynamicsWorld()
{
	if(m_bodyParams->Body == NULL)
		return;
	

	float height(m_bodyParams->eyeHeight +10);

	m_DynamicsWorld->addRigidBody(m_bodyParams->Body);
	
	btVector3 pos = btVector3(m_bodyParams->Position.x,
		m_bodyParams->Position.y,
		m_bodyParams->Position.z);
	
	pos += btVector3(0,height/2,0);

	m_bodyParams->Body->translate(pos);
}

void AnnEngine::updatePlayerFromPhysics()
{
	if (m_bodyParams->Body == NULL)
		return;
	
	//Get pos from bullet
	btVector3 phyPos = m_bodyParams->Body->getCenterOfMassPosition();
	m_bodyParams->Position =
		Ogre::Vector3(phyPos.getX(),
		phyPos.getY(),
		phyPos.getZ());
	
	//Get orientation from bullet
	btQuaternion phyOrient = m_bodyParams->Body->getOrientation();

	Ogre::Euler GraphicOrient;
	GraphicOrient.fromQuaternion
		(Ogre::Quaternion(phyOrient.getW(),
		phyOrient.getX(),
		phyOrient.getY(),
		phyOrient.getZ()));
	m_bodyParams->Orientation = GraphicOrient;
}

//move player's body IGNORING COLLISION !
void AnnEngine::translatePhysicBody(Ogre::Vector3 translation)
{
	m_bodyParams->Body->translate(btVector3(translation.x,translation.y,translation.z));
}

//the most convinient for controling the player : set the linear velocity
void AnnEngine::setPhysicBodyLinearSpeed(Ogre::Vector3 V)
{
	m_bodyParams->Body->setLinearVelocity(btVector3(V.x,V.y,V.z));
}

//loading ressources
void AnnEngine::loadZip(const char path[])
{
	log("Loading resources from Zip archive :");
	log(path, false);
	if(readyForLoadingRessources)
		Ogre::ResourceGroupManager::getSingleton().addResourceLocation(path,"Zip");
}

void AnnEngine::loadDir(const char path[])
{
	log("Loading resources from Filesystem directory :");
	log(path, false);
	if(readyForLoadingRessources)
		Ogre::ResourceGroupManager::getSingleton().addResourceLocation(path,"FileSystem");
}

void AnnEngine::loadResFile(const char path[])
{
    Ogre::String res= path;
    Ogre::ConfigFile cf;
    cf.load(res);
    Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();

    Ogre::String secName, typeName, archName;
    while (seci.hasMoreElements())
    {
        secName = seci.peekNextKey();
        Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
        Ogre::ConfigFile::SettingsMultiMap::iterator i;
        for (i = settings->begin(); i != settings->end(); ++i)
        {
            typeName = i->first;
            archName = i->second;
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
                    archName, typeName, secName);
        }
    }
}

void AnnEngine::initRessources()
{
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	log("Resources initialized");
}

//initalize oculus rendering
void AnnEngine::oculusInit()
{
	log("Initialize Oculus system");
	oculus.setupOculus();
	log("Configuring Ogre Rendering from Oculus system");
	oculus.setupOgre(m_SceneManager,m_Window);
	log("Creating camera");
	m_Camera = oculus.getCameraNode();
	
	m_Camera->setPosition(m_bodyParams->Position + 
		Ogre::Vector3(0.0f,m_bodyParams->eyeHeight,0.0f));

	//oculus.setupDriftCorrection();
}


AnnGameObject* AnnEngine::createGameObject(const char entityName[])
{
	if(std::string(entityName).empty())
		return NULL;

	Annwvyn::AnnGameObject* obj = new AnnGameObject;
	Ogre::Entity* ent = m_SceneManager->createEntity(entityName);

	Ogre::SceneNode* node = 
		m_SceneManager->getRootSceneNode()->createChildSceneNode();
	
	node->attachObject(ent);

	obj->setNode(node);
	obj->setEntity(ent);
	obj->setAudioEngine(AudioEngine);

	obj->setBulletDynamicsWorld(m_DynamicsWorld);

	objects.push_back(obj); //keep address in list

	return obj;
}

void AnnEngine::renderOneFrame()
{
	m_Root->renderOneFrame();
#if OGRE_PLATFORM == PLATFORM_WIN32 || OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    Sleep(1); //pause 1ms
#elif __gnu_linux__
    usleep(1000);//pause 1ms
#endif
}

void AnnEngine::setDebugPhysicState(bool state)
{
	debugPhysics = state;
}

void AnnEngine::setAmbiantLight(Ogre::ColourValue v)
{
	m_SceneManager->setAmbientLight(v);
}

Annwvyn::AnnLightObject* AnnEngine::addLight()
{
	//Actualy here i'm cheating, the AnnLightObjet is a simple typdef to Ogre LightSceneNode
	//I'll add a proper class to do it later
	AnnLightObject* Light = m_SceneManager->createLight();
	Light->setType(Ogre::Light::LT_POINT);
	return Light;
}

void AnnEngine::runPhysics()
{
	for(unsigned int i(0); i < objects.size(); i++)
		objects[i]->stepBulletSimulation();
}

bool AnnEngine::requestStop()
{
	//pres ESC to quite. Stupid but efficient. I like that.
	if(m_Keyboard->isKeyDown(OIS::KC_ESCAPE))
		return true;
	return false;
}

void AnnEngine::log(std::string message,bool flag)
{
	if(flag)
		std::cerr << "Annwvyn - ";
	std::cerr << message << std::endl;
}

void AnnEngine::updateCamera()
{
	oculus.getCameraNode()->setPosition(m_bodyParams->Position);
    Ogre::Quaternion temp = QuatReference * m_bodyParams->Orientation.toQuaternion();
	oculus.getCameraNode()->setOrientation(temp * oculus.getOrientation());
}

void AnnEngine::refresh()
{

	//windows specific
#if OGRE_PLATFORM == PLATFORM_WIN32 || OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	MSG msg;
	// Check for windows messages
	while( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
	{
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}
#else
	//not windows specific equivalent
    Ogre::WindowEventUtilities::messagePump();
#endif

	updateCamera();
	//bullet part
	m_DynamicsWorld->stepSimulation(1.f/60,1);

	//ogre part
	Ogre::Vector3 translate(0,0,0);
	
	//animations playing :
	deltaT = updateTime();
	playObjectsAnnimation();

	//	OIS Events 
	captureEvents();
	if(activateWASD && m_bodyParams->Body != NULL)//classic fps control
	{
    	m_bodyParams->Body->activate(); //don't sleep !
		btVector3 curVel = m_bodyParams->Body->getLinearVelocity();
		if(processWASD(&translate))
		{
		 	Ogre::Vector3 velocity = m_bodyParams->Orientation*(translate);
			m_bodyParams->Body->setLinearVelocity(
				btVector3(velocity.x,curVel.y(),velocity.z));
		}
		else
		{	
			m_bodyParams->Body->setLinearVelocity((curVel * btVector3(0,1,0)));//we keep the original vertical velocity
		}
	}

    if(m_bodyParams->Body != NULL)
    {
	    btTransform Transform = m_bodyParams->Body->getCenterOfMassTransform();
	    Transform.setRotation(fixedBodyOrient);
	    m_bodyParams->Body->setCenterOfMassTransform(Transform);
    }

	m_bodyParams->Orientation.yaw
		(Ogre::Radian(-m_Mouse->getMouseState().X.rel*m_bodyParams->turnSpeed));
		
    if(m_bodyParams->Body != NULL)
    {
	m_bodyParams->Position =
		Ogre::Vector3( 
		m_bodyParams->Body->getCenterOfMassPosition().x(),
		m_bodyParams->Body->getCenterOfMassPosition().y() + m_bodyParams->eyeHeight/2,
		m_bodyParams->Body->getCenterOfMassPosition().z());
    }

	if(m_Ground != NULL)
		if(activateJump)
		{
			
			bool colide(collisionWithGround());
			
			if(colide)
				if(m_Keyboard->isKeyDown(OIS::KC_SPACE))
				{
					m_bodyParams->Body->applyCentralImpulse(btVector3(0,jumpForce,0));
				}
		}

		processCollisionTesting();
		processTriggersContacts();

	if(debugPhysics)
	{
		m_debugDrawer->step();
		//display triggers position, influence zone and state
	}

	AudioEngine->updateListenerPos(oculus.getCameraNode()->getPosition());
	AudioEngine->updateListenerOrient(oculus.getCameraNode()->getOrientation());
	
	for(unsigned int i = 0; i < objects.size(); i++)
		objects[i]->updateOpenAlPos();

	renderOneFrame();
}

bool AnnEngine::processWASD(Ogre::Vector3* translate)
{
	bool move(false);
		if(m_Keyboard->isKeyDown(OIS::KC_W))
		{
			move = true;
			translate->z = -m_bodyParams->walkSpeed;
		}
		if(m_Keyboard->isKeyDown(OIS::KC_S))
		{	
			move = true;
			translate->z = m_bodyParams->walkSpeed;
		}
		if(m_Keyboard->isKeyDown(OIS::KC_A))
		{
			move = true;
			translate->x = -m_bodyParams->walkSpeed;
		}
		if(m_Keyboard->isKeyDown(OIS::KC_D))
		{
			move = true;
			translate->x = m_bodyParams->walkSpeed;
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
	now = m_Root->getTimer()->getMilliseconds();
	return (now-last)/1000.0f;
}

float AnnEngine::getTime()
{
	return deltaT;
}

void AnnEngine::setGround(AnnGameObject* Ground)
{
	m_Ground = Ground;
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

bool AnnEngine::isKeyDown(OIS::KeyCode key)
{
	return m_Keyboard->isKeyDown(key);
}

bool AnnEngine::collisionWithGround()
{
	if(m_Ground == NULL || m_bodyParams == NULL || m_bodyParams->Body == NULL)
		return false;

	void* player = (void*) m_bodyParams->Body;
	void* ground = (void*) m_Ground->getBody();
	
	int numManifolds = m_Dispatcher->getNumManifolds();

	for (int i=0;i<numManifolds;i++)
	{
		btPersistentManifold* contactManifold =
		m_DynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);

		const btCollisionObject* obA = contactManifold->getBody0();
		const btCollisionObject* obB = contactManifold->getBody1();

		void* pair1 = (void*) obA;
		void* pair2 = (void*) obB;

		if((pair1 == player && pair2 == ground) || (pair2 == player && pair1 == ground))
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
	for (int m = 0;m <numManifolds;m++)
	{
		btPersistentManifold* contactManifold =
		m_DynamicsWorld->getDispatcher()->getManifoldByIndexInternal(m);

		const btCollisionObject* obA = contactManifold->getBody0();
		const btCollisionObject* obB = contactManifold->getBody1();

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

btDiscreteDynamicsWorld* AnnEngine::getDynamicsWorld()
{
	return m_DynamicsWorld;
}

AnnTriggerObject* AnnEngine::createTriggerObject()
{
	AnnTriggerObject* object = new AnnTriggerObject;

	triggers.push_back(object);

	return object;
}

void AnnEngine::processTriggersContacts()
{
	for(size_t i = 0; i < triggers.size(); i++)
	{
		if(Tools::Geometry::distance(m_bodyParams->Position,
			triggers[i]->getPosition()) <= triggers[i]->getThreshold())
		{
			triggers[i]->setContactInformation(true);
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
		objects[i]->addTime(getTime());
}

Ogre::SceneManager* AnnEngine::getSceneManager()
{
		return m_SceneManager;
}

void AnnEngine::setSkyDomeMaterial(bool activate, const char materialName[], float curvature, float tiling)
{
	m_SceneManager->setSkyDome(activate,materialName,curvature,tiling);
}

AnnGameObject* AnnEngine::playerLookingAt()
{
	//Origin vector
	Ogre::Vector3 Orig(oculus.getCameraNode()->getPosition());
	//Direction Vector
	Ogre::Quaternion Orient = oculus.getCameraNode()->getOrientation();
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

	return NULL;
}



Annwvyn::bodyParams* AnnEngine::getBodyParams()
{
	return m_bodyParams;
}

Ogre::SceneNode* AnnEngine::getCamera()
{
	return oculus.getCameraNode();
}

float AnnEngine::getCentreOffset()
{
    return oculus.getCentreOffset();
}

