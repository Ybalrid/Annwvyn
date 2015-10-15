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
		void actuate(float deltaTime);
	};
}

#endif