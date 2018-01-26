#pragma once

#include "systemMacro.h"
#include "AnnEvents.hpp"

namespace Annwvyn
{
	class AnnPlayerBody;

	///Base class for all event listener
	class AnnDllExport AnnEventListener : public std::enable_shared_from_this<AnnEventListener>
	{
		//Base Event listener class. Technically not abstract since it provides a default implementation for all
		//virtual members. But theses definitions are pointless because they actually don't do anything.
		//You need to subclass it to create an EventListener

	public:
		virtual ~AnnEventListener();

		///Construct a listener
		AnnEventListener();
		///Event from the keyboard
		virtual void KeyEvent(AnnKeyEvent e) {}
		///Event from the mouse
		virtual void MouseEvent(AnnMouseEvent e) {}
		///Event for a Joystick
		virtual void StickEvent(AnnStickEvent e) {}
		///Event from a timer
		virtual void TimeEvent(AnnTimeEvent e) {}
		///Event from a trigger
		virtual void TriggerEvent(AnnTriggerEvent e) {}
		///Event from an HandController
		virtual void HandControllerEvent(AnnHandControllerEvent e) {}
		///Event from detected collisions
		virtual void CollisionEvent(AnnCollisionEvent e) {}
		///Event from detected player collisions
		virtual void PlayerCollisionEvent(AnnPlayerCollisionEvent e) {}
		///Events from code outside of Annwvyn itself
		virtual void EventFromUserSubsystem(AnnUserSpaceEvent& e, AnnUserSpaceEventLauncher* origin) {}
		///This method is called at each frame. Useful for updating player's movement command for example
		virtual void tick() {}
		///Utility function for applying a dead-zone on a joystick axis
		static float trim(float value, float deadzone);
		///return a shared_ptr to this listener
		std::shared_ptr<AnnEventListener> getSharedListener();

	protected:
		///Pointer to the player. Set by the constructor, provide easy access to the AnnPlayerBody
		AnnPlayerBody* player;
	};

	using AnnEventListenerPtr	 = std::shared_ptr<AnnEventListener>;
	using AnnEventListenerWeakPtr = std::weak_ptr<AnnEventListener>;
}
