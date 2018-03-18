// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#pragma once

#include "AnnOpenVRMotionController.hpp"
#include <AnnGetter.hpp>

using namespace Annwvyn;

void AnnOpenVRMotionController::rumbleStart(float value)
{
	current = AnnGetVRRenderer()->getTimer()->getMilliseconds();

	//wait at lest 50 milliesconds
	if(current - last > 50)
	{
		last = current;
		//Max value of one pulse will be 3500µs
		vrSystem->TriggerHapticPulse(deviceIndex,
									 vr::EVRButtonId::k_EButton_SteamVR_Touchpad - vr::k_EButton_Axis0,
									 static_cast<unsigned short>(value * 3500));
	}
}

void AnnOpenVRMotionController::rumbleStop()
{
	//NB : The "rumbeling" of OpenVR controllers is pulse based. Meaning that telling it to "not move" doesn't make much sense.
}

AnnOpenVRMotionController::AnnOpenVRMotionController(vr::IVRSystem* vrsystem,
													 vr::TrackedDeviceIndex_t OpenVRDeviceIndex,
													 Ogre::SceneNode* handNode,
													 AnnHandControllerID controllerID,
													 AnnHandControllerSide controllerSide) :
 AnnHandController("OpenVR Hand Controller",
				   handNode,
				   controllerID,
				   controllerSide),
 deviceIndex(OpenVRDeviceIndex),
 vrSystem(vrsystem),
 last(0),
 current(0)
{
	capabilites = RotationalTracking
		| PositionalTracking
		| AngularAccelerationTracking
		| LinearAccelerationTracking
		| ButtonInputs
		| AnalogInputs
		| HapticFeedback;
}
