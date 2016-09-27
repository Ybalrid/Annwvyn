#pragma once

#include <Annwvyn.h>
#include <memory>

using namespace Annwvyn;

//Forward definition of the listener class
class DemoHubTriggerListener;

class triggerCallback
{
public:
	triggerCallback()
	{
		triggerListener = static_pointer_cast<AnnEventListener>
			(make_shared<DemoHubTriggerListener>(this));
	}
	virtual void triggerEventCallback(AnnTriggerEvent e) = 0;

protected:
	shared_ptr<AnnEventListener> triggerListener;
};

//Hub to select Demos
class DemoHub : LEVEL, public triggerCallback
{
public:

	DemoHub() : constructLevel(), triggerCallback()
	{
	
	}

	~DemoHub()
	{
		AnnDebug() << "Demo hub destructed!";
	}

	void load()
	{
		//Register ourselve as event listener
		AnnGetEventManager()->addListener(triggerListener);

		//Add static geometry
		auto Ground = addGameObject("Ground.mesh");
		Ground->setUpPhysics();

		auto Stone = addGameObject("DemoStone.mesh");
		Stone->setPosition({ 0, 0, -3 });
		Stone->setUpPhysics();

		auto Sun = addLightObject();
		Sun->setType(AnnLightObject::ANN_LIGHT_DIRECTIONAL);
		Sun->setDirection({ -0.5, -1, -0.5 });

		auto Demo0Trigger = static_pointer_cast<AnnSphericalTriggerObject>(addTrggerObject());
		Demo0Trigger->setThreshold(1.5f);
		Demo0Trigger->setPosition(Stone->getPosition() + AnnVect3(0, 0.5f, 0));
		demo0trig = Demo0Trigger;

		AnnGetPlayer()->setPosition({ 0, 1, 0 });
		AnnGetPlayer()->setOrientation(Ogre::Euler(0));
		AnnGetPlayer()->resetPlayerPhysics();
	}

	//Called at each frame
	void runLogic()
	{
	}

	void unload()
	{
		//Unregister the listener
		AnnGetEventManager()->removeListener(triggerListener);
		AnnLevel::unload();
	}

	void triggerEventCallback(AnnTriggerEvent e)
	{
		AnnDebug() << "Hey. Got a trigger event bro.";
		if (e.getContactStatus())
			jumpToLevelTriggeredBy(e.getSender());
	}

	void jumpToLevelTriggeredBy(std::shared_ptr<AnnTriggerObject> trigger)
	{
		if (demo0trig == trigger)
			AnnGetLevelManager()->jump(getDemo(0));
	}

	level_id getDemo(level_id id)
	{
		return 1 + id;
	}

private:
	std::shared_ptr<AnnTriggerObject> demo0trig;

};

class DemoHubTriggerListener : LISTENER
{
public:
	DemoHubTriggerListener(triggerCallback* hubCallback) : constructListener(),
		callback(hubCallback)
	{
	}

	virtual void TriggerEvent(AnnTriggerEvent e)
	{
		callback->triggerEventCallback(e);
	}

private:
	triggerCallback* callback;
};

class Demo0 : LEVEL, public triggerCallback
{
public:
	void load()
	{
		auto Ground = addGameObject("Ground.mesh");
		Ground->setUpPhysics();

		auto MyObject = addGameObject("MyObject.mesh");
		MyObject->setPosition({ 0, 1,-5 });
		MyObject->setUpPhysics(200, convexShape);

		auto Sun = addLightObject();
		Sun->setType(AnnLightObject::ANN_LIGHT_DIRECTIONAL);
		Sun->setDirection({ 0,1,-0.75 });

		AnnGetPlayer()->setPosition({ 0, 1, 0 });
		AnnGetPlayer()->setOrientation(Ogre::Euler(0));
		AnnGetPlayer()->resetPlayerPhysics();

	}

	void triggerEventCallback(AnnTriggerEvent e)
	{

	}

	void runLogic()
	{

	}
};
