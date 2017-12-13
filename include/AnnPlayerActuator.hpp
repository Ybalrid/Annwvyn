/**
* \file AnnPlayerActuator.hpp
* \brief Player Actuator permit to change the code run by the player class when updating player (position/rotation, etc)
* The VR point of view is relative to the player object.
* \author A. Brainville (Ybalrid)
*/
#pragma once

#include "systemMacro.h"

namespace Annwvyn
{
	class AnnPlayerBody;

	///Callback class that permit you to define how physics is applied to the player.
	class AnnDllExport AnnPlayerActuator
	{
	public:
		virtual ~AnnPlayerActuator();
		///Construct a player actuator
		AnnPlayerActuator();

		///Overload this method to define how the player object should react to physics
		virtual void actuate(float deltaTime) = 0;
	protected:
		AnnPlayerBody* player;
		friend class AnnPlayerBody;
	};

	///Class that overload the "actuate" method of the abstract actuator.
	class AnnDllExport AnnDefaultPlayerActuator : public AnnPlayerActuator
	{
	public:
		///Construct the default player actuator
		AnnDefaultPlayerActuator();
		///Allow gravity to pull the player to the ground while permitting the user to control movement on the (XZ) plane
		void actuate(float deltaTime) override;
	};
}
