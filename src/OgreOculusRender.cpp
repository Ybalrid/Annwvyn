#include "stdafx.h"
#include "OgreOculusRender.hpp"

#include "AnnLogger.hpp"

using namespace Annwvyn;

//Static class members
bool OgreOculusRender::mirrorHMDView(true);
Ogre::TextureUnitState* OgreOculusRender::debugTexturePlane(nullptr);
OgreOculusRender* OgreOculusRender::OculusSelf = nullptr;
OgreOculusRender::OgreOculusRender(std::string winName) : OgreVRRender(winName),
	debugSmgr(nullptr),
	Oculus(nullptr),
	nearClippingDistance(0.5f),
	farClippingDistance(4000.0f),
	lastOculusPosition(headPosition),
	lastOculusOrientation(headOrientation),
	textureSwapChain(0),
	perfHudMode(ovrPerfHud_Off)
{
	
	eyeCameras[left ] = nullptr;
	eyeCameras[right] = nullptr;
	vpts[left ] = nullptr;		
	vpts[right] = nullptr;
	monoCam = nullptr;
	OculusSelf = static_cast<OgreOculusRender*>(self);

}

OgreOculusRender::~OgreOculusRender()
{
	//Destroy any Oculus SDK related objects
	ovr_DestroyTextureSwapChain(Oculus->getSession(), textureSwapChain);
	ovr_DestroyMirrorTexture(Oculus->getSession(), mirrorTexture);
	delete Oculus;

	//Clean the Ogre environement
	root->destroySceneManager(debugSmgr);
	root->destroySceneManager(smgr);

	//Set the material shared pointer to null. If not done, Ogre will crash.
	DebugPlaneMaterial.setNull();

	root->unloadPlugin("Plugin_OctreeSceneManager");
	delete root;


}

bool OgreOculusRender::shouldQuit()
{
	if (getSessionStatus().ShouldQuit == ovrTrue)
		return true;
	return false;
}

bool OgreOculusRender::shouldRecenter()
{
	if (getSessionStatus().ShouldRecenter == ovrTrue)
		return true;
	return false;
}

bool OgreOculusRender::isVisibleInHmd()
{
	if (getSessionStatus().IsVisible == ovrTrue)
		return true;
	return false;
}

void OgreOculusRender::cycleDebugHud()
{
	//Loop through the perf HUD mode available
	perfHudMode = (perfHudMode+1) % (ovrPerfHud_Count);

	//Set the current perf hud mode
	ovr_SetInt(Oculus->getSession(), "PerfHudMode", perfHudMode);
}

void OgreOculusRender::changeViewportBackgroundColor(Ogre::ColourValue color)
{
	//save the color then apply it to each viewport
	backgroundColor = (color);

	//Render buffers
	for(byte i(0); i < 2; i++)
		if(vpts[i])
			vpts[i]->setBackgroundColour(backgroundColor);

	//Debug window
	if(debugViewport && !mirrorHMDView)
		debugViewport->setBackgroundColour(backgroundColor);
}

void OgreOculusRender::debugPrint()
{
	for(byte i(0); i < 2; i++)
	{
		AnnDebug() << "eyeCamera " << i << " " << eyeCameras[i]->getPosition();
		AnnDebug() << eyeCameras[i]->getOrientation();
	}
}

void OgreOculusRender::debugSaveToFile(const char path[])
{
	//Check if texture exist
	if(Ogre::TextureManager::getSingleton().getByName("RttTex").getPointer()) 
		//Write buffer to specified file. This is really slow and should only be used to debug the renderer
		Ogre::TextureManager::getSingleton().getByName("RttTex").getPointer()->getBuffer(0, 0)->getRenderTarget()->writeContentsToFile(path);
}


Ogre::Timer* OgreOculusRender::getTimer()
{
	if(root)
		return root->getTimer();
	return nullptr;
}


void OgreOculusRender::recenter()
{
	ovr_RecenterTrackingOrigin(Oculus->getSession());
}

void OgreOculusRender::initVrHmd()
{
	//Class to get basic information from the Rift. Initialize the RiftSDK
	Oculus = new OculusInterface();
	hmdSize = Oculus->getHmdDesc().Resolution;
	ovr_GetSessionStatus(Oculus->getSession(), &sessionStatus);
	updateTime = 1.0 / static_cast<double>(Oculus->getHmdDesc().DisplayRefreshRate);
}

void OgreOculusRender::getOgreConfig()
{
	//Ogre as to be initialized
	if(!root) exit(ANN_ERR_NOTINIT);

	//Load OgrePlugins
	root->loadPlugin("RenderSystem_GL");
	root->loadPlugin("Plugin_OctreeSceneManager");
	
	//Set the classic OpenGL render system
	root->setRenderSystem(root->getRenderSystemByName("OpenGL Rendering Subsystem"));
	root->getRenderSystem()->setFixedPipelineEnabled(true);
}

void OgreOculusRender::createWindow()
{
	if(!root) exit(ANN_ERR_NOTINIT);
	if(!Oculus) 
	{
		AnnDebug() << "Please initialize the OculusInterface before creating window";
		exit(ANN_ERR_NOTINIT);
	}

	Ogre::NameValuePairList misc;
	misc["vsync"] = "false"; //This vsync parameter has no scence in VR. The display is done by the Compositor
	misc["top"] = "0";
	misc["left"] = "0";
	root->initialise(false);

	float w(hmdSize.w), h(hmdSize.h);
	if(w >= 1920) w /=1.5;
	if(h >= 1080) h /=1.5;

	window = root->createRenderWindow(name + " : mirror Rift view output (Please put your headset)", w, h, false, &misc);
}

void OgreOculusRender::initCameras()
{
	if(!smgr)
	{
		AnnDebug() << "Cannot init cameras before having the scene(s) manager(s) in place";
		exit(ANN_ERR_NOTINIT);
	}
	//Mono view camera
	monoCam = 
	smgr->createCamera("monocam");
	monoCam->setAspectRatio(16.0/9.0);
	monoCam->setAutoAspectRatio(false);
	monoCam->setPosition(headPosition);
	monoCam->setNearClipDistance(0.1);
	monoCam->setFarClipDistance(4000);
	monoCam->setFOVy(Ogre::Degree(90));

	//VR Eye cameras
	eyeCameras[left ] = smgr->createCamera("lcam");
	eyeCameras[left ]->setPosition(headPosition);
	eyeCameras[left ]->setAutoAspectRatio(true);
	eyeCameras[right] = smgr->createCamera("rcam");
	eyeCameras[right]->setPosition(headPosition);
	eyeCameras[right]->setAutoAspectRatio(true);

	//do NOT attach camera to this node...
	headNode = smgr->getRootSceneNode()->createChildSceneNode();
}

void OgreOculusRender::setMonoFov(float degreeFov)
{
	if(monoCam) monoCam->setFOVy(Ogre::Degree(degreeFov));
}

void OgreOculusRender::setCamerasNearClippingDistance(float distance)
{
	nearClippingDistance = distance;
}

void OgreOculusRender::setCameraFarClippingDistance(float distance)
{
	farClippingDistance = distance;
}

void OgreOculusRender::initScene()
{
	//Create the scene manager for the engine
	if(!root) exit(ANN_ERR_NOTINIT);
	smgr = root->createSceneManager("OctreeSceneManager", "OSM_SMGR"); 
	smgr->setShadowTechnique(Ogre::ShadowTechnique::SHADOWTYPE_STENCIL_ADDITIVE);

	//Create the scene manager for the debug output
	debugSmgr = root->createSceneManager(Ogre::ST_GENERIC);
	debugSmgr->setAmbientLight(Ogre::ColourValue::White); //no shadow

	//Create the camera with a 16:9 ratio in Orthographic projection
	debugCam = debugSmgr->createCamera("DebugRender");
	debugCam->setAutoAspectRatio(true);
	debugCam->setNearClipDistance(0.001f);
	debugCam->setProjectionType(Ogre::PT_ORTHOGRAPHIC);
	float X(16), Y(9);
	float x(X/2), y(Y/2);
	debugCam->setOrthoWindow(X,Y);

	//Attach the camera to a node
	debugCamNode = debugSmgr->getRootSceneNode()->createChildSceneNode();
	debugCamNode->attachObject(debugCam);

	//--Create the debug texture plane
	//Base setup inside the scene manager
	debugPlaneNode = debugCamNode->createChildSceneNode();
	debugPlaneNode->setPosition(0,0,-1);

	//Create a manual object
	Ogre::ManualObject* debugPlane = debugSmgr->createManualObject("DebugPlane");

	//Create a manual material and add a texture unit state to the default pass
	DebugPlaneMaterial = Ogre::MaterialManager::getSingleton().create("DebugPlaneMaterial", "General", true);
	debugTexturePlane = DebugPlaneMaterial.getPointer()->getTechnique(0)->getPass(0)->createTextureUnitState();

	//Describe the manual object
	/*
	 *  This is a 4 vertices quad with a size of 16x9 units with it's origin in the center
	 *  The quad got mapped square texture coordinates at each corner, covering the whole UV map
	 *
	 * The debugPlane is a perfect rectangle drawn by 2 polygons (tiangles). The position in object-space are defined as folowing
	 * on the "points" array :
	 *  0 +---------------+ 2
	 *    |           /   |
	 *    |        /      |
	 *    |     /         |
	 *    |  /            |
	 *  1 +----------------+ 3
	 * Texture coordinates are also mapped. To display properly, the texture should respect the same aspect ratio (2:1)
	 */

	debugPlane->begin("DebugPlaneMaterial", Ogre::RenderOperation::OT_TRIANGLE_STRIP);
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
	AnnDebug() << "Init GL Extension Wrangler";
	GLenum err = glewInit();
	if(err != GLEW_OK)
	{
		AnnDebug("Failed to glewTnit()\n\
						  Cannot call manual OpenGL\n\
						  Error Code : " + (unsigned int)err);
		exit(ANN_ERR_RENDER);
	}
	AnnDebug() << "Using GLEW version : " << glewGetString(GLEW_VERSION);

	//Get texture size from ovr with the maximal FOV for each eye
	ovrSizei texSizeL = ovr_GetFovTextureSize(Oculus->getSession(), ovrEye_Left , Oculus->getHmdDesc().DefaultEyeFov[left ], 1.f);
	ovrSizei texSizeR = ovr_GetFovTextureSize(Oculus->getSession(), ovrEye_Right, Oculus->getHmdDesc().DefaultEyeFov[right], 1.f);
	
	//Calculate the render buffer size for both eyes
	bufferSize.w = texSizeL.w + texSizeR.w;
	bufferSize.h = max(texSizeL.h, texSizeR.h);
	AnnDebug() << "Buffer texture size : " << bufferSize.w << " x " <<bufferSize.h  << " px";

	//Define the creation option of the texture swap chain
	ovrTextureSwapChainDesc textureSwapChainDesc = {};
	textureSwapChainDesc.Type = ovrTexture_2D;
	textureSwapChainDesc.ArraySize = 1;
	textureSwapChainDesc.Format = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;
	textureSwapChainDesc.Width = bufferSize.w;
	textureSwapChainDesc.Height = bufferSize.h;
	textureSwapChainDesc.MipLevels = 1;
	textureSwapChainDesc.SampleCount = 1;
	textureSwapChainDesc.StaticImage = ovrFalse;

	//Request the creation of an OpenGL swapChain from the Oculus Library
	if (ovr_CreateTextureSwapChainGL(Oculus->getSession(), &textureSwapChainDesc, &textureSwapChain) != ovrSuccess)
	{
		//If we can't get the textures, there is no point trying more.
		AnnDebug("Cannot create Oculus OpenGL SwapChain");
		exit(ANN_ERR_RENDER);
	}
	
	//Create the Ogre equivalent of the texture as a render target for Ogre
	Ogre::GLTextureManager* textureManager(static_cast<Ogre::GLTextureManager*>(Ogre::GLTextureManager::getSingletonPtr()));
	Ogre::TexturePtr rtt_texture(textureManager->createManual("RttTex", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
		Ogre::TEX_TYPE_2D, bufferSize.w, bufferSize.h, 0, Ogre::PF_R8G8B8, Ogre::TU_RENDERTARGET));

	//Save the texture id for low-level GL call on the texture during render
	Ogre::RenderTexture* rttEyes = rtt_texture->getBuffer(0, 0)->getRenderTarget();
	Ogre::GLTexture* gltex = static_cast<Ogre::GLTexture*>(Ogre::GLTextureManager::getSingleton().getByName("RttTex").getPointer());
	renderTextureGLID = gltex->getGLID();

	//Create viewports on the texture to render the eyeCameras
	vpts[left]  = rttEyes->addViewport(eyeCameras[left ], 0,    0, 0, 0.5f);
	vpts[right] = rttEyes->addViewport(eyeCameras[right], 1, 0.5f, 0, 0.5f);

	changeViewportBackgroundColor(backgroundColor);
	
	//Fill in MirrorTexture parameters
	ovrMirrorTextureDesc mirrorTextureDesc = {};
	mirrorTextureDesc.Width = hmdSize.w;
	mirrorTextureDesc.Height = hmdSize.h;
	mirrorTextureDesc.Format = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;	

	//Create the Oculus Mirror Texture
	if (ovr_CreateMirrorTextureGL(Oculus->getSession(), &mirrorTextureDesc, &mirrorTexture) != ovrSuccess)
	{
		//If for some weird reason (stars alignment, dragons, northen gods, reaper invasion) we can't create the mirror texture
		AnnDebug("Cannot create Oculus mirror texture");
		exit(ANN_ERR_RENDER);
	}

	//Create the Ogre equivalent of this buffer
	Ogre::TexturePtr mirror_texture(textureManager->createManual("MirrorTex", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
		Ogre::TEX_TYPE_2D, hmdSize.w, hmdSize.h, 0, Ogre::PF_R8G8B8, Ogre::TU_RENDERTARGET));

	//Save the GL texture id for updating the mirror texture
	ogreMirrorTextureGLID = static_cast<Ogre::GLTexture*>(Ogre::GLTextureManager::getSingleton().getByName("MirrorTex").getPointer())->getGLID();
	ovr_GetTextureSwapChainBufferGL(Oculus->getSession(), textureSwapChain, 0, &oculusRenderTextureGLID);
	
	//Attach the camera of the debug render scene to a viewport on the actuall application window
	debugViewport = window->addViewport(debugCam);
	debugViewport->setBackgroundColour(Ogre::ColourValue::Black);
	debugTexturePlane->setTextureName("MirrorTex");
	debugTexturePlane->setTextureFiltering(Ogre::FO_POINT, Ogre::FO_POINT, Ogre::FO_NONE);
	debugViewport->setAutoUpdated(false);
}

void OgreOculusRender::showRawView()
{
	mirrorHMDView = false;
	if(!debugTexturePlane) return;
	OculusSelf->debugViewport->setCamera(OculusSelf->debugCam);
	debugTexturePlane->setTextureName("RttTex");
	OculusSelf->debugViewport->setBackgroundColour(OculusSelf->backgroundColor);
	AnnDebug() << "Switched to Raw view";
}

void OgreOculusRender::showMirrorView()
{
	mirrorHMDView = true;
	if(!debugTexturePlane) return;
	OculusSelf->debugViewport->setCamera(OculusSelf->debugCam);
	debugTexturePlane->setTextureName("MirrorTex");
	OculusSelf->debugViewport->setBackgroundColour(Ogre::ColourValue::Black);
	AnnDebug() << "Switched to Oculus Compositor view";
}

void OgreOculusRender::showMonscopicView()
{
	mirrorHMDView = false;
	if(!OculusSelf) return;
	OculusSelf->debugViewport->setCamera(OculusSelf->monoCam);
	OculusSelf->debugViewport->setBackgroundColour(OculusSelf->backgroundColor);
	AnnDebug() << "Switched to Monoscopic view";
}

void OgreOculusRender::initClientHmdRendering()
{
	//Populate OVR structures
	EyeRenderDesc[left ] = ovr_GetRenderDesc(Oculus->getSession(), ovrEye_Left , Oculus->getHmdDesc().DefaultEyeFov[left ]);
	EyeRenderDesc[right] = ovr_GetRenderDesc(Oculus->getSession(), ovrEye_Right, Oculus->getHmdDesc().DefaultEyeFov[right]);
	offset[left ] = EyeRenderDesc[left ].HmdToEyeOffset;
	offset[right] = EyeRenderDesc[right].HmdToEyeOffset;

	//Create a layer with our single swaptexture on it. Each side is an eye.
	layer.Header.Type = ovrLayerType_EyeFov;
	layer.Header.Flags = 0;
	layer.ColorTexture[left ] = textureSwapChain;
	layer.ColorTexture[right] = textureSwapChain;
	layer.Fov[left ] = EyeRenderDesc[left ].Fov;
	layer.Fov[right] = EyeRenderDesc[right].Fov;

	//Define the two viewports : 
	ovrRecti leftRect, rightRect;
	leftRect.Size = bufferSize;			//same size than the buffer
	leftRect.Size.w /= 2;				//but half the width
	rightRect = leftRect;				//The two rect are of the same size, but not at the same position
	ovrVector2i leftPos, rightPos;		
	leftPos.x = 0;						//The left one start at the bottom left corner
	leftPos.y = 0;
	rightPos = leftPos;
	rightPos.x = bufferSize.w/2;		//But the right start at half the buffer width
	leftRect.Pos = leftPos;				
	rightRect.Pos = rightPos;

	//Assign the viewports
	layer.Viewport[left ] = leftRect;
	layer.Viewport[right] = rightRect;

	//Get the projection matrix for the desired near/far clipping from Oculus and apply them to the eyeCameras
	calculateProjectionMatrix();

	//Make sure that the perf hud will not show up by himself...
	perfHudMode = ovrPerfHud_Off;
	ovr_SetInt(Oculus->getSession(), "PerfHudMode", perfHudMode);
}

void OgreOculusRender::calculateProjectionMatrix()
{
	//The average human has 2 eyes, but for some reason there's an "ovrEye_Count" constant on the oculus library. 
	for(byte eyeIndex(0); eyeIndex < ovrEye_Count; eyeIndex++)
	{
		//Get the projection matrix
		ovrMatrix4f proj = ovrMatrix4f_Projection(EyeRenderDesc[eyeIndex].Fov, 
			nearClippingDistance, 
			farClippingDistance, 
			0);

		//Convert it to Ogre matrix
		Ogre::Matrix4 OgreProj;
		for(byte x(0); x < 4; x++)
			for(byte y(0); y < 4; y++)
				OgreProj[x][y] = proj.M[x][y];

		//Set the matrix
		eyeCameras[eyeIndex]->setCustomProjectionMatrix(true, OgreProj);
	}
}

ovrSessionStatus OgreOculusRender::getSessionStatus()
{
	ovr_GetSessionStatus(Oculus->getSession(), &sessionStatus);
	return sessionStatus;
}

void OgreOculusRender::initPipeline()
{
	getOgreConfig();
	createWindow();
	initScene();
	initCameras();
	setCamerasNearClippingDistance();
	initRttRendering();
}

bool OgreOculusRender::usesCustomAudioDevice()
{
	return true;
}

std::string OgreOculusRender::getAudioDeviceIdentifierSubString()
{
	return std::string("Rift Audio");
}

void OgreOculusRender::updateTracking()
{
	//Get current camera base information
	headPosition = headNode->getPosition();
	headOrientation = headNode->getOrientation();

	//Begin frame - get timing
	lastFrameDisplayTime = currentFrameDisplayTime;

	//Reorient the headset if the runtime flags for it
	if(getSessionStatus().ShouldRecenter) recenter();

	//Get the tracking state 
	ts = ovr_GetTrackingState(Oculus->getSession(), 
		currentFrameDisplayTime = ovr_GetPredictedDisplayTime(Oculus->getSession(), 0),
		ovrTrue);
	
	//Calculate delta between last and this frame
	updateTime = currentFrameDisplayTime - lastFrameDisplayTime;

	//Get the pose
	pose = ts.HeadPose.ThePose;
	ovr_CalcEyePoses(pose, offset, layer.RenderPose); 
	
	//Get the hmd orientation
	oculusOrient = pose.Orientation;
	oculusPos = pose.Position;

	//Apply pose to the two cameras
	for(byte eye = 0; eye < ovrEye_Count; eye++)
	{
		//headOrientation and headPosition are the player position/orientation on the space
		eyeCameras[eye]->setOrientation(headOrientation * Ogre::Quaternion(oculusOrient.w, oculusOrient.x, oculusOrient.y, oculusOrient.z));
		eyeCameras[eye]->setPosition
			(headPosition  //the "gameplay" position of player's avatar head

			+ (eyeCameras[eye]->getOrientation() * Ogre::Vector3( //realword camera orientation + the  
			EyeRenderDesc[eye].HmdToEyeOffset.x, //view adjust vector.
			EyeRenderDesc[eye].HmdToEyeOffset.y, //The translations has to occur in function of the current head orientation.
			EyeRenderDesc[eye].HmdToEyeOffset.z) //That's why just multiply by the quaternion we just calculated. 

			+ headOrientation * Ogre::Vector3( //headOrientation is in fact the direction the avatar is facing expressed as an Ogre::Quaternion
			oculusPos.x,
			oculusPos.y,
			oculusPos.z)));
	}

	//Update the pose for gameplay purposes
	returnPose.position = headPosition + headOrientation * Ogre::Vector3(oculusPos.x, oculusPos.y, oculusPos.z);
	returnPose.orientation = headOrientation * Ogre::Quaternion(oculusOrient.w, oculusOrient.x, oculusOrient.y, oculusOrient.z);
	monoCam->setPosition(returnPose.position);
	monoCam->setOrientation(returnPose.orientation);
}

void OgreOculusRender::renderAndSubmitFrame()
{
	//Ogre's documentation ask for this function to be called once per frame
	Ogre::WindowEventUtilities::messagePump();

	//Select the current render texture
	ovr_GetTextureSwapChainCurrentIndex(Oculus->getSession(), textureSwapChain, &currentIndex);
	
	//Update the relevent OpenGL IDs
	ovr_GetTextureSwapChainBufferGL(Oculus->getSession(), textureSwapChain, currentIndex, &oculusRenderTextureGLID);
	ovr_GetMirrorTextureBufferGL(Oculus->getSession(), mirrorTexture, &oculusMirrorTextureGLID);

	//Render
	root->_fireFrameRenderingQueued();
	vpts[left]->update();
	vpts[right]->update();
	
	//Copy the rendered image to the Oculus Swap Texture
	glCopyImageSubData(renderTextureGLID, GL_TEXTURE_2D, 0, 0, 0, 0, 
		oculusRenderTextureGLID, GL_TEXTURE_2D, 0, 0, 0, 0, 
		bufferSize.w,bufferSize.h, 1);
	
	//Get the rendering layer
	layers = &layer.Header;

	//Submit the frame 
	ovr_CommitTextureSwapChain(Oculus->getSession(), textureSwapChain);
	ovr_SubmitFrame(Oculus->getSession(), 0, nullptr, &layers, 1);

	//Update the render mirrored view
	if(window->isVisible())
	{
		//Put the mirrored view available for Ogre if asked for
		if(mirrorHMDView)
			glCopyImageSubData(oculusMirrorTextureGLID, GL_TEXTURE_2D, 0, 0, 0, 0, 
			ogreMirrorTextureGLID, GL_TEXTURE_2D, 0, 0, 0, 0, 
			hmdSize.w, hmdSize.h, 1);
		
		//Update the window
		debugViewport->update();
		window->update();
	}
}
