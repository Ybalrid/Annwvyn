#pragma once
#include <systemMacro.h>
#include <AnnOgreVRRenderer.hpp>
#include <openhmd.h>

namespace Annwvyn
{
	class AnnDllExport AnnOgreOpenHMDRenderer : public AnnOgreVRRenderer
	{
		static AnnOgreOpenHMDRenderer* ohmdSelf;
		ohmd_context* ctx;
		ohmd_device_settings* settings;
		ohmd_device* hmd;

		int hmd_w, hmd_h;
		float ipd;
		float viewport_scale[2];
		float distortion_coeffs[4];
		float aberr_scale[3];
		float sep;
		float left_lens_center[2];
		float right_lens_center[2];
		float warp_scale, warp_adj;
		int device_class;

		float vectorBuffer[3];
		float quaternionBuffer[4];

		std::array<GLuint, 2> ogreTextures;

		enum ohmdSide : uint8_t { left  = 0x0,
								  right = 0x1 };

		Ogre::Vector3 toOgreVector3(const float* v) const;

		Ogre::Quaternion toOgreQuat(const float* v) const;

	public:
		AnnOgreOpenHMDRenderer(const std::string& windowName);
		~AnnOgreOpenHMDRenderer() override;
		void initVrHmd() override;
		void initScene() override;
		void initRttRendering() override;
		void initClientHmdRendering() override;
		bool shouldQuit() override;
		bool shouldRecenter() override;
		bool isVisibleInHmd() override;
		void getTrackingPoseAndVRTiming() override;
		void renderAndSubmitFrame() override;
		void recenter() override;
		void updateProjectionMatrix() override;
		void showDebug(DebugMode mode) override;
		void handleIPDChange() override;
		void initCameras() override;
		void cycleDebugHud() override;
		bool usesCustomAudioDevice() override;
		std::string getAudioDeviceIdentifierSubString() override;
	};
}

extern "C" AnnDllExport Annwvyn::AnnOgreVRRenderer* AnnRendererBootstrap_OpenHMD(const std::string& appName);
