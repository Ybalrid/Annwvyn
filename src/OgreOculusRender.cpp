#include "OgreOculusRender.hpp"

OgreOculusRender::OgreOculusRender(std::string winName)
{
	//Initialize some variables
	name = winName;
	root = NULL;
	window = NULL;
	smgr = NULL;

	for(size_t i(0); i < 2; i++)
	{
		cams[i] = NULL;
		rtts[i] = NULL;
		vpts[i] = NULL;
	}

	//Oc is an OculusInterface object. Communication with the Rift SDK is handeled by that class
	oc = NULL;
	CameraNode = NULL;

	cameraPosition = Ogre::Vector3(0,0,10);
	cameraOrientation = Ogre::Quaternion::IDENTITY;
	
	this->nearClippingDistance = (float) 0.05;
	this->lastOculusPosition = cameraPosition;
	this->lastOculusOrientation = cameraOrientation;
	this->updateTime = 0;
	
	fullscreen = true;
	hsDissmissed = false;
}

OgreOculusRender::~OgreOculusRender()
{
	//The only thig we dynamicly load is the oculus interface
	delete oc;
}

//I may move this method back to the AnnEngine class... 
void OgreOculusRender::loadReseourceFile(const char path[])
{
	/*from ogre wiki : load the given resource file*/
	Ogre::ConfigFile configFile;
	configFile.load(path);
	Ogre::ConfigFile::SectionIterator seci = configFile.getSectionIterator();
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

//See commant of the loadResourceFile method
void OgreOculusRender::initAllResources()
{
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}

void OgreOculusRender::initLibraries()
{
	//Create the ogre root with standards Ogre configuration file
	root = new Ogre::Root("plugins.cfg","ogre.cfg","Ogre.log");

	//Class to get basic information from the Rift. Initialize the RiftSDK
	oc = new OculusInterface();
}

void OgreOculusRender::initialize()
{
	//init libraries;
	initLibraries();

	//Mandatory. If thouse pointers are unitilalized, program have to stop here.
	assert(root != NULL && oc != NULL);

	//Get configuration via ogre.cfg OR via the config dialog.
	getOgreConfig();

	//Create the render window with the given sice from the Oculus
	createWindow();

	//Load resources from the resources.cfg file
	loadReseourceFile("resources.cfg");
	initAllResources();

	//Create scene manager
	initScene();

	//Create cameras and handeling nodes
	initCameras();

	//Create rtts and viewports on them
	initRttRendering();

	//Init the oculus rendering
	initOculus();
}

void OgreOculusRender::getOgreConfig()
{
	assert(root != NULL);
	if(!root->restoreConfig())
		if(!root->showConfigDialog())
			abort();
}

void OgreOculusRender::createWindow()
{
	assert(root != NULL && oc != NULL);
	Ogre::NameValuePairList misc;

	//This one only works on windows : "Borderless = no decoration"
	misc["border"]="none";
	misc["vsync"]="true";
	misc["displayFrequency"]="75";
	misc["monitorIndex"]="1"; //Use the 2nd monitor, assuming the Oculus Rift is not the primary. Or is the only screen on the system.

	//Initialize a window ans specify that creation is manual
	window = root->initialise(false, name);

	//Create a non-fullscreen window using custom parameters
	if(fullscreen)
		window = root->createRenderWindow(name, oc->getHmd()->Resolution.w, oc->getHmd()->Resolution.h, true,&misc);
	else
		window = root->createRenderWindow(name, oc->getHmd()->Resolution.w, oc->getHmd()->Resolution.h, false, &misc);

	//Put the window at the place given by the SDK
	window->reposition(oc->getHmd()->WindowsPos.x,oc->getHmd()->WindowsPos.y);
}

void OgreOculusRender::initCameras()
{
	assert(smgr != NULL);
	cams[left] = smgr->createCamera("lcam");
	cams[right] = smgr->createCamera("rcam");
	for(int i = 0; i < 2; i++)
	{
		cams[i]->setPosition(cameraPosition);
		cams[i]->setAutoAspectRatio(true);
		cams[i]->setNearClipDistance(1);
		cams[i]->setFarClipDistance(1000);
	}

	//do NOT attach camera to this node...
	CameraNode = smgr->getRootSceneNode()->createChildSceneNode();
}

void OgreOculusRender::setCamerasNearClippingDistance(float distance)
{
	nearClippingDistance = distance;
}

void OgreOculusRender::initScene()
{
	assert(root != NULL);
	smgr = root->createSceneManager("OctreeSceneManager","OSM_SMGR");
}

void OgreOculusRender::initRttRendering()
{
	//get texture sice from ovr with default FOV
	texSizeL = ovrHmd_GetFovTextureSize(oc->getHmd(), ovrEye_Left, oc->getHmd()->DefaultEyeFov[0], 1.0f);
	texSizeR = ovrHmd_GetFovTextureSize(oc->getHmd(), ovrEye_Right, oc->getHmd()->DefaultEyeFov[1], 1.0f);

	//Create texture
	Ogre::TexturePtr rtt_textureL = Ogre::TextureManager::getSingleton().createManual("RttTexL", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::TEX_TYPE_2D, texSizeL.w, texSizeL.h, 0, Ogre::PF_R8G8B8, Ogre::TU_RENDERTARGET);
	Ogre::TexturePtr rtt_textureR = Ogre::TextureManager::getSingleton().createManual("RttTexR", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::TEX_TYPE_2D, texSizeR.w, texSizeR.h, 0, Ogre::PF_R8G8B8, Ogre::TU_RENDERTARGET);

	//Create Render Texture
	Ogre::RenderTexture* rttEyeLeft = rtt_textureL->getBuffer(0,0)->getRenderTarget();
	Ogre::RenderTexture* rttEyeRight = rtt_textureR->getBuffer(0,0)->getRenderTarget();

	//Create and bind a viewport to the texture
	Ogre::Viewport* vptl = rttEyeLeft->addViewport(cams[left]);
	vptl->setBackgroundColour(Ogre::ColourValue(0.3f,0.3f,0.9f));
	Ogre::Viewport* vptr = rttEyeRight->addViewport(cams[right]);
	vptr->setBackgroundColour(Ogre::ColourValue(0.3f,0.3f,0.9f));

	//Store viewport pointer
	vpts[left] = vptl;
	vpts[right] = vptr;

	//Pupulate textures with an initial render
	rttEyeLeft->update();
	rttEyeRight->update();

	//Store rtt textures pointer
	rtts[left] = rttEyeLeft;
	rtts[right] = rttEyeRight;
}

void OgreOculusRender::initOculus(bool fullscreenState)
{
	setFullScreen(fullscreenState);

	//Get FOV
	EyeFov[left] = oc->getHmd()->DefaultEyeFov[left];
	EyeFov[right] = oc->getHmd()->DefaultEyeFov[right];

	//Set OpenGL configuration
	ovrGLConfig cfg;
	cfg.OGL.Header.API = ovrRenderAPI_OpenGL;
	cfg.OGL.Header.Multisample = 1;
	cfg.OGL.Header.BackBufferSize = oc->getHmd()->Resolution;
	//cfg.OGL.Header.RTSize = oc->getHmd()->Resolution;

	//OpenGL initialization differ between Windows and Linux
#ifdef _WIN32 //If windows

	//Get window
	HWND hwnd;
	window->getCustomAttribute("WINDOW",&hwnd); //potential pointer problem here
	cfg.OGL.Window = hwnd;

	//Get GL Context
	HDC dc;
	window->getCustomAttribute("HDC", &dc);
	cfg.OGL.DC = dc;

#else //Linux, even if OVR 0.4.2 is still NOT running on Linux

	//Get X window id
	size_t wID;
	window->getCustomAttribute("WINDOW", &wID);
	std::cout << "Wid : " << wID << endl;
	cfg.OGL.Win = wID;

	//Get X Display
	Display* display;
	window->getCustomAttribute("DISPLAY",&display);
	cfg.OGL.Disp = display;

#endif

	if(!ovrHmd_ConfigureRendering(
		oc->getHmd(),
		&cfg.Config,
		oc->getHmd()->DistortionCaps,
		EyeFov,
		EyeRenderDesc))
		abort();

	// Direct rendering from a window handle to the Hmd.
	// Not required if ovrHmdCap_ExtendDesktop flag is set.
#ifdef _WIN32
	HWND directHWND;
	window->getCustomAttribute("WINDOW", &directHWND);
	ovrHmd_AttachToWindow(oc->getHmd(), directHWND, NULL, NULL);
#else
	//Not currently available
#endif

	//Send texture data to OVR for rendering
	//->left eye texture :
	EyeTexture[left].OGL.Header.API = ovrRenderAPI_OpenGL;
	EyeTexture[left].OGL.Header.TextureSize = texSizeL;
	EyeTexture[left].OGL.Header.RenderViewport.Pos.x = 0;
	EyeTexture[left].OGL.Header.RenderViewport.Pos.y = 0;
	EyeTexture[left].OGL.Header.RenderViewport.Size = texSizeL;
	Ogre::GLTexture* gl_rtt_l = static_cast<Ogre::GLTexture*>(Ogre::GLTextureManager::getSingleton().getByName("RttTexL").get());
	EyeTexture[left].OGL.TexId = gl_rtt_l->getGLID();

	//right eye texture :
	EyeTexture[right] = EyeTexture[left]; //Basic configuration is shared.
	EyeTexture[right].OGL.Header.TextureSize = texSizeR;
	EyeTexture[right].OGL.Header.RenderViewport.Size = texSizeR;
	Ogre::GLTexture* gl_rtt_r = static_cast<Ogre::GLTexture*>(Ogre::GLTextureManager::getSingleton().getByName("RttTexR").get());
	EyeTexture[right].OGL.TexId = gl_rtt_r->getGLID();
}

void OgreOculusRender::RenderOneFrame()
{
	//get some info
	cameraPosition = this->CameraNode->getPosition();
	cameraOrientation = this->CameraNode->getOrientation();

	//Begin frame
	ovrFrameTiming hmdFrameTiming = ovrHmd_BeginFrame(oc->getHmd(), 0);

	//Tell ogre that Frame started
	root->_fireFrameStarted();

	for (Ogre::SceneManagerEnumerator::SceneManagerIterator it = root->getSceneManagerIterator(); it.hasMoreElements(); it.moveNext())
		it.peekNextValue()->_handleLodEvents();

	ovrPosef headPose[2];

	//Message pump events
	Ogre::WindowEventUtilities::messagePump();


	for(int eyeIndex = 0; eyeIndex < ovrEye_Count; eyeIndex++)
	{
		//Get the correct eye to render
		ovrEyeType eye = oc->getHmd()->EyeRenderOrder[eyeIndex];

		//Set the Ogre render target to the texture
		//root->getRenderSystem()->_setRenderTarget(rtts[eye]);
		vpts[eye]->clear();

		//Get the eye pose
		ovrPosef eyePose = ovrHmd_GetHmdPosePerEye(oc->getHmd(), eye);
		headPose[eye] = eyePose;

		//Get the hmd orientation
		OVR::Quatf camOrient = eyePose.Orientation;

		//Get the projection matrix
		OVR::Matrix4f proj = ovrMatrix4f_Projection(EyeRenderDesc[eye].Fov,static_cast<float>(nearClippingDistance), 10000.0f, true);

		//Convert it to Ogre matrix
		Ogre::Matrix4 OgreProj;
		for(int x(0); x < 4; x++)
			for(int y(0); y < 4; y++)
				OgreProj[x][y] = proj.M[x][y];

		//Set the matrix
		cams[eye]->setCustomProjectionMatrix(true, OgreProj);


		//Set the orientation
		cams[eye]->setOrientation(cameraOrientation * Ogre::Quaternion(camOrient.w,camOrient.x,camOrient.y,camOrient.z));

		//Set Position
		cams[eye]->setPosition
			(cameraPosition  //the "gameplay" position of player's avatar head
			+ 
			(cams[eye]->getOrientation() * - Ogre::Vector3( //realword camera orientation + the oposite of the 
			EyeRenderDesc[eye].HmdToEyeViewOffset.x, //view adjust vector. we translate the camera, not the whole world
			EyeRenderDesc[eye].HmdToEyeViewOffset.y,
			EyeRenderDesc[eye].HmdToEyeViewOffset.z)

			+ cameraOrientation * Ogre::Vector3( //cameraOrientation is in fact the direction the avatar is facing expressed as an Ogre::Quaternion
			headPose[eye].Position.x,
			headPose[eye].Position.y,
			headPose[eye].Position.z)));

		root->_fireFrameRenderingQueued();
		rtts[eye]->update();
	}

	//Ogre::Root::getSingleton().getRenderSystem()->_setRenderTarget(window);
	 
	this->updateTime = hmdFrameTiming.DeltaSeconds;
	//Do the rendering then the buffer swap

	ovrHmd_EndFrame(oc->getHmd(), headPose, (ovrTexture*)EyeTexture);
	//Tell Ogre that frame ended

	root->_fireFrameEnded();

	returnPose.position = cameraPosition + 
		Ogre::Vector3
		(headPose[0].Position.x,
		headPose[0].Position.y,
		headPose[0].Position.z);
	
	returnPose.orientation = cameraOrientation * Ogre::Quaternion
		(headPose[0].Orientation.w,
		headPose[0].Orientation.x,
		headPose[0].Orientation.y,
		headPose[0].Orientation.z);
}

void OgreOculusRender::dissmissHS()
{
	ovrHmd_DismissHSWDisplay(oc->getHmd());
	hsDissmissed = true;
}