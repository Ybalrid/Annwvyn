#ifndef OGRE_NO_VR_RENDER
#define OGRE_NO_VR_RENDER

#include "systemMacro.h"

#include "OgreVRRender.hpp"

///Dummy VR renderer, just render the scene to a window like a non vr game. Doesn't need special hardware or runtime to run
class DLL OgreNoVRRender : public OgreVRRender
{
public:
	OgreNoVRRender(std::string winName = "OgreVRNoVRRender");
	~OgreNoVRRender();

	void initVrHmd() override;
	void createWindow() override;
	void initScene() override;
	void initCameras() override;
	void initRttRendering() override;
	void initClientHmdRendering() override;
	bool shouldQuit() override;
	bool shouldRecenter() override;
	bool isVisibleInHmd() override;

	void getTrackingPoseAndVRTiming() override;

	void renderAndSubmitFrame() override;
	void recenter() override;

	void changeViewportBackgroundColor(Ogre::ColourValue) override;

	void showDebug(DebugMode mode) override;

	void updateProjectionMatrix() override;

	void handleIPDChange() override;

private:
	static OgreNoVRRender* noVRself;
	Ogre::Viewport* noVRViewport;
	double then, now;
	bool running;
};

#endif //OGRE_NO_VR_RENDER