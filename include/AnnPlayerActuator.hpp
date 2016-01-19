/**
 * \file AnnPlayerActuator.hpp
 * \brief Player Actuator permit to change the code run by the player class when updating player (position/rotation, etc)
		  The VR point of view is relative to the player object. 
 * \author A. Brainville (Ybalrid)
 */
#ifndef PLAYER_ACTUATOR
#define PLAYER_ACTUATOR
#include "systemMacro.h"

namespace Annwvyn
{
	class AnnPlayer;

	class DLL AnnPlayerActuator
	{
	public:
		AnnPlayerActuator();
		virtual void actuate(float deltaTime) = 0;
	protected:
		AnnPlayer* player;
		friend class AnnPlayer;
	};

	class DLL AnnDefaultPlayerActuator : public AnnPlayerActuator
	{
	public:
		AnnDefaultPlayerActuator();
		///Allow gravity to pull the player to the ground while permiting the player to move on the (XZ) plane
		void actuate(float deltaTime);
	};
}

#endif