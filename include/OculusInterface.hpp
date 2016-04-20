/**
 * \file OculusInterface.hpp
 * \brief Simple class for accessing Oculus Rift data
 * \author A. Brainville (Ybalrid)
 */

#ifndef OCULUS_INTERFACE 
#define OCULUS_INTERFACE 

#include <iostream>

//Oculus VR API
//#include <OVR.h>
#include <OVR_CAPI.h>
#include <Extras/OVR_Math.h>
#include "systemMacro.h"
#include "AnnErrorCode.hpp"
#include <Ogre.h>
#define USE_OGRE

///Comunicate with the Rift runtime (initialize OVR and get the info)
class DLL OculusInterface
{
    public:

	///Construct an OculusInterface object. This create an Oculus Session for comunication with the Oculus Runtime
    OculusInterface();

	///Destructor of Oculus Interface
    ~OculusInterface();

    ///Update. We use the predicted values from the rift. Therefore, we want the timecode of the "pose". Default : exact current time
    void update(double time = ovr_GetTimeInSeconds());
    
    ///Return a position vector
    OVR::Vector3f getPosition();
    
    ///Return a quaternion orentetion
    OVR::Quatf getOrientation();

	///Return the active hmd desk object
    ovrHmdDesc getHmdDesc();

	///Return the current oculus session
	ovrSession getSession();
    
    ///Print debuggin information to standard input;
    void debugPrint();

    private:

	///Print to the log all information about the headset
    void customReport();

	///Init the oculus library
    void init();
	
	///Shutdown the oculus library
    void shutdown();

    private:

	///set to true when it's safe to call ovr_ function on the ovrSession
    bool initialized;

	///This object know a trackingstate from the Oculus Service
    bool firstUpdated;
	
	///Oculus session for the application
    ovrSession session;

	///Description of the HMD
    ovrHmdDesc hmdDesc; 

	///Tracked state of the HMD
	ovrTrackingState ts;

	///Graphics device identifier (internal for the Rift SDK)
	ovrGraphicsLuid luid;
};
#endif
