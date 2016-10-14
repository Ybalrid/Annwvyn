#pragma once

#include <Annwvyn.h>

using namespace Annwvyn;

//Forward definition of the listener class
class DemoHubTriggerListener;

class GoBackToDemoHub : LISTENER
{
public:
	GoBackToDemoHub() : constructListener()
	{
	}

	virtual void KeyEvent(AnnKeyEvent e)
	{
		if (e.isPressed() && e.getKey() == KeyCode::space)
			jumpToHub();
	}

	virtual void StickEvent(AnnStickEvent e)
	{
		if (e.isPressed(8)) jumpToHub();
	}

private:
	void jumpToHub()
	{
		AnnGetLevelManager()->jumpToFirstLevel();
	}
};