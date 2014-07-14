#ifndef OCULUS_INTERFACE 
#define OCULUS_INTERFACE 

#include <iostream>
#include <OVR.h>
#include <OVR_CAPI.h>

using namespace std;
using namespace OVR;

class OculusInterface
{
    public:
    OculusInterface();
    ~OculusInterface();

    ///Update. We use the predicted values from the rift. Therefore, we want the timecode of the "pose". Default : exact current time
    void update(double time = ovr_GetTimeInSeconds());
    
    ///Return a position vector
    OVR::Vector3f getPosition();
    
    ///Return a quaternion orentetion
    OVR::Quatf getOrientation();

    ovrHmdDesc getHmdDesc();

    ovrHmd getHmd();
    
    ///Print debuggin information to standard input;
    void debugPrint();

    private:
    void customReport();
    void init();
    void shutdown();

    private:
    bool initialized;
    bool firstUpdated;

    ovrHmd hmd;
    ovrHmdDesc hmdDesc; 
    ovrSensorState ss;
};

#endif
