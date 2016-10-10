#ifndef OGRE_NO_VR_RENDER
#define OGRE_NO_VR_RENDER

#include "systemMacro.h"

#include "OgreVRRender.hpp"

///Dummy VR renderer, just render the scene to a window like a non vr game. Doesn't need special hardware or runtime to run
class DLL OgreNoVRRender : public OgreVRRender
{
public:
	OgreNoVRRender(std::string winName = "OgreVRNoVRRender");
	
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

	virtual void showDebug(DebugMode mode);

	virtual void updateProjectionMatrix();

private:
	static OgreNoVRRender* noVRself;
	Ogre::Camera* noVRCam;
	Ogre::Viewport* noVRViewport;
	double then, now;
};

#endif //OGRE_NO_VR_RENDER

