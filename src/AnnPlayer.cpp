#include "AnnPlayer.hpp"

using namespace Annwvyn;

bodyParams::bodyParams()
{
	eyeHeight = 1.59f;
	walkSpeed = 3.0f;
	turnSpeed = 0.003f;
	mass = 80.0f;
	Position = Ogre::Vector3(0,0,10);
	HeadOrientation = Ogre::Quaternion(1,0,0,0);
	Shape = NULL;
	Body = NULL;
}

AnnPlayer::AnnPlayer()
{
	playerBody = new bodyParams;
}

AnnPlayer::~AnnPlayer()
{
	delete playerBody;
}

bodyParams* AnnPlayer::getLowLevelBodyParams()
{
	return playerBody;
}