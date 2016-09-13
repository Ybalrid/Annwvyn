#ifndef OGRE_NO_VR_RENDER
#define OGRE_NO_VR_RENDER

#include "systemMacro.h"

#include "OgreVRRender.hpp"

class DLL OgreNoVRRender : public OgreVRRender
{
public:
	OgreNoVRRender(std::string winName);
	
	virtual void initPipeline();
	virtual void initVrHmd();
	virtual void createWindow();
	virtual void initScene();
	virtual void initCameras();
	virtual void initRttRendering();
	virtual void initClientHmdRendering();
	virtual bool shouldQuit() { return false; }
	virtual bool shouldRecenter() { return false; }
	virtual bool isVisibleInHmd() { return true; }
	
	virtual void updateTracking();
	virtual void renderAndSubmitFrame();
	virtual void recenter();

	virtual void changeViewportBackgroundColor(Ogre::ColourValue);

	virtual void setCamerasNearClippingDistance(float distance);
	virtual void setCameraFarClippingDistance(float distance);

	virtual void showDebug(DebugMode mode);

private:

	static OgreNoVRRender* noVRself;


	Ogre::Camera* noVRCam;

	Ogre::Viewport* noVRViewport;

};

#endif //OGRE_NO_VR_RENDER

