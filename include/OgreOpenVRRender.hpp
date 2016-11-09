#ifndef OGRE_OPENVR_RENDER
#define OGRE_OPENVR_RENDER

#include "systemMacro.h"
#include "OgreVRRender.hpp"

//OpenVR (HTC Vive; SteamVR) SDK
#include <openvr.h>
#include <openvr_capi.h>

#ifdef _WIN32
#include <Windows.h>
#include <glew.h>
#endif

//We need to get low level access to Ogre's RenderSystem_GL
#include <RenderSystems/GL/OgreGLTextureManager.h>
#include <RenderSystems/GL/OgreGLRenderSystem.h>
#include <RenderSystems/GL/OgreGLTexture.h>

#include "AnnErrorCode.hpp"
#include "AnnTypes.h"

#include "AnnHandController.hpp"

class DLL OgreOpenVRRender : public OgreVRRender
{
	///Marker for left and right : "Ogre Open VR Eye Type"
	enum oovrEyeType
	{
		left, right
	};
public:
	///Construct an OgreOpenVR object
	OgreOpenVRRender(std::string windowName = "OgreOpenVRRender");

	///Clear the OgreOpenVR object
	virtual ~OgreOpenVRRender();

	///Initialize the rendering pipeline
	void initPipeline() override;

	///Initialize the OpenVR HMD
	void initVrHmd() override;

	///Initialize additional rendering features
	void initClientHmdRendering() override;

	///Return true if the application should terminate
	bool shouldQuit() override;

	///Return true if the application should recenter
	bool shouldRecenter() override;

	///Return true if the app can be view in the HMD
	bool isVisibleInHmd() override;

	///Update the tracking state of the HMD (and possibly other tracked objects)
	void updateTracking() override;

	///Render each frames
	void renderAndSubmitFrame() override;

	///Recenter the tracking origin
	void recenter() override;

	///Change the background color of every viewport on the rendering pipeline
	void changeViewportBackgroundColor(Ogre::ColourValue color) override;

	///Return true if this VR system has an integrated audio device
	bool usesCustomAudioDevice() override { return false; }

	///Return the substr that can help identify the correct audio device to use
	std::string getAudioDeviceIdentifierSubString() override { return ""; }

	///Change the debug window mode
	void showDebug(DebugMode mode) override;

	///Create a window. Even if we do "off screen rendering", that's a vital part of how Ogre does stuff.
	void createWindow() override;

	///Create and initialize the scene managers
	void initScene() override;

	///Create and initialize the cameras
	void initCameras() override;

	///Create the render targets
	void initRttRendering() override;

	///Get the projection matrix from the OpenVR API and apply it to the cameras using the near/far clip planes distances
	void updateProjectionMatrix() override;

	///Get a "vr::EVREye" from an "oovrEyeType"
	static inline vr::EVREye getEye(oovrEyeType eye);

	///Setup the distortion rendering. Apparently this is actually not needed. Even if the official sample does it. This function is a placeholder
	void setupDistrotion();

private:

	///Get the HMD position in the OpenVR tracking space
	inline Ogre::Vector3 getTrackedHMDTranslation();

	///Get the HMD orientation in the OpenVR tracking space
	inline Ogre::Quaternion getTrackedHMDOrieation();

	///Take a Matrix34 from OpenVR and spew out an Ogre::Matrix4 that represent the same transform
	static inline Ogre::Matrix4 getMatrix4FromSteamVRMatrix34(const vr::HmdMatrix34_t& mat);

	///Iterate through the list of events from SteamVR and call code that react to it
	void processVREvents();

	///Process through the array of OpenVR tracked devices, get the data of the interesting ones (hand controllers)
	void processTrackedDevices();

	void processController(vr::TrackedDeviceIndex_t controllerDeviceIndex, Annwvyn::AnnHandController::AnnHandControllerSide side);

	void extractButtons(size_t side);

	///Reset the IPD displacement of the cameras according to the EyeToHeadTransform matrix
	void handleIPDChange();

	///Singleton pointer
	static OgreOpenVRRender* OpenVRSelf;

	///main OpenVR object
	vr::IVRSystem* vrSystem;

	///Error handling viable
	vr::HmdError hmdError;

	///window size
	unsigned int windowWidth, windowHeight;

	///EyeCamera render textures
	Ogre::TexturePtr rttTexture;

	///OpenGL "id" of the render textures
	GLuint rttTextureGLID;

	///EyeCameraViewport
	std::array<Ogre::Viewport*, 2> rttViewports;

	///Use hardware gamma correction
	bool gamma;

	///API handler, should be initialized to "OpenGL"
	vr::GraphicsAPIConvention API;

	///OpenVR texture handlers
	std::array<vr::Texture_t, 2> vrTextures;

	///Monoscopic camera
	Ogre::Camera* monoCam;

	///Viewport located on a window
	Ogre::Viewport* windowViewport;

	///OpenVR device strings
	std::string strDriver, strDisplay;

	///Geometry of an OpenGL texture
	std::array<vr::VRTextureBounds_t, 2> GLBounds;

	///Timing marker
	double then, now;

	///Array of tracked poses
	vr::TrackedDevicePose_t trackedPoses[vr::k_unMaxTrackedDeviceCount];

	///Transform that correspond to the HMD tracking
	Ogre::Matrix4 hmdAbsoluteTransform;

	///Camera Rig that holds the 2 cameras on the same plane
	Ogre::SceneNode* eyeRig;

	///State of the "should quit" marker. If it goes to true, the game loop should stop
	bool shouldQuitState;

	///IDs of buttons to handle
	std::vector<vr::EVRButtonId> buttonsToHandle;

	///Buffered button states
	std::array<std::vector<bool>, 2> currentControllerButtonsPressed, lastControllerButtonsPressed;

	///Dynamically sized containers for pressed/released events
	std::vector<uint8_t> pressed, released;

	///Structure to hold the current state of the controller. Have to pass a pointer to this to an OpenVR function
	vr::VRControllerState_t controllerState;

	///Constant values needed for extracting axis informations
	const size_t numberOfAxes, axoffset;

	///To hold axis values
	float touchpadXNormalizedValue, touchpadYNormalizedValue, triggerNormalizedValue;
};

#endif