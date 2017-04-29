#pragma once
#include <Annwvyn.h>
#include "TutorialLevel.hpp"

///Event
class TextMessageEvent : public Annwvyn::AnnUserSpaceEvent
{
public:
	TextMessageEvent(std::string message) : AnnUserSpaceEvent("TextMessage"),
		enclosedMessage(message)
	{}

	~TextMessageEvent() = default;

	std::string getMessage() const { return enclosedMessage; };

private:
	std::string enclosedMessage;
};

///Event Listener
class SomeEventListener : LISTENER
{
public:
	SomeEventListener() : constructListener(),
		hashTypeCheckTextEvent(AnnGetStringUtility()->hash("TextMessage"))
	{}

	void EventFromUserSubsystem(AnnUserSpaceEvent& e, AnnUserSpaceEventLauncher* origin) override
	{
		if (e.getType() == hashTypeCheckTextEvent)
			Annwvyn::AnnDebug() << "SomeEventListener got the TextMessageEvent \""
			<< dynamic_cast<TextMessageEvent&>(e).getMessage()
			<< "\" from " << origin;
	}

private:
	const AnnUserSpaceEvent::AnnUserSpaceEventTypeHash hashTypeCheckTextEvent;
};

///A user defined subsystem sending the event declared above regularly
class SomeSubSystem : public Annwvyn::AnnUserSubSystem
{
public:
	SomeSubSystem(double updateRate = 3) : AnnUserSubSystem("Useless Subsystem"),
		now(0),
		last(0),
		delay(updateRate)
	{}

	void update() override
	{
		now = Annwvyn::AnnGetEngine()->getTimeFromStartupSeconds();

		if (now - last > delay)
		{
			static unsigned int count = 0;
			last = now;
			dispatchEvent(std::make_shared<TextMessageEvent>("Useless message! " + std::to_string(++count)));
		}
	}

	bool needUpdate() override
	{
		return true;
	}

private:
	double now, last, delay;
};

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
		subsystem = std::make_shared<SomeSubSystem>(1.0f);
		//Subsystems needs to be added to the engine to get updated
		AnnGetEngine()->registerUserSubSystem(subsystem);

		//Create an instance of a listener that intercept our text message events
		listener = std::make_shared<SomeEventListener>();
		//Listeners needs to be added to the event manager to receive events
		AnnGetEventManager()->addListener(listener);

		loadBasic();

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