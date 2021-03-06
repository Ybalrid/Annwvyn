#pragma once

#include <systemMacro.h>
#include <AnnOgreVRRenderer.hpp>
#include <AnnHandController.hpp>

//OpenVR (HTC Vive; SteamVR) SDK
#include <openvr.h>

namespace Annwvyn
{

	///Renderer for OpenVR
	class AnnDllExport AnnOgreOpenVRRenderer : public AnnOgreVRRenderer
	{
		///Marker for left and right : "Ogre Open VR Eye Type"
		enum OpenVREyeType {
			left,
			right
		};

	public:
		///Construct an OgreOpenVR object
		AnnOgreOpenVRRenderer(std::string windowName = "AnnOgreOpenVRRenderer");

		///Clear the OgreOpenVR object
		virtual ~AnnOgreOpenVRRenderer();

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
		void getTrackingPoseAndVRTiming() override;

		///Render each frames
		void renderAndSubmitFrame() override;

		///Recenter the tracking origin
		void recenter() override;

		///Return true if this VR system has an integrated audio device
		bool usesCustomAudioDevice() override { return false; }

		///Return the substr that can help identify the correct audio device to use
		std::string getAudioDeviceIdentifierSubString() override { return ""; }

		///Change the debug window mode
		void showDebug(DebugMode mode) override;

		///Create and initialize the scene managers
		void initScene() override;

		///Create the render targets
		void initRttRendering() override;

		///Get the projection matrix from the OpenVR API and apply it to the cameras using the near/far clip planes distances
		void updateEyeCameraFrustrum() override;

		///Get a "vr::EVREye" from an "OpenVREyeType"
		static vr::EVREye getEye(OpenVREyeType eye);

	private:
		///Get the HMD position in the OpenVR tracking space
		Ogre::Vector3 getTrackedHMDTranslation() const;

		///Get the HMD orientation in the OpenVR tracking space
		Ogre::Quaternion getTrackedHMDOrieation() const;

		///Take a Matrix34 from OpenVR and spew out an Ogre::Matrix4 that represent the same transform
		static Ogre::Matrix4 getMatrix4FromSteamVRMatrix34(const vr::HmdMatrix34_t& mat);

		///Iterate through the list of events from SteamVR and call code that react to it
		void processVREvents();

		///Process through the array of OpenVR tracked devices, get the data of the interesting ones (hand controllers)
		void processTrackedDevices();

		///Go extract the controller data from the tracked OpenVR device
		void processController(vr::TrackedDeviceIndex_t controllerDeviceIndex, Annwvyn::AnnHandController::AnnHandControllerSide side);

		///Extract the list of buttons state from the button from the registered bit flag for this side, and construct the buffered pressed/released lists.
		void extractButtons(size_t side);

		///Reset the IPD displacement of the cameras according to the EyeToHeadTransform matrix
		void handleIPDChange() override;

		///Singleton pointer
		static AnnOgreOpenVRRenderer* OpenVRSelf;

		///main OpenVR object
		vr::IVRSystem* vrSystem;

		///Error handling viable
		vr::HmdError hmdError;

		///window size
		unsigned int windowWidth, windowHeight;

		///OpenGL "id" of the render textures
		GLuint rttTextureGLID, rttLeftTextureGLID, rttRightTextureGLID;

		///Use hardware gamma correction
		bool gamma;

		///TextureType, set to OpenGL by constructor
		vr::ETextureType TextureType;

		///OpenVR texture handlers
		std::array<vr::Texture_t, 2> vrTextures;

		///OpenVR device strings
		std::string strDriver, strDisplay;

		///Geometry of an OpenGL texture
		std::array<vr::VRTextureBounds_t, 2> GLBounds;

		///Array of tracked poses
		vr::TrackedDevicePose_t trackedPoses[vr::k_unMaxTrackedDeviceCount];

		///Transform that correspond to the HMD tracking
		Ogre::Matrix4 hmdAbsoluteTransform;

		///State of the "should quit" marker. If it goes to true, the game loop should stop
		bool shouldQuitState;

		///IDs of buttons to handle
		std::vector<vr::EVRButtonId> buttonsToHandle;

		///Structure to hold the current state of the controller. Have to pass a pointer to this to an OpenVR function
		vr::VRControllerState_t controllerState;

		///Constant values needed for extracting axis informations
		const size_t numberOfAxes, axoffset;

		///X axis of the touchpad
		float touchpadXNormalizedValue;
		///Y axis of the touchpad
		float touchpadYNormalizedValue;
		///Value of the main trigger
		float triggerNormalizedValue;

		///To store the type of an event
		vr::VREvent_t event;
	};
}

///Initialize the OpenVR renderer and return a naked pointer. Call it from the
extern "C" AnnDllExport Annwvyn::AnnOgreVRRenderer* AnnRendererBootstrap_OpenVR(const std::string& name);
