#pragma once

#include <Annwvyn.h>
#include "TutorialLevel.hpp"

using namespace Annwvyn;

class TutorialTimer : public TutorialLevel, LISTENER
{
public:
	TutorialTimer() : TutorialLevel(), constructListener(),
		waitFor{ -1 }
	{
	}

	void load() override
	{
		auto eventManager = AnnGetEventManager();

		rotator = addGameObject("sword.mesh").get();
		rotator->setPosition(-3, 1, 8);
		waitFor = eventManager->fireTimer(1);
		eventManager->addListener(getSharedListener());

		loadBasic();

		loadTextPannel(
			"TutorialTimer:\n"
			"--------------\n"
			"On this sample, we are showing how to fire \"timers\" and catch the timeout event generated. "
			"You should look at TutorialTimer.hpp to see how it works, but basically:\n"
			//"\n"
			" - At any point, you can call \"fireTimer(Millisec)\" on the event manager with a duration\n"
			" - Any listener can override the \"TimeEvent\" method to get timer events\n"
			" - \"fireTimer\" returned a timer ID, cheeking this ID with the event received by the listener permit "
			"you to know if the timer you are looking for is the one that expired\n"
		);
	}

	void unload() override
	{
		waitFor = -1;
		AnnGetEventManager()->removeListener(getSharedListener());

		TutorialLevel::unload();
	}

	void runLogic() override
	{
	}

	void TimeEvent(AnnTimeEvent e) override
	{
		if (waitFor == e.getID())
		{
			AnnDebug() << "Timer " << waitFor << " expired!";
			waitFor = AnnGetEventManager()->fireTimer(1);

			static auto counter = 0;
			counter = ++counter % angles.size();

			rotator->setOrientation({ AnnDegree(angles[counter]), AnnVect3::UNIT_Y });
		}
	}

private:
	timerID waitFor;
	AnnGameObject* rotator;
	const std::array<float, 4> angles{ 0, 90, 180, -90 };
};