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

	backgroundColor = Ogre::ColourValue(0.3f,0.3f,0.9f);
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
	//Ogre as to be initialized
	assert(root != NULL);
	//Try to resore the config from an ogre.cfg file
	if(!root->restoreConfig())
		//Open the config dialog of Ogre (even if we're ignoring part of the parameters you can input from it)
			if(!root->showConfigDialog())
				//If the user clicked the "cancel" button or other bad stuff happened during the configuration (like a dragon attack)
					abort();
}

void OgreOculusRender::createWindow()
{
	assert(root != NULL && oc != NULL);
	Ogre::NameValuePairList misc;

	//This one only works on windows : "Borderless = no decoration"
	misc["border"]="none"; //In case the program is not running in fullscreen mode, don't put windwo borders
	misc["vsync"]="true";
	misc["displayFrequency"]="75";
	misc["monitorIndex"]="1"; //Use the 2nd monitor, assuming the Oculus Rift is not the primary. Or is the only screen on the system.

	//Initialize a window ans specify that creation is manual
	window = root->initialise(false, name);
	//Actually create the window
	window = root->createRenderWindow(name, oc->getHmd()->Resolution.w, oc->getHmd()->Resolution.h, fullscreen,&misc);

	//Put the window at the place given by the SDK (usefull on linux system where the X server thinks multiscreen is a single big one...)
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
	texSizeL = ovrHmd_GetFovTextureSize(oc->getHmd(), ovrEye_Left, oc->getHmd()->MaxEyeFov[0], 1.0f);
	texSizeR = ovrHmd_GetFovTextureSize(oc->getHmd(), ovrEye_Right, oc->getHmd()->MaxEyeFov[1], 1.0f);

	std::cerr << "Texure size to create : " << texSizeL.w << " x " <<texSizeL.h  << " px" << std::endl;

	rift_smgr = root->createSceneManager(Ogre::ST_GENERIC);
	rift_smgr->setAmbientLight(Ogre::ColourValue(1,1,1));
	mLeftEyeRenderTexture = Ogre::TextureManager::getSingleton().createManual("RttTexL", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::TEX_TYPE_2D, texSizeL.w, texSizeL.h, 0, Ogre::PF_R8G8B8, Ogre::TU_RENDERTARGET);
	mRightEyeRenderTexture = Ogre::TextureManager::getSingleton().createManual("RttTexR", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::TEX_TYPE_2D, texSizeR.w, texSizeR.h, 0, Ogre::PF_R8G8B8, Ogre::TU_RENDERTARGET);
}

void OgreOculusRender::initOculus(bool fullscreenState)
{
	setFullScreen(fullscreenState);

	mMatLeft = Ogre::MaterialManager::getSingleton().getByName( "Oculus/LeftEye" );
	mMatLeft->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setTexture( mLeftEyeRenderTexture );
	mMatRight = Ogre::MaterialManager::getSingleton().getByName( "Oculus/RightEye" );
	mMatRight->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setTexture( mRightEyeRenderTexture );


	EyeRenderDesc[0] = ovrHmd_GetRenderDesc( oc->getHmd(), ovrEye_Left, oc->getHmd()->MaxEyeFov[0] );
	EyeRenderDesc[1] = ovrHmd_GetRenderDesc( oc->getHmd(), ovrEye_Right, oc->getHmd()->MaxEyeFov[1] );

	ovrVector2f UVScaleOffset[2];
	ovrRecti viewports[2];
	viewports[0].Pos.x = 0;
	viewports[0].Pos.y = 0;
	viewports[0].Size = texSizeL;
	viewports[1].Pos.x = 0;
	viewports[1].Pos.y = 0;
	viewports[1].Size = texSizeR;

	Ogre::SceneNode* meshNode = rift_smgr->getRootSceneNode()->createChildSceneNode();

	for (int eyeNum(0); eyeNum < 2; eyeNum++)
	{
		ovrDistortionMesh meshData;

		ovrHmd_CreateDistortionMesh(oc->getHmd(),
			EyeRenderDesc[eyeNum].Eye,
			EyeRenderDesc[eyeNum].Fov,
			0,
			&meshData);

		Ogre::GpuProgramParametersSharedPtr params;

		if(eyeNum == 0)
		{
			ovrHmd_GetRenderScaleAndOffset( EyeRenderDesc[eyeNum].Fov,
				texSizeL, viewports[eyeNum],
				UVScaleOffset);
			params = mMatLeft->getTechnique(0)->getPass(0)->getVertexProgramParameters();
		}
		else
		{
			ovrHmd_GetRenderScaleAndOffset( EyeRenderDesc[eyeNum].Fov,
				texSizeR, viewports[eyeNum],
				UVScaleOffset);
			params = mMatRight->getTechnique(0)->getPass(0)->getVertexProgramParameters();
		}

		params->setNamedConstant( "eyeToSourceUVScale",
			Ogre::Vector2( UVScaleOffset[0].x, UVScaleOffset[0].y ) );
		params->setNamedConstant( "eyeToSourceUVOffset",
			Ogre::Vector2( UVScaleOffset[1].x, UVScaleOffset[1].y ) );
		Ogre::ManualObject* manual;
		if( eyeNum == 0 )
		{
			manual = rift_smgr->createManualObject("RiftRenderObjectLeft");
			manual->begin("Oculus/LeftEye", Ogre::RenderOperation::OT_TRIANGLE_LIST);
			//manual->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_TRIANGLE_LIST);
		}
		else
		{
			manual = rift_smgr->createManualObject("RiftRenderObjectRight");
			manual->begin("Oculus/RightEye", Ogre::RenderOperation::OT_TRIANGLE_LIST);
			//manual->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_TRIANGLE_LIST);
		}

		for( unsigned int i = 0; i < meshData.VertexCount; i++ )
		{
			ovrDistortionVertex v = meshData.pVertexData[i];
			manual->position( v.ScreenPosNDC.x,
				v.ScreenPosNDC.y, 0 );
			manual->textureCoord( v.TanEyeAnglesR.x,//*UVScaleOffset[0].x + UVScaleOffset[1].x,
				v.TanEyeAnglesR.y);//*UVScaleOffset[0].y + UVScaleOffset[1].y);
			manual->textureCoord( v.TanEyeAnglesG.x,//*UVScaleOffset[0].x + UVScaleOffset[1].x,
				v.TanEyeAnglesG.y);//*UVScaleOffset[0].y + UVScaleOffset[1].y);
			manual->textureCoord( v.TanEyeAnglesB.x,//*UVScaleOffset[0].x + UVScaleOffset[1].x,
				v.TanEyeAnglesB.y);//*UVScaleOffset[0].y + UVScaleOffset[1].y);
			float vig = std::max( v.VignetteFactor, (float)0.0 );
			manual->colour( vig, vig, vig, vig );
		}
		for( unsigned int i = 0; i < meshData.IndexCount; i++ )
		{
			manual->index( meshData.pIndexData[i] );
		}


		// tell Ogre, your definition has finished
		manual->end();

		ovrHmd_DestroyDistortionMesh( &meshData );

		meshNode->attachObject( manual );
	}

	// Create a camera in the (new, external) scene so the mesh can be rendered onto it:
	rift_cam = rift_smgr->createCamera("OculusRiftExternalCamera");
	rift_cam->setFarClipDistance( 50 );
	rift_cam->setNearClipDistance( 0.001 );
	rift_cam->setProjectionType( Ogre::PT_ORTHOGRAPHIC );
	rift_cam->setOrthoWindow( 2, 2 );

	rift_smgr->getRootSceneNode()->attachObject(rift_cam);

	meshNode->setPosition( 0, 0, -1 );
	meshNode->setScale( 1, 1, -1 );

	mViewport = window->addViewport( rift_cam );
	mViewport->setBackgroundColour(Ogre::ColourValue::Black);
	mViewport->setOverlaysEnabled(true);

	IPD = ovrHmd_GetFloat(oc->getHmd(), OVR_KEY_IPD,  0.064f);

	Ogre::RenderTexture* renderTexture = mLeftEyeRenderTexture->getBuffer()->getRenderTarget();
	vpts[0] = renderTexture->addViewport(cams[0]);
	renderTexture->getViewport(0)->setClearEveryFrame(true);
	renderTexture->getViewport(0)->setBackgroundColour(backgroundColor);
	renderTexture->getViewport(0)->setOverlaysEnabled(true);

	renderTexture = mRightEyeRenderTexture->getBuffer()->getRenderTarget();
	vpts[1] = renderTexture->addViewport(cams[1]);
	renderTexture->getViewport(0)->setClearEveryFrame(true);
	renderTexture->getViewport(0)->setBackgroundColour(backgroundColor);
	renderTexture->getViewport(0)->setOverlaysEnabled(true);

	ovrFovPort fovLeft = oc->getHmd()->MaxEyeFov[ovrEye_Left];
	ovrFovPort fovRight = oc->getHmd()->MaxEyeFov[ovrEye_Right];

	float combinedTanHalfFovHorizontal = std::max( fovLeft.LeftTan, fovLeft.RightTan );
	float combinedTanHalfFovVertical = std::max( fovLeft.UpTan, fovLeft.DownTan );

	float aspectRatio = combinedTanHalfFovHorizontal / combinedTanHalfFovVertical;

	Ogre::Camera* camLeft = cams[0];
	Ogre::Camera* camRight = cams[1];

	camLeft->setAspectRatio( aspectRatio );
	camRight->setAspectRatio( aspectRatio );

	ovrMatrix4f projL = ovrMatrix4f_Projection ( fovLeft, 0.001, 50.0, true );
	ovrMatrix4f projR = ovrMatrix4f_Projection ( fovRight, 0.001, 50.0, true );

	camLeft->setCustomProjectionMatrix( true,
		Ogre::Matrix4( projL.M[0][0], projL.M[0][1], projL.M[0][2], projL.M[0][3],
		projL.M[1][0], projL.M[1][1], projL.M[1][2], projL.M[1][3],
		projL.M[2][0], projL.M[2][1], projL.M[2][2], projL.M[2][3],
		projL.M[3][0], projL.M[3][1], projL.M[3][2], projL.M[3][3] ) );
	camRight->setCustomProjectionMatrix( true,
		Ogre::Matrix4( projR.M[0][0], projR.M[0][1], projR.M[0][2], projR.M[0][3],
		projR.M[1][0], projR.M[1][1], projR.M[1][2], projR.M[1][3],
		projR.M[2][0], projR.M[2][1], projR.M[2][2], projR.M[2][3],
		projR.M[3][0], projR.M[3][1], projR.M[3][2], projR.M[3][3] ) );

		
		//The average  human has 2 eyes
		for(int eyeIndex(0); eyeIndex < 2; eyeIndex++)
		{
		ovrEyeType eye = oc->getHmd()->EyeRenderOrder[eyeIndex];

		//Get the projection matrix
		OVR::Matrix4f proj = ovrMatrix4f_Projection(EyeRenderDesc[eye].Fov,static_cast<float>(nearClippingDistance), 10000.0f, true);

		//Convert it to Ogre matrix
		Ogre::Matrix4 OgreProj;
		for(int x(0); x < 4; x++)
			for(int y(0); y < 4; y++)
				OgreProj[x][y] = proj.M[x][y];

		//Set the matrix
		cams[eye]->setCustomProjectionMatrix(true, OgreProj);
		}
}

void OgreOculusRender::RenderOneFrame()
{
	//Ogre::WindowEventUtilities::messagePump();
	unsigned long timerStart = Ogre::Root::getSingleton().getTimer()->getMilliseconds();
	//get some info
	cameraPosition = this->CameraNode->getPosition();
	cameraOrientation = this->CameraNode->getOrientation();

	//Begin frame
	ovrFrameTiming hmdFrameTiming = ovrHmd_BeginFrame(oc->getHmd(), 0);
	ovrTrackingState ts = ovrHmd_GetTrackingState(oc->getHmd(), hmdFrameTiming.ScanoutMidpointSeconds);
	Posef pose = ts.HeadPose.ThePose;
	//Get the hmd orientation
	OVR::Quatf camOrient = pose.Rotation;
	OVR::Vector3f camPos = pose.Translation;
	ovrEyeType eye;
	for(int eyeIndex = 0; eyeIndex < ovrEye_Count; eyeIndex++)
	{
		eye = oc->getHmd()->EyeRenderOrder[eyeIndex];
		cams[eye]->setOrientation(cameraOrientation * Ogre::Quaternion(camOrient.w,camOrient.x,camOrient.y,camOrient.z));

		cams[eye]->setPosition
			(cameraPosition  //the "gameplay" position of player's avatar head
			+ 
			(cams[eye]->getOrientation() * - Ogre::Vector3( //realword camera orientation + the oposite of the 
			EyeRenderDesc[eye].HmdToEyeViewOffset.x, //view adjust vector. we translate the camera, not the whole world
			EyeRenderDesc[eye].HmdToEyeViewOffset.y, //The translations has to occur in function of the current head orientation.
			EyeRenderDesc[eye].HmdToEyeViewOffset.z) //That's why just multiply by the quaternion we just calculated. 

			+ cameraOrientation * Ogre::Vector3( //cameraOrientation is in fact the direction the avatar is facing expressed as an Ogre::Quaternion
			camPos.x,
			camPos.y,
			camPos.z)));
	}
	
	this->updateTime = hmdFrameTiming.DeltaSeconds;
	if(updateTime == 0)
	{
		unsigned long timerStop = Ogre::Root::getSingleton().getTimer()->getMilliseconds();
		updateTime = (timerStart - timerStop) / 1000.0f;
	}
	//update the pose for gameplay purposes
	returnPose.position = cameraPosition + cameraOrientation * Ogre::Vector3(camPos.x, camPos.y, camPos.z);
	returnPose.orientation = cameraOrientation * Ogre::Quaternion(camOrient.w, camOrient.x, camOrient.y, camOrient.z);
	
	ovrHmd_EndFrameTiming(oc->getHmd());
	root->renderOneFrame();

}

void OgreOculusRender::dissmissHS()
{
	ovrHmd_DismissHSWDisplay(oc->getHmd());
	hsDissmissed = true;
}

void OgreOculusRender::setFullScreen(bool fs)
{
	fullscreen = fs;
}

bool OgreOculusRender::isFullscreen()
{
	return fullscreen;
}

Ogre::SceneManager* OgreOculusRender::getSceneManager()
{
	return smgr;
}

Ogre::RenderWindow* OgreOculusRender::getWindow()
{
	return window;
}

void OgreOculusRender::debugPrint()
{
	for(int i(0); i < 2; i++)
	{
		cout << "cam " << i << " " << cams[i]->getPosition() << endl;
		cout << cams[i]->getOrientation() << endl;
	}
}

void OgreOculusRender::debugSaveToFile(const char path[])
{
	if(rtts[0]) rtts[0]->writeContentsToFile(path);
}

Ogre::SceneNode* OgreOculusRender::getCameraInformationNode()
{
	return CameraNode;
}

Ogre::Timer* OgreOculusRender::getTimer()
{
	if(root)
		return root->getTimer();
	return NULL;
}

float OgreOculusRender::getUpdateTime()
{
	return updateTime;
}

void OgreOculusRender::recenter()
{
	ovrHmd_RecenterPose(oc->getHmd());
}

bool OgreOculusRender::IsHsDissmissed()
{
	return hsDissmissed;
}