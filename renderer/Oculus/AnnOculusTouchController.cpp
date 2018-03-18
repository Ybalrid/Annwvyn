// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "AnnOculusTouchController.hpp"
using namespace Annwvyn;

void AnnOculusTouchController::rumbleStart(float factor)
{
	ovr_SetControllerVibration(currentSession, myControllerType, 0, factor);
}

void AnnOculusTouchController::rumbleStop()
{
	ovr_SetControllerVibration(currentSession, myControllerType, 0, 0);
}

AnnOculusTouchController::AnnOculusTouchController(ovrSession session,
												   Ogre::SceneNode* handNode,
												   AnnHandControllerID controllerID,
												   AnnHandControllerSide controllerSide) :
 AnnHandController("Oculus Touch", handNode, controllerID, controllerSide),
 currentSession(session)
{
	if(side == leftHandController)
		myControllerType = ovrControllerType_LTouch;
	else if(side == rightHandController)
		myControllerType = ovrControllerType_RTouch;

	capabilites = RotationalTracking | PositionalTracking | AngularAccelerationTracking
		| LinearAccelerationTracking | ButtonInputs | AnalogInputs | HapticFeedback | DiscreteHandGestures;
}
