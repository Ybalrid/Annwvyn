#include "stdafx.h"
#include "OgreOculusRender.hpp"
#include <OVR_CAPI_GL.h>
//#include <OVR_CAPI_0_8_0.h>
//We need to get low level access to some GL textures 
#include <RenderSystems/GL/OgreGLTextureManager.h>
#include <RenderSystems/GL/OgreGLRenderSystem.h>
#include <RenderSystems/GL/OgreGLTexture.h>
#include "AnnLogger.hpp"

//using namespace OVR;

bool OgreOculusRender::mirror(true);
bool OgreOculusRender::forceNextUpdate(false);
OgreOculusRender* OgreOculusRender::self(nullptr);
Ogre::TextureUnitState* OgreOculusRender::debugTexturePlane(nullptr);
OgreOculusRender::OgreOculusRender(std::string winName, bool activateVsync) :
	name(winName),
	root(nullptr),
	smgr(nullptr),
	debugSmgr(nullptr),
	Oculus(nullptr),
	cameraPosition(0,0,10),
	cameraOrientation(Ogre::Quaternion::IDENTITY),
	nearClippingDistance(0.5f),
	farClippingDistance(4000.0f),
	lastOculusPosition(cameraPosition),
	lastOculusOrientation(cameraOrientation),
	updateTime(0),
	backgroundColor(0,0.56f,1),
	textureSwapChain(0),
	perfHudMode(ovrPerfHud_Off)
{
	if(self)
	{
		MessageBox(NULL, L"Fatal error with renderer initialisation. OgreOculusRender object allready created.", L"Fatal Error", MB_ICONERROR);
		exit(ANN_ERR_RENDER);
	}
	self=this;
		cams[left] = nullptr;
		rtts[left] = nullptr;
		vpts[left] = nullptr;		
		cams[right] = nullptr;
		rtts[right] = nullptr;
		vpts[right] = nullptr;
		monoCam = nullptr;
}

OgreOculusRender::~OgreOculusRender()
{
	Annwvyn::AnnDebug() << "Destructing OgreOculus object and uninitializing Ogre...";
	delete Oculus;
	Oculus = nullptr;
	root->destroySceneManager(debugSmgr);
	root->destroySceneManager(smgr);

	//Apparently manually removing the manually created texture unit state prevent ogre from crashing during cleanup... 
	DebugPlaneMaterial.getPointer()->getTechnique(0)->getPass(0)->removeAllTextureUnitStates();

	root->unloadPlugin("Plugin_OctreeSceneManager");
	delete root;
	root = nullptr;
}

void OgreOculusRender::cycleOculusHUD()
{
	perfHudMode = (perfHudMode+1) % (ovrPerfHud_Count);
	ovr_SetInt(Oculus->getSession(), "PerfHudMode", perfHudMode);
}

void OgreOculusRender::changeViewportBackgroundColor(Ogre::ColourValue color)
{
	//save the color then apply it to each viewport
	backgroundColor = color;
	for(size_t i(0); i < 2; i++)
		if(vpts[i])
			vpts[i]->setBackgroundColour(color);
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
		Annwvyn::AnnDebug() << "cam " << i << " " << cams[i]->getPosition();
		Annwvyn::AnnDebug() << cams[i]->getOrientation();
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
	return nullptr;
}

double OgreOculusRender::getUpdateTime()
{
	return updateTime;
}

void OgreOculusRender::recenter()
{
	ovr_RecenterTrackingOrigin(Oculus->getSession());
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
	root = new Ogre::Root("", "ogre.cfg", loggerName.c_str());
	Ogre::LogManager::getSingleton().setLogDetail(Ogre::LoggingLevel::LL_BOREME);
	//Class to get basic information from the Rift. Initialize the RiftSDK
	Oculus = new OculusInterface();
}

void OgreOculusRender::initialize()
{
	//init libraries;
	initLibraries();
	//Mandatory. If thouse pointers are unitilalized, program have to stop here.
	assert(root != NULL && Oculus != NULL);
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
	root->loadPlugin("RenderSystem_GL");
	root->loadPlugin("Plugin_OctreeSceneManager");
	root->setRenderSystem(root->getRenderSystemByName("OpenGL Rendering Subsystem"));
	root->getRenderSystem()->setFixedPipelineEnabled(true);
}

void OgreOculusRender::createWindow()
{
	assert(root != NULL && oc != NULL);
	Ogre::NameValuePairList misc;
	misc["vsync"] = "false";

	root->initialise(false);

	float w(Oculus->getHmdDesc().Resolution.w), h(Oculus->getHmdDesc().Resolution.h);
	if(w >= 1920) w /=2;
	if(h >= 1080) h /=2;

	window = root->createRenderWindow(name + ": Mirror output (Please put your headset)", w, h, false, &misc);
}

void OgreOculusRender::initCameras()
{
	assert(smgr != NULL); 
	monoCam = 
	smgr->createCamera("monocam");
	monoCam->setAspectRatio(16.0/9.0);
	monoCam->setAutoAspectRatio(false);
	monoCam->setPosition(cameraPosition);
	monoCam->setNearClipDistance(0.1);
	monoCam->setFarClipDistance(4000);
	monoCam->setFOVy(Ogre::Degree(90));
	

	cams[left] = smgr->createCamera("lcam");
	cams[left]->setPosition(cameraPosition);
	cams[left]->setAutoAspectRatio(true);
	cams[right] = smgr->createCamera("rcam");
	cams[right]->setPosition(cameraPosition);
	cams[right]->setAutoAspectRatio(true);


	//do NOT attach camera to this node...
	CameraNode = smgr->getRootSceneNode()->createChildSceneNode();
}

void OgreOculusRender::setMonoFov(float degreeFov)
{
	if(monoCam) monoCam->setFOVy(Ogre::Degree(degreeFov));
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
	float X(16), Y(9);
	debugCam->setOrthoWindow(X,Y);
	debugCamNode = debugSmgr->getRootSceneNode()->createChildSceneNode();
	debugCamNode->attachObject(debugCam);

//--Create the debug plane
	//Base setup inside the scene manager
	debugPlaneNode = debugCamNode->createChildSceneNode();
	debugPlaneNode->setPosition(0,0,-1);
	Ogre::ManualObject* debugPlane = debugSmgr->createManualObject("DebugPlane");
	//Material
	DebugPlaneMaterial = Ogre::MaterialManager::getSingleton().create("DebugPlaneMaterial", "General", true);
	debugTexturePlane = DebugPlaneMaterial.getPointer()->getTechnique(0)->getPass(0)->createTextureUnitState();
	//The manual object itself
	debugPlane->begin("DebugPlaneMaterial", Ogre::RenderOperation::OT_TRIANGLE_STRIP);
	//4 verticies with texture coodinates
	float x(X/2), y(Y/2);
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

///This will create the Oculus Textures and the Ogre textures for rendering and mirror display
void OgreOculusRender::initRttRendering()
{
	//Init GLEW here to be able to call OpenGL functions
	GLenum err = glewInit();
	if(err != GLEW_OK)
	{
		Annwvyn::AnnDebug("Failed to glewTnit()\nCannot call manual OpenGL\nError Code : " + (unsigned int)err);
		exit(ANN_ERR_RENDER);
	}
	Annwvyn::AnnDebug() << "Using GLEW version : " << glewGetString(GLEW_VERSION);

	//Get texture size from ovr with the maximal FOV for each eye
	texSizeL = ovr_GetFovTextureSize(Oculus->getSession(), ovrEye_Left, Oculus->getHmdDesc().MaxEyeFov[left], 1.0f);
	texSizeR = ovr_GetFovTextureSize(Oculus->getSession(), ovrEye_Right, Oculus->getHmdDesc().MaxEyeFov[right], 1.0f);
	//Calculate the render buffer size for both eyes
	bufferSize.w = texSizeL.w + texSizeR.w;
	bufferSize.h = max(texSizeL.h, texSizeR.h);
	Annwvyn::AnnDebug() << "Buffer texture size : " << bufferSize.w << " x " <<bufferSize.h  << " px";

	ovrTextureSwapChainDesc textureSwapChainDesc = {};
	textureSwapChainDesc.Type = ovrTexture_2D;
	textureSwapChainDesc.ArraySize = 1;
	textureSwapChainDesc.Format = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;
	textureSwapChainDesc.Width = bufferSize.w;
	textureSwapChainDesc.Height = bufferSize.h;
	textureSwapChainDesc.MipLevels = 1;
	textureSwapChainDesc.SampleCount = 1;
	textureSwapChainDesc.StaticImage = ovrFalse;

	//Request the creation of an OpenGL swap texture set from the Oculus Library
	if (ovr_CreateTextureSwapChainGL(Oculus->getSession(), &textureSwapChainDesc, &textureSwapChain) != ovrSuccess)
	{
		//If we can't get the textures, there is no point trying more.
		Annwvyn::AnnDebug("Cannot create Oculus swap texture");
		exit(ANN_ERR_RENDER);
	}
	
	//Create the Ogre equivalent of the texture as a render target for Ogre
	Ogre::GLTextureManager* textureManager(static_cast<Ogre::GLTextureManager*>(Ogre::GLTextureManager::getSingletonPtr()));
	Ogre::TexturePtr rtt_texture(textureManager->createManual("RttTex", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
		Ogre::TEX_TYPE_2D, bufferSize.w, bufferSize.h, 0, Ogre::PF_R8G8B8, Ogre::TU_RENDERTARGET));

	//Save the texture id for low-level GL call on the texture during render
	Ogre::RenderTexture* rttEyes = rtt_texture->getBuffer(0, 0)->getRenderTarget();
	Ogre::GLTexture* gltex = static_cast<Ogre::GLTexture*>(Ogre::GLTextureManager::getSingleton().getByName("RttTex").getPointer());
	renderTextureID = gltex->getGLID();

	vpts[left]  = rttEyes->addViewport(cams[left],  0, 0,    0, 0.5f);
	vpts[right] = rttEyes->addViewport(cams[right], 1, 0.5f, 0, 0.5f);

	changeViewportBackgroundColor(backgroundColor);
	ovrMirrorTextureDesc mirrorTextureDesc = {};
	mirrorTextureDesc.Width = Oculus->getHmdDesc().Resolution.w;
	mirrorTextureDesc.Height = Oculus->getHmdDesc().Resolution.h;
	mirrorTextureDesc.Format = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;	

	if (ovr_CreateMirrorTextureGL(Oculus->getSession(), &mirrorTextureDesc, &mirrorTexture) != ovrSuccess)
	{
		//If for some weird reason (stars alignment, dragons, northen gods, reaper invasion) we can't create the mirror texture
		Annwvyn::AnnDebug("Cannot create Oculus mirror texture");
		exit(ANN_ERR_RENDER);
	}
	//Create the Ogre equivalent of this buffer
	Ogre::TexturePtr mirror_texture(textureManager->createManual("MirrorTex", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
		Ogre::TEX_TYPE_2D, Oculus->getHmdDesc().Resolution.w, Oculus->getHmdDesc().Resolution.h, 0, Ogre::PF_R8G8B8, Ogre::TU_RENDERTARGET));

	//Save the GL texture id for updating the mirror texture
	ogreMirrorTextureID = static_cast<Ogre::GLTexture*>(Ogre::GLTextureManager::getSingleton().getByName("MirrorTex").getPointer())->getGLID();
	ovr_GetTextureSwapChainBufferGL(Oculus->getSession(), textureSwapChain, 0, &oculusMirrorTextureID);
	
	//Attach the camera of the debug render scene to a viewport on the actuall application window
	debugViewport = window->addViewport(debugCam);
	debugViewport->setBackgroundColour(Ogre::ColourValue::Black);
	debugTexturePlane->setTextureName("MirrorTex");
	debugTexturePlane->setTextureFiltering(Ogre::FO_POINT, Ogre::FO_POINT, Ogre::FO_NONE);
	debugViewport->setAutoUpdated(false);
}

void OgreOculusRender::showRawView()
{
	mirror = false;
	if(!debugTexturePlane)return;
	self->debugViewport->setCamera(self->debugCam);
	debugTexturePlane->setTextureName("RttTex");
	Annwvyn::AnnDebug() << "Switched to Raw view";
}
void OgreOculusRender::showMirrorView()
{
	mirror = true;
	if(!debugTexturePlane)return;
	self->debugViewport->setCamera(self->debugCam);
	debugTexturePlane->setTextureName("MirrorTex");
	Annwvyn::AnnDebug() << "Switched to Oculus Compositor view";
}

void OgreOculusRender::showMonscopicView()
{
	mirror = false;
	if(!self) return;
	self->debugViewport->setCamera(self->monoCam);
	self->debugViewport->setBackgroundColour(self->backgroundColor);
	Annwvyn::AnnDebug() << "Switched to Monoscopic view";
}

void OgreOculusRender::initOculus()
{
	//Populate OVR structures
	EyeRenderDesc[left]  = ovr_GetRenderDesc(Oculus->getSession(), ovrEye_Left,  Oculus->getHmdDesc().MaxEyeFov[left]);
	EyeRenderDesc[right] = ovr_GetRenderDesc(Oculus->getSession(), ovrEye_Right, Oculus->getHmdDesc().MaxEyeFov[right]);
	offset[left] =EyeRenderDesc[left].HmdToEyeOffset;
	offset[right]=EyeRenderDesc[right].HmdToEyeOffset;

	//Create a layer with our single swaptexture on it. Each side is an eye.
	layer.Header.Type = ovrLayerType_EyeFov;
	layer.Header.Flags = 0;
	layer.ColorTexture[left]  = textureSwapChain;
	layer.ColorTexture[right] = textureSwapChain;
	layer.Fov[left]  = EyeRenderDesc[left].Fov;
	layer.Fov[right] = EyeRenderDesc[right].Fov;
	ovrRecti leftRect, rightRect;
	leftRect.Size = bufferSize;
	leftRect.Size.w /= 2;
	rightRect = leftRect;
	ovrVector2i leftPos, rightPos;
	leftPos.x = 0;
	leftPos.y = 0;
	rightPos = leftPos;
	rightPos.x = bufferSize.w/2;
	leftRect.Pos = leftPos;
	rightRect.Pos = rightPos;
	layer.Viewport[left]  = leftRect;
	layer.Viewport[right] = rightRect;

	//This needs to be called at least once
	calculateProjectionMatrix();

	//Make sure that the perf hud will not show up by himself...
	perfHudMode = ovrPerfHud_Off;
	ovr_SetInt(Oculus->getSession(), "PerfHudMode", perfHudMode);
}

void OgreOculusRender::calculateProjectionMatrix()
{
	//The average human has 2 eyes, but for some reason there's an "ovrEye_Count" constant on the oculus library. 
	for(size_t eyeIndex(0); eyeIndex < ovrEye_Count; eyeIndex++)
	{
		//Get the projection matrix
		
		ovrMatrix4f proj = ovrMatrix4f_Projection(EyeRenderDesc[eyeIndex].Fov, 
			nearClippingDistance, 
			farClippingDistance, 
			0);

		//Convert it to Ogre matrix
		Ogre::Matrix4 OgreProj;
		for(size_t x(0); x < 4; x++)
			for(size_t y(0); y < 4; y++)
				OgreProj[x][y] = proj.M[x][y];

		//Set the matrix
		cams[eyeIndex]->setCustomProjectionMatrix(true, OgreProj);
	}
}

void OgreOculusRender::updateTracking()
{
	//Get current camera base information
	cameraPosition = CameraNode->getPosition();
	cameraOrientation = CameraNode->getOrientation();

	//Begin frame - get timing
	lastFrameDisplayTime = currentFrameDisplayTime;
	ts = ovr_GetTrackingState(Oculus->getSession(), currentFrameDisplayTime = ovr_GetPredictedDisplayTime(Oculus->getSession(), 0), ovrTrue);
	updateTime = currentFrameDisplayTime - lastFrameDisplayTime;

	//Get the pose
	pose = ts.HeadPose.ThePose;
	ovr_CalcEyePoses(pose, offset, layer.RenderPose); 
	
	//Get the hmd orientation
	oculusOrient = pose.Orientation;
	oculusPos = pose.Position;

	//Apply pose to the two cameras
	for(size_t eye = 0; eye < ovrEye_Count; eye++)
	{
		//cameraOrientation and cameraPosition are the player position/orientation on the space
		cams[eye]->setOrientation(cameraOrientation * Ogre::Quaternion(oculusOrient.w, oculusOrient.x, oculusOrient.y, oculusOrient.z));
		cams[eye]->setPosition
			(cameraPosition  //the "gameplay" position of player's avatar head

			+ (cams[eye]->getOrientation() * Ogre::Vector3( //realword camera orientation + the  
			EyeRenderDesc[eye].HmdToEyeOffset.x, //view adjust vector.
			EyeRenderDesc[eye].HmdToEyeOffset.y, //The translations has to occur in function of the current head orientation.
			EyeRenderDesc[eye].HmdToEyeOffset.z) //That's why just multiply by the quaternion we just calculated. 

			+ cameraOrientation * Ogre::Vector3( //cameraOrientation is in fact the direction the avatar is facing expressed as an Ogre::Quaternion
			oculusPos.x,
			oculusPos.y,
			oculusPos.z)));
	}

	//Update the pose for gameplay purposes
	returnPose.position = cameraPosition + cameraOrientation * Ogre::Vector3(oculusPos.x, oculusPos.y, oculusPos.z);
	returnPose.orientation = cameraOrientation * Ogre::Quaternion(oculusOrient.w, oculusOrient.x, oculusOrient.y, oculusOrient.z);
	monoCam->setPosition(returnPose.position);
	monoCam->setOrientation(returnPose.orientation);
}

void OgreOculusRender::renderAndSubmitFrame()
{
	Ogre::WindowEventUtilities::messagePump();
	//Select the current render texture
	//textureSet->CurrentIndex = (textureSet->CurrentIndex + 1) % textureSet->TextureCount;
	ovr_GetTextureSwapChainCurrentIndex(Oculus->getSession(), textureSwapChain, &currentIndex);
	ovr_GetTextureSwapChainBufferGL(Oculus->getSession(), textureSwapChain, currentIndex, &oculusMirrorTextureID);


	//root->renderOneFrame();
	root->_fireFrameRenderingQueued();
	vpts[left]->update();
	vpts[right]->update();
	
	//Copy the rendered image to the Oculus Swap Texture
	glCopyImageSubData(renderTextureID, GL_TEXTURE_2D, 0, 0, 0, 0, 
		oculusMirrorTextureID, GL_TEXTURE_2D, 0, 0, 0, 0, 
		bufferSize.w,bufferSize.h, 1);
	
	//Get the rendering layer
	layers = &layer.Header;

	//Submit the frame 
	ovr_CommitTextureSwapChain(Oculus->getSession(), textureSwapChain);
	ovr_SubmitFrame(Oculus->getSession(), 0, nullptr, &layers, 1);


	//Update the render mirrored view
	if(window->isVisible())
	{
		//Put the mirrored view available for OGRE
		if(mirror)
		glCopyImageSubData(oculusMirrorTextureID, GL_TEXTURE_2D, 0, 0, 0, 0, 
		ogreMirrorTextureID, GL_TEXTURE_2D, 0, 0, 0, 0, 
		Oculus->getHmdDesc().Resolution.w, Oculus->getHmdDesc().Resolution.h, 1);
		
		debugViewport->update();
		window->update();
	}

}
