#pragma once

#include <Annwvyn.h>
#include "TutorialLevel.hpp"

using namespace Annwvyn;

///Super simple example using a time event to do something
class TutorialTimer : public TutorialLevel, LISTENER //This level class is also a listener
{
public:
	TutorialTimer() :
	 TutorialLevel(), constructListener(),
	 waitFor { -1 },
	 rotator(nullptr)
	{
	}

	//Load the level
	void load() override
	{
		auto eventManager = AnnGetEventManager();

		rotator = addGameObject("sword.mesh").get();
		rotator->setPosition(-3, 1, 8);
		eventManager->addListener(getSharedListener());

		loadBasicTutorialLevel();

		loadTextPannel(
			"TutorialTimer:\n"
			"--------------\n"
			"On this sample, we are showing how to fire \"timers\" and catch the timeout event generated. "
			"You should look at TutorialTimer.hpp to see how it works, but basically:\n"
			" - At any point, you can call \"fireTimer(Millisec)\" on the event manager with a duration\n"
			" - Any listener can override the \"TimeEvent\" method to get timer events\n"
			" - \"fireTimer\" returned a timer ID, cheeking this ID with the event received by the listener permit "
			"you to know if the timer you are looking for is the one that expired\n");

		//This will start a timer inside the event manager. fireTimer returns an ID for this timer. The time event
		//will have this ID that you can test against. Here we start a first timer that will timeout one second later
		waitFor = eventManager->fireTimer(1);

		//Look at the TimeEvent method to see how to intercept and do stuff with the time event
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

	//When this method is called "a" timer (any one of them) did timeout. Test the "getID()" value of the event to get the ID you want
	void TimeEvent(AnnTimeEvent e) override
	{
		//Seeing if this event is really the one we are looking for
		if(waitFor == e.getID())
		{
			//You can start a timer event from anywhere, even inside the "responce" from a TimeEvent.
			AnnDebug() << "Timer " << waitFor << " expired!";
			waitFor = AnnGetEventManager()->fireTimer(1);

			//Here we are going through the angles stored in the "angles" array, just to turn the object pointed by rotator
			static auto counter = 0;
			counter				= ++counter % angles.size();
			rotator->setOrientation({ AnnDegree(angles[counter]), AnnVect3::UNIT_Y });
		}
	}

private:
	AnnTimerID waitFor;
	AnnGameObject* rotator;
	const std::array<float, 4> angles { 0, 90, 180, -90 };
};
