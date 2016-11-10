#include "stdafx.h"
#include "OgreOpenVRRender.hpp"
#include "AnnLogger.hpp"
#include "AnnEngine.hpp"

OgreOpenVRRender* OgreOpenVRRender::OpenVRSelf(nullptr);

OgreOpenVRRender::OgreOpenVRRender(std::string winName) : OgreVRRender(winName),
vrSystem(nullptr),
hmdError(vr::EVRInitError::VRInitError_None),
windowWidth(1280),
windowHeight(720),
gamma(false),
API(vr::API_OpenGL),
monoCam(nullptr),
windowViewport(nullptr),
then(0),
now(0),
hmdAbsoluteTransform({}),
eyeRig(nullptr),
shouldQuitState(false),
numberOfAxes{ 3 },
axoffset{ vr::k_EButton_Axis0 },
touchpadXNormalizedValue{ 0 },
touchpadYNormalizedValue{ 0 },
triggerNormalizedValue{ 0 }
{
	//Get the singleton pointer
	OpenVRSelf = static_cast<OgreOpenVRRender*>(self);

	//I like to initialize everything to zero
	rttTexture.setNull();
	rttTextureGLID = 0;

	//buttonsToHandle.push_back(vr::k_EButton_System);
	buttonsToHandle.push_back(vr::k_EButton_ApplicationMenu);
	buttonsToHandle.push_back(vr::k_EButton_Grip);
	buttonsToHandle.push_back(vr::k_EButton_A);
	buttonsToHandle.push_back(vr::k_EButton_SteamVR_Touchpad);
	buttonsToHandle.push_back(vr::k_EButton_SteamVR_Trigger);
	//buttonsToHandle.push_back(vr::k_EButton_DPad_Up);
	//buttonsToHandle.push_back(vr::k_EButton_DPad_Down);
	//buttonsToHandle.push_back(vr::k_EButton_DPad_Left);
	//buttonsToHandle.push_back(vr::k_EButton_DPad_Right);

	for (auto side : { left, right })
	{
		currentControllerButtonsPressed[side].resize(buttonsToHandle.size(), false);
		lastControllerButtonsPressed[side].resize(buttonsToHandle.size(), false);
		rttViewports[side] = nullptr;
		handControllers[side] = nullptr;
		vrTextures[side] = {};
		GLBounds[side] = {};
	}
}

OgreOpenVRRender::~OgreOpenVRRender()
{
	//Shutdown SteamVR
	vr::VR_Shutdown();

	//Need to forget Ogre's smart pointers
	rttTexture.setNull();

	//Destroy the main scene manager
	root->destroySceneManager(smgr);

	//Unload manually loaded plug-ins
	root->unloadPlugin("Plugin_OctreeSceneManager");

	rttTexture.setNull();

	//Destroy the root. Everything Ogre related that is remaining should be cleaned up by the root's destructor
	delete root;
}

void OgreOpenVRRender::initPipeline()
{
	getOgreConfig();
	createWindow();
	initScene();
	initCameras();
	updateProjectionMatrix();
	initRttRendering();
}

//This function is from VALVe
std::string GetTrackedDeviceString(vr::IVRSystem *pHmd, vr::TrackedDeviceIndex_t unDevice, vr::TrackedDeviceProperty prop, vr::TrackedPropertyError *peError = nullptr)
{
	auto unRequiredBufferLen = pHmd->GetStringTrackedDeviceProperty(unDevice, prop, nullptr, 0, peError);
	if (unRequiredBufferLen == 0)
		return "";

	auto pchBuffer = new char[unRequiredBufferLen];
	/*unRequiredBufferLen = */
	pHmd->GetStringTrackedDeviceProperty(unDevice, prop, pchBuffer, unRequiredBufferLen, peError);
	std::string sResult = pchBuffer;
	delete[] pchBuffer;
	return sResult;
}

void OgreOpenVRRender::initVrHmd()
{
	//Initialize OpenVR
	vrSystem = VR_Init(&hmdError, vr::EVRApplicationType::VRApplication_Scene);
	if (hmdError != vr::VRInitError_None) //Check for errors
		switch (hmdError)
		{
			default:
				displayWin32ErrorMessage(L"Error: failed OpenVR VR_Init",
										 L"Non described error when initializing the OpenVR Render object");
				exit(ANN_ERR_NOTINIT);

			case vr::VRInitError_Init_HmdNotFound:
			case vr::VRInitError_Init_HmdNotFoundPresenceFailed:
				displayWin32ErrorMessage(L"Error: cannot find HMD",
										 L"OpenVR cannot find HMD.\n"
										 L"Please install SteamVR and launch it, and verify HMD USB and HDMI connection");
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
	//Init GLEW here to be able to call OpenGL functions
	Annwvyn::AnnDebug() << "Init GL Extension Wrangler";
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		Annwvyn::AnnDebug("Failed to glewTnit()\n\
						  Cannot call manual OpenGL\n\
						  Error Code : " + static_cast<unsigned int>(err));
		exit(ANN_ERR_RENDER);
	}
	Annwvyn::AnnDebug() << "Using GLEW version : " << glewGetString(GLEW_VERSION);
	setupDistrotion();
	//Should init the device model things here if we want to display the vive controllers

	//Declare the textures for SteamVR
	vrTextures[left] = { reinterpret_cast<void*>(rttTextureGLID), API, vr::ColorSpace_Gamma };

	//Set the OpenGL texture geometry
	//V axis is reversed, U between 0 and 0.5 is for the left eye, between 0.5 and 1 is for the right eye.
	GLBounds[left].uMin = 0;
	GLBounds[left].uMax = 0.5f;
	GLBounds[left].vMin = 1;
	GLBounds[left].vMax = 0;

	GLBounds[right].uMin = 0.5f;
	GLBounds[right].uMax = 1;
	GLBounds[right].vMin = 1;
	GLBounds[right].vMax = 0;
}

bool OgreOpenVRRender::shouldQuit()
{
	return shouldQuitState;
}

bool OgreOpenVRRender::shouldRecenter()
{
	return false; //Only useful with the Oculus runtime
}

bool OgreOpenVRRender::isVisibleInHmd()
{
	return true; //Only useful with the oculus runtime
}

void OgreOpenVRRender::updateTracking()
{
	//Process the event from OpenVR
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
	processTrackedDevices();

	//Here we just care about the HMD
	vr::TrackedDevicePose_t hmdPose;
	if ((hmdPose = trackedPoses[vr::k_unTrackedDeviceIndex_Hmd]).bPoseIsValid)
		hmdAbsoluteTransform = getMatrix4FromSteamVRMatrix34(hmdPose.mDeviceToAbsoluteTracking);

	//Update the monoscopic camera view
	monoCam->setPosition(feetPosition
						 + Annwvyn::AnnGetPlayer()->getEyeTranslation()
						 + bodyOrientation * getTrackedHMDTranslation());
	monoCam->setOrientation(bodyOrientation * getTrackedHMDOrieation());

	//Update the eye rig tracking to make the eyes match your
	eyeRig->setPosition(feetPosition
						+ bodyOrientation * getTrackedHMDTranslation());
	eyeRig->setOrientation(bodyOrientation * getTrackedHMDOrieation());

	//Get the head reference back to the gameplay code
	returnPose.position = eyeRig->getPosition();
	returnPose.orientation = eyeRig->getOrientation();
}

void OgreOpenVRRender::renderAndSubmitFrame()
{
	//Make Windows happy by pumping clearing it's event queue
	Ogre::WindowEventUtilities::messagePump();

	//Mark the fact that the frame rendering will happen to Ogre (unlock the animation state updates for example)
	root->_fireFrameRenderingQueued();

	//Update each viewports
	rttViewports[0]->update();
	rttViewports[1]->update();
	rttEyes->update();
	windowViewport->update();
	window->update();

	//Submit the textures to the SteamVR compositor

	vr::VRCompositor()->Submit(vr::Eye_Left, &vrTextures[0], &GLBounds[0]);
	vr::VRCompositor()->Submit(vr::Eye_Right, &vrTextures[0], &GLBounds[1]);

	//OMG WE RENDERED A FRAME!!! QUICK!!!! INCREMENT THE COUNTER!!!!!!!
	frameCounter++;
}

void OgreOpenVRRender::recenter()
{
	//This should do. But the effect of that commend doesn't look to affect a "seated" pose if you're actually standing
	if (vrSystem) vrSystem->ResetSeatedZeroPose();
}

void OgreOpenVRRender::changeViewportBackgroundColor(Ogre::ColourValue color)
{
	backgroundColor = color;
	//Eye camera viewports
	for (char i(0); i < 2; i++) if (rttViewports[i])
		rttViewports[i]->setBackgroundColour(backgroundColor);

	//Debug window viewports
	if (windowViewport) windowViewport->setBackgroundColour(backgroundColor);
}

void OgreOpenVRRender::showDebug(DebugMode mode)
{
	return;
}

void OgreOpenVRRender::createWindow()
{
	//Need to have the root created before
	if (!root) exit(ANN_ERR_NOTINIT);

	//Basic window configuration
	Ogre::NameValuePairList misc;
	misc["vsync"] = "false"; //This vsync parameter has no scene in VR. The display is done by the Compositor
	misc["top"] = "0";
	misc["left"] = "0";

	//Manual ogre init
	root->initialise(false);

	//Create a manual window
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
	monoCam->setPosition(feetPosition + Annwvyn::AnnGetPlayer()->getEyeTranslation());
	monoCam->setNearClipDistance(nearClippingDistance);
	monoCam->setFarClipDistance(farClippingDistance);
	monoCam->setFOVy(Ogre::Degree(90));

	//do NOT attach camera to this node...
	headNode = smgr->getRootSceneNode()->createChildSceneNode();
}

void OgreOpenVRRender::initRttRendering()
{
	//Get the render texture size recommended by the OpenVR API for the current Driver/Display
	unsigned int w, h;
	vrSystem->GetRecommendedRenderTargetSize(&w, &h);
	w *= 2;

	if (UseSSAA)
	{
		if (AALevel / 2 > 0)
		{
			w *= AALevel / 2;
			h *= AALevel / 2;
			AALevel = 0;
		}
	}

	Annwvyn::AnnDebug() << "Recommended Render Target Size : " << w << "x" << h;

	//Create theses textures in OpenGL and get their OpenGL ID
	//
	//When the parent class *OgreVRRender* initialize Ogre, the OpenGL RenderSystem is loaded in hard.
	//We don't need to check that we're using OpenGL before doing this kind of cast:
	auto textureManager = static_cast<Ogre::GLTextureManager*>(Ogre::TextureManager::getSingletonPtr());

	//shared texture
	rttTexture = textureManager->createManual("RTT_TEX", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
											  Ogre::TEX_TYPE_2D, w, h, 0, Ogre::PF_R8G8B8, Ogre::TU_RENDERTARGET, nullptr, gamma, AALevel);
	rttTextureGLID = static_cast<Ogre::GLTexture*>(textureManager->getByName("RTT_TEX").getPointer())->getGLID();
	rttEyes = rttTexture->getBuffer()->getRenderTarget();

	//Create viewport for each cameras in each render texture
	rttViewports[left] = rttTexture->getBuffer()->getRenderTarget()->addViewport(eyeCameras[left], 0, 0, 0, 0.5f, 1);
	rttViewports[right] = rttTexture->getBuffer()->getRenderTarget()->addViewport(eyeCameras[right], 1, 0.5f, 0, 0.5f, 1);

	//Do the same for the window
	windowViewport = window->addViewport(monoCam);

	//Make sure the default viewport background color is set for everything
	changeViewportBackgroundColor(backgroundColor);
}

void OgreOpenVRRender::updateProjectionMatrix()
{
	//Get the couple of matrices
	std::array<vr::HmdMatrix44_t, 2> openVRProjectionMatrix
	{
		vrSystem->GetProjectionMatrix(getEye(left), nearClippingDistance, farClippingDistance, API),
		vrSystem->GetProjectionMatrix(getEye(right), nearClippingDistance, farClippingDistance, API)
	};

	std::array<Ogre::Matrix4, 2> ogreProjectionMatrix{};

	//Apply them to the camera
	for (auto eye : { left, right })
	{
		//Need to convert them to Ogre's object
		for (auto x : { 0, 1, 2, 3 }) for (auto y : { 0, 1, 2, 3 })
			ogreProjectionMatrix[eye][x][y] = openVRProjectionMatrix[eye].m[x][y];

		//Apply projection matrix
		eyeCameras[eye]->setCustomProjectionMatrix(true, ogreProjectionMatrix[eye]);
	}
}

inline vr::EVREye OgreOpenVRRender::getEye(oovrEyeType eye)
{
	if (eye == left) return vr::Eye_Left;
	return vr::Eye_Right;
}

void OgreOpenVRRender::setupDistrotion()
{
	//Actually there's nothing to do here :)
	return;
}

inline Ogre::Vector3 OgreOpenVRRender::getTrackedHMDTranslation()
{
	//Extract translation vector from the matrix
	return hmdAbsoluteTransform.getTrans();
}

inline Ogre::Quaternion OgreOpenVRRender::getTrackedHMDOrieation()
{
	//Orientation/scale as quaternion (the matrix transform has no scale component.
	return hmdAbsoluteTransform.extractQuaternion();
}

void OgreOpenVRRender::processVREvents()
{
	vr::VREvent_t event;
	//Pump the events, and for each event, switch on it type
	while (vrSystem->PollNextEvent(&event, sizeof(event))) switch (event.eventType)
	{
		//Handle quiting the app from Steam
		case vr::VREvent_DriverRequestedQuit:
		case vr::VREvent_Quit:
			shouldQuitState = true;
			break;

		//Handle user IPD adjustment
		case vr::VREvent_IpdChanged:
			handleIPDChange();
			break;
		default: break;
	}
}

const bool DEBUG(true);

void OgreOpenVRRender::processController(vr::TrackedDeviceIndex_t controllerDeviceIndex, Annwvyn::AnnHandController::AnnHandControllerSide side)
{
	//Extract tracking information from the device
	auto transform = getMatrix4FromSteamVRMatrix34(trackedPoses[controllerDeviceIndex].mDeviceToAbsoluteTracking);

	//Extract the pose from the transformation matrix
	auto position = transform.getTrans();
	auto orientation = transform.extractQuaternion();

	//Get the state of the controller. The state contains the buttons and triggers data at the last sample
	vrSystem->GetControllerState(controllerDeviceIndex, &controllerState);

	//This will fill the buttons array
	extractButtons(side);

	//Extract axis values
	touchpadXNormalizedValue = controllerState.rAxis[vr::EVRButtonId::k_EButton_SteamVR_Touchpad - axoffset].x;
	touchpadYNormalizedValue = controllerState.rAxis[vr::EVRButtonId::k_EButton_SteamVR_Touchpad - axoffset].y;
	triggerNormalizedValue = controllerState.rAxis[vr::EVRButtonId::k_EButton_SteamVR_Trigger - axoffset].x;

	//create or update controllers object
	//Dynamically allocate the controller if the controller doesn't exist yet
	if (!handControllers[side])
	{
		handControllers[side] = std::make_shared<Annwvyn::AnnHandController>
			("OpenVR Hand Controller", smgr->getRootSceneNode()->createChildSceneNode(), size_t(controllerDeviceIndex), side);

		if (DEBUG) handControllers[side]->attachModel(smgr->createEntity("gizmo.mesh"));
	}

	auto handController = handControllers[side];
	auto& axesVector = handController->getAxesVector();
	//Axis map:
	//0 -> Touchpad X
	//1 -> Touchpad Y
	//2 -> AnalogTrigger

	if (axesVector.size() == 0)
	{
		axesVector.push_back(Annwvyn::AnnHandControllerAxis{ "Touchpad X", touchpadXNormalizedValue });
		axesVector.push_back(Annwvyn::AnnHandControllerAxis{ "Touchpad Y", touchpadYNormalizedValue });
		axesVector.push_back(Annwvyn::AnnHandControllerAxis{ "Trigger X", triggerNormalizedValue });
	}

	axesVector[0].updateValue(touchpadXNormalizedValue);
	axesVector[1].updateValue(touchpadYNormalizedValue);
	axesVector[2].updateValue(triggerNormalizedValue);

	//Assign the buttons
	handController->getButtonStateVector() = currentControllerButtonsPressed[side];
	handController->getPressedButtonsVector() = pressed;
	handController->getReleasedButtonsVector() = released;
	handController->setTrackedPosition(feetPosition + bodyOrientation * position);
	handController->setTrackedOrientation(bodyOrientation * orientation);
	handController->setTrackedLinearSpeed(Annwvyn::AnnVect3(trackedPoses[controllerDeviceIndex].vVelocity.v));
	handController->setTrackedAngularSpeed(Annwvyn::AnnVect3(trackedPoses[controllerDeviceIndex].vAngularVelocity.v));
}

void OgreOpenVRRender::extractButtons(size_t side)
{
	pressed.clear(); released.clear();
	for (uint8_t i(0); i < buttonsToHandle.size(); i++)
	{
		lastControllerButtonsPressed[side][i] = currentControllerButtonsPressed[side][i];
		currentControllerButtonsPressed[side][i] = (controllerState.ulButtonPressed & ButtonMaskFromId(buttonsToHandle[i])) != 0;
		if (currentControllerButtonsPressed[side][i] && !lastControllerButtonsPressed[side][i])
		{
			pressed.push_back(i);
			continue;
		}
		else if (!currentControllerButtonsPressed[side][i] && lastControllerButtonsPressed[side][i])
			released.push_back(i);
	}
}

void OgreOpenVRRender::processTrackedDevices()
{
	//Iterate through the possible trackedDeviceIndexes
	for (auto trackedDevice = vr::k_unTrackedDeviceIndex_Hmd + 1;
		 trackedDevice < vr::k_unMaxTrackedDeviceCount;
		 trackedDevice++)
	{
		//If the device is not connected, pass.
		//If the device is not recognized as a controller, pass
		//If we don't have a valid pose of the controller, pass
		if (!vrSystem->IsTrackedDeviceConnected(trackedDevice)
			|| vrSystem->GetTrackedDeviceClass(trackedDevice) != vr::TrackedDeviceClass_Controller
			|| !trackedPoses[trackedDevice].bPoseIsValid)
			continue;

		//From here we know that "trackedDevice" is the device index of a valid controller that has been tracked by the system
		//Extract basic information of the device, detect if they are left/right hands
		Annwvyn::AnnHandController::AnnHandControllerSide side;
		switch (vrSystem->GetControllerRoleForTrackedDeviceIndex(trackedDevice))
		{
			case vr::ETrackedControllerRole::TrackedControllerRole_LeftHand:
				side = Annwvyn::AnnHandController::leftHandController;
				break;

			case vr::ETrackedControllerRole::TrackedControllerRole_RightHand:
				side = Annwvyn::AnnHandController::rightHandController;
				break;

			case vr::ETrackedControllerRole::TrackedControllerRole_Invalid:
			default:
				continue;
		}

		processController(trackedDevice, side);
	}
}

void OgreOpenVRRender::handleIPDChange()
{
	//Get the eyeToHeadTransform (they contain the IPD translation)
	for (char i(0); i < 2; i++)
		eyeCameras[i]->setPosition(getMatrix4FromSteamVRMatrix34(
			vrSystem->GetEyeToHeadTransform(getEye(oovrEyeType(i)))).getTrans());
}

inline Ogre::Matrix4 OgreOpenVRRender::getMatrix4FromSteamVRMatrix34(const vr::HmdMatrix34_t& mat)
{
	return Ogre::Matrix4
	{
		mat.m[0][0],	mat.m[0][1],	mat.m[0][2],	mat.m[0][3],
		mat.m[1][0],	mat.m[1][1],	mat.m[1][2],	mat.m[1][3],
		mat.m[2][0],	mat.m[2][1],	mat.m[2][2],	mat.m[2][3],
		0.0f,			0.0f,			0.0f,			1.0f
	};
}