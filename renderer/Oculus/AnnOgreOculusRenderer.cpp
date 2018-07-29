// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "AnnOgreOculusRenderer.hpp"
#include "AnnOculusTouchController.hpp"

#ifdef _WIN32

#include <string>
#include <AnnLogger.hpp>
#include <AnnGetter.hpp>
#include <AnnException.hpp>

#include <OVR_CAPI_Audio.h>

#include <iostream>
#include <sstream>

using namespace Annwvyn;

//Static class members
bool AnnOgreOculusRenderer::mirrorHMDView{ true };
Ogre::TextureUnitState* AnnOgreOculusRenderer::debugTexturePlane{ nullptr };
AnnOgreOculusRenderer* AnnOgreOculusRenderer::oculusSelf{ nullptr };

AnnOgreOculusRenderer::AnnOgreOculusRenderer(std::string winName) :
 AnnOgreVRRenderer(winName),
 texSizeL{ 0, 0 },
 texSizeR{ 0, 0 },
 currentFrameDisplayTime{ 0 },
 eyeRenderDescArray{},
 hmdSize{ 0, 0 },
 mirrorTexture{ nullptr },
 oculusMirrorTextureGLID{ 0 },
 ogreMirrorTextureGLID{ 0 },
 oculusRenderTextureCombinedGLID{ 0 },
 ogreRenderTextureCombinedGLID{ 0 },
 oculusRenderTexturesSeparatedGLID{ { 0 } },
 ogreRenderTexturesSeparatedGLID{ { 0 } },
 layer{},
 textureCombinedSwapChain{ nullptr },
 eyeToHmdPoseOffset{ { {}, {} } },
 pose{},
 handPoses{},
 inputState{},
 ts{},
 layers{ nullptr },
 sessionStatus{},
 perfHudMode{ ovrPerfHud_Off },
 currentCombinedIndex{ 0 },
 currentSeparatedIndex{ { 0, 0 } },
 currentSessionStatusFrameIndex{ 0 },
 debugSmgr{ nullptr },
 debugCam{ nullptr },
 debugCamNode{ nullptr },
 debugPlaneNode{ nullptr }
{
	rendererName = "OpenGL/Oculus";
	oculusSelf   = this;

	//List of bitmask for each buttons as we will test them
	touchControllersButtons[left][0]  = ovrButton_X;
	touchControllersButtons[left][1]  = ovrButton_Y;
	touchControllersButtons[left][2]  = ovrButton_Enter;
	touchControllersButtons[left][3]  = ovrButton_LThumb;
	touchControllersButtons[right][0] = ovrButton_A;
	touchControllersButtons[right][1] = ovrButton_B;
	touchControllersButtons[right][2] = 0; //This button is the Oculus Dashboard button. Be false all the time
	touchControllersButtons[right][3] = ovrButton_RThumb;

	//Initialize the vector<byte>s that will hold the processed button states
	for(const auto side : { left, right })
	{
		currentControllerButtonsPressed[side].resize(touchControllersButtons[side].size(), false);
		lastControllerButtonsPressed[side].resize(touchControllersButtons[side].size(), false);
	}
}

AnnOgreOculusRenderer::~AnnOgreOculusRenderer()
{
	//Destroy any Oculus SDK related objects
	ovr_DestroyTextureSwapChain(oculusInterface->getSession(), textureCombinedSwapChain);
	ovr_DestroyMirrorTexture(oculusInterface->getSession(), mirrorTexture);
}

bool AnnOgreOculusRenderer::shouldQuit()
{
	return getSessionStatus().ShouldQuit == ovrTrue;
}

bool AnnOgreOculusRenderer::shouldRecenter()
{
	return getSessionStatus().ShouldRecenter == ovrTrue;
}

bool AnnOgreOculusRenderer::isVisibleInHmd()
{
	return getSessionStatus().IsVisible == ovrTrue;
}

void AnnOgreOculusRenderer::cycleDebugHud()
{
	//Loop through the perf HUD mode available
	perfHudMode = (perfHudMode + 1) % ovrPerfHud_Count;

	//Set the current perf hud mode
	oculusInterface->setPerfHudMode(ovrPerfHudMode(perfHudMode));
}

void AnnOgreOculusRenderer::debugPrint()
{
	AnnDebug() << "Eye camera states :";
	for(auto eye : eyeUpdateOrder)
	{
		AnnDebug() << "eyeCamera " << eye << " " << eyeCameras[eye]->getPosition();
		AnnDebug() << eyeCameras[eye]->getOrientation();
	}
}

inline Ogre::Vector3 AnnOgreOculusRenderer::oculusToOgreVect3(const ovrVector3f& v)
{
	return Ogre::Vector3{ v.x, v.y, v.z };
}

inline Ogre::Quaternion AnnOgreOculusRenderer::oculusToOgreQuat(const ovrQuatf& q)
{
	return Ogre::Quaternion{ q.w, q.x, q.y, q.z };
}

void AnnOgreOculusRenderer::recenter()
{
	oculusInterface->recenterTrackingOrigin();
}

void AnnOgreOculusRenderer::initVrHmd()
{
	//Class to get basic information from the Rift. Initialize the RiftSDK
	oculusInterface = std::make_unique<AnnOculusInterfaceHelper>();
	hmdSize			= oculusInterface->getHmdResolution();
	updateTime		= 1.0 / double(oculusInterface->getHmdDisplayRefreshRate());

	ovr_GetSessionStatus(oculusInterface->getSession(), &sessionStatus);
	oculusInterface->setTrackingOriginToFloorLevel();

	const auto playerEyeHeight = oculusInterface->getUserEyeHeight();
	if(playerEyeHeight != -1.0f) AnnGetPlayer()->setEyesHeight(playerEyeHeight);

	AnnDebug() << "Player eye height : " << playerEyeHeight << "m";
	AnnDebug() << "Eye leveling translation : " << AnnGetPlayer()->getEyeTranslation();
}

void AnnOgreOculusRenderer::setMonoFov(float degreeFov) const
{
	if(monoCam) monoCam->setFOVy(Ogre::Degree(degreeFov));
}

void AnnOgreOculusRenderer::initScene()
{
	//Get if the complied buffer are correct
	if(!debugPlaneSanityCheck())
		throw AnnInitializationError(ANN_ERR_NOTINIT, "Sanity check failed, check the static buffer in AnnOgreOculusRenderer.hpp");

	createMainSmgr();
}

///This will create the Oculus Textures and the Ogre textures for rendering and mirror display
void AnnOgreOculusRenderer::initRttRendering()
{
	//Init GLEW here to be able to call OpenGL functions
	loadOpenGLFunctions();

	//Get texture size from ovr with the maximal FOV for each eye
	texSizeL = ovr_GetFovTextureSize(oculusInterface->getSession(), ovrEye_Left, oculusInterface->getHmdDesc().DefaultEyeFov[left], 1.f);
	texSizeR = ovr_GetFovTextureSize(oculusInterface->getSession(), ovrEye_Right, oculusInterface->getHmdDesc().DefaultEyeFov[right], 1.f);

	//Create left eye render texture
	ovrTextureSwapChainDesc textureSwapChainDesc = {};
	textureSwapChainDesc.Type					 = ovrTexture_2D;
	textureSwapChainDesc.ArraySize				 = 1;
	textureSwapChainDesc.Format					 = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;
	textureSwapChainDesc.Width					 = texSizeL.w;
	textureSwapChainDesc.Height					 = texSizeL.h;
	textureSwapChainDesc.MipLevels				 = 1;
	textureSwapChainDesc.SampleCount			 = 1;
	textureSwapChainDesc.StaticImage			 = ovrFalse;
	if(OVR_FAILURE(ovr_CreateTextureSwapChainGL(oculusInterface->getSession(), &textureSwapChainDesc, &texturesSeparatedSwapChain[left])))
	{
		AnnDebug() << "Cannot create Oculus OpenGL SwapChain";
		throw AnnInitializationError(ANN_ERR_RENDER, "Cannot create Oculus OpenGL swapchain");
	}

	//Create right eye render texture (same basic configuration, just replace the texture sizes)
	textureSwapChainDesc.Width  = texSizeR.w;
	textureSwapChainDesc.Height = texSizeR.h;
	;
	if(OVR_FAILURE(ovr_CreateTextureSwapChainGL(oculusInterface->getSession(), &textureSwapChainDesc, &texturesSeparatedSwapChain[right])))
	{
		//If we can't get the textures, there is no point trying more.
		AnnDebug() << "Cannot create Oculus OpenGL SwapChain";
		throw AnnInitializationError(ANN_ERR_RENDER, "Cannot create Oculus OpenGL swapchain");
	}

	//Create the equivalent textures in Ogre side
	const combinedTextureSizeArray textureDimentions{ { { { size_t(texSizeL.w), size_t(texSizeL.h) } }, { { size_t(texSizeR.w), size_t(texSizeR.h) } } } };
	ogreRenderTexturesSeparatedGLID = createSeparatedRenderTextures(textureDimentions);

	//Setup Ogre compositor
	auto compositor							 = root->getCompositorManager2();
	compositorWorkspaces[leftEyeCompositor]  = compositor->addWorkspace(smgr, rttEyeSeparated[left], eyeCameras[left], "HdrWorkspace", true);
	compositorWorkspaces[rightEyeCompositor] = compositor->addWorkspace(smgr, rttEyeSeparated[right], eyeCameras[right], "HdrWorkspace", true);
	compositorWorkspaces[monoCompositor]	 = compositor->addWorkspace(smgr, window, monoCam, "HdrWorkspace", true);

	//Same deal but for the mirror (debuging) texture
	ovrMirrorTextureDesc mirrorTextureDesc = {};
	mirrorTextureDesc.Width				   = hmdSize.w;
	mirrorTextureDesc.Height			   = hmdSize.h;
	mirrorTextureDesc.Format			   = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;
	if(OVR_FAILURE(ovr_CreateMirrorTextureGL(oculusInterface->getSession(), &mirrorTextureDesc, &mirrorTexture)))
	{
		//If for some weird reason (stars alignment, dragons, northern gods, reaper invasion) we can't create the mirror texture
		AnnDebug() << "Cannot create Oculus mirror texture";
		throw AnnInitializationError(ANN_ERR_RENDER, "Cannot create Oculus mirror texture");
	}
	auto mirror			  = createAdditionalRenderBuffer(hmdSize.w, hmdSize.h, "MirrorTex");
	ogreMirrorTextureGLID = std::get<1>(mirror);

	ovr_GetTextureSwapChainBufferGL(oculusInterface->getSession(), textureCombinedSwapChain, 0, &oculusRenderTextureCombinedGLID);
}

void AnnOgreOculusRenderer::showRawView()
{
}

void AnnOgreOculusRenderer::showMirrorView()
{
}

void AnnOgreOculusRenderer::showMonscopicView()
{
}

void AnnOgreOculusRenderer::initClientHmdRendering()
{
	//Populate OVR structures
	eyeRenderDescArray[left]  = ovr_GetRenderDesc(oculusInterface->getSession(), ovrEye_Left, oculusInterface->getHmdDesc().DefaultEyeFov[left]);
	eyeRenderDescArray[right] = ovr_GetRenderDesc(oculusInterface->getSession(), ovrEye_Right, oculusInterface->getHmdDesc().DefaultEyeFov[right]);
	eyeToHmdPoseOffset[left]  = eyeRenderDescArray[left].HmdToEyePose;
	eyeToHmdPoseOffset[right] = eyeRenderDescArray[right].HmdToEyePose;

	//Report on the values given by the SDK
	logHeadsetGeometry();

	//Create a layer with our single swaptexture on it. Each side is an eye.
	layer.Header.Type		  = ovrLayerType_EyeFov;
	layer.Header.Flags		  = 0;
	layer.ColorTexture[left]  = texturesSeparatedSwapChain[left];
	layer.ColorTexture[right] = texturesSeparatedSwapChain[right];
	layer.Fov[left]			  = eyeRenderDescArray[left].Fov;
	layer.Fov[right]		  = eyeRenderDescArray[right].Fov;

	//Define the two viewports dimensions :
	ovrRecti leftRect, rightRect;

	leftRect.Size   = texSizeL;
	rightRect.Size  = texSizeR;
	leftRect.Pos.x  = 0;
	leftRect.Pos.y  = 0;
	rightRect.Pos.x = 0;
	rightRect.Pos.y = 0;
	//Assign the defined viewport to the layer
	layer.Viewport[left]  = leftRect;
	layer.Viewport[right] = rightRect;

	//Get the projection matrix for the desired near/far clipping from Oculus and apply them to the eyeCameras
	updateEyeCameraFrustrum();

	//Make sure that the perf hud will not show up by itself...
	perfHudMode = ovrPerfHud_Off;
	oculusInterface->setPerfHudMode(ovrPerfHud_Off);
}

void AnnOgreOculusRenderer::updateEyeCameraFrustrum()
{
	for(size_t i{0}; i < 2; ++i)
	{
		eyeCameras[i]->setNearClipDistance(nearClippingDistance);
		eyeCameras[i]->setFarClipDistance(farClippingDistance);

		//instead of loading the raw projection matrix, actually update the frustrum and let Ogre deal with it
		const auto& currentEyeFov = oculusInterface->getHmdDesc().DefaultEyeFov[i];

		AnnDebug() << "Frustrum left  : " << currentEyeFov.LeftTan;
		AnnDebug() << "Frustrum right : " << currentEyeFov.RightTan;
		AnnDebug() << "Frustrum top   : " << currentEyeFov.UpTan;
		AnnDebug() << "Frustrum bottom: " << currentEyeFov.DownTan;


		//The Oculus SDK give theses angles as starting from the "view vector". It's like giving use the absolute value of the angle, and not the angle itself
		//We need to negate the one looking ot the left and down to get the correct angles of the frustrum edges.

		eyeCameras[i]->setFrustumExtents(-currentEyeFov.LeftTan, //This one is flipped
										 currentEyeFov.RightTan,
										 currentEyeFov.UpTan,
										 -currentEyeFov.DownTan, //This one is flipped
										 Ogre::FrustrumExtentsType::FET_TAN_HALF_ANGLES);
	}
}

ovrSessionStatus AnnOgreOculusRenderer::getSessionStatus()
{
	if(currentSessionStatusFrameIndex != frameCounter)
	{
		ovr_GetSessionStatus(oculusInterface->getSession(), &sessionStatus);
		currentSessionStatusFrameIndex = frameCounter;
	}
	return sessionStatus;
}

bool AnnOgreOculusRenderer::usesCustomAudioDevice()
{
	return true;
}

std::string AnnOgreOculusRenderer::getAudioDeviceIdentifierSubString()
{
	//The result will be stored statically here.
	static std::string audioDeviceName = "";

	//If we know the string already, return it.
	if(!audioDeviceName.empty()) return audioDeviceName;
	AnnDebug() << "Looking for the audio output selected inside the Oculus App...";

	//Get GUID of device, and get it's equivalent name
	GUID audioDeviceGuid;
	ovr_GetAudioDeviceOutGuid(&audioDeviceGuid);
	audioDeviceName = getAudioDeviceNameFromGUID(audioDeviceGuid);

	//Return the return string
	return audioDeviceName;
}

void AnnOgreOculusRenderer::showDebug(DebugMode mode)
{
	switch(mode)
	{
		case RAW_BUFFER: return showRawView();
		case HMD_MIRROR: return showMirrorView();
		case MONOSCOPIC: return showMonscopicView();
		default: break;
	}
}

void AnnOgreOculusRenderer::handleIPDChange()
{
	for(auto eye : eyeUpdateOrder)
	{
		const auto eyeOffset = eyeRenderDescArray[eye].HmdToEyePose;
		eyeCameras[eye]->setPosition(oculusToOgreVect3(eyeOffset.Position));
		eyeCameras[eye]->setOrientation(oculusToOgreQuat(eyeOffset.Orientation));
		eyeToHmdPoseOffset[eye] = eyeOffset;
	}
}

void AnnOgreOculusRenderer::logHeadsetGeometry()
{
	for(auto eye : eyeUpdateOrder)
	{
		AnnDebug() << "For eye : " << eye;
		AnnDebug() << oculusToOgreVect3(eyeToHmdPoseOffset[eye].Position);
		AnnDebug() << oculusToOgreQuat(eyeToHmdPoseOffset[eye].Orientation);
	}
}

void AnnOgreOculusRenderer::getTrackingPoseAndVRTiming()
{
	//Get timing
	currentFrameDisplayTime = ovr_GetPredictedDisplayTime(oculusInterface->getSession(), ++frameCounter);
	calculateTimingFromOgre();

	//Reorient the headset if the runtime flags for it
	if(getSessionStatus().ShouldRecenter) recenter();

	//Get the tracking state
	ts = ovr_GetTrackingState(oculusInterface->getSession(),
							  currentFrameDisplayTime,
							  ovrTrue);

	//Update pose and controllers
	pose = ts.HeadPose.ThePose;
	ovr_CalcEyePoses(pose, eyeToHmdPoseOffset.data(), layer.RenderPose);
	updateTouchControllers();
	handleIPDChange();

	//Apply pose to the two cameras
	trackedHeadPose.orientation = bodyOrientation * oculusToOgreQuat(pose.Orientation);
	trackedHeadPose.position	= feetPosition + bodyOrientation * oculusToOgreVect3(pose.Position);
}

void AnnOgreOculusRenderer::renderAndSubmitFrame()
{
	handleWindowMessages();
	hideHands = !getSessionStatus().HasInputFocus;

	if(!getSessionStatus().IsVisible)
	{
		pauseFlag = true;
		return;
	}
	pauseFlag = false;

	ovr_WaitToBeginFrame(oculusInterface->getSession(), frameCounter);
	ovr_BeginFrame(oculusInterface->getSession(), frameCounter);
	root->renderOneFrame();

	for(auto i{ 0U }; i < 2; ++i)
	{
		ovr_GetTextureSwapChainCurrentIndex(oculusInterface->getSession(), texturesSeparatedSwapChain[i], &currentSeparatedIndex[i]);
		ovr_GetTextureSwapChainBufferGL(oculusInterface->getSession(), texturesSeparatedSwapChain[i], currentSeparatedIndex[i], &oculusRenderTexturesSeparatedGLID[i]);
		//Copy the rendered image to the Oculus Swap Texture
		glEasyCopy(ogreRenderTexturesSeparatedGLID[i],
				   oculusRenderTexturesSeparatedGLID[i],
				   texSizeL.w,
				   texSizeL.h);
		ovr_CommitTextureSwapChain(oculusInterface->getSession(), texturesSeparatedSwapChain[i]);
	}

	//Submit the frame
	layers = &layer.Header;
	ovr_EndFrame(oculusInterface->getSession(), frameCounter, nullptr, &layers, 1);

	//Update the render debug view if the window is visible
	if(window->isVisible() && mirrorHMDView)
	{
		//Put the mirrored view available for Ogre if asked for
		ovr_GetMirrorTextureBufferGL(oculusInterface->getSession(), mirrorTexture, &oculusMirrorTextureGLID);
		glEasyCopy(oculusMirrorTextureGLID,
				   ogreMirrorTextureGLID,
				   hmdSize.w,
				   hmdSize.h);
	}
}

void AnnOgreOculusRenderer::initializeHandObjects(const OgreOculusEyeType side)
{
	//If it's the first time we have access data on this hand controller, instantiate the object
	if(!handControllers[side])
	{
		handControllers[side] = std::make_shared<AnnOculusTouchController>(oculusInterface->getSession(), smgr->getRootSceneNode()->createChildSceneNode(), size_t(side), AnnHandController::AnnHandControllerSide(side));
	}
}

void AnnOgreOculusRenderer::initializeControllerAxes(const OgreOculusEyeType side, std::vector<AnnHandControllerAxis>& axesVector)
{
	axesVector.push_back(AnnHandControllerAxis{ "Thumbstick X", inputState.Thumbstick[side].x });
	axesVector.push_back(AnnHandControllerAxis{ "Thumbstick Y", inputState.Thumbstick[side].y });
	axesVector.push_back(AnnHandControllerAxis{ "Trigger X", inputState.IndexTrigger[side] });
	axesVector.push_back(AnnHandControllerAxis{ "GripTrigger X", inputState.HandTrigger[side] });
}

void AnnOgreOculusRenderer::processButtonStates(const OgreOculusEyeType side)
{
	//Extract button states and deduce press/released events
	pressed.clear();
	released.clear();
	for(auto i(0); i < currentControllerButtonsPressed[side].size(); i++)
	{
		//Save the current polled state as the last one
		lastControllerButtonsPressed[side][i] = currentControllerButtonsPressed[side][i];
		//Get the current state of the button
		currentControllerButtonsPressed[side][i] = (inputState.Buttons & touchControllersButtons[side][i]) != 0;

		//Detect pressed/released event and add it to the list
		if(!lastControllerButtonsPressed[side][i] && currentControllerButtonsPressed[side][i])
			pressed.push_back(uint8_t(i));
		else if(lastControllerButtonsPressed[side][i] && !currentControllerButtonsPressed[side][i])
			released.push_back(uint8_t(i));
	}
}

void AnnOgreOculusRenderer::updateTouchControllers()
{
	//Get the controller state
	if(OVR_FAILURE(ovr_GetInputState(oculusInterface->getSession(), ovrControllerType_Active, &inputState))) return;
	//Check if there's Oculus Touch Data on this thing
	if(!(inputState.ControllerType & ovrControllerType_Touch)) return;

	for(const auto side : { left, right })
	{
		initializeHandObjects(side);

		//Extract the hand pose from the tracking state
		handPoses[side] = ts.HandPoses[side];

		//Get the controller
		auto handController = handControllers[side];

		//Set axis informations
		auto& axesVector = handController->getAxesVector();
		if(axesVector.size() == 0)
			initializeControllerAxes(side, axesVector);

		//Update the values of the axes
		axesVector[0].updateValue(inputState.Thumbstick[side].x);
		axesVector[1].updateValue(inputState.Thumbstick[side].y);
		axesVector[2].updateValue(inputState.IndexTrigger[side]);
		axesVector[3].updateValue(inputState.HandTrigger[side]);

		processButtonStates(side);

		//Set all the data on the controller
		handController->getButtonStateVector()	 = currentControllerButtonsPressed[side];
		handController->getPressedButtonsVector()  = pressed;
		handController->getReleasedButtonsVector() = released;
		handController->setTrackedPosition(feetPosition + bodyOrientation * oculusToOgreVect3(handPoses[side].ThePose.Position));
		handController->setTrackedOrientation(bodyOrientation * oculusToOgreQuat(handPoses[side].ThePose.Orientation));
		handController->setTrackedAngularSpeed(oculusToOgreVect3(handPoses[side].AngularVelocity));
		handController->setTrackedLinearSpeed(oculusToOgreVect3(handPoses[side].LinearVelocity));

		handController->updateVisibility();
	}
}

Annwvyn::AnnOgreVRRenderer* AnnRendererBootstrap_Oculus(const std::string& appName)
{
	return static_cast<AnnOgreVRRenderer*>(new AnnOgreOculusRenderer(appName));
}

#endif
