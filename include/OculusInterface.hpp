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

#include "systemMacro.h"
#include "AnnErrorCode.hpp"
#include <Ogre.h>
#define USE_OGRE
using namespace std;
//using namespace OVR;

///Comunicate with the Rift (initialize OVR and get the info)
class DLL OculusInterface
{
    public:
    OculusInterface();
    ~OculusInterface();

    ///Update. We use the predicted values from the rift. Therefore, we want the timecode of the "pose". Default : exact current time
    void update(double time = ovr_GetTimeInSeconds());
    
    ///Return a position vector
    //OVR::Vector3f getPosition();
    
    ///Return a quaternion orentetion
    //OVR::Quatf getOrientation();

	///Return the active hmd desk object
    ovrHmdDesc getHmdDesc();

	///Return the active hmd object
    //ovrHmd getHmd();
    
    ///Print debuggin information to standard input;
    //void debugPrint();
	ovrSession getSession();

    private:
	///Print to the log all information about the headset
    void customReport();
	///Init the oculus library
    void init();
	
	///Shutdown the oculus library
    void shutdown();




    private:
    bool initialized;
    bool firstUpdated;
	
    ovrSession hmd;
    ovrHmdDesc hmdDesc; 
	ovrTrackingState ss;
#ifdef _WIN32
	ovrGraphicsLuid luid;
	
#endif
};
#endif
