#pragma once

#include <Annwvyn.h>
#include <memory>

#include "DemoUtils.hpp"

using namespace Annwvyn;

//Hub to select Demos
class DemoHub : LEVEL, public TriggerCallback
{
public:

	DemoHub() : constructLevel(), TriggerCallback()
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
		Stone->setPosition({ -2, 0, -4 });
		Stone->setOrientation(AnnQuaternion(Ogre::Degree(45), AnnVect3::UNIT_Y));
		Stone->setUpPhysics();

		auto TextPane = make_shared<Ann3DTextPlane>(2.f, 1.f, "Demo 0\nDemo the loading of a demo... xD", 512, 18.f);
		TextPane->setTextAlign(Ann3DTextPlane::ALIGN_CENTER);
		TextPane->setTextColor(AnnColor{ 0, 0, 0 });
		TextPane->setPosition(Stone->getPosition() + Stone->getOrientation()*  AnnVect3{ 0, 2, -0.35 });
		TextPane->setOrientation(Stone->getOrientation());
		TextPane->update();
		addManualMovableObject(TextPane);
		
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

class Demo0 : LEVEL, public TriggerCallback
{
public:

	Demo0() : constructLevel(), TriggerCallback()
	{
	}

	void load()
	{
		AnnGetEventManager()->addListener(goBackListener = make_shared<GoBackToDemoHub>());
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

	void unload()
	{
		AnnGetEventManager()->removeListener(goBackListener);
		goBackListener.reset();

		AnnLevel::unload();
	}

	void triggerEventCallback(AnnTriggerEvent e)
	{

	}

	void runLogic()
	{

	}

private:
	shared_ptr<GoBackToDemoHub> goBackListener;
};
