#pragma once

#include <Annwvyn.h>

using namespace Annwvyn;

class GoBackToDemoHub : LISTENER
{
public:
	GoBackToDemoHub() :
	 constructListener()
	{
		AnnDebug() << "constructed a GoBackToDemoHub listener";
	}

	void KeyEvent(AnnKeyEvent e) override
	{
		if(e.shouldIgnore()) return;
		if(e.isPressed() && e.getKey() == KeyCode::space)
			jumpToHub();
	}

	void ControllerEvent(AnnControllerEvent e) override
	{
		if(e.isXboxController() && e.isPressed(8))
			jumpToHub();
	}

	void HandControllerEvent(AnnHandControllerEvent e) override
	{
		if(e.buttonPressed(3))
			switch(e.getSide())
			{
				default: break;
				case AnnHandController::leftHandController:
					jumpToHub();
					break;
			}
	}

	void tick() override
	{
	}

private:
	// ReSharper disable once CppMemberFunctionMayBeStatic
	void jumpToHub() const
	{
		AnnGetLevelManager()->switchToFirstLoadedLevel();
	}
};
