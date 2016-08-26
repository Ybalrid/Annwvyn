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
API(vr::API_OpenGL)
{
	OpenVRSelf = static_cast<OgreOpenVRRender*>(self);
}

OgreOpenVRRender::~OgreOpenVRRender()
{
	vr::VR_Shutdown();
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
	vrSystem = vr::VR_Init(&hmdError, vr::EVRApplicationType::VRApplication_Scene);
	if (hmdError != vr::VRInitError_None)
	{
		switch (hmdError)
		{
			default:
				displayWin32ErrorMessage(L"Error: failed OpenVR VR_Init",
										 L"Undescribed error when initalizing the OpenVR Render object");
				exit(ANN_ERR_NOTINIT);
			case vr::VRInitError_Init_HmdNotFound:
			case vr::VRInitError_Init_HmdNotFoundPresenceFailed:
				displayWin32ErrorMessage(L"Error: cannot find HMD",
										 L"OpenVR cannot find HMD.\n"
										 L"Please install SteamVR and launchj it, and verrify HMD USB and HDMI connection");
				exit(ANN_ERR_CANTHMD);
		}
	}

	if (!vr::VRCompositor())
	{
		displayWin32ErrorMessage(L"Error: failed to init OpenVR VRCompositor",
								 L"Failed to initialize the VR Compositor");
		exit(ANN_ERR_NOTINIT);
	}

	strDriver = GetTrackedDeviceString(vrSystem, vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_TrackingSystemName_String);
	strDisplay = GetTrackedDeviceString(vrSystem, vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_SerialNumber_String);

	Annwvyn::AnnDebug() << "Driver : " << strDriver;
	Annwvyn::AnnDebug() << "Display : " << strDisplay;

}

void OgreOpenVRRender::initClientHmdRendering()
{
	setupDistrotion();

}

bool OgreOpenVRRender::shouldQuit()
{
	return false;
}

bool OgreOpenVRRender::shouldRecenter()
{
	return false;
}

bool OgreOpenVRRender::isVisibleInHmd()
{
	return true;
}

void OgreOpenVRRender::updateTracking()
{
	//Get current camera base information
	headPosition = headNode->getPosition();
	headOrientation = headNode->getOrientation();


	/*vr::VRCompositor()->GetFrameTiming(&frameTiming, 0);*/
	then = now;
	now = getTimer()->getMilliseconds()/1000.0;
	updateTime = now - then;

	vr::VRCompositor()->WaitGetPoses(trackedPoses, vr::k_unMaxTrackedDeviceCount, nullptr, 0);

	//TODO: insert other device tracking here. We're only interested in the HMD itself right now
	vr::TrackedDevicePose_t hmdPose;
	if ((hmdPose =  trackedPoses[vr::k_unTrackedDeviceIndex_Hmd]).bPoseIsValid)
		hmdAbsoluteTransform = getMatrix4FromSteamVRMatrix34(hmdPose.mDeviceToAbsoluteTracking);

	monoCam->setPosition(headPosition + getTrackedHMDTranslation());
	monoCam->setOrientation(headOrientation * getTrackedHMDOrieation());

	eyeRig->setPosition(headPosition + getTrackedHMDTranslation() - Ogre::Vector3(0, Annwvyn::AnnGetPlayer()->getEyesHeight(), 0));
	eyeRig->setOrientation(headOrientation * getTrackedHMDOrieation());
}

void OgreOpenVRRender::renderAndSubmitFrame()
{
	Ogre::WindowEventUtilities::messagePump();
	root->_fireFrameRenderingQueued();

	rttViewports[left]->update();
	rttViewports[right]->update();
	windowViewport->update();
	window->update();

	vrTextures[left] = { (void*)rttTextureGLID[left], API, vr::ColorSpace_Gamma };
	vrTextures[right] = { (void*)rttTextureGLID[right], API, vr::ColorSpace_Gamma };

	vr::VRCompositor()->Submit(getEye(left), &vrTextures[left], &GLBounds);
	vr::VRCompositor()->Submit(getEye(right), &vrTextures[right], &GLBounds);
	frameCounter++;
}

void OgreOpenVRRender::recenter()
{
	if (vrSystem) vrSystem->ResetSeatedZeroPose();
}

void OgreOpenVRRender::changeViewportBackgroundColor(Ogre::ColourValue color)
{
	backgroundColor = color;
	for (char i(0); i < 2; i++)
		rttViewports[i]->setBackgroundColour(backgroundColor);
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
}

void OgreOpenVRRender::initCameras()
{
	Ogre::Matrix4 eyeToHeadTransform[2];
	for (char i(0); i < 2; i++)
		eyeToHeadTransform[i] = getMatrix4FromSteamVRMatrix34(vrSystem->GetEyeToHeadTransform(getEye(oovrEyeType(i))));

	//VR Eye cameras
	eyeRig = smgr->getRootSceneNode()->createChildSceneNode();
	eyeCameras[left] = smgr->createCamera("lcam");
	eyeCameras[left]->setAutoAspectRatio(true);
	eyeRig->attachObject(eyeCameras[left]);
	eyeCameras[left]->setPosition(eyeToHeadTransform[left].getTrans());

	eyeCameras[right] = smgr->createCamera("rcam");
	eyeCameras[right]->setAutoAspectRatio(true);
	eyeRig->attachObject(eyeCameras[right]);
	eyeCameras[right]->setPosition(eyeToHeadTransform[right].getTrans());

	Annwvyn::AnnDebug() << "eyeLeftTranformTrans " << eyeToHeadTransform[left].getTrans();
	Annwvyn::AnnDebug() << "eyeRightTranformTrans " << eyeToHeadTransform[right].getTrans();

	monoCam = smgr->createCamera("mcam");
	monoCam->setAspectRatio(16.0 / 9.0);
	monoCam->setAutoAspectRatio(false);
	monoCam->setPosition(headPosition);
	monoCam->setNearClipDistance(0.1);
	monoCam->setFarClipDistance(4000);
	monoCam->setFOVy(Ogre::Degree(90));

	//do NOT attach camera to this node...
	headNode = smgr->getRootSceneNode()->createChildSceneNode();

}

void OgreOpenVRRender::initRttRendering()
{
	//Init GLEW here to be able to call OpenGL functions
	Annwvyn::AnnDebug() << "Init GL Extension Wrangler";
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		Annwvyn::AnnDebug("Failed to glewTnit()\n\
						  Cannot call manual OpenGL\n\
						  Error Code : " + (unsigned int)err);
		exit(ANN_ERR_RENDER);
	}
	Annwvyn::AnnDebug() << "Using GLEW version : " << glewGetString(GLEW_VERSION);

	unsigned int w, h;
	vrSystem->GetRecommendedRenderTargetSize(&w, &h);
	Annwvyn::AnnDebug() << "Recomended Render Target Size : " << w << "x" << h;

	//When OgreVRRender() intialize Ogre, the OpenGL RenderSystem is loaded in hard.
	//We don't need to check that we're using OpenGL before doing this kind of cast:
	Ogre::GLTextureManager* textureManager = static_cast<Ogre::GLTextureManager*>(Ogre::TextureManager::getSingletonPtr());

	rttTexture[left] = textureManager->createManual("RTT_TEX_L", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
													Ogre::TEX_TYPE_2D, w, h, 0, Ogre::PF_R8G8B8, Ogre::TU_RENDERTARGET, nullptr, gamma);
	rttTextureGLID[left] = static_cast<Ogre::GLTexture*>(textureManager->getByName("RTT_TEX_L").getPointer())->getGLID();

	rttTexture[right] = textureManager->createManual("RTT_TEX_R", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
													 Ogre::TEX_TYPE_2D, w, h, 0, Ogre::PF_R8G8B8, Ogre::TU_RENDERTARGET, nullptr, gamma);
	rttTextureGLID[right] = static_cast<Ogre::GLTexture*>(textureManager->getByName("RTT_TEX_R").getPointer())->getGLID();


	rttViewports[left] = rttTexture[left]->getBuffer()->getRenderTarget()->addViewport(eyeCameras[left]);
	rttViewports[right] = rttTexture[right]->getBuffer()->getRenderTarget()->addViewport(eyeCameras[right]);
	
	windowViewport = window->addViewport(monoCam);
	windowViewport->setBackgroundColour(Ogre::ColourValue(1, 0, 1, 1));

	GLBounds = {};
	GLBounds.uMin = 0;
	GLBounds.uMax = 1;
	GLBounds.vMin = 1;
	GLBounds.vMax = 0;

}

void OgreOpenVRRender::getProjectionMatrix()
{
	vr::HmdMatrix44_t prj[2]
		= { vrSystem->GetProjectionMatrix(getEye(left), nearClippingDistance, farClippingDistance, API),
		vrSystem->GetProjectionMatrix(getEye(right), nearClippingDistance, farClippingDistance, API) };

	for (char eye(0); eye < 2; eye++)
	{
		Ogre::Matrix4 m;
		for (char i(0); i < 4; i++) for (char j(0); j < 4; j++)
			m[i][j] = prj[eye].m[i][j];

		eyeCameras[eye]->setCustomProjectionMatrix(true, m);
	}
}

void OgreOpenVRRender::setupDistrotion()
{
	//distortionScene = root->createSceneManager(Ogre::ST_GENERIC, "DistortionScene");
	
	//Actually there's nothing to do here :

}

inline Ogre::Vector3 OgreOpenVRRender::getTrackedHMDTranslation()
{	
	return hmdAbsoluteTransform.getTrans();
}

inline Ogre::Quaternion OgreOpenVRRender::getTrackedHMDOrieation()
{
	return hmdAbsoluteTransform.extractQuaternion();
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
/*
Ogre::Real OgreOpenVRRender::getIPD()
{
	
	//TODO get Real IPD
	return 0.063f;
}
*/