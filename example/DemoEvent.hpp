#pragma once
#include <Annwvyn.h>
#include "TutorialLevel.hpp"

// This class define an "user space" event. An event type that can be created from "user space" code
// For doing that, you just need to inherit from AnnUserSpaceEvent.
//
// The interesting bit here is on the constructor. You pass an unique string that identify the type of this
// Event.
//
// This type string is hashed and stored. Listeners will test against this hash to check if an event is of a type
// They are interested.
class TextMessageEvent : public Annwvyn::AnnUserSpaceEvent
{
public:
	//Call the constructor of AnnUserSpaceEvent with the string "TextMessage" to identify this type of event.
	TextMessageEvent(std::string message) : AnnUserSpaceEvent("TextMessage"),
		enclosedMessage(message)
	{}

	//Getter for retrieving the message
	std::string getMessage() const { return enclosedMessage; };

private:
	std::string enclosedMessage;
};

// This is an event listener defined has any event listener, in the same way you'll define any event listener for Annwvyn.
class SomeEventListener : LISTENER
{
public:
	//Since we're testing against that hash, we should store it to have it handy in a const member.
	SomeEventListener() : constructListener(),
		hashTypeCheckTextEvent(AnnGetStringUtility()->hash("TextMessage"))
	{}

	//The business part is here. Overriding this method permit you to catch user space events.
	//You get a reference to the event, and you get a raw pointer to the event "sender".
	void EventFromUserSubsystem(AnnUserSpaceEvent& e, AnnUserSpaceEventLauncher* origin) override
	{
		if (e.getType() == hashTypeCheckTextEvent)
			Annwvyn::AnnDebug() << "SomeEventListener got the TextMessageEvent \""
			<< dynamic_cast<TextMessageEvent&>(e).getMessage()
			<< "\" from " << origin;
	}

private:
	//We are storing the hashed value of the type here
	const AnnUserSpaceEvent::AnnUserSpaceEventTypeHash hashTypeCheckTextEvent;
};

// A user defined subsystem sending the event declared above regularly.
// This is a AnnUserSubSystem. AnnUserSubSystem inherit from AnnUserSpaceEventLauncher.
// Don't really worry about that bit, just know that you have access to the "dispatchEvent" method,
// You have to give it a shared_ptr to a message instance, but you are NOT supposed to keep the event alive here.
// The engine will keep it, dispatch it to all the listeners (as a reference to it's pointed content), then be freed.
class SomeSubSystem : public Annwvyn::AnnUserSubSystem
{
public:
	SomeSubSystem(double updateRate = 3) : AnnUserSubSystem("Useless Subsystem"),
		now(0),
		last(0),
		delay(updateRate)
	{}

	//If the time since last "message sent" is higher than the delay, send a message
	void update() override
	{
		now = Annwvyn::AnnGetEngine()->getTimeFromStartupSeconds();

		if (now - last > delay)
		{
			static unsigned int count = 0;
			last = now;
			dispatchEvent<TextMessageEvent>("Useless message! " + std::to_string(++count));
		}
	}

	bool needUpdate() override
	{
		return true;
	}

private:
	double now, last, delay;
};

// This level setup the subsystem and listener declared above.
class DemoEvent : public TutorialLevel
{
public:
	DemoEvent() : TutorialLevel(),
		subsystem(nullptr),
		listener(nullptr)
	{
	}

	void load() override
	{
		//Create an instance of the subsystem class defined above configured to launch it's event once per second
		//Subsystems needs to be added to the engine to get updated
		subsystem = AnnGetEngine()->registerUserSubSystem<SomeSubSystem>(1.0f);

		//Create an instance of a listener that intercept our text message events
		//Listeners needs to be added to the event manager to receive events
		listener = AnnGetEventManager()->addListener<SomeEventListener>();

		loadBasicTutorialLevel();

		//Some text for the user that encourage to go read this file
		loadTextPannel(
			"DemoEvent Level :\n"
			"-----------------\n"
			"\n"
			"This level shows how to create  your own subsystems and use custom event classes with Annwvyn\n"
			"\n"
			"If you open the debug console, you should see a series of \"useless messages\" prints\n"
			"These are sent from a custom subsystem, and use a custom event class that has been user defined\n"
			"\n"
			"To understand how it works, look at the file \"DemoEvent.hpp\" in the example directory\n"
		);
	}

	void unload() override
	{
		AnnGetEngine()->removeUserSubSystem(subsystem);
		subsystem.reset();

		AnnGetEventManager()->removeListener(listener);
		listener.reset();

		TutorialLevel::unload();
	}

	void runLogic() override
	{
	}

private:
	std::shared_ptr<SomeSubSystem> subsystem;
	std::shared_ptr<SomeEventListener> listener;
};