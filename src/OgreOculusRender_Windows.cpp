#include "stdafx.h"
#include "OgreOculusRender.hpp"
#include <OVR_CAPI_GL.h>
#include <OVR_CAPI_0_6_0.h>
//We need to get low level access to some GL textures 
#include <RenderSystems/GL/OgreGLTextureManager.h>
#include <RenderSystems/GL/OgreGLRenderSystem.h>
#include <RenderSystems/GL/OgreGLTexture.h>

using namespace OVR;

bool OgreOculusRender::forceNextUpdate(false);
Ogre::TextureUnitState* OgreOculusRender::debugTexturePlane(NULL);
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

	debugSmgr = NULL;
	debugCam = NULL;
	//Oc is an OculusInterface object. Communication with the Rift SDK is handeled by that class
	oc = NULL;
	CameraNode = NULL;
	cameraPosition = Ogre::Vector3(0, 0, 10);
	cameraOrientation = Ogre::Quaternion::IDENTITY;
	nearClippingDistance = 0.05f;
	farClippingDistance = 8000.0f;
	lastOculusPosition = cameraPosition;
	lastOculusOrientation = cameraOrientation;
	updateTime = 0;
	fullscreen = false;
	vsync = activateVsync;
	hsDissmissed = false;
	backgroundColor = Ogre::ColourValue(0.f, 0.56f, 1.f);
	debug = NULL;
	textureSet = NULL;
	perfHudMode = ovrPerfHud_Off;
}

OgreOculusRender::~OgreOculusRender()
{
	Ogre::LogManager::getSingleton().logMessage("Destructing OgreOculus object and uninitializing Ogre...");
	delete oc;

	//TODO clean Ogre properly. There is stuff to delete manually before being able to delete "root".
	window->removeAllViewports();
	root->getRenderSystem()->destroyRenderWindow(window->getName());
	//root->getRenderSystem()->detachRenderTarget("RttTex");
	Ogre::TextureManager::getSingleton().getByName("RttTex")->getBuffer(0,0)->getRenderTarget()->removeAllViewports();
	root->getRenderSystem()->destroyRenderTexture("RttTex");
	debugSmgr->destroyAllCameras();
	debugSmgr->destroyAllManualObjects(); 
	debugSmgr->clearScene();
	root->destroySceneManager(debugSmgr);
	smgr->clearScene();
	root->destroySceneManager(smgr);
	Ogre::MaterialManager::getSingleton().remove("DebugPlaneMaterial");
	delete root;
	std::cerr << "Ogre root deleted." << std::endl;

//this is a "stop stupid crash hack" here:
#pragma warning(disable : 4722) 
	exit(0);
}

void OgreOculusRender::cycleOculusHUD()
{
	perfHudMode = (perfHudMode+1) % (ovrPerfHud_Count+1 /*the +1 is because somebody at Oculus don't understand how enum works in C/C++*/);
	ovrHmd_SetInt(oc->getHmd(), "PerfHudMode", perfHudMode);
}

void OgreOculusRender::changeViewportBackgroundColor(Ogre::ColourValue color)
{
	//save the color
	backgroundColor = color;
	for(size_t i(0); i < 2; i++)
	{
		if(vpts[i])
			vpts[i]->setBackgroundColour(color);
	}
}

void OgreOculusRender::dissmissHS()
{
	//ovrHmd_DismissHSWDisplay(oc->getHmd());
	hsDissmissed = true;
}

void OgreOculusRender::setFullScreen(bool fs)
{
	return;//deprecated
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
	if(Ogre::TextureManager::getSingleton().getByName("RttTex").getPointer()) 
		Ogre::TextureManager::getSingleton().getByName("RttTex").getPointer()->getBuffer(0, 0)->getRenderTarget()->writeContentsToFile(path);
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
	root = new Ogre::Root("plugins.cfg", "ogre.cfg", loggerName.c_str());

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
	//misc["border"]				=	"none"; //In case the program is not running in fullscreen mode, don't put window borders
	/*if (vsync) misc["vsync"]	=	"true";
	//This is for DK2
	misc["displayFrequency"]	=	"75";
	misc["monitorIndex"]		=	"0";
	*/
	//Initialize a window ans specify that creation is manual
	root->initialise(false);
	//debug = root->createRenderWindow("Debug out", oc->getHmd()->Resolution.w/2, oc->getHmd()->Resolution.h/2, false);
	//Actually create the window
	
	window = root->createRenderWindow("debug rift output " + name, oc->getHmd()->Resolution.w/2, oc->getHmd()->Resolution.h/2, false, &misc);
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
	//Create the scene manager for the engine
	assert(root != NULL);
	smgr = root->createSceneManager("OctreeSceneManager", "OSM_SMGR");
	smgr->setShadowTechnique(Ogre::ShadowTechnique::SHADOWTYPE_STENCIL_ADDITIVE);

	//Create the scene manager for the debug output
	debugSmgr = root->createSceneManager(Ogre::ST_GENERIC);
	debugSmgr->setAmbientLight(Ogre::ColourValue::White); //no shadow

	//Create the camera
	debugCam = debugSmgr->createCamera("DebugRender");
	debugCam->setAutoAspectRatio(true);
	debugCam->setNearClipDistance(0.001f);
	debugCam->setProjectionType(Ogre::PT_ORTHOGRAPHIC);
	debugCam->setOrthoWindow(16,9);
	debugCamNode = debugSmgr->getRootSceneNode()->createChildSceneNode();
	debugCamNode->attachObject(debugCam);

	//---Create the debug plane
	//Base setup inside the scene manager
	debugPlaneNode = debugCamNode->createChildSceneNode();
	debugPlaneNode->setPosition(0,0,-1);
	Ogre::ManualObject* debugPlane = debugSmgr->createManualObject("DebugPlane");
	float x = 16.0f/2, y = 9.0f/2;
	//Material
	DebugPlaneMaterial = Ogre::MaterialManager::getSingleton().create("DebugPlaneMaterial", "General", true);
	debugTexturePlane = DebugPlaneMaterial.getPointer()->getTechnique(0)->getPass(0)->createTextureUnitState();
	//The manual object itself
	debugPlane->begin("DebugPlaneMaterial",Ogre::RenderOperation::OT_TRIANGLE_STRIP);
	//4 verticies with texture coodinates
	debugPlane->position(-x, y, 0);
	debugPlane->textureCoord(0, 0);
	debugPlane->position(-x, -y,0);
	debugPlane->textureCoord(0, 1);
	debugPlane->position(x, y,  0);
	debugPlane->textureCoord(1, 0);
	debugPlane->position(x, -y, 0);
	debugPlane->textureCoord(1, 1);
	debugPlane->end();

	//Add it to the scene
	debugPlaneNode->attachObject(debugPlane);
	debugPlaneNode->setVisible(true);
}

void OgreOculusRender::initRttRendering()
{
	//Init GLEW here to be able to call OpenGL functions
	GLenum err = glewInit();
	if(err == GLEW_OK)
	{
		std::stringstream out;
		out << "Using GLEW version : " << glewGetString(GLEW_VERSION) << std::endl;
		Ogre::LogManager::getSingleton().logMessage(out.str());
	}
	else
	{
		Ogre::LogManager::getSingleton().logMessage("Failed to glewTnit()\nCannot call manual OpenGL\nError Code : " + (unsigned int)err);
		abort();
	}

	//Get texture size from ovr with the maximal FOV for each eye
	texSizeL = ovrHmd_GetFovTextureSize(oc->getHmd(), ovrEye_Left, oc->getHmd()->MaxEyeFov[left], 1.0f);
	texSizeR = ovrHmd_GetFovTextureSize(oc->getHmd(), ovrEye_Right, oc->getHmd()->MaxEyeFov[right], 1.0f);
	//Calculate the render buffer size for both eyes
	bufferSize.w = texSizeL.w + texSizeR.w;
	bufferSize.h = max(texSizeL.h, texSizeR.h);
	std::cerr << "Texure size to create : " << bufferSize.w << " x " <<bufferSize.h  << " px" << std::endl;

	//Request the creation of an OpenGL swap texture set from the Oculus Library
	if (ovrHmd_CreateSwapTextureSetGL(oc->getHmd(), GL_RGB, bufferSize.w, bufferSize.h, &textureSet) != ovrSuccess)
	{
		//If we can't get the textures, there is no point trying more.
		Ogre::LogManager::getSingleton().logMessage("Cannot create Oculus swap texture");
		abort();
	}
	
	//Create the Ogre equivalent of the texture as a render target for Ogre
	Ogre::GLTextureManager* textureManager(static_cast<Ogre::GLTextureManager*>(Ogre::GLTextureManager::getSingletonPtr()));
	Ogre::TexturePtr rtt_texture(textureManager->createManual("RttTex", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
		Ogre::TEX_TYPE_2D, bufferSize.w, bufferSize.h, 0, Ogre::PF_R8G8B8, Ogre::TU_RENDERTARGET));
	//Save the texture id for low-level GL call on the texture during render
	Ogre::RenderTexture* rttEyes = rtt_texture->getBuffer(0, 0)->getRenderTarget();
	Ogre::GLTexture* gltex = static_cast<Ogre::GLTexture*>(Ogre::GLTextureManager::getSingleton().getByName("RttTex").getPointer());
	renderTextureID = gltex->getGLID();

	vpts[left] = rttEyes->addViewport(cams[left], 0, 0, 0, 0.5f);
	vpts[right] = rttEyes->addViewport(cams[right], 1, 0.5f, 0, 0.5f);

	changeViewportBackgroundColor(backgroundColor);

	if (ovrHmd_CreateMirrorTextureGL(oc->getHmd(), GL_RGB, oc->getHmd()->Resolution.w, oc->getHmd()->Resolution.h, &mirrorTexture) != ovrSuccess)
	{
		//If for some weird reason (stars alignment, dragons, northen gods) we can't create the mirror texture
		Ogre::LogManager::getSingleton().logMessage("Cannot create Oculus mirror texture");
		abort();
	}
	//Create the Ogre equivalent of this buffer
	Ogre::TexturePtr mirror_texture(textureManager->createManual("MirrorTex", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
		Ogre::TEX_TYPE_2D, oc->getHmd()->Resolution.w, oc->getHmd()->Resolution.h, 0, Ogre::PF_R8G8B8, Ogre::TU_RENDERTARGET));

	//Save the GL texture id for updating the mirror texture
	ogreMirrorTextureID = static_cast<Ogre::GLTexture*>(Ogre::GLTextureManager::getSingleton().getByName("MirrorTex").getPointer())->getGLID();
	oculusMirrorTextureID = ((ovrGLTexture*)mirrorTexture)->OGL.TexId;
	
	//Attach the camera of the debug render scene to a viewport on the actuall application window
	debugViewport = window->addViewport(debugCam);
	debugViewport->setBackgroundColour(Ogre::ColourValue::Black);
	debugTexturePlane->setTextureName("MirrorTex");
	debugTexturePlane->setTextureFiltering(Ogre::FO_POINT, Ogre::FO_POINT, Ogre::FO_NONE);
	debugViewport->setAutoUpdated(false);
}

void OgreOculusRender::showRawView()
{
	if(debugTexturePlane)
	debugTexturePlane->setTextureName("RttTex");
}

void OgreOculusRender::showMirrorView()
{
	if(debugTexturePlane)
	debugTexturePlane->setTextureName("MirrorTex");
}

void OgreOculusRender::initOculus(bool fullscreenState)
{
	//Populate OVR structures
	EyeRenderDesc[left] = ovrHmd_GetRenderDesc(oc->getHmd(), ovrEye_Left, oc->getHmd()->MaxEyeFov[left]);
	EyeRenderDesc[right] = ovrHmd_GetRenderDesc(oc->getHmd(), ovrEye_Right, oc->getHmd()->MaxEyeFov[right]);
	offset[left]=EyeRenderDesc[left].HmdToEyeViewOffset;
	offset[right]=EyeRenderDesc[right].HmdToEyeViewOffset;

	//Create a layer with our single swaptexture on it. Each side is an eye.
	layer.Header.Type = ovrLayerType_EyeFov;
	layer.Header.Flags = left;
	layer.ColorTexture[left] = textureSet;
	layer.ColorTexture[right] = textureSet;
	layer.Fov[left] = EyeRenderDesc[left].Fov;
	layer.Fov[right] = EyeRenderDesc[right].Fov;
	layer.Viewport[left] = Recti(0, 0, bufferSize.w/2, bufferSize.h);
	layer.Viewport[right] = Recti(bufferSize.w/2, 0, bufferSize.w/2, bufferSize.h);

	//This need to be called at least once
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
			farClippingDistance, 
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
	textureSet->CurrentIndex = (textureSet->CurrentIndex + 1) % textureSet->TextureCount;
	//get some info
	cameraPosition = CameraNode->getPosition();
	cameraOrientation = CameraNode->getOrientation();

	//Begin frame
	hmdFrameTiming = ovrHmd_GetFrameTiming(oc->getHmd(), 0);
	ts = ovrHmd_GetTrackingState(oc->getHmd(), hmdFrameTiming.DisplayMidpointSeconds);
	pose = ts.HeadPose.ThePose;
	ovr_CalcEyePoses(pose, offset, layer.RenderPose); 
	
	//Get the hmd orientation
	oculusOrient = pose.Rotation;
	oculusPos = pose.Translation;

	//Apply pose to cameras
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
	
	if(oorc) oorc->renderCallback();
	if(forceNextUpdate) //quick&dirty fix for a texture buffer update problem : render a second time a buffer. Need to find better.
	{
		vpts[left]->update();
		forceNextUpdate = false;
	}

	updateTime = hmdFrameTiming.FrameIntervalSeconds;
	root->renderOneFrame();
	
	//Copy the rendered image to the Oculus Swap Texture
	glCopyImageSubData(renderTextureID, GL_TEXTURE_2D, 0, 0, 0, 0, 
		((ovrGLTexture*)(&textureSet->Textures[textureSet->CurrentIndex]))->OGL.TexId, GL_TEXTURE_2D, 0, 0, 0, 0, 
		bufferSize.w,bufferSize.h, 1);

	layers = &layer.Header;
	ovrHmd_SubmitFrame(oc->getHmd(), 0, nullptr, &layers, 1);

	//Put the mirrored view available for OGRE
	glCopyImageSubData(oculusMirrorTextureID, GL_TEXTURE_2D, 0, 0, 0, 0, 
		ogreMirrorTextureID, GL_TEXTURE_2D, 0, 0, 0, 0, 
		oc->getHmd()->Resolution.w, oc->getHmd()->Resolution.h, 1);
	if(window->isVisible())
		debugViewport->update();
}


void OgreOculusRender::openDebugWindow()
{
	///Deprecated

	return;
}