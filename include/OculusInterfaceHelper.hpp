/**
 * \file OculusInterfaceHelper.hpp
 * \brief Simple class for accessing Oculus Rift data
 * \author A. Brainville (Ybalrid)
 */

#ifndef OCULUS_INTERFACE
#define OCULUS_INTERFACE

#include <iostream>

 //Oculus VR API
#include <OVR_CAPI.h>
#include <Extras/OVR_Math.h>

#include <Ogre.h>
#include "systemMacro.h"
#include "AnnErrorCode.hpp"

///Communicate with the Rift runtime (initialize OVR and get the info)
class AnnDllExport OculusInterfaceHelper
{
public:

	///Construct an OculusInterfaceHelper object. This create an Oculus Session for communication with the Oculus Runtime
	OculusInterfaceHelper();

	///Destructor of Oculus Interface
	~OculusInterfaceHelper();

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
#endif