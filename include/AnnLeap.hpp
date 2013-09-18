#ifndef ANN_LEAP
#define ANN_LEAP

//std C++
#include <iostream>
//Leap
#include <Leap.h>



//windows DLL
#ifdef DLLDIR_EX
   #define DLL  __declspec(dllexport)   // export DLL information
#else
   #define DLL  __declspec(dllimport)   // import DLL information
#endif

#ifdef __gnu_linux__
#define DLL 
#endif

namespace Annwvyn
{
	enum{LEFT,RIGHT};
	class AnnLeap;

	class DLL CustomListener : public Leap::Listener
	{
	public :
		//class constructor
		CustomListener(AnnLeap* callbackClass);
		//this method is called each time a new frame is available
		void onFrame(const Leap::Controller& c);
		//extract and compute usefull data from a frame
		void processFrame(const Leap::Frame);
		//send leapMotion data to the AnnLeap object
		void sendData();

	private:
		AnnLeap* callback;
		int nbHands;
		Leap::Hand lHand,rHand;
	};

	class DLL AnnLeap
	{
	public :
		AnnLeap();
		~AnnLeap();
		void receiveData(int nbHands, Leap::Hand lHand, Leap::Hand rHand);

		//getters
		int getNbHands();
		Leap::Hand getLeftHand();
		Leap::Hand getRightHand();

		bool LeapConnected();

	private :
		Leap::Controller controller;
		Annwvyn::CustomListener* listener;

		int calls, m_nbHands;
		Leap::Hand m_lHand, m_rHand;

	};
}

#endif