#include "stdafx.h"
#include "OgreOculusRender.hpp"
#include <glew.h>
#include <OVR_CAPI_0_6_0.h>
#include <OVR_CAPI_GL.h>

//We need to get low level access to some GL textures 
#include <RenderSystems/GL/OgreGLRenderSystem.h>
#include <RenderSystems/GL/OgreGLTextureManager.h>

using namespace OVR;

bool OgreOculusRender::forceNextUpdate(false);
OgreOculusRender::OgreOculusRender(std::string winName, bool activateVsync)
{
	oorc = NULL;
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
		debugVP[i] = NULL;
	}

	//Oc is an OculusInterface object. Communication with the Rift SDK is handeled by that class
	oc = NULL;
	CameraNode = NULL;
	cameraPosition = Ogre::Vector3(0,0,10);
	cameraOrientation = Ogre::Quaternion::IDENTITY;
	nearClippingDistance = (float) 0.05;
	lastOculusPosition = cameraPosition;
	lastOculusOrientation = cameraOrientation;
	updateTime = 0;
	fullscreen = true;
	vsync = activateVsync;
	hsDissmissed = false;
	backgroundColor = Ogre::ColourValue(0.f,0.56f,1.f);
	debug = NULL;
}

OgreOculusRender::~OgreOculusRender()
{
	Ogre::LogManager::getSingleton().logMessage("Destructing OgreOculus object and uninitializing Ogre...");
	delete oc;

	//TODO clean Ogre properly. There is stuff to delete manually before being able to delete "root".
	//delete root;
}

void OgreOculusRender::changeViewportBackgroundColor(Ogre::ColourValue color)
{
	//save the color;
	backgroundColor = color;
	for(size_t i(0); i < 2; i++)
	{
		if(vpts[i])
			vpts[i]->setBackgroundColour(color);
		if(debugVP[i])
			debugVP[i]->setBackgroundColour(color); 
	}
}

void OgreOculusRender::dissmissHS()
{
	//ovrHmd_DismissHSWDisplay(oc->getHmd());
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
	for(size_t i(0); i < 2; i++)
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

double OgreOculusRender::getUpdateTime()
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
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(archName, typeName, secName);
		}
	}
}

//See commant of the loadResourceFile method
void OgreOculusRender::initAllResources()
{
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}

void OgreOculusRender::initLibraries(std::string loggerName)
{
	//Create the ogre root with standards Ogre configuration file
	root = new Ogre::Root("plugins.cfg","ogre.cfg",loggerName.c_str());

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
	misc["border"]				=	"none"; //In case the program is not running in fullscreen mode, don't put window borders
	if (vsync) misc["vsync"]	=	"true";
	misc["displayFrequency"]	=	"75";
	misc["monitorIndex"]		=	"1"; //Use the 2nd monitor, assuming the Oculus Rift is not the primary. Or is the only screen on the system.

	//Initialize a window ans specify that creation is manual
	root->initialise(false);
	//debug = root->createRenderWindow("Debug out", oc->getHmd()->Resolution.w/2, oc->getHmd()->Resolution.h/2, false);
	//Actually create the window
#ifdef __gnu_linux__
	//Assuming the 2nd screen is used as a "normal" display, rotated. Ogre fullscreen on Linux does stranges things, so I don't permit it at all
	fullscreen = false;
#endif
	window = root->createRenderWindow(name, oc->getHmd()->Resolution.w, oc->getHmd()->Resolution.h, fullscreen, &misc);

	//Put the window at the place given by the SDK (usefull on linux system where the X server thinks multiscreen is a single big one...)
	//window->reposition(oc->getHmd()->WindowsPos.x, oc->getHmd()->WindowsPos.y);
}

void OgreOculusRender::initCameras()
{
	assert(smgr != NULL);
	cams[left] = smgr->createCamera("lcam");
	cams[right] = smgr->createCamera("rcam");
	for(size_t i = 0; i < 2; i++)
	{
		cams[i]->setPosition(cameraPosition);
		cams[i]->setAutoAspectRatio(true);
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
	smgr = root->createSceneManager("OctreeSceneManager", "OSM_SMGR");
	smgr->setShadowTechnique(Ogre::ShadowTechnique::SHADOWTYPE_STENCIL_ADDITIVE);
}

void OgreOculusRender::initRttRendering()
{
	//get texture sice from ovr with default FOV
	texSizeL = ovrHmd_GetFovTextureSize(oc->getHmd(), ovrEye_Left, oc->getHmd()->MaxEyeFov[0], 1.0f);
	texSizeR = ovrHmd_GetFovTextureSize(oc->getHmd(), ovrEye_Right, oc->getHmd()->MaxEyeFov[1], 1.0f);
	Sizei bufferSize;
	bufferSize.w = texSizeL.w + texSizeR.w;
	bufferSize.h = max ( texSizeL.h, texSizeR.h );
	std::cerr << "Texure size to create : " << bufferSize.w << " x " <<bufferSize.h  << " px" << std::endl;
	/*
	rift_smgr = root->createSceneManager(Ogre::ST_GENERIC);
	rift_smgr->setAmbientLight(Ogre::ColourValue(1,1,1));

	mLeftEyeRenderTexture = Ogre::TextureManager::getSingleton().createManual("RttTexL", 
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
		Ogre::TEX_TYPE_2D, 
		texSizeL.w, texSizeL.h, 0, 
		Ogre::PF_R8G8B8, Ogre::TU_RENDERTARGET);
	mRightEyeRenderTexture = Ogre::TextureManager::getSingleton().createManual("RttTexR", 
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
		Ogre::TEX_TYPE_2D, 
		texSizeR.w, texSizeR.h, 0, 
		Ogre::PF_R8G8B8, Ogre::TU_RENDERTARGET);
		*/	

	ovrSwapTextureSet* textureSet(NULL);

	if (ovrHmd_CreateSwapTextureSetGL(oc->getHmd(), GL_RGBA, bufferSize.w, bufferSize.h, &textureSet) != ovrSuccess)
	{
		Ogre::LogManager::getSingleton().logMessage("Cannot create Oculus swap texture");
		abort();
	}

	//get the texture manager pointer 

	//Get eye textures
	ovrGLTexture* tex0  = (ovrGLTexture*) &textureSet->Textures[0];
	ovrGLTexture* tex1  = (ovrGLTexture*) &textureSet->Textures[1];

	GLuint tex0id = tex0->OGL.TexId;
	GLuint tex1id = tex1->OGL.TexId;

	//Ok... I have textures that are outsite of Ogre TextureManager.
	//I need to open a rendering viewport attached to each camera on theses textures
	//Doing some google seach tends to go in the "you need to patch ogre for that" way. 


	std::cerr << "Texture GLID " << tex0id << " " << tex1id << std::endl;
	Ogre::GLTextureManager* textureManager(static_cast<Ogre::GLTextureManager*>(Ogre::GLTextureManager::getSingletonPtr()));

}

void OgreOculusRender::initOculus(bool fullscreenState)
{
	setFullScreen(fullscreenState);

	EyeRenderDesc[0] = ovrHmd_GetRenderDesc(oc->getHmd(), ovrEye_Left, oc->getHmd()->MaxEyeFov[0]);
	EyeRenderDesc[1] = ovrHmd_GetRenderDesc(oc->getHmd(), ovrEye_Right, oc->getHmd()->MaxEyeFov[1]);

	calculateProjectionMatrix();
}

void OgreOculusRender::calculateProjectionMatrix()
{
	//The average  human has 2 eyes
	for(size_t eyeIndex(0); eyeIndex < ovrEye_Count; eyeIndex++)
	{
		//Get the projection matrix
		OVR::Matrix4f proj = ovrMatrix4f_Projection(EyeRenderDesc[eyeIndex].Fov, 
			static_cast<float>(nearClippingDistance), 
			8000.0f, 
			true);

		//Convert it to Ogre matrix
		Ogre::Matrix4 OgreProj;
		for(size_t x(0); x < 4; x++)
			for(size_t y(0); y < 4; y++)
				OgreProj[x][y] = proj.M[x][y];

		//Set the matrix
		cams[eyeIndex]->setCustomProjectionMatrix(true, OgreProj);
	}
}

void OgreOculusRender::RenderOneFrame()
{
	Ogre::WindowEventUtilities::messagePump();

	//get some info
	cameraPosition = CameraNode->getPosition();
	cameraOrientation = CameraNode->getOrientation();

	unsigned long timerStart = getTimer()->getMilliseconds();

	//Begin frame
	//ovrFrameTiming hmdFrameTiming = ovrHmd_BeginFrame(oc->getHmd(), 0);
	ovrTrackingState ts; //= ovrHmd_GetTrackingState(oc->getHmd(), hmdFrameTiming.ScanoutMidpointSeconds);
	Posef pose = ts.HeadPose.ThePose;

	//Get the hmd orientation
	Quatf oculusOrient = pose.Rotation;
	Vector3f oculusPos = pose.Translation;

	ovrEyeType eye;
	for(size_t eyeIndex = 0; eyeIndex < ovrEye_Count; eyeIndex++)
	{
		//Calculate and apply the orientation of the rift to the player (world space)
		eye = oc->getHmd()->EyeRenderOrder[eyeIndex];
		cams[eye]->setOrientation(cameraOrientation * Ogre::Quaternion(oculusOrient.w, oculusOrient.x, oculusOrient.y, oculusOrient.z));

		cams[eye]->setPosition
			(cameraPosition  //the "gameplay" position of player's avatar head

			+ (cams[eye]->getOrientation() * Ogre::Vector3( //realword camera orientation + the  
			EyeRenderDesc[eye].HmdToEyeViewOffset.x, //view adjust vector.
			EyeRenderDesc[eye].HmdToEyeViewOffset.y, //The translations has to occur in function of the current head orientation.
			EyeRenderDesc[eye].HmdToEyeViewOffset.z) //That's why just multiply by the quaternion we just calculated. 

			+ cameraOrientation * Ogre::Vector3( //cameraOrientation is in fact the direction the avatar is facing expressed as an Ogre::Quaternion
			oculusPos.x,
			oculusPos.y,
			oculusPos.z)));
	}

	//update the pose for gameplay purposes
	returnPose.position = cameraPosition + cameraOrientation * Ogre::Vector3(oculusPos.x, oculusPos.y, oculusPos.z);
	returnPose.orientation = cameraOrientation * Ogre::Quaternion(oculusOrient.w, oculusOrient.x, oculusOrient.y, oculusOrient.z);

	if(oorc)oorc->renderCallback();


	if(forceNextUpdate)//quick&dirty fix for a texture buffer update problem : render a second time a buffer. Need to find better.
	{
		//vpts[0]->clear();
		//vpts[1]->clear();
		vpts[0]->update();
		//vpts[1]->update();
		forceNextUpdate = false;
	}
	//root->renderOneFrame();

	//Timewarp is not implemented yet... need to recode sharders or to 
	//ovr_WaitTillTime(hmdFrameTiming.TimewarpPointSeconds);

	unsigned long timerStop = getTimer()->getMilliseconds();
	updateTime = double(timerStop - timerStart) / 1000.0;
	//ovrHmd_EndFrameTiming(oc->getHmd());

	//updateTime = hmdFrameTiming.DeltaSeconds;
}

void OgreOculusRender::openDebugWindow()
{
	if(debug) 
	{

		/*if(!debug->isVisible());
		{
		debug->setHidden(true);
		debug->setHidden(false);
		window->setHidden(true);
		window->setHidden(false);
		}*/
		return;
	}
	debug = root->createRenderWindow("Annwvyn debug window " + name,  
		oc->getHmd()->Resolution.w/2, oc->getHmd()->Resolution.h/2, //Half of Oculus resolution by default
		false); //no fullscreen

	if(!debug) return; //We need that window to exist now
	//Attach camera
	debugVP[left] = debug->addViewport(cams[left], 0, 0, 0, 0.5f);
	debugVP[right] = debug->addViewport(cams[right], 1, 0.5f, 0, 0.5f);
	//update viewports background
	changeViewportBackgroundColor(backgroundColor);

	//Bring back the focus on the real main window (hack)
	window->setHidden(true);
	window->setHidden(false);
}