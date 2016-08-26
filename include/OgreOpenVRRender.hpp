#pragma once
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


class DLL OgreOpenVRRender : public OgreVRRender
{

	enum oovrEyeType
	{
		left, right
	};

public:
	OgreOpenVRRender(std::string windowName = "OgreOpenVRRender");

	virtual ~OgreOpenVRRender();

	virtual void initPipeline();

	virtual void initVrHmd();

	virtual void initClientHmdRendering();

	virtual bool shouldQuit();

	virtual bool shouldRecenter();

	virtual bool isVisibleInHmd();

	virtual void updateTracking();

	virtual void renderAndSubmitFrame();

	virtual void recenter();

	virtual void changeViewportBackgroundColor(Ogre::ColourValue color);

	virtual void setCamerasNearClippingDistance(float distance = 0.15f);

	virtual void setCameraFarClippingDistance(float distance = 4000.0f);

	virtual bool usesCustomAudioDevice() { return false; }

	virtual std::string getAudioDeviceIdentifierSubString() { return ""; }

	virtual void showDebug(DebugMode mode);

	virtual void createWindow();

	virtual void initScene();

	virtual void initCameras();

	virtual void initRttRendering();

	void getProjectionMatrix();

	inline vr::EVREye getEye(oovrEyeType eye)
	{
		if (eye == left) return vr::Eye_Left;
		return vr::Eye_Right;
	}

	void setupDistrotion();

protected:
private:

	inline Ogre::Vector3 getTrackedHMDTranslation();
	inline Ogre::Quaternion getTrackedHMDOrieation();
	
	void processVREvents();
	void processTrackedPoses();

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


	Ogre::Camera* monoCam;
	Ogre::Viewport* windowViewport;


	Ogre::SceneManager* distortionScene;

	std::string strDriver, strDisplay;

	vr::VRTextureBounds_t GLBounds;
	
	//Timing
	double then, now;

	vr::TrackedDevicePose_t trackedPoses[vr::k_unMaxTrackedDeviceCount];

	Ogre::Matrix4 hmdAbsoluteTransform;
	
	inline Ogre::Matrix4 getMatrix4FromSteamVRMatrix34(const vr::HmdMatrix34_t& mat);
	Ogre::SceneNode* eyeRig;

	bool shouldQuitState;
};
