#pragma once

#include <systemMacro.h>
#include <AnnHandController.hpp>

#include <openvr.h>
#include <openvr_capi.h>

namespace Annwvyn
{

	///Specialization of AnnHandController for an OpenVR Motion Controller
	class AnnDllExport AnnOpenVRMotionController : public AnnHandController
	{
	public:
		///Needs a pointer to the currently initialized IVRSystem, and the raw TrackedDeviceIndex of the controller
		AnnOpenVRMotionController(vr::IVRSystem* vrsystem,
								  vr::TrackedDeviceIndex_t OpenVRDeviceIndex,
								  Ogre::SceneNode* handNode,
								  AnnHandControllerID controllerID,
								  AnnHandControllerSide controllerSide);

		///This will trigger one impulse of the hap tics actuator by calling VrSystem->TriggerHapticPulse
		void rumbleStart(float value) override;

		///This will trigger one impulse of duration 0 (as in : asking not to move)
		void rumbleStop() override;

	private:
		///OpenVR device index of this controller
		const vr::TrackedDeviceIndex_t deviceIndex;

		///Pointer to the vrSystem
		vr::IVRSystem* vrSystem;

		///To measure some time
		long last, current;
	};
}
