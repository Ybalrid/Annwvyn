#include "stdafx.h"
#include "AnnOgreOpenHMDRenderer.hpp"
#include "Annwvyn.h"

Annwvyn::AnnOgreOpenHMDRenderer* Annwvyn::AnnOgreOpenHMDRenderer::ohmdSelf = nullptr;

Ogre::Vector3 Annwvyn::AnnOgreOpenHMDRenderer::toOgreVector3(const float* v) const
{
	return Ogre::Vector3{ v };
}

Ogre::Quaternion Annwvyn::AnnOgreOpenHMDRenderer::toOgreQuat(const float* v) const
{
	return { v[3], v[0], v[1], v[2] };
}

Annwvyn::AnnOgreOpenHMDRenderer::AnnOgreOpenHMDRenderer(const std::string& windowName) :
 AnnOgreVRRenderer(windowName),
 ctx(nullptr),
 settings(nullptr),
 hmd(nullptr),
 hmd_w(0),
 hmd_h(0),
 ipd(0),
 sep(0),
 warp_scale(0),
 warp_adj(0),
 device_class(0)
{
	rendererName = "OpenGL/OpenHMD";
	ohmdSelf	 = this;
}

Annwvyn::AnnOgreOpenHMDRenderer::~AnnOgreOpenHMDRenderer()
{
}

void Annwvyn::AnnOgreOpenHMDRenderer::initVrHmd()
{
	ctx				 = ohmd_ctx_create();
	auto num_devices = ohmd_ctx_probe(ctx);

	if(num_devices < 0) throw AnnInitializationError(ANN_ERR_CANTHMD, "Couldn't find any devices with OpenHMD");

	AnnDebug() << "Found " << num_devices << " OpenHMD recognised devices";
	for(auto i{ 0 }; i < num_devices; ++i)
	{
		AnnDebug() << ohmd_list_gets(ctx, i, OHMD_VENDOR) << ' '
				   << ohmd_list_gets(ctx, i, OHMD_PRODUCT) << ' '
				   << ohmd_list_gets(ctx, i, OHMD_PATH);
	}

	settings = ohmd_device_settings_create(ctx);

	auto auto_update = 1;
	ohmd_device_settings_seti(settings, OHMD_IDS_AUTOMATIC_UPDATE, &auto_update);

	hmd = ohmd_list_open_device_s(ctx, 0, settings);

	ohmd_device_geti(hmd, OHMD_SCREEN_HORIZONTAL_RESOLUTION, &hmd_w);
	ohmd_device_geti(hmd, OHMD_SCREEN_VERTICAL_RESOLUTION, &hmd_h);
	ohmd_device_getf(hmd, OHMD_EYE_IPD, &ipd);
	ohmd_device_getf(hmd, OHMD_SCREEN_HORIZONTAL_SIZE, &(viewport_scale[0]));
	viewport_scale[0] /= 2.0f;
	ohmd_device_getf(hmd, OHMD_SCREEN_VERTICAL_SIZE, &(viewport_scale[1]));
	//distortion coefficients
	ohmd_device_getf(hmd, OHMD_UNIVERSAL_DISTORTION_K, &(distortion_coeffs[0]));
	ohmd_device_getf(hmd, OHMD_UNIVERSAL_ABERRATION_K, &(aberr_scale[0]));
	//calculate lens centers (assuming the eye separation is the distance between the lens centers)
	ohmd_device_getf(hmd, OHMD_LENS_HORIZONTAL_SEPARATION, &sep);
	ohmd_device_getf(hmd, OHMD_LENS_VERTICAL_POSITION, &(left_lens_center[1]));
	ohmd_device_getf(hmd, OHMD_LENS_VERTICAL_POSITION, &(right_lens_center[1]));
	left_lens_center[0]  = viewport_scale[0] - sep / 2.0f;
	right_lens_center[0] = sep / 2.0f;
	//assume calibration was for lens view to which ever edge of screen is further away from lens center
	warp_scale = (left_lens_center[0] > right_lens_center[0]) ? left_lens_center[0] : right_lens_center[0];
	warp_adj   = 1.0f;

	ohmd_device_geti(hmd, OHMD_DEVICE_CLASS, &device_class);

	ohmd_device_settings_destroy(settings);

	AnnDebug() << "HMD Raw Parameters:";
	AnnDebug() << " - HMD size: " << hmd_w << 'x' << hmd_h;
	AnnDebug() << " - Viewport Scale: " << viewport_scale[0] << 'x' << viewport_scale[1];
	AnnDebug() << " - IPD: " << ipd;
	AnnDebug() << " - Distortion coeff: " << distortion_coeffs[0] << ' ' << distortion_coeffs[1] << ' ' << distortion_coeffs[2] << ' ' << distortion_coeffs[3];
	AnnDebug() << " - Aberation Scale: " << aberr_scale[0] << ' ' << aberr_scale[1] << ' ' << aberr_scale[2];
	AnnDebug() << " - Lens center: left=" << left_lens_center[0] << 'x' << left_lens_center[1] << " right=" << right_lens_center[0] << 'x' << right_lens_center[1];
	AnnDebug() << " - Warp scale: " << warp_scale;

	if(!hmd) throw AnnInitializationError(ANN_ERR_CANTHMD, "Failed to open device: " + std::string(ohmd_ctx_get_error(ctx)));
}

void Annwvyn::AnnOgreOpenHMDRenderer::initScene()
{
	createMainSmgr();
}

void Annwvyn::AnnOgreOpenHMDRenderer::initRttRendering()
{
	loadOpenGLFunctions();

	const combinedTextureSizeArray textureDimentions{ { { { size_t(hmd_w / 2), size_t(hmd_h) } }, { { size_t(hmd_w / 2), size_t(hmd_h) } } } };
	ogreTextures = createSeparatedRenderTextures(textureDimentions);

	{
		auto compositor = root->getCompositorManager2();

		compositorWorkspaces[leftEyeCompositor]  = compositor->addWorkspace(smgr, rttEyeSeparated[left], eyeCameras[left], "HdrWorkspace", true);
		compositorWorkspaces[rightEyeCompositor] = compositor->addWorkspace(smgr, rttEyeSeparated[right], eyeCameras[right], "HdrWorkspace", true);
		compositorWorkspaces[monoCompositor]	 = compositor->addWorkspace(smgr, window, monoCam, "HdrWorkspace", true);
	}
}

void Annwvyn::AnnOgreOpenHMDRenderer::initClientHmdRendering()
{
}

bool Annwvyn::AnnOgreOpenHMDRenderer::shouldQuit()
{
	return false;
}

bool Annwvyn::AnnOgreOpenHMDRenderer::shouldRecenter()
{
	return false;
}

bool Annwvyn::AnnOgreOpenHMDRenderer::isVisibleInHmd()
{
	return true;
}

void Annwvyn::AnnOgreOpenHMDRenderer::getTrackingPoseAndVRTiming()
{
	calculateTimingFromOgre();

	ohmd_ctx_update(ctx);
	ohmd_device_getf(hmd, OHMD_POSITION_VECTOR, vectorBuffer);
	ohmd_device_getf(hmd, OHMD_ROTATION_QUAT, quaternionBuffer);

	trackedHeadPose.orientation = bodyOrientation * toOgreQuat(quaternionBuffer);
	trackedHeadPose.position	= feetPosition + bodyOrientation * (toOgreVector3(vectorBuffer) + Ogre::Vector3{ 0, 1.65f, 0 });
}

void Annwvyn::AnnOgreOpenHMDRenderer::renderAndSubmitFrame()
{
	handleWindowMessages();
	root->renderOneFrame();
}

void Annwvyn::AnnOgreOpenHMDRenderer::recenter()
{
}

void Annwvyn::AnnOgreOpenHMDRenderer::updateProjectionMatrix()
{
}

void Annwvyn::AnnOgreOpenHMDRenderer::showDebug(DebugMode mode)
{
}

void Annwvyn::AnnOgreOpenHMDRenderer::handleIPDChange()
{
	ohmd_device_getf(hmd, OHMD_EYE_IPD, &ipd);
	eyeCameras[left]->setPosition(-ipd / 2, 0, 0);
	eyeCameras[right]->setPosition(+ipd / 2, 0, 0);
}

void Annwvyn::AnnOgreOpenHMDRenderer::initCameras()
{
	AnnOgreVRRenderer::initCameras();
}

void Annwvyn::AnnOgreOpenHMDRenderer::cycleDebugHud()
{
}

bool Annwvyn::AnnOgreOpenHMDRenderer::usesCustomAudioDevice()
{
	return false;
}

std::string Annwvyn::AnnOgreOpenHMDRenderer::getAudioDeviceIdentifierSubString()
{
	return {};
}

Annwvyn::AnnOgreVRRenderer* AnnRendererBootstrap_OpenHMD(const std::string& appName)
{
	return static_cast<Annwvyn::AnnOgreVRRenderer*>(new Annwvyn::AnnOgreOpenHMDRenderer(appName));
}
