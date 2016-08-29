#include "stdafx.h"
#include "OgreOpenVRRender.hpp"
#include "AnnLogger.hpp"
#include "AnnEngine.hpp"

OgreOpenVRRender* OgreOpenVRRender::OpenVRSelf(nullptr);

OgreOpenVRRender::OgreOpenVRRender(std::string winName) : OgreVRRender(winName),
vrSystem(nullptr),
windowWidth(1280),
windowHeight(720),
gamma(false),
API(vr::API_OpenGL),
shouldQuitState(false)
{
	OpenVRSelf = static_cast<OgreOpenVRRender*>(self);

	rttViewports[left] = nullptr;
	rttViewports[right] = nullptr;
}

OgreOpenVRRender::~OgreOpenVRRender()
{
	vr::VR_Shutdown();

	rttTexture[left].setNull();
	rttTexture[right].setNull();
	root->destroySceneManager(smgr);
	root->unloadPlugin("Plugin_OctreeSceneManager");
	delete root;
}

void OgreOpenVRRender::initPipeline()
{
	getOgreConfig();
	createWindow();
	initScene();
	initCameras();
	setCamerasNearClippingDistance();
	initRttRendering();
}

//This function is from VALVe
std::string GetTrackedDeviceString(vr::IVRSystem *pHmd, vr::TrackedDeviceIndex_t unDevice, vr::TrackedDeviceProperty prop, vr::TrackedPropertyError *peError = NULL)
{
	uint32_t unRequiredBufferLen = pHmd->GetStringTrackedDeviceProperty(unDevice, prop, NULL, 0, peError);
	if (unRequiredBufferLen == 0)
		return "";

	char *pchBuffer = new char[unRequiredBufferLen];
	unRequiredBufferLen = pHmd->GetStringTrackedDeviceProperty(unDevice, prop, pchBuffer, unRequiredBufferLen, peError);
	std::string sResult = pchBuffer;
	delete[] pchBuffer;
	return sResult;
}

void OgreOpenVRRender::initVrHmd()
{
	//Initialize OpenVR
	vrSystem = vr::VR_Init(&hmdError, vr::EVRApplicationType::VRApplication_Scene);
	if (hmdError != vr::VRInitError_None) //Check for errors
		switch (hmdError)
		{
			default:
				displayWin32ErrorMessage(L"Error: failed OpenVR VR_Init",
										 L"Undescribed error when initalizing the OpenVR Render object"
				);
				exit(ANN_ERR_NOTINIT);
			case vr::VRInitError_Init_HmdNotFound:
			case vr::VRInitError_Init_HmdNotFoundPresenceFailed:
				displayWin32ErrorMessage(L"Error: cannot find HMD",
										 L"OpenVR cannot find HMD.\n"
										 L"Please install SteamVR and launchj it, and verrify HMD USB and HDMI connection");
				exit(ANN_ERR_CANTHMD);
		}

	//Check if VRCompositor is present
	if (!vr::VRCompositor())
	{
		displayWin32ErrorMessage(L"Error: failed to init OpenVR VRCompositor",
								 L"Failed to initialize the VR Compositor");
		exit(ANN_ERR_NOTINIT);
	}

	//Get Driver and Display information
	strDriver = GetTrackedDeviceString(vrSystem, vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_TrackingSystemName_String);
	strDisplay = GetTrackedDeviceString(vrSystem, vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_SerialNumber_String);
	Annwvyn::AnnDebug() << "Driver : " << strDriver;
	Annwvyn::AnnDebug() << "Display : " << strDisplay;

}

void OgreOpenVRRender::initClientHmdRendering()
{
	setupDistrotion();
	//Should init the device model things here if we want to display the vive controllers 

	//Declare the textures for SteamVR
	vrTextures[left] = { (void*)rttTextureGLID[left], API, vr::ColorSpace_Gamma };
	vrTextures[right] = { (void*)rttTextureGLID[right], API, vr::ColorSpace_Gamma };

	//Set the OpenGL texture geometry
	GLBounds = {};
	GLBounds.uMin = 0;
	GLBounds.uMax = 1;
	GLBounds.vMin = 1;
	GLBounds.vMax = 0;

}

bool OgreOpenVRRender::shouldQuit()
{
	return shouldQuitState;
}

bool OgreOpenVRRender::shouldRecenter()
{
	return false; //Only usefull with the Oculus runtime
}

bool OgreOpenVRRender::isVisibleInHmd()
{
	return true; //Only usefull with the oculus runtime
}

void OgreOpenVRRender::updateTracking()
{
	processVREvents();

	//Get current camera base information
	feetPosition = headNode->getPosition();
	bodyOrientation = headNode->getOrientation();

	//Calculate update time
	then = now;
	now = getTimer()->getMilliseconds() / 1000.0;
	updateTime = now - then;

	//Wait for next frame pose 
	vr::VRCompositor()->WaitGetPoses(trackedPoses, vr::k_unMaxTrackedDeviceCount, nullptr, 0);

	//Process other tracked device. (Vive Controllers, maybe? :p)
	processTrackedPoses();

	//Here we just care about the HMD
	vr::TrackedDevicePose_t hmdPose;
	if ((hmdPose = trackedPoses[vr::k_unTrackedDeviceIndex_Hmd]).bPoseIsValid)
		hmdAbsoluteTransform = getMatrix4FromSteamVRMatrix34(hmdPose.mDeviceToAbsoluteTracking);

	//Update the monoscopic camera view
	monoCam->setPosition(feetPosition + Annwvyn::AnnGetPlayer()->getEyesHeight()*Ogre::Vector3::UNIT_Y + bodyOrientation* getTrackedHMDTranslation());
	monoCam->setOrientation(bodyOrientation * getTrackedHMDOrieation());

	//Update the eye rig tracking to make the eyes match yours
	eyeRig->setPosition(feetPosition + bodyOrientation * getTrackedHMDTranslation());
	eyeRig->setOrientation(bodyOrientation * getTrackedHMDOrieation());

	//Get the head reference back to the gameplay code 
	returnPose.position = eyeRig->getPosition();
	returnPose.orientation = eyeRig->getOrientation();

}

void OgreOpenVRRender::renderAndSubmitFrame()
{
	//Make Windows happy by pumping clearing it's event queue
	Ogre::WindowEventUtilities::messagePump();
	root->_fireFrameRenderingQueued();

	//Update each viewports
	rttViewports[left]->update();
	rttViewports[right]->update();
	windowViewport->update();
	window->update();

	//Submit the textures to the SteamVR compositor
	vr::VRCompositor()->Submit(getEye(left), &vrTextures[left], &GLBounds);
	vr::VRCompositor()->Submit(getEye(right), &vrTextures[right], &GLBounds);

	//OMG WE RENDERED A FRAME!!! QUICK!!!! INCREMENT THE COUNTER!!!!!!!
	frameCounter++;
}

void OgreOpenVRRender::recenter()
{
	if (vrSystem) vrSystem->ResetSeatedZeroPose();
}

void OgreOpenVRRender::changeViewportBackgroundColor(Ogre::ColourValue color)
{
	backgroundColor = color;
	for (char i(0); i < 2; i++) if (rttViewports[i])
		rttViewports[i]->setBackgroundColour(backgroundColor);
	if (windowViewport) windowViewport->setBackgroundColour(backgroundColor);
}

void OgreOpenVRRender::setCamerasNearClippingDistance(float distance)
{
	if (distance <= 0) return;
	nearClippingDistance = distance;
	getProjectionMatrix();
}

void OgreOpenVRRender::setCameraFarClippingDistance(float distance)
{
	if (distance <= 0) return;
	farClippingDistance = distance;
	getProjectionMatrix();
}

void OgreOpenVRRender::showDebug(DebugMode mode)
{
	return;
}

void OgreOpenVRRender::createWindow()
{
	if (!root) exit(ANN_ERR_NOTINIT);

	//Basic window configuration
	Ogre::NameValuePairList misc;
	misc["vsync"] = "false"; //This vsync parameter has no scence in VR. The display is done by the Compositor
	misc["top"] = "0";
	misc["left"] = "0";

	//manual ogre init
	root->initialise(false);

	window = root->createRenderWindow(name + " : Vive debug mirror view. Please put on HMD.",
									  windowWidth, windowHeight,
									  false, &misc);
}

void OgreOpenVRRender::initScene()
{
	//Create the scene manager for the engine
	if (!root) exit(ANN_ERR_NOTINIT);
	smgr = root->createSceneManager("OctreeSceneManager", "OSM_SMGR");
	smgr->setShadowTechnique(Ogre::ShadowTechnique::SHADOWTYPE_STENCIL_ADDITIVE);

	//Optional additional scenes here
}

void OgreOpenVRRender::initCameras()
{
	//VR Eye cameras
	eyeRig = smgr->getRootSceneNode()->createChildSceneNode();

	//Camera for  each eye
	eyeCameras[left] = smgr->createCamera("lcam");
	eyeCameras[left]->setAutoAspectRatio(true);
	eyeRig->attachObject(eyeCameras[left]);

	eyeCameras[right] = smgr->createCamera("rcam");
	eyeCameras[right]->setAutoAspectRatio(true);
	eyeRig->attachObject(eyeCameras[right]);

	//This will translate the cameras to put the correct IPD distance for the user
	handleIPDChange();

	//Monoscopic view camera, for non-VR display
	monoCam = smgr->createCamera("mcam");
	monoCam->setAspectRatio(16.0 / 9.0);
	monoCam->setAutoAspectRatio(false);
	monoCam->setPosition(feetPosition + Annwvyn::AnnGetPlayer()->getEyesHeight()*Ogre::Vector3::UNIT_Y);
	monoCam->setNearClipDistance(0.1);
	monoCam->setFarClipDistance(4000);
	monoCam->setFOVy(Ogre::Degree(90));

	//do NOT attach camera to this node...
	headNode = smgr->getRootSceneNode()->createChildSceneNode();
}

void OgreOpenVRRender::initRttRendering()
{
	//Get the render texture size recomended by the OpenVR API for the current Driver/Display
	unsigned int w, h;
	vrSystem->GetRecommendedRenderTargetSize(&w, &h);
	Annwvyn::AnnDebug() << "Recomended Render Target Size : " << w << "x" << h;

	//Create theses textures in OpenGL and get their OpenGL ID
	//
	//When the parent class *OgreVRRender* intialize Ogre, the OpenGL RenderSystem is loaded in hard.
	//We don't need to check that we're using OpenGL before doing this kind of cast:
	Ogre::GLTextureManager* textureManager = static_cast<Ogre::GLTextureManager*>(Ogre::TextureManager::getSingletonPtr());

	//Left eye texture
	rttTexture[left] = textureManager->createManual("RTT_TEX_L", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
													Ogre::TEX_TYPE_2D, w, h, 0, Ogre::PF_R8G8B8, Ogre::TU_RENDERTARGET, nullptr, gamma);
	rttTextureGLID[left] = static_cast<Ogre::GLTexture*>(textureManager->getByName("RTT_TEX_L").getPointer())->getGLID();

	//Right eye texture
	rttTexture[right] = textureManager->createManual("RTT_TEX_R", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
													 Ogre::TEX_TYPE_2D, w, h, 0, Ogre::PF_R8G8B8, Ogre::TU_RENDERTARGET, nullptr, gamma);
	rttTextureGLID[right] = static_cast<Ogre::GLTexture*>(textureManager->getByName("RTT_TEX_R").getPointer())->getGLID();

	//Create viewport for each cameras in each render texture
	rttViewports[left] = rttTexture[left]->getBuffer()->getRenderTarget()->addViewport(eyeCameras[left]);
	rttViewports[right] = rttTexture[right]->getBuffer()->getRenderTarget()->addViewport(eyeCameras[right]);

	//Do the same for the window 
	windowViewport = window->addViewport(monoCam);

	//Make sure the default viewport background color is set for everything
	changeViewportBackgroundColor(backgroundColor);
}

void OgreOpenVRRender::getProjectionMatrix()
{
	//Get the couple of matrices
	vr::HmdMatrix44_t prj[2]
		= { vrSystem->GetProjectionMatrix(getEye(left), nearClippingDistance, farClippingDistance, API),
		vrSystem->GetProjectionMatrix(getEye(right), nearClippingDistance, farClippingDistance, API) };

	//Apply them to the camera
	for (char eye(0); eye < 2; eye++)
	{
		//Need to convert them to Ogre's object
		Ogre::Matrix4 m;
		for (char i(0); i < 4; i++) for (char j(0); j < 4; j++)
			m[i][j] = prj[eye].m[i][j];

		eyeCameras[eye]->setCustomProjectionMatrix(true, m);
	}
}

void OgreOpenVRRender::setupDistrotion()
{
	//Actually there's nothing to do here :)
	return;
}

inline Ogre::Vector3 OgreOpenVRRender::getTrackedHMDTranslation()
{
	return hmdAbsoluteTransform.getTrans();
}

inline Ogre::Quaternion OgreOpenVRRender::getTrackedHMDOrieation()
{
	//Orientation/scale as quaternion (the matrix transfrom has no scale componant.
	return hmdAbsoluteTransform.extractQuaternion();
}

void OgreOpenVRRender::processVREvents()
{
	vr::VREvent_t event;
	while (vrSystem->PollNextEvent(&event, sizeof(event))) switch (event.eventType)
	{
		case vr::VREvent_DriverRequestedQuit:
		case vr::VREvent_Quit:
			shouldQuitState = true;
			break;
		case vr::VREvent_IpdChanged:
			handleIPDChange();
			break;
	}
}

void OgreOpenVRRender::processTrackedPoses()
{
	for (vr::TrackedDeviceIndex_t i(0); i < vr::k_unMaxTrackedDeviceCount; i++)
	{
		if (trackedPoses[i].bPoseIsValid)
		{
			//I think I need actual vive controllers to go further. And Annwvyn has no abstraction for hand controllers right now. 
		}
	}
}

void OgreOpenVRRender::handleIPDChange()
{
	//Ghe teh eyeToHeadTransform (they contain the IPD translation)
	Ogre::Matrix4 eyeToHeadTransform[2];
	for (char i(0); i < 2; i++)
	{
		eyeToHeadTransform[i] = getMatrix4FromSteamVRMatrix34(vrSystem->GetEyeToHeadTransform(getEye(oovrEyeType(i))));
		eyeCameras[i]->setPosition(eyeToHeadTransform[i].getTrans());
	}
}

inline Ogre::Matrix4 OgreOpenVRRender::getMatrix4FromSteamVRMatrix34(const vr::HmdMatrix34_t & mat)
{
	return Ogre::Matrix4
	{
		mat.m[0][0], mat.m[0][1], mat.m[0][2], mat.m[0][3],
		mat.m[1][0], mat.m[1][1], mat.m[1][2], mat.m[1][3],
		mat.m[2][0], mat.m[2][1], mat.m[2][2], mat.m[2][3],
		0.0f,		 0.0f,		  0.0f,		   1.0f
	};
}
