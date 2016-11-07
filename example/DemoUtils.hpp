#pragma once

#include <Annwvyn.h>

using namespace Annwvyn;

class GoBackToDemoHub : LISTENER
{
public:
	GoBackToDemoHub() : constructListener()
	{
	}

	void KeyEvent(AnnKeyEvent e) override
	{
		if (e.isPressed() && e.getKey() == KeyCode::space)
			jumpToHub();
	}

	void StickEvent(AnnStickEvent e) override
	{
		if (e.isXboxController() && e.isPressed(8))
			jumpToHub();
	}

	void HandControllerEvent(AnnHandControllerEvent e) override
	{
		if (e.getController()->hasBeenPressed(3))
			jumpToHub();
	}

private:
	void jumpToHub()
	{
		AnnGetLevelManager()->jumpToFirstLevel();
	}
};