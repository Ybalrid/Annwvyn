#pragma once

#include <Annwvyn.h>
#include <memory>

#include "DemoUtils.hpp"

using namespace Annwvyn;

//Hub to select Demos
class DemoHub : LEVEL, LISTENER
{
public:

	DemoHub() : constructLevel(), constructListener(),
		rotating(nullptr),
		panelDpi(18)
	{
	}

	~DemoHub()
	{
		AnnDebug() << "Demo hub destructed!";
	}

	void load() override
	{
		AnnDebug("DemoHub");
		////Register ourselves as event listener
		AnnGetEventManager()->addListener(getSharedListener());

		//AnnGetSceneryManager()->setAmbientLight(AnnColor(0, 0, 0), 0, AnnColor(0, 0, 0), 0, AnnVect3::NEGATIVE_UNIT_Y);

		AnnGetSceneryManager()->setExposure(1.0f, -2, +2);
		AnnGetSceneryManager()->setBloomThreshold(8);
		//Some ambient lighting is needed

		AnnGetPlayer()->teleport({ 0, 2, 0.125f }, 0);
		//Add static geometry

		auto pbrTest = addGameObject("SubstanceSphereDecimated.mesh");
		pbrTest->setPosition({ -1, 1.5f, -2 });
		pbrTest->playSound("beep.wav", true, 0.5);
		rotating = pbrTest.get();

		auto Ground = addGameObject("floorplane.mesh");
		Ground->setUpPhysics();

		AnnGetPlayer()->regroundOnPhysicsBody();

		auto StoneDemo0 = addGameObject("DemoStone.mesh");
		StoneDemo0->setPosition({ -6, 0, -5 });
		StoneDemo0->setOrientation({ AnnDegree(45), AnnVect3::UNIT_Y });
		StoneDemo0->setUpPhysics();

		auto TextPane = std::make_shared<Ann3DTextPlane>(2.f, 1.f, "Demo 0\nDemo the loading of a demo... xD", 512, panelDpi);
		TextPane->setTextAlign(Ann3DTextPlane::ALIGN_CENTER);
		TextPane->setTextColor(AnnColor{ 0, 0, 0 });
		TextPane->setPosition(StoneDemo0->getPosition() + StoneDemo0->getOrientation()*  AnnVect3 { 0, 2, -0.35 });
		TextPane->setOrientation(StoneDemo0->getOrientation());
		TextPane->update();
		addManualMovableObject(TextPane);

		/*	auto Demo0Trigger = std::static_pointer_cast<AnnSphericalTriggerObject>(addTrggerObject());
			Demo0Trigger->setThreshold(1.5f);
			Demo0Trigger->setPosition(StoneDemo0->getPosition() + AnnVect3(0, 0.5f, 0));
			demo0trig = Demo0Trigger;
			*/
		auto StoneTestLevel = addGameObject("DemoStone.mesh");
		StoneTestLevel->setPosition({ +0, 0, -7 });
		StoneTestLevel->setOrientation(AnnQuaternion(AnnDegree(0), AnnVect3::UNIT_Y));
		StoneTestLevel->setUpPhysics();

		auto TestLevelText = std::make_shared<Ann3DTextPlane>(2.f, 1.f, "TestLevel\nA simple test level", 512, panelDpi);
		TestLevelText->setTextAlign(Ann3DTextPlane::ALIGN_CENTER);
		TestLevelText->setTextColor(AnnColor{ 0, 0, 0 });
		TestLevelText->setPosition(StoneTestLevel->getPosition() + StoneTestLevel->getOrientation()*  AnnVect3 { 0, 2, -0.35 });
		TestLevelText->setOrientation(StoneTestLevel->getOrientation());
		TestLevelText->update();
		addManualMovableObject(TestLevelText);

		/*auto TestLevelTrigger = std::static_pointer_cast<AnnSphericalTriggerObject>(addTrggerObject());
		TestLevelTrigger->setThreshold(1.5f);
		TestLevelTrigger->setPosition(StoneTestLevel->getPosition() + AnnVect3(0, 0.5f, 0));
		testLevelTrig = TestLevelTrigger;
		*/
		auto StoneEvent = addGameObject("DemoStone.mesh");
		StoneEvent->setPosition({ +6, 0, -5 });
		StoneEvent->setOrientation(AnnQuaternion(AnnDegree(-45), AnnVect3::UNIT_Y));
		StoneEvent->setUpPhysics();

		auto EventText = std::make_shared<Ann3DTextPlane>(2.f, 1.f, "DemoEvent\nShows user-defined events in action", 512, panelDpi);
		EventText->setTextAlign(Ann3DTextPlane::ALIGN_CENTER);
		EventText->setTextColor(AnnColor{ 0, 0, 0 });
		EventText->setPosition(StoneEvent->getPosition() + StoneEvent->getOrientation()*  AnnVect3 { 0, 2, -0.35 });
		EventText->setOrientation(StoneEvent->getOrientation());
		EventText->update();
		addManualMovableObject(EventText);

		/*
		auto EventTrigger = std::static_pointer_cast<AnnSphericalTriggerObject>(addTrggerObject());
		EventTrigger->setThreshold(1.5f);
		EventTrigger->setPosition(StoneEvent->getPosition() + AnnVect3(0, 0.5f, 0));
		EventTrig = EventTrigger;
		*/
		auto Sun = addLightObject();
		Sun->setType(AnnLightObject::ANN_LIGHT_DIRECTIONAL);
		Sun->setDirection({ 0, -1, -0.5 });
		Sun->setPower(97.0f);
	}

	//Called at each frame
	void runLogic() override
	{
		if (rotating)
		{
			rotating->setOrientation({ AnnDegree(float(AnnGetEngine()->getTimeFromStartupSeconds()) * 45.0f), AnnVect3::UNIT_Y });
		}
	}

	void unload() override
	{
		//Unregister the listener
		AnnGetEventManager()->removeListener(getSharedListener());
		AnnLevel::unload();
		demo0trig.reset();
		testLevelTrig.reset();
		rotating = nullptr;
	}

	void TriggerEvent(AnnTriggerEvent e) override
	{
		if (e.getContactStatus())
			jumpToLevelTriggeredBy(e.getSender());
	}

	void jumpToLevelTriggeredBy(AnnTriggerObject* trigger) const
	{
		if (demo0trig.get() == trigger)
		{
			AnnGetLevelManager()->jump(getDemo(0));
			return;
		}
		if (testLevelTrig.get() == trigger)
		{
			AnnGetLevelManager()->jump(getDemo(1));
			return;
		}
		if (EventTrig.get() == trigger)
		{
			AnnGetLevelManager()->jump(getDemo(2));
			return;
		}
	}

	static level_id getDemo(level_id id)
	{
		return 1 + id;
	}

private:
	std::shared_ptr<AnnTriggerObject> demo0trig;
	std::shared_ptr<AnnTriggerObject> testLevelTrig;
	std::shared_ptr<AnnTriggerObject> EventTrig;

	AnnGameObject* rotating;

	float panelDpi;
};

class Demo0 : LEVEL
{
public:

	Demo0() : constructLevel()
	{
	}

	void load() override
	{
		AnnGetEventManager()->addListener(goBackListener = std::make_shared<GoBackToDemoHub>());
		auto Ground = addGameObject("floorplane.mesh");
		Ground->setUpPhysics();

		auto MyObject = addGameObject("MyObject.mesh", "MyObject");
		MyObject->setPosition({ 0, 1, -5 });
		MyObject->setUpPhysics(200, convexShape);

		auto objectQueryFromNode = AnnGetGameObjectManager()->getFromNode(MyObject->getNode());

		auto Sun = addLightObject();
		Sun->setType(AnnLightObject::ANN_LIGHT_DIRECTIONAL);
		Sun->setDirection({ 0, -1, -0.75 });
		Sun->setPower(97.0f);

		AnnGetPlayer()->teleport({ 0, 1, 0 }, 0);
		AnnGetPlayer()->regroundOnPhysicsBody();
	}

	void unload() override
	{
		AnnGetEventManager()->removeListener(goBackListener);
		goBackListener.reset();

		AnnLevel::unload();
	}

	void triggerEventCallback(AnnTriggerEvent e)
	{
	}

	void runLogic() override
	{
		for (auto controller : AnnGetVRRenderer()->getHandControllerArray())
			if (controller)
				controller->rumbleStart(1);
	}

private:
	std::shared_ptr<GoBackToDemoHub> goBackListener;
};