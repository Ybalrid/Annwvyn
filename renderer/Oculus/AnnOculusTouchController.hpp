#pragma once
#ifdef _WIN32

#include <systemMacro.h>
#include <AnnHandController.hpp>
#include <OVR_CAPI.h>

namespace Annwvyn
{
	///Specialization fo the HandController class for Oculus Touch
	class AnnDllExport AnnOculusTouchController : public AnnHandController
	{
	public:
		///Need to get the oculus session of the controller
		AnnOculusTouchController(ovrSession session, Ogre::SceneNode* handNode, AnnHandControllerID controllerID, AnnHandControllerSide controllerSide);

		///This will call ovr_SetControllerVibration
		void rumbleStart(float factor) override;

		///This will call ovr_SetControllerVibration
		void rumbleStop() override;

	private:
		///Current Oculus session
		ovrSession currentSession;

		///ovrControllerType (left or right touch controller)
		ovrControllerType myControllerType;
	};
}

#endif
