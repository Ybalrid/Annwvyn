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
		setPosition(0,0,-5);
		setScale(0.2f,0.2f,0.2f);
		setAnimation("Dance");
		playAnimation(true);
		loopAnimation(true);
		setUpPhysics(40, phyShapeType::boxShape);
	}

	void atRefresh()
	{
																													   
	}
};

class TestLevel : public AnnAbstractLevel
{
public:
	///Construct the Level : 
	void load()
	{
		//Set some ambiant light
		AnnEngine::Instance()->setAmbiantLight(AnnColor(.6f,.6f,.6f));
		
		//Add other source of light
		auto Sun = AnnEngine::Instance()->createLightObject();
		Sun->setType(Ogre::Light::LT_DIRECTIONAL);
		Sun->setDirection(Ogre::Vector3::NEGATIVE_UNIT_Y + 1.5* Ogre::Vector3::NEGATIVE_UNIT_Z);
		levelLighting.push_back(Sun);

		//Create objects and register them as content of the level
		auto S = AnnEngine::Instance()->createGameObject("Sinbad.mesh", new Sinbad);
		levelContent.push_back(S);

		//Add water
		auto Water = addGameObject("Water.mesh");	

		//Add the island
		auto Island = addGameObject("Island.mesh");
		Island->setUpBullet();

		//Add the sign
		auto Sign(addGameObject("Sign.mesh"));
		Sign->setPosition(1,-0,-2);
		Sign->setUpPhysics(0, phyShapeType::staticShape);
		Sign->setOrientation(Ogre::Quaternion(Ogre::Degree(-45), Ogre::Vector3::UNIT_Y));

		AnnTriggerObject* t(AnnEngine::Instance()->createTriggerObject(new AnnAlignedBoxTriggerObject));
		dynamic_cast<AnnAlignedBoxTriggerObject*>(t)->setBoundaries(-1,1,-1,1,-1,1);
		levelTrigger.push_back(t);

		//Put some music here
		AnnEngine::Instance()->getAudioEngine()->playBGM("media/bgm/bensound-happyrock.ogg", 0.4);

		//Place the starting point 
		AnnPlayer* player(AnnEngine::Instance()->getPlayer());
		player->setPosition(AnnVect3::ZERO);
		player->setOrientation(Ogre::Euler(0));
		AnnEngine::Instance()->resetPlayerPhysics();
	}

	void runLogic()
	{}

private:
};




#endif //TESTLEVEL