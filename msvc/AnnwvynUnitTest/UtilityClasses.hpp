#pragma once

#include <Annwvyn.h>

class SimpleLevel : LEVEL
{
public:
	SimpleLevel() : constructLevel(),
		counter(0),
		end(3 * 60)
	{
	}

	void load() override
	{
	}

	void runLogic() override
	{
		//Quit after 10 seconds
		Annwvyn::AnnDebug() << "Test Frame " << counter << " over " << end;
		if (counter++ >= end)
			Annwvyn::AnnGetEngine()->requestQuit();
	}
private:
	int counter;
	const int end;
};

class LessSimpleLevel : public SimpleLevel
{
public:
	LessSimpleLevel() : SimpleLevel() {}
	void load() override
	{
		auto ground = addGameObject("Ground.mesh", "TheGround");
		auto ogre = addGameObject("Sinbad.mesh", "TheOgre");
		ogre->setScale({ 1 / 10,1 / 10,1 / 10 });
		auto sunlight = addLightObject("sun");
		sunlight->setType(Annwvyn::AnnLightObject::LightTypes::ANN_LIGHT_DIRECTIONAL);
		sunlight->setDirection({ -1 / 4, -1, -1 / 4 });
	}
};