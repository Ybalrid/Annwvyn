#include "AnnLeap.hpp"

using namespace Annwvyn;


CustomListener::CustomListener(AnnLeap* callbackClass) : Leap::Listener()
{
	std::cout << "listener declared" << std::endl;
	nbHands = 0;
	callback = callbackClass;
}

void CustomListener::onFrame(const Leap::Controller& c)
{
	//std::cout << "onFrame" << std::endl;
	//const Leap::Frame frame(c.frame());
	processFrame(c.frame());
	sendData();
}

void CustomListener::processFrame(const Leap::Frame frame)
{
	if(frame.hands().isEmpty())
	{
		nbHands = 0;
		return;
	}

	lHand = frame.hands().leftmost();
	rHand = frame.hands().rightmost();
	
	
	if(lHand == rHand)
		nbHands = 1;
	else
		nbHands = 2;

	//std::cout << "nbHands : " << nbHands << std::endl;
}

void CustomListener::sendData()
{
	//send one or two Leap::Hand object to the callback AnnLeap object
	callback->receiveData(nbHands, lHand, rHand);
}

AnnLeap::AnnLeap()
{
	calls = 0;
	m_nbHands = 0;
	listener  = new CustomListener(this);

	controller.addListener(*listener);
	std::cout << "AnnLeap declared" << std::endl;
}

AnnLeap::~AnnLeap()
{
	delete listener;
}

void AnnLeap::receiveData(int nbHands, Leap::Hand lHand, Leap::Hand rHand)
{
	calls ++;
	//std::cout << "data !!!!! " << calls <<  std::endl;

	m_nbHands = nbHands;
	m_lHand = lHand;
	m_rHand = rHand;
}

int AnnLeap::getNbHands()
{
	return m_nbHands;
}

Leap::Hand AnnLeap::getLeftHand()
{
	return m_lHand;
}

Leap::Hand AnnLeap::getRightHand()
{
	return m_rHand;
}

bool AnnLeap::LeapConnected()
{
	return controller.isConnected();
}