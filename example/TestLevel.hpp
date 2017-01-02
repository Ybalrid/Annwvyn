#ifndef TESTLEVEL
#define TESTLEVEL

#include <Annwvyn.h>
#include "DemoUtils.hpp"
using namespace Annwvyn;

//Custom object:
class Sinbad : public AnnGameObject
{
public:
	void postInit() override
	{
		setPosition(0, 0, -5);
		setScale(0.2f, 0.2f, 0.2f);
		setAnimation("Dance");
		playAnimation(true);
		loopAnimation(true);
		//setUpPhysics(40, boxShape);
	}

	void atRefresh() override
	{
		//AnnDebug() << "Sinbad position is : " << getPosition();
		//AnnDebug() << getName();
	}
};

class TestLevel : LEVEL
{
public:
	///Construct the Level :
	void load() override
	{
		AnnGetEventManager()->addListener(goBackListener = std::make_shared<GoBackToDemoHub>());
		//Set some ambient light
		AnnGetSceneryManager()->setAmbientLight(AnnColor(.6f, .6f, .6f));

		//We add our brand new 3D object
		auto MyObject = addGameObject("MyObject.mesh");
		MyObject->setPosition(5, 1, 0);//We put it 5 meters to the right, and 1 meter up...
		//MyObject->setUpPhysics(); // <---- This activate the physics for the object as static geometry
		MyObject->setUpPhysics(100, convexShape); // <------- this activate the physics as a dynamic object. We need to tell the shape approximation to use. and a mass in Kg
		MyObject->attachScript("DummyBehavior2");
		//The shape approximation is put at the Object CENTER POINT. The CENTER POINT should be at the object's bounding box CENTER before exporting from blender.

		auto text = std::make_shared<Ann3DTextPlane>(1.0f, 0.5f, "Accent test: 'é' Hello, Virtual World!\nthis is one line only one line only", 128, 96.0f, "LibSerif", "LiberationSerif-regular.ttf");
		//text->setTextAlign(text->ALIGN_CENTER);
		text->setBackgroundColor(AnnColor(0, 1, 0));
		text->setPosition({ 0, 0.5f, -1 });
		text->setBackgroundImage("background.png");
		text->setMargin(0.1f);

		text->update();
		addManualMovableObject(text);

		//Add other source of light
		auto Sun = addLightObject();
		Sun->setType(AnnLightObject::ANN_LIGHT_DIRECTIONAL);
		Sun->setDirection(AnnVect3::NEGATIVE_UNIT_Y + 1.5f* AnnVect3::NEGATIVE_UNIT_Z);

		//Create objects and register them as content of the level
		auto S = AnnGetGameObjectManager()->createGameObject("Sinbad.mesh", "SuperSinbad", std::make_shared<Sinbad>());
		levelContent.push_back(S);
		S->playSound("media/monster.wav", true, 1);
		S->attachScript("DummyBehavior");

		auto Gizmo = AnnGetGameObjectManager()->createGameObject("Gizmo.mesh", "Gizmo");
		S->attachChildObject(Gizmo);
		Gizmo->setScale(10, 10, 10);
		Gizmo->setUpPhysics(10, boxShape);
		S->setUpPhysics(10, boxShape);

		//Add water
		auto Water = addGameObject("environment/Water.mesh");

		//Add the island
		auto Island = addGameObject("environment/Island.mesh");
		Island->setUpPhysics();

		//Add the sign
		auto Sign(addGameObject("environment/Sign.mesh"));
		Sign->setPosition(1, -0, -2);
		Sign->setUpPhysics(0, staticShape);
		Sign->setOrientation(Ogre::Quaternion(Ogre::Degree(-45), Ogre::Vector3::UNIT_Y));

		auto t(AnnGetGameObjectManager()->createTriggerObject(std::make_shared<AnnAlignedBoxTriggerObject>()));
		dynamic_cast<AnnAlignedBoxTriggerObject*>(t.get())->setBoundaries(-1, 1, -1, 1, -1, 1);
		levelTrigger.push_back(t);

		//Put some music here
		//AnnGetAudioEngine()->playBGM("media/bgm/bensound-happyrock.ogg", 0.4);

		//Place the starting point
		AnnGetPlayer()->setPosition(AnnVect3::ZERO);
		AnnGetPlayer()->setOrientation(Ogre::Euler(0));
		AnnGetPlayer()->resetPlayerPhysics();
	}

	void unload() override
	{
		AnnGetEventManager()->removeListener(goBackListener);

		//Do the normal unloading
		AnnLevel::unload();
	}

	void runLogic() override
	{
		AnnGetPlayer()->regroundOnPhysicsBody(1000, { 0,100,0 });
	}

private:
	std::shared_ptr<GoBackToDemoHub> goBackListener;
};

class PhysicsDebugLevel : LEVEL
{
	void load() override
	{
		AnnGetSceneryManager()->setWorldBackgroundColor(AnnColor(0, 0, 0));
		AnnGetPhysicsEngine()->getWorld()->setGravity(btVector3(0, 0, 0));
		AnnGetPlayer()->setPosition(AnnVect3::ZERO);
		AnnGetPlayer()->resetPlayerPhysics();
	}

	void runLogic() override {
		AnnDebug() << "Player position is : " << AnnGetPlayer()->getPosition();
	}
};

#endif //TESTLEVEL