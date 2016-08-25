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

	vr::EVREye getEye(oovrEyeType eye)
	{
		if (eye == left) return vr::Eye_Left;
		return vr::Eye_Right;
	}

protected:
private:
	static OgreOpenVRRender* OpenVRSelf;

	vr::IVRSystem* vrSystem;
	vr::HmdError hmdError;

	vr::IVRCompositor* vrCompositor;

	unsigned int windowWidth, windowHeight;

	Ogre::TexturePtr rttTexture[2];
	GLuint rttTextureGLID[2];

	bool gamma;

	vr::GraphicsAPIConvention API;
};
