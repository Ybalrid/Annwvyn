/**
* \file AnnPlayerActuator.hpp
* \brief Player Actuator permit to change the code run by the player class when updating player (position/rotation, etc)
* The VR point of view is relative to the player object. 
* \author A. Brainville (Ybalrid)
*/
#ifndef PLAYER_ACTUATOR
#define PLAYER_ACTUATOR
#include "systemMacro.h"

namespace Annwvyn
{
	class AnnPlayer;

	///Callback class that permit you to define how physics is appliyed to the player.
	class DLL AnnPlayerActuator
	{
	public:
		///Construct a player actuator
		AnnPlayerActuator();

		///Overload this method to define how the player object should react to physics
		virtual void actuate(float deltaTime) = 0;
	protected:
		AnnPlayer* player;
		friend class AnnPlayer;
	};

	///Class that overload the "actuate" method of the abstract actuator.
	class DLL AnnDefaultPlayerActuator : public AnnPlayerActuator
	{
	public:
		///Construct the default player actuator
		AnnDefaultPlayerActuator();
		///Allow gravity to pull the player to the ground while permiting the user to controll mouvement on the (XZ) plane
		void actuate(float deltaTime);
	};
}

#endif