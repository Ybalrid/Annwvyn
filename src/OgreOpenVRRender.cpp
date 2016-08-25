#include "stdafx.h"
#include "OgreOpenVRRender.hpp"
#include "AnnLogger.hpp"

OgreOpenVRRender* OgreOpenVRRender::OpenVRSelf(nullptr);

OgreOpenVRRender::OgreOpenVRRender(std::string winName) : OgreVRRender(winName),
vrSystem(nullptr),
vrCompositor(nullptr),
windowWidth(1280),
windowHeight(720),
gamma(false),
API(vr::API_OpenGL)

{
	OpenVRSelf = static_cast<OgreOpenVRRender*>(self);
}

OgreOpenVRRender::~OgreOpenVRRender()
{
	vr::VR_Shutdown();
}

void OgreOpenVRRender::initPipeline()
{
	getOgreConfig();
	createWindow();
	initScene();
	initCameras();
	setCamerasNearClippingDistance();
	initRttRendering();
}

void OgreOpenVRRender::initVrHmd()
{
	vrSystem = vr::VR_Init(&hmdError, vr::EVRApplicationType::VRApplication_Scene);
	if (hmdError != vr::VRInitError_None)
	{
		switch (hmdError)
		{
			default:
				displayWin32ErrorMessage(L"Error: failed OpenVR VR_Init",
										 L"Undescribed error when initalizing the OpenVR Render object");
				exit(ANN_ERR_NOTINIT);
			case vr::VRInitError_Init_HmdNotFound:
				displayWin32ErrorMessage(L"Error: cannot find HMD",
										 L"OpenVR cannot find HMD.\n"
										 L"Please install SteamVR and verrify HMD USB and HDMI connection");
				exit(ANN_ERR_CANTHMD);

		}
	}

	if (!vr::VRCompositor())
	{
		displayWin32ErrorMessage(L"Error: failed to init OpenVR VRCompositor",
								 L"Failed to initialize the VR Compositor");
		exit(ANN_ERR_NOTINIT);
	}




}

void OgreOpenVRRender::initClientHmdRendering()
{
}

bool OgreOpenVRRender::shouldQuit()
{
	return false;
}

bool OgreOpenVRRender::shouldRecenter()
{
	return false;
}

bool OgreOpenVRRender::isVisibleInHmd()
{
	return true;
}

void OgreOpenVRRender::updateTracking()
{
}

void OgreOpenVRRender::renderAndSubmitFrame()
{
	Ogre::WindowEventUtilities::messagePump();

}

void OgreOpenVRRender::recenter()
{
}

void OgreOpenVRRender::changeViewportBackgroundColor(Ogre::ColourValue color)
{
}

void OgreOpenVRRender::setCamerasNearClippingDistance(float distance)
{
	if (distance <= 0) return;
	nearClippingDistance = distance;
	getProjectionMatrix();
}

void OgreOpenVRRender::setCameraFarClippingDistance(float distance)
{
	if (distance <= 0) return;
	farClippingDistance = distance;
	getProjectionMatrix();
}

void OgreOpenVRRender::showDebug(DebugMode mode)
{
	return;
}

void OgreOpenVRRender::createWindow()
{
	if (!root) exit(ANN_ERR_NOTINIT);

	//Basic window configuration
	Ogre::NameValuePairList misc;
	misc["vsync"] = "false"; //This vsync parameter has no scence in VR. The display is done by the Compositor
	misc["top"] = "0";
	misc["left"] = "0";

	//manual ogre init
	root->initialise(false);

	window = root->createRenderWindow(name + " : Vive debug mirror view. Please put on HMD.",
									  windowWidth, windowHeight,
									  false, &misc);

}

void OgreOpenVRRender::initScene()
{
	//Create the scene manager for the engine
	if (!root) exit(ANN_ERR_NOTINIT);
	smgr = root->createSceneManager("OctreeSceneManager", "OSM_SMGR");
	smgr->setShadowTechnique(Ogre::ShadowTechnique::SHADOWTYPE_STENCIL_ADDITIVE);
}

void OgreOpenVRRender::initCameras()
{
	//VR Eye cameras
	eyeCameras[left] = smgr->createCamera("lcam");
	eyeCameras[left]->setPosition(headPosition);
	eyeCameras[left]->setAutoAspectRatio(true);
	eyeCameras[right] = smgr->createCamera("rcam");
	eyeCameras[right]->setPosition(headPosition);
	eyeCameras[right]->setAutoAspectRatio(true);

	//do NOT attach camera to this node...
	headNode = smgr->getRootSceneNode()->createChildSceneNode();
}

void OgreOpenVRRender::initRttRendering()
{
	//Init GLEW here to be able to call OpenGL functions
	Annwvyn::AnnDebug() << "Init GL Extension Wrangler";
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		Annwvyn::AnnDebug("Failed to glewTnit()\n\
						  Cannot call manual OpenGL\n\
						  Error Code : " + (unsigned int)err);
		exit(ANN_ERR_RENDER);
	}
	Annwvyn::AnnDebug() << "Using GLEW version : " << glewGetString(GLEW_VERSION);

	unsigned int w, h;
	vrSystem->GetRecommendedRenderTargetSize(&w, &h);

	Annwvyn::AnnDebug() << "Recomended Render Target Size : " << w << "x" << h;

	//When OgreVRRender() intialize Ogre, the OpenGL RenderSystem is loaded in hard.
	//We don't need to check that we're using OpenGL before doing this kind of cast:
	Ogre::GLTextureManager* textureManager = static_cast<Ogre::GLTextureManager*>(Ogre::TextureManager::getSingletonPtr());

	rttTexture[left] = textureManager->createManual("RTT_TEX_L", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
													Ogre::TEX_TYPE_2D, w, h, 0, Ogre::PF_R8G8B8, Ogre::TU_RENDERTARGET, nullptr, gamma);

	rttTextureGLID[left] = static_cast<Ogre::GLTexture*>(textureManager->getByName("RTT_TEX_L").getPointer())->getGLID();

	rttTexture[right] = textureManager->createManual("RTT_TEX_R", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
													 Ogre::TEX_TYPE_2D, w, h, 0, Ogre::PF_R8G8B8, Ogre::TU_RENDERTARGET, nullptr, gamma);

	rttTextureGLID[right] = static_cast<Ogre::GLTexture*>(textureManager->getByName("RTT_TEX_R").getPointer())->getGLID();


	rttTexture[left]->getBuffer()->getRenderTarget()->addViewport(eyeCameras[left]);
	rttTexture[right]->getBuffer()->getRenderTarget()->addViewport(eyeCameras[right]);


}

void OgreOpenVRRender::getProjectionMatrix()
{
	vr::HmdMatrix44_t prj[2]
		= { vrSystem->GetProjectionMatrix(getEye(left), nearClippingDistance, farClippingDistance, API),
		vrSystem->GetProjectionMatrix(getEye(right), nearClippingDistance, farClippingDistance, API) };

	for (char eye(0); eye < 2; eye++) for (char i(0); i < 4; i++) for (char j(0); j < 4; j++)
	{
		Ogre::Matrix4 m;
		m[i][j] = prj[eye].m[i][j];
		eyeCameras[eye]->setCustomProjectionMatrix(true, m);
	}

}
