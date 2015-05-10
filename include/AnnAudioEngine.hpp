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
#include <string>

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
		/// \param Filename name of the file to load
		ALuint loadSndFile(const std::string& Filename);
		
		///play background music. you can specify the volume of the music (0.0f to 1.0f)
		/// \param path path of the audio file to use as background music
		/// \param volume Float number between 0 and 1, Loudness of the sound 
		void playBGM(const std::string path, const float volume = 0.5f);

		///Get the last error message that ocured in-engine
		const std::string getLastError();

	private:
		///For the engine: update the listener position to match the player's head
		/// \param pos The position of the player
		void updateListenerPos(Ogre::Vector3 pos);
		
		///For the engine : update the listener orientation to mach the player's head 
        /// \param orient The orientatio of the player
		void updateListenerOrient(Ogre::Quaternion orient);


        ///For engine : update listener Oirentation
        friend class Annwvyn::AnnEngine;

    private:
		std::string lastError;
		ALCdevice* Device;
		ALCcontext* Context;
	    
        ALuint buffer; //static buffer 
		ALuint bgm;//background music source
	};
}
#endif
