#ifndef TESTLEVEL
#define TESTLEVEL

#include <Annwvyn.h>
using namespace Annwvyn;

AnnGameObject* toDestroy = nullptr;

//Custom object:
class Sinbad : public AnnGameObject
{
public:
	void postInit()
	{
		setPos(0,0,-5);
		setScale(0.2f,0.2f,0.2f);
		setAnimation("Dance");
		playAnimation(true);
		loopAnimation(true);
		setUpPhysics(40, phyShapeType::boxShape);
	}

	void atRefresh()
	{
		if(AnnEngine::Instance()->isKeyDown(OIS::KeyCode::KC_Z))
		{
			AnnEngine::Instance()->destroyGameObject(toDestroy);
			toDestroy = nullptr;
		}
	}
};

class TestLevel : public AnnAbstractLevel
{
public:
	///Construct the Level : 
	void load()
	{
		//Set some ambiant light
		AnnEngine::Instance()->setAmbiantLight(Ogre::ColourValue(.6f,.6f,.6f));
		
		//Add other source of light
		auto Sun = AnnEngine::Instance()->addLight();
		Sun->setType(Ogre::Light::LT_DIRECTIONAL);
		Sun->setDirection(Ogre::Vector3::NEGATIVE_UNIT_Y + 1.5* Ogre::Vector3::NEGATIVE_UNIT_Z);
		levelLighting.push_back(Sun);

		//Create objects and register them as content of the level
		auto S = AnnEngine::Instance()->createGameObject("Sinbad.mesh", new Sinbad);
		levelContent.push_back(S);
		auto Water = AnnEngine::Instance()->createGameObject("Water.mesh");
		levelContent.push_back(Water);
		auto Island = AnnEngine::Instance()->createGameObject("Island.mesh");
		Island->setUpBullet();
		levelContent.push_back(Island);
		auto Sign(AnnEngine::Instance()->createGameObject("Sign.mesh"));
		Sign->setPos(1,-0,-2);
		Sign->setUpPhysics(0, phyShapeType::staticShape);
		Sign->setOrientation(Ogre::Quaternion(Ogre::Degree(-45), Ogre::Vector3::UNIT_Y));
		levelContent.push_back(Sign);

		AnnTriggerObject* t(AnnEngine::Instance()->createTriggerObject(new AnnAlignedBoxTriggerObject));
		dynamic_cast<AnnAlignedBoxTriggerObject*>(t)->setBoundaries(-1,1,-1,1,-1,1);
		levelTrigger.push_back(t);

		//Put some music here
		//AnnEngine::Instance()->getAudioEngine()->playBGM("media/bgm/bensound-happyrock.ogg");

		//Place the starting point 
		AnnPlayer* player(AnnEngine::Instance()->getPlayer());
		player->setPosition(AnnVect3::ZERO);
		player->setOrientation(Ogre::Euler(0));
		AnnEngine::Instance()->resetPlayerPhysics();

		toDestroy = AnnEngine::Instance()->createGameObject("Sinbad.mesh");
	}

	void runLogic()
	{}

private:
};




#endif //TESTLEVEL