#pragma once
#include <systemMacro.h>
#include <AnnOgreVRRenderer.hpp>

namespace Annwvyn
{
	class AnnDllExport AnnOgreOpenHMDRenderer : public AnnOgreVRRenderer
	{
	public:
		explicit AnnOgreOpenHMDRenderer(const std::string& windowName);
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
