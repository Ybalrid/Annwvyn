#pragma once

#include <Annwvyn.h>

using namespace Annwvyn;

//Forward definition of the listener class
class DemoHubTriggerListener;

//TODO: see if there is a way to NOT do this
//The original problem was to have the level be itself an event listener. This was causing problems when unloading the LevelManager
//and the the EventManager.
//Having a method inside the level that catch the event seemed a good idea, but the lifecycle of the listener object cannot be the same
//as the lifecycle of a level object.
//It's fine to have a listener registred at level::load() and unregistered at level::unload(). There's no special thing to do when
//creating/destroing a listener object. 
//The problem is that I have no way to make this look clean. I'm going to use a bunch of trigger events here, so I made a callback class so 
//that I can inherit and spare 4 lines of code, but I dont like this code at all.
//Maybe it would be less bad to have the listener class nested inside the level if it's specific to that level. They are all "AnnLevelListener"
//instances, so maybe the AnnLevl class can be adapted to be aware of this. The problem is to get code executed when an event is triggered. Because
//with a raw, separated listener. The listener catch the event, but the level will not be aware of it.
//I may generalize the "eventCallback" thing. But that's actually doing the same thing of a listener. And it's pretty ridiculus do have
//Event manage -> call event listener method -> call callback method of a level. Or is it?


class TriggerCallback
{
public:
	///At the construct, create the listener as a shared ptr, because it's owning will be shared with the event manager
	TriggerCallback()
	{
		triggerListener = static_pointer_cast<AnnEventListener>
			(make_shared<DemoHubTriggerListener>(this));
	}

	///Implement this to get the event back
	virtual void triggerEventCallback(AnnTriggerEvent e) = 0;

protected:
	shared_ptr<AnnEventListener> triggerListener;
};

///Normal listener
class DemoHubTriggerListener : LISTENER
{
public:

	///Take who to call back to 
	DemoHubTriggerListener(TriggerCallback* hubCallback) : constructListener(),
		callback(hubCallback)
	{
	}

	///If we have an event, pass it to who wanted it in the first place. 
	virtual void TriggerEvent(AnnTriggerEvent e)
	{
		callback->triggerEventCallback(e);
	}

private:
	TriggerCallback* callback;
};


class GoBackToDemoHub : LISTENER
{
public:
	GoBackToDemoHub() : constructListener()
	{
	}

	virtual void KeyEvent(AnnKeyEvent e)
	{
		if (e.isPressed() && e.getKey() == KeyCode::space)
			jumpToHub();
	}

	virtual void StickEvent(AnnStickEvent e)
	{
		if (e.isPressed(8)) jumpToHub();
	}

private:
	void jumpToHub()
	{
		AnnGetLevelManager()->jumpToFirstLevel();
	}
};
