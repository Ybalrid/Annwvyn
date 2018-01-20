#pragma once

#include <Annwvyn.h>
#include <memory>

#include "DemoUtils.hpp"

using namespace Annwvyn;

//Hub to select Demos
class DemoHub : LEVEL, LISTENER
{
public:
	DemoHub() :
	 constructLevel(), constructListener(),
	 Demo0Trig(nullptr),
	 TestLevelTrig(nullptr),
	 EventTrig(nullptr),
	 TimerTrig(nullptr),
	 rotating(nullptr),
	 panelDpi(18)
	{
	}

	~DemoHub()
	{
		AnnDebug() << "Demo hub destructed!";
	}

	AnnTriggerObject* createStone(const AnnVect3& position, const AnnQuaternion& orientation, const std::string& text)
	{
		auto stone = addGameObject("DemoStone.mesh");
		stone->setPosition(position);
		stone->setOrientation(orientation);
		stone->setUpPhysics();

		auto textpane = std::make_shared<Ann3DTextPlane>(2.f, 1.f, text, 512, panelDpi, "PannelFont");
		textpane->setTextAlign(Ann3DTextPlane::ALIGN_CENTER);
		textpane->setTextColor(AnnColor{ 0, 0, 0 });
		textpane->setPosition(position + orientation * AnnVect3{ 0, 2.f, -0.35f });
		textpane->setOrientation(stone->getOrientation());
		textpane->update();
		addManualMovableObject(textpane);

		auto trigger = addTrggerObject();
		trigger->setShape(AnnTriggerObjectShapeGenerator::sphere(1.25f));
		trigger->setPosition(position + AnnVect3(0, 1.2f, 0));

		return trigger.get();
	}

	void load() override
	{
		AnnDebug("DemoHub");
		//Register ourselves as event listener
		AnnGetEventManager()->addListener(getSharedListener());
		AnnGetSceneryManager()->setExposure(1.0f, -2, +2);
		AnnGetSceneryManager()->setBloomThreshold(8);

		AnnGetPlayer()->teleport({ 0, 2, 0.125f }, 0);

		//Add static geometry
		auto pbrTest = addGameObject("SubstanceSphereDecimated.mesh");
		pbrTest->setPosition({ -1, 1.5f, -2 });
		pbrTest->playSound("beep.wav", true, 0.5);
		rotating = pbrTest.get();

		auto Ground = addGameObject("floorplane.mesh");
		Ground->setUpPhysics();

		AnnGetPlayer()->regroundOnPhysicsBody();

		Demo0Trig	 = createStone({ -6, 0, -5 }, { AnnDegree(45), AnnVect3::UNIT_Y }, "Demo 0\nDemo the loading of a demo... xD");
		TestLevelTrig = createStone({ 0, 0, -7 }, AnnQuaternion::IDENTITY, "TestLevel\nA simple test level");
		EventTrig	 = createStone({ 6, 0, -5 }, { AnnDegree(-45), AnnVect3::UNIT_Y }, "DemoEvent\nShows user-defined events in action");
		TimerTrig	 = createStone({ 8, 0, 1 }, { AnnDegree(-90), AnnVect3::UNIT_Y }, "DemoTimer\nShow how timer works");

		auto Sun = addLightObject();
		Sun->setType(AnnLightObject::ANN_LIGHT_DIRECTIONAL);
		Sun->setDirection({ 0, -1, -0.5 });
		Sun->setPower(97.0f);
	}

	//Called at each frame
	void runLogic() override
	{
		if(rotating)
		{
			rotating->setOrientation({ AnnDegree(float(AnnGetEngine()->getTimeFromStartupSeconds()) * 45.0f), AnnVect3::UNIT_Y });
		}
	}

	void unload() override
	{
		//Unregister the listener
		AnnGetEventManager()->removeListener(getSharedListener());
		AnnLevel::unload();

		Demo0Trig	 = nullptr;
		TestLevelTrig = nullptr;
		EventTrig	 = nullptr;
		TimerTrig	 = nullptr;
		rotating	  = nullptr;
	}

	void TriggerEvent(AnnTriggerEvent e) override
	{
		AnnDebug() << "got trigger event";
		if(e.getContactStatus())
			jumpToLevelTriggeredBy(e.getSender());
	}

	void jumpToLevelTriggeredBy(AnnTriggerObject* trigger) const
	{
		if(Demo0Trig == trigger)
		{
			AnnGetLevelManager()->jump(getDemo(0));
			return;
		}
		if(TestLevelTrig == trigger)
		{
			AnnGetLevelManager()->jump(getDemo(1));
			return;
		}
		if(EventTrig == trigger)
		{
			AnnGetLevelManager()->jump(getDemo(2));
			return;
		}

		if(TimerTrig == trigger)
		{
			AnnGetLevelManager()->jump(getDemo(3));
			return;
		}
	}

	static level_id getDemo(level_id id)
	{
		return 1 + id;
	}

private:
	AnnTriggerObject* Demo0Trig;
	AnnTriggerObject* TestLevelTrig;
	AnnTriggerObject* EventTrig;
	AnnTriggerObject* TimerTrig;

	AnnGameObject* rotating;

	float panelDpi;
};

class Demo0 : LEVEL
{
public:
	Demo0() :
	 constructLevel()
	{
	}

	void load() override
	{
		goBackListener = AnnGetEventManager()->addListener<GoBackToDemoHub>();
		auto Ground	= addGameObject("floorplane.mesh");
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

	void runLogic() override
	{
		for(auto controller : AnnGetVRRenderer()->getHandControllerArray())
			if(controller)
				controller->rumbleStart(1);
	}

private:
	std::shared_ptr<GoBackToDemoHub> goBackListener;
};