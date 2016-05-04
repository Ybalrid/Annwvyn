#pragma once
#include "systemMacro.h"

#include <string>

#include <Ogre.h>
#include "AnnErrorCode.hpp"



///A pose refer to the combinaison of a position and an orientation. 
///It permit to define the placement of an object with 6DOF
struct OgrePose
{
	///A 3D vector representing a position
	Ogre::Vector3 position;
	///A quaternion representiong an orientation
	Ogre::Quaternion orientation;
};

class DLL OgreVRRender
{
public:
	OgreVRRender(std::string windowName);
	virtual ~OgreVRRender();

	///Get the scene manager of the virtual world
	Ogre::SceneManager* getSceneManager();

	///Get the Ogre::Root object 
	Ogre::Root* getRoot();

	///Get the RenderWidow that should display a debug render the VR view
	Ogre::RenderWindow* getWindow();

	///Get the node that is the player's head anchor point
	Ogre::SceneNode* getCameraInformationNode();

	///Get Ogre's internal timer
	Ogre::Timer* getTimer();

	///Get frame update time from the VR renderer
	double getUpdateTime();

	///Init Ogre, please provie the name of the output log file
	void initOgreRoot(std::string loggerName);

	///Init the VR rendering pipeline
	virtual void initPipeline() = 0;

	///Init the VR client library
	virtual void initVrHmd() = 0;
	
	///Init the VR client rendering
	virtual void initClientHmdRendering() =0;

	///If true, you should cleanly quit the program from now
	virtual bool shouldQuit() = 0;

	///If true you should use the current position as the new center of tracking
	virtual bool shouldRecenter() = 0;

	///If true, the user can see the VR render inside his HMD
	virtual bool isVisibleInHmd() = 0;

	///Refresh and update the head tracking. May tell the VR client library to prepare for new frame
	virtual void updateTracking() = 0;

	///Render frame internally inside Ogre, and submit it to the VR client
	virtual void renderAndSubmitFrame() = 0;

	///Put the current position as the center of tracking
	virtual void recenter() = 0;

	///Change the color of the pixels when there is *nothing*
	virtual void changeViewportBackgroundColor(Ogre::ColourValue) = 0;

	///(Optional) Cycle through the client debug display if available.
	virtual void cycleDebugHud() {};

	///Set the VR cameras near clipping plane distance
	virtual void setCamerasNearClippingDistance(float distance) = 0;
	
	///Set the VR cameras far clipping plane distance
	virtual void setCameraFarClippingDistance(float distance) = 0;

	///(Optional) return true if audio has to come out from a specific audio device
	virtual bool usesCustomAudioDevice() { return false; }

	///(Optionalà retrun the sub string to search on the audio device list to get the correct one
	virtual std::string getAudioDeviceIdentifierSubString() { return std::string(""); }

	///The current position of the head center defined by the client library projected in World Space
	OgrePose returnPose;

protected:
	///Singleton pointer
	static OgreVRRender* self;

	///SceneManager of the VR world
	Ogre::SceneManager* smgr;

	///Ogre root object
	Ogre::Root* root;

	///Render window. VR isn't drawn to this window. A window is mandatory to init the RenderSystem. 
	Ogre::RenderWindow* window;

	///Update Time
	double updateTime;

	///Distance between eyeCamera and nearClippingDistance
	Ogre::Real nearClippingDistance;

	///Distance between eyeCamera and farClippingDistance
	Ogre::Real farClippingDistance;

	///Position of the head
	Ogre::Vector3 headPosition;

	///Orientation of the head
	Ogre::Quaternion headOrientation;

	///Name of the window
	std::string name;

	///Node that represent the head base. Move this in 3D to move the viewpoint
	Ogre::SceneNode* headNode;

	///background color of viewports
	Ogre::ColourValue backgroundColor;

	///Cameras that have to be put where the user's eye is
	Ogre::Camera* eyeCameras[2];

};