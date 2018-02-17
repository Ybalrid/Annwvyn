/**
 * \file AnnAnnOculusInterfaceHelper.hpp
 * \brief Simple class for accessing Oculus Rift data
 * \author A. Brainville (Ybalrid)
 */

#pragma once

#include "systemMacro.h"

//Oculus VR API
#include <OVR_CAPI.h>
#include <Extras/OVR_Math.h>
#include <Ogre.h>
#include <AnnException.hpp>

///Communicate with the Rift runtime (initialize OVR and get the info)
class AnnDllExport AnnOculusInterfaceHelper
{
public:
	///Construct an AnnOculusInterfaceHelper object. This create an Oculus Session for communication with the Oculus Runtime
	AnnOculusInterfaceHelper();

	///Destructor of Oculus Interface
	~AnnOculusInterfaceHelper();

	///Return the active HmdDesc object
	ovrHmdDesc getHmdDesc() const;

	///Return the current oculus session
	ovrSession getSession() const;

	///Return the eye's height of the current user
	float getUserEyeHeight() const;

	///Recenter the tracking origin
	void recenterTrackingOrigin() const;

	///Set the performance HUD mode
	void setPerfHudMode(ovrPerfHudMode mode) const;

	///get HMD resolution in the Oculus format
	ovrSizei getHmdResolution() const;

	///get HMD Refresh rate. e.g. 90 (or 89.99) for an Oculus Rift CV1
	float getHmdDisplayRefreshRate() const;

	///Set the reference point to be a point on the ground, not the "zero" position of the headset
	void setTrackingOriginToFloorLevel() const;

private:
	///Print every known characteristics about the hardware to the log output
	void logHardwareReport() const;

	///Abort, display error, log it and crash the game
	void abortOnFailure();

	///Oculus session for the application
	ovrSession session;

	///Description of the HMD
	ovrHmdDesc hmdDesc;

	///Graphics device identifier (internal for the Rift SDK)
	ovrGraphicsLuid luid;
};
