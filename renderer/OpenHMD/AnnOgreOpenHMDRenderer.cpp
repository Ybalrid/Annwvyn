#include "AnnOgreOpenHMDRenderer.hpp"
#include "Annwvyn.h"

Annwvyn::AnnOgreOpenHMDRenderer::AnnOgreOpenHMDRenderer(const std::string& windowName) :
 AnnOgreVRRenderer(windowName), ctx(nullptr), settings(nullptr), hmd(nullptr), hmd_w(0), hmd_h(0), ipd(0), sep(0), warp_scale(0), warp_adj(0)
{
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
		AnnDebug() << ohmd_list_gets(ctx, i, OHMD_VENDOR);
		AnnDebug() << ohmd_list_gets(ctx, i, OHMD_PRODUCT);
		AnnDebug() << ohmd_list_gets(ctx, i, OHMD_PATH);
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

	ohmd_device_settings_destroy(settings);

	if(!hmd) throw AnnInitializationError(ANN_ERR_CANTHMD, "Failed to open device: " + std::string(ohmd_ctx_get_error(ctx)));
}

void Annwvyn::AnnOgreOpenHMDRenderer::initScene()
{
}

void Annwvyn::AnnOgreOpenHMDRenderer::initRttRendering()
{
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
	return false;
}

void Annwvyn::AnnOgreOpenHMDRenderer::getTrackingPoseAndVRTiming()
{
}

void Annwvyn::AnnOgreOpenHMDRenderer::renderAndSubmitFrame()
{
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
}

void Annwvyn::AnnOgreOpenHMDRenderer::initCameras()
{
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
