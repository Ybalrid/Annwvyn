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
		if (e.shouldIgnore()) return;
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
		e.getController()->rumbleStart(1);
		if (e.getController()->hasBeenPressed(1))
			e.getController()->rumbleStop();

		if (e.getController()->hasBeenPressed(3))
			switch (e.getController()->getSide())
			{
			default:break;
			case AnnHandController::leftHandController:
				jumpToHub();
				break;
			}
	}

private:
	// ReSharper disable once CppMemberFunctionMayBeStatic
	void jumpToHub()
	{
		AnnGetLevelManager()->jumpToFirstLevel();
	}
};