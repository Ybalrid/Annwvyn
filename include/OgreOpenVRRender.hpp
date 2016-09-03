#ifndef OGRE_OPENVR_RENDER
#define OGRE_OPENVR_RENDER

#include "systemMacro.h"
#include "OgreVRRender.hpp"

//OpenVR (HTC Vive; SteamVR) SDK
#include <openvr.h>
#include <openvr_capi.h>

//OS Specific build macro 
#include "systemMacro.h"
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
	virtual void initPipeline();

	///Initialize the OpenVR HMD
	virtual void initVrHmd();

	///Initialize additional rendering features 
	virtual void initClientHmdRendering();

	///Return true if the application should terminate
	virtual bool shouldQuit();

	///Return true if the application should recenter
	virtual bool shouldRecenter();

	///Return true if the app can be view in the HMD
	virtual bool isVisibleInHmd();

	///Update the tracking state of the HMD (and possibly other tracked objects)
	virtual void updateTracking();

	///Render each frames
	virtual void renderAndSubmitFrame();

	///Recenter the tracking origin
	virtual void recenter();

	///Change the background color of every viewport on the rendering pipeline
	virtual void changeViewportBackgroundColor(Ogre::ColourValue color);

	///Set the cameras near clip plane distance
	virtual void setCamerasNearClippingDistance(float distance = 0.15f);

	///Set the cameras far clip plane distance
	virtual void setCameraFarClippingDistance(float distance = 4000.0f);

	///Return true if this VR system has an integrated audio device
	virtual bool usesCustomAudioDevice() { return false; }

	///Return the substr that can help identify the correct audio device to use
	virtual std::string getAudioDeviceIdentifierSubString() { return ""; }

	///Change the debug window mode
	virtual void showDebug(DebugMode mode);

	///Create a window. Even if we do "off screen rendering", that's a vital part of how Ogre does stuff.
	virtual void createWindow();

	///Create and initialize the scene managers
	virtual void initScene();

	///Create and initialize the cameras
	virtual void initCameras();

	///Create the render targets
	virtual void initRttRendering();

	///Get the correct projection matrix for the cameras and set each camera to use it
	void getProjectionMatrix();

	///Get a "vr::EVREye" from an "oovrEyeType"
	inline vr::EVREye getEye(oovrEyeType eye);

	///Setup the distortion rendering. Apparently this is actually not needed. Even if the official sample does it. This funciton is a placeholder
	void setupDistrotion();

protected:
private:

	///Get the HMD position in the OpenVR tracking space
	inline Ogre::Vector3 getTrackedHMDTranslation();

	///Get the HMD orientation in the OpenVR tracking space
	inline Ogre::Quaternion getTrackedHMDOrieation();

	///Take a Matrix34 from OpenVR and spew out an Ogre::Matrix4 that represent the same transform
	inline Ogre::Matrix4 getMatrix4FromSteamVRMatrix34(const vr::HmdMatrix34_t& mat);
	
	///Iterate through the list of events from SteamVR and call code that react to it
	void processVREvents();
	
	void processTrackedDevices();

	///Reset the IPD displacement of the cameras according to the EyeToHeadTransform matrix
	void handleIPDChange();

	///Singleton pointer
	static OgreOpenVRRender* OpenVRSelf;

	///main OpenVR object
	vr::IVRSystem* vrSystem;

	///Error handeling vaiable
	vr::HmdError hmdError;

	///window size
	unsigned int windowWidth, windowHeight;

	///EyeCamera render texures
	Ogre::TexturePtr rttTexture[2];
	
	///OpenGL "id" of the render textures
	GLuint rttTextureGLID[2];

	///EyeCameraViewport
	Ogre::Viewport* rttViewports[2];

	///Use hardware gamma correction
	bool gamma;

	///API handler, should be initialized to "OpenGL"
	vr::GraphicsAPIConvention API;

	///OpenVR texture handlers
	vr::Texture_t vrTextures[2];

	///Monoscopic camera
	Ogre::Camera* monoCam;

	///Viewport located on a window
	Ogre::Viewport* windowViewport;

	///OpenVR device strings
	std::string strDriver, strDisplay;

	///Geometry of an OpenGL texture
	vr::VRTextureBounds_t GLBounds;
	
	///Timing marker
	double then, now;

	///Array of tracked poses
	vr::TrackedDevicePose_t trackedPoses[vr::k_unMaxTrackedDeviceCount];

	///Transform that corespond to the HMD tracking
	Ogre::Matrix4 hmdAbsoluteTransform;
	
	///Camera Rig that holds the 2 cameras on the same plane 
	Ogre::SceneNode* eyeRig;

	///State of the "should quit" marker. If it goes to true, the game loop should stop 
	bool shouldQuitState;



};

#endif
