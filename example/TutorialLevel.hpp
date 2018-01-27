#pragma once
#include <Annwvyn.h>
#include "DemoUtils.hpp"

class TutorialLevel : LEVEL
{
public:
	TutorialLevel() :
	 constructLevel()
	{
	}

protected:
	void loadBasicTutorialLevel()
	{
		goBackListener = AnnGetEventManager()->addListener<GoBackToDemoHub>();
		auto floor	 = addGameObject("floorplane.mesh");
		floor->setupPhysics();

		auto Sun = addLightObject();
		Sun->setType(AnnLightObject::ANN_LIGHT_DIRECTIONAL);
		Sun->setDirection({ 0, -1, -0.5 });
		Sun->setPower(97.0f);

		AnnGetPlayer()->teleport({ 0, 5, 10 }, 0);
		AnnGetPlayer()->regroundOnPhysicsBody();
	}

	std::shared_ptr<Ann3DTextPlane> loadTextPannel(const std::string& text, const float& w = 2, const float& h = 1.5, const int& fontSize = 200, const float& ppi = 12)
	{
		auto explaination = std::make_shared<Ann3DTextPlane>(w, h, text, fontSize, ppi, "TutorialFont");
		explaination->setTextAlign(Ann3DTextPlane::TextAlign::ALIGN_LEFT);
		explaination->setMargin(0.05);
		explaination->setBackgroundColor(AnnColor(0.9, 0.9, 0.9));
		explaination->setTextColor(AnnColor{ 0, 0, 0 });
		explaination->setPosition({ 0, 1.5f, 9 });
		explaination->update();
		addManualMovableObject(explaination);
		return explaination;
	}

	void unload() override
	{
		AnnGetEventManager()->removeListener(goBackListener);
		goBackListener.reset();

		AnnLevel::unload();
	}

private:
	std::shared_ptr<GoBackToDemoHub> goBackListener;
};
