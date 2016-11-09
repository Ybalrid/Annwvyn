/**
* \file AnnAudioEngine.hpp
* \brief OpenAL audio handling for Annwvyn
*        handle the OpenAL context creation and the loading of sound files
*        handle the position/orientation of the listener
* \author A. Brainville (Ybalrid)
*/

#ifndef ANN_AUDIO
#define ANN_AUDIO

#include "systemMacro.h"

#include "AnnVect3.hpp"
#include "AnnQuaternion.hpp"
#include <iostream>
#include <string>
#include <unordered_map>
#include <memory>

//OpenAl
#include <al.h>
#include <alc.h>

//libsndfile
#include <sndfile.h>

#include "AnnSubsystem.hpp"

namespace Annwvyn
{
	class AnnEngine;
	class AnnAudioEngine;

	///Represent an audio source in the engine
	class DLL AnnAudioSource
	{
	public:
		///Private constructor. You have to call AnnAudioEngine::createAudioSource() to get an AnnAudioSource object
		AnnAudioSource();

		///Destroy audio source
		~AnnAudioSource();

		///Put the audio source at this position in space
		void setPositon(AnnVect3 position);
		///Set the volume at the given gain (between 0 & 1)
		/// \param gain value between 0 and 1
		void setVolume(float gain);
		///Put the audio read position at the origin
		void rewind();
		///Play the sound
		void play();
		///Pause the sound
		void pause();
		///Stop playing the sound
		void stop();
		///Change the sound buffer this source plays
		void changeSound(std::string path);

		///If looping is activated, the sound will replay when finished
		void setLooping(bool looping = true);
		///If true, the sound position will use (and update) the player's current position as origin
		void setPositionRelToPlayer(bool relToPlayer = true);

	private:
		friend class AnnAudioEngine;
		///Name of the buffer (filename)
		std::string bufferName;
		///OpenAL source object
		ALuint source;
		///Position of this source
		AnnVect3 pos;
		///Relative to player, or not
		bool posRelToPlayer;
	};

	///Class that handle the OpenAL audio.
	class DLL AnnAudioEngine : public AnnSubSystem
	{
	public:
		///class constructor
		AnnAudioEngine();

		///class destructor
		~AnnAudioEngine();

		///init OpenAL
		bool initOpenAL();

		///shutdown and cleanup OpenAL
		void shutdownOpenAL();

		///Load a sound file. return a sound buffer. Add the buffer to the buffer list.
		///This permit to pre-load sound files to the engine. If want to avoid loading a
		///Bunch of sound file (that causes disk I/O access) you can just load the sound file
		///before the start of your gameplay sequence.
		/// \param filePath Path of the file you want to load
		ALuint loadBuffer(const std::string& filePath);

		///This method is intended to be used in moments like loading levels
		///If a buffer is already loaded, getting it with loadBuffer is equivalent at
		///getting something from an unordered map.
		///If loadBuffer is called with a "new" sound file, the engine will load it in memory
		///before doing anything else, delaying stuff because of disk I/O
		/// \copydoc loadBuffer()
		void preLoadBuffer(const std::string& filePath);

		///Return "false" if buffer not loaded. Return buffer index if buffer is loaded.
		ALuint isBufferLoader(const std::string& filePath);

		///Unload a buffer from the engine. The buffer is identified by the sound file it represent
		/// \param path Path of the file you want to load
		void unloadBuffer(const std::string& path);

		///play background music. you can specify the volume of the music (0.0f to 1.0f)
		/// \param path path of the audio file to use as background music
		/// \param volume Float number between 0 and 1, Loudness of the sound
		void playBGM(const std::string path, const float volume = 0.5f);

		///stop the current background music from playing
		void stopBGM();

		///Get the last error message that occurred in-engine
		std::string getLastError();

		///Create an audio source
		std::shared_ptr<AnnAudioSource> createSource();

		///Create an audio source and attach (and load if necessary) buffer from audio file
		std::shared_ptr<AnnAudioSource> createSource(std::string path);

		DEPRECATED void destroySource(std::shared_ptr<AnnAudioSource> source);
		void removeSource(std::shared_ptr<AnnAudioSource> source);

		///Write last error text to the log
		void logError();

	private:
		///For the engine: update the listener position to match the player's head
		/// \param pos The position of the player
		void updateListenerPos(AnnVect3 pos);

		///For the engine : update the listener orientation to match the player's head
		/// \param orient The orientation of the player
		void updateListenerOrient(AnnQuaternion orient);

		///For engine : update listener Orientation
		friend class AnnEngine;

		void update() override;

	private:
		///Detect playback devices from the device enumeration string
		void detectPlaybackDevices(const char *list);

		///The last error this class has generated
		std::string lastError;
		///AL Device
		ALCdevice* Device;
		///AL Context
		ALCcontext* Context;

		///Audio buffer for background music
		ALuint bgmBuffer;
		///Audio source for background music
		ALuint bgm;

		///Map between audio filenames and OpenAL buffer
		std::unordered_map<std::string, ALuint> buffers;
		bool locked;
		///List of the audio sources object present in the audio engine
		std::list<std::shared_ptr<AnnAudioSource>> AudioSources;
		std::vector<std::string> detectedDevices;
	};
}
#endif