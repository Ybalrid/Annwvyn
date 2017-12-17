// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "stdafx.h"
#include "AnnOgreOpenVRRenderer.hpp"
#include "AnnLogger.hpp"
#include "AnnEngine.hpp"
#include "AnnGetter.hpp"
#include "Annwvyn.h"

using namespace Annwvyn;

AnnOgreOpenVRRenderer* AnnOgreOpenVRRenderer::OpenVRSelf(nullptr);

AnnOgreOpenVRRenderer::AnnOgreOpenVRRenderer(std::string winName) : AnnOgreVRRenderer(winName),
vrSystem{ nullptr },
hmdError{ vr::EVRInitError::VRInitError_None },
windowWidth{ 1280 },
windowHeight{ 720 },
rttTextureGLID{ 0 },
rttLeftTextureGLID{ 0 },
rttRightTextureGLID{ 0 },
gamma{ false },
TextureType{ vr::TextureType_OpenGL },
vrTextures{ nullptr },
hmdAbsoluteTransform{ Ogre::Matrix4::IDENTITY },
shouldQuitState{ false },
numberOfAxes{ 3 },
axoffset{ vr::k_EButton_Axis0 },
touchpadXNormalizedValue{ 0 },
touchpadYNormalizedValue{ 0 },
triggerNormalizedValue{ 0 }
{
	rendererName = "OpengGL/OpenVR";
	//Get the singleton pointer
	OpenVRSelf = static_cast<AnnOgreOpenVRRenderer*>(self);

	buttonsToHandle.reserve(5);
	buttonsToHandle.push_back(vr::k_EButton_ApplicationMenu);
	buttonsToHandle.push_back(vr::k_EButton_Grip);
	buttonsToHandle.push_back(vr::k_EButton_A);
	buttonsToHandle.push_back(vr::k_EButton_SteamVR_Touchpad);
	buttonsToHandle.push_back(vr::k_EButton_SteamVR_Trigger);

	for (auto side : { left, right })
	{
		currentControllerButtonsPressed[side].resize(buttonsToHandle.size(), false);
		lastControllerButtonsPressed[side].resize(buttonsToHandle.size(), false);
		handControllers[side] = nullptr;
		GLBounds[side] = {};
	}
}

AnnOgreOpenVRRenderer::~AnnOgreOpenVRRenderer()
{
	//Shutdown SteamVR
	vr::VR_Shutdown();

	OpenVRSelf = nullptr;
}

//This function is taken straight from VALVe's example.
std::string GetTrackedDeviceString(vr::IVRSystem *pHmd, vr::TrackedDeviceIndex_t unDevice, vr::TrackedDeviceProperty prop, vr::TrackedPropertyError *peError = nullptr)
{
	auto unRequiredBufferLen = pHmd->GetStringTrackedDeviceProperty(unDevice, prop, nullptr, 0, peError);
	if (unRequiredBufferLen == 0)
		return "";

	std::vector<char> pchBuffer(unRequiredBufferLen);
	pHmd->GetStringTrackedDeviceProperty(unDevice, prop, pchBuffer.data(), unRequiredBufferLen, peError);
	std::string sResult = pchBuffer.data();
	return sResult;
}

void AnnOgreOpenVRRenderer::initVrHmd()
{
	//Initialize OpenVR
	vrSystem = VR_Init(&hmdError, vr::EVRApplicationType::VRApplication_Scene);
	if (hmdError != vr::VRInitError_None) //Check for errors
		switch (hmdError)
		{
		default:
			displayWin32ErrorMessage("Error: failed OpenVR VR_Init",
				"Non described error when initializing the OpenVR Render object");
			throw AnnInitializationError(ANN_ERR_NOTINIT, "Unknown error while initializing OpenVR");

		case vr::VRInitError_Init_HmdNotFound:
		case vr::VRInitError_Init_HmdNotFoundPresenceFailed:
			displayWin32ErrorMessage("Error: cannot find HMD",
				"OpenVR cannot find HMD.\n"
				"Please install SteamVR and launch it, and verify HMD USB and HDMI connection");
			throw AnnInitializationError(ANN_ERR_CANTHMD, "OpenVR can't find an HMD");
		}

	//Check if VRCompositor is present
	if (!vr::VRCompositor())
	{
		displayWin32ErrorMessage("Error: failed to init OpenVR VRCompositor",
			"Failed to initialize the VR Compositor");
		throw AnnInitializationError(ANN_ERR_NOTINIT, "Failed to init the OpenVR VRCompositor");
	}

	//Get Driver and Display information
	strDriver = GetTrackedDeviceString(vrSystem, vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_TrackingSystemName_String);
	strDisplay = GetTrackedDeviceString(vrSystem, vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_SerialNumber_String);
	AnnDebug() << "Driver : " << strDriver;
	AnnDebug() << "Display : " << strDisplay;
}

void AnnOgreOpenVRRenderer::initClientHmdRendering()
{
	loadOpenGLFunctions();

	//here the sample inits the device model things here if we want to display the vive controllers.
	//we don't really want to display Vive wands or whatever controller model SteamVR is using, but in-game stuff.
	//To display a 3D model in place of the hands of the character, it's done via the AnnHandController objects

	//Declare the textures for SteamVR
	vrTextures = { {
		{reinterpret_cast<void*>(rttLeftTextureGLID), TextureType, vr::ColorSpace_Gamma },
		{reinterpret_cast<void*>(rttRightTextureGLID), TextureType, vr::ColorSpace_Gamma }
	} };

	//Set the OpenGL texture geometry
	//V axis is reversed
	GLBounds[left].uMin = 0;
	GLBounds[left].uMax = 1;
	GLBounds[left].vMin = 1;
	GLBounds[left].vMax = 0;

	GLBounds[right].uMin = 0;
	GLBounds[right].uMax = 1;
	GLBounds[right].vMin = 1;
	GLBounds[right].vMax = 0;
}

bool AnnOgreOpenVRRenderer::shouldQuit()
{
	return shouldQuitState;
}

bool AnnOgreOpenVRRenderer::shouldRecenter()
{
	return false; //Only useful with the Oculus runtime
}

bool AnnOgreOpenVRRenderer::isVisibleInHmd()
{
	return true; //Only useful with the oculus runtime
}

void AnnOgreOpenVRRenderer::getTrackingPoseAndVRTiming()
{
	//Wait for next frame pose, get timing and process additional devices (controllers, anything)
	vr::VRCompositor()->WaitGetPoses(trackedPoses, vr::k_unMaxTrackedDeviceCount, nullptr, 0);
	calculateTimingFromOgre();

	//Here the controllers are handled
	processTrackedDevices();
	processVREvents();

	//Here we just care about the HMD
	vr::TrackedDevicePose_t hmdPose;
	if ((hmdPose = trackedPoses[vr::k_unTrackedDeviceIndex_Hmd]).bPoseIsValid)
		hmdAbsoluteTransform = getMatrix4FromSteamVRMatrix34(hmdPose.mDeviceToAbsoluteTracking);

	//Apply tracking
	handleIPDChange();
	trackedHeadPose.position = feetPosition + bodyOrientation * getTrackedHMDTranslation();
	trackedHeadPose.orientation = bodyOrientation * getTrackedHMDOrieation();
}

void AnnOgreOpenVRRenderer::renderAndSubmitFrame()
{
	handleWindowMessages();

	root->renderOneFrame();

	//Submit the textures to the SteamVR compositor
	vr::VRCompositor()->Submit(vr::Eye_Left, &vrTextures[0], &GLBounds[0]);
	vr::VRCompositor()->Submit(vr::Eye_Right, &vrTextures[1], &GLBounds[1]);

	//OMG WE RENDERED A FRAME!!! QUICK!!!! INCREMENT THE COUNTER!!!!!!!
	frameCounter++;
}

void AnnOgreOpenVRRenderer::recenter()
{
	//This should do. But the effect of that commend doesn't look to affect a "seated" pose if you're actually standing
	if (vrSystem) vrSystem->ResetSeatedZeroPose();
}

void AnnOgreOpenVRRenderer::showDebug(DebugMode mode)
{}

void AnnOgreOpenVRRenderer::initScene()
{
	createMainSmgr();
}

void AnnOgreOpenVRRenderer::initRttRendering()
{
	//Get the render texture size recommended by the OpenVR API for the current Driver/Display
	unsigned int w, h;
	vrSystem->GetRecommendedRenderTargetSize(&w, &h);
	//w *= 2;

	if (UseSSAA)
	{
		if (AALevel / 2 > 0)
		{
			w *= AALevel / 2;
			h *= AALevel / 2;
			AALevel = 0;
		}
	}

	AnnDebug() << "Recommended Render Target Size : " << w << "x" << h;

	//Create the render texture
	//rttTextureGLID = createCombinedRenderTexture(w, h);
	auto glids = createSeparatedRenderTextures({ { {w, h}, {w, h} } });
	rttLeftTextureGLID = glids[0];
	rttRightTextureGLID = glids[1];

	auto compositor = root->getCompositorManager2();
	compositorWorkspaces[leftEyeCompositor] = compositor->addWorkspace(smgr, rttEyeSeparated[left], eyeCameras[left], "HdrWorkspace", true);
	compositorWorkspaces[rightEyeCompositor] = compositor->addWorkspace(smgr, rttEyeSeparated[right], eyeCameras[right], "HdrWorkspace", true);
	compositorWorkspaces[monoCompositor] = compositor->addWorkspace(smgr, window, monoCam, "HdrWorkspace", true);
}

void AnnOgreOpenVRRenderer::updateProjectionMatrix()
{
	//Get the couple of matrices
	std::array<vr::HmdMatrix44_t, 2> openVRProjectionMatrix
	{
		vrSystem->GetProjectionMatrix(getEye(left), nearClippingDistance, farClippingDistance),
		vrSystem->GetProjectionMatrix(getEye(right), nearClippingDistance, farClippingDistance)
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

inline vr::EVREye AnnOgreOpenVRRenderer::getEye(oovrEyeType eye)
{
	if (eye == left) return vr::Eye_Left;
	return vr::Eye_Right;
}

inline Ogre::Vector3 AnnOgreOpenVRRenderer::getTrackedHMDTranslation() const
{
	//Extract translation vector from the matrix
	return hmdAbsoluteTransform.getTrans();
}

inline Ogre::Quaternion AnnOgreOpenVRRenderer::getTrackedHMDOrieation() const
{
	//Orientation/scale as quaternion (the matrix transform has no scale component.
	return hmdAbsoluteTransform.extractQuaternion();
}

void AnnOgreOpenVRRenderer::processVREvents()
{
	vr::VREvent_t event;
	//Pump the events, and for each event, switch on it type
	while (vrSystem->PollNextEvent(&event, sizeof event)) switch (event.eventType)
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

void AnnOgreOpenVRRenderer::processController(vr::TrackedDeviceIndex_t controllerDeviceIndex, AnnHandController::AnnHandControllerSide side)
{
	//Extract tracking information from the device
	auto transform = getMatrix4FromSteamVRMatrix34(trackedPoses[controllerDeviceIndex].mDeviceToAbsoluteTracking);

	//Extract the pose from the transformation matrix
	const auto position = transform.getTrans();
	const auto orientation = transform.extractQuaternion();

	//Get the state of the controller. The state contains the buttons and triggers data at the last sample
	vrSystem->GetControllerState(controllerDeviceIndex, &controllerState, sizeof controllerState);

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
		handControllers[side] = std::make_shared<AnnOpenVRMotionController>
			(vrSystem, controllerDeviceIndex, smgr->getRootSceneNode()->createChildSceneNode(), size_t(controllerDeviceIndex), side);
	}

	auto handController = handControllers[side];
	auto& axesVector = handController->getAxesVector();
	//Axis map:
	//0 -> Touchpad X
	//1 -> Touchpad Y
	//2 -> AnalogTrigger

	if (axesVector.size() == 0)
	{
		axesVector.push_back(AnnHandControllerAxis{ "Touchpad X", touchpadXNormalizedValue });
		axesVector.push_back(AnnHandControllerAxis{ "Touchpad Y", touchpadYNormalizedValue });
		axesVector.push_back(AnnHandControllerAxis{ "Trigger X", triggerNormalizedValue });
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
	handController->setTrackedLinearSpeed(AnnVect3(trackedPoses[controllerDeviceIndex].vVelocity.v));
	handController->setTrackedAngularSpeed(AnnVect3(trackedPoses[controllerDeviceIndex].vAngularVelocity.v));
}

void AnnOgreOpenVRRenderer::extractButtons(size_t side)
{
	pressed.clear(); released.clear();
	for (uint8_t i(0); i < buttonsToHandle.size(); i++)
	{
		lastControllerButtonsPressed[side][i] = currentControllerButtonsPressed[side][i];
		currentControllerButtonsPressed[side][i] = (controllerState.ulButtonPressed & ButtonMaskFromId(buttonsToHandle[i])) != 0;
		if (currentControllerButtonsPressed[side][i] && !lastControllerButtonsPressed[side][i])
			pressed.push_back(i);
		else if (!currentControllerButtonsPressed[side][i] && lastControllerButtonsPressed[side][i])
			released.push_back(i);
	}
}

void AnnOgreOpenVRRenderer::processTrackedDevices()
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
		AnnHandController::AnnHandControllerSide side;
		switch (vrSystem->GetControllerRoleForTrackedDeviceIndex(trackedDevice))
		{
		case vr::ETrackedControllerRole::TrackedControllerRole_LeftHand:
			side = AnnHandController::leftHandController;
			break;

		case vr::ETrackedControllerRole::TrackedControllerRole_RightHand:
			side = AnnHandController::rightHandController;
			break;

		case vr::ETrackedControllerRole::TrackedControllerRole_Invalid:
		default:
			continue;
		}

		processController(trackedDevice, side);
	}
}

void AnnOgreOpenVRRenderer::handleIPDChange()
{
	//Get the eyeToHeadTransform (they contain the IPD translation)
	for (char i(0); i < 2; i++)
		eyeCameras[i]->setPosition(getMatrix4FromSteamVRMatrix34(
			vrSystem->GetEyeToHeadTransform(getEye(oovrEyeType(i)))).getTrans());
}

inline Ogre::Matrix4 AnnOgreOpenVRRenderer::getMatrix4FromSteamVRMatrix34(const vr::HmdMatrix34_t& mat)
{
	return Ogre::Matrix4
	{
		mat.m[0][0],	mat.m[0][1],	mat.m[0][2],	mat.m[0][3],
		mat.m[1][0],	mat.m[1][1],	mat.m[1][2],	mat.m[1][3],
		mat.m[2][0],	mat.m[2][1],	mat.m[2][2],	mat.m[2][3],
		0.0f,			0.0f,			0.0f,			1.0f
	};
}

void AnnOpenVRMotionController::rumbleStart(float value)
{
	current = AnnGetVRRenderer()->getTimer()->getMilliseconds();
	//Limit frequency to 1/50 hz
	if (current - last > 50)
	{
		last = current;
		//Max value of one pulse will be 3500µs
		vrSystem->TriggerHapticPulse(deviceIndex, vr::EVRButtonId::k_EButton_SteamVR_Touchpad - vr::k_EButton_Axis0, static_cast<unsigned short>(value * 3500));
	}
}

void AnnOpenVRMotionController::rumbleStop()
{
	//NB : The "rumbeling" of OpenVR controllers is pulse based. Meaning that telling it to "not move" doesn't make much sense.
}

AnnOpenVRMotionController::AnnOpenVRMotionController(vr::IVRSystem* vrsystem,
	vr::TrackedDeviceIndex_t OpenVRDeviceIndex,
	Ogre::SceneNode* handNode,
	AnnHandControllerID controllerID,
	AnnHandControllerSide controllerSide) : AnnHandController("OpenVR Hand Controller", handNode, controllerID, controllerSide),
	deviceIndex(OpenVRDeviceIndex),
	vrSystem(vrsystem), last(0), current(0)
{
	capabilites = RotationalTracking | PositionalTracking | AngularAccelerationTracking | LinearAccelerationTracking | ButtonInputs | AnalogInputs | HapticFeedback;
}
