/**
 * \file AnnAudioEngine.hpp
 * \brief OpenAL audio handeling for Annwvvyn
 *        handle the OpenAL context creation and the loading of sound files
 *        handle the position/orientation of the listener
 * \author A. Brainville (Ybalrid)
 */

#ifndef ANN_AUDIO
#define ANN_AUDIO

#include "systemMacro.h"

#include <Ogre.h>

#include <iostream>
#include <string.h>

//OpenAl
#include <al.h>
#include <alc.h>
//libsndfile
#include <sndfile.h>


namespace Annwvyn
{
    class AnnEngine;

	///Class that handle the OpenAL audio.
	class DLL AnnAudioEngine
	{
	public:
		///class constuctor
		AnnAudioEngine();

		///class destructor
		~AnnAudioEngine();

		///init openal
		bool initOpenAL();

		///shutdown and cleanup openal
		void shutdownOpenAL();
		
		///load a sound file. return a sond buffer
		ALuint loadSndFile(const std::string& Filename);
		
		///play background music. you can specify the volume of the music (0.0f to 1.0f)
		void playBGM(const std::string path, const float volume = 0.5f);

		const std::string getLastError();

	private:
		///For engine : uptade listenter pos and orientation with Ogre coordinates
		void updateListenerPos(Ogre::Vector3 pos);
		
        ///For engine : update listener Oirentation
        friend class Annwvyn::AnnEngine;

    private:
        void updateListenerOrient(Ogre::Quaternion orient);
		std::string lastError;
		ALCdevice* Device;
		ALCcontext* Context;
	    
        ALuint buffer;
		ALuint bgm;//background music source
		//ALuint soundSource;
	};
}
#endif
