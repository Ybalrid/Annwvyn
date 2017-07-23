#include "stdafx.h"
#include "AnnAudioEngine.hpp"
#include "AnnLogger.hpp"
#include "AnnGetter.hpp"

using namespace Annwvyn;

AnnAudioEngine::AnnAudioEngine() : AnnSubSystem("AudioEngine"),
lastError("Initialize OpenAL based sound system"),
alDevice(nullptr),
alContext(nullptr),
audioFileManager(nullptr)
{
	//Try to init OpenAL
	if (!initOpenAL())
		logError();

	//Set the listener to base position
	const auto player = AnnGetPlayer();
	const auto position = player->getPosition();
	alListener3f(AL_POSITION, position.x, position.y, position.z);

	//Define the default orientation
	const AnnQuaternion orientation = player->getOrientation().toQuaternion();
	const auto at = orientation.getAtVector();
	const auto up = orientation.getUpVector();
	ALfloat alOrientation[] = { at.x, at.y, at.z, //LookAt vector
							up.x, up.y, up.z }; //Up vector

	//Apply the orientation
	alListenerfv(AL_ORIENTATION, alOrientation);

	//Create a source for the BGM
	alGenSources(1, &bgmSource);
	locked = false;

	audioFileManager = new AnnAudioFileManager;
}

void AnnAudioEngine::logError() const
{
	AnnDebug() << lastError;
}

AnnAudioEngine::~AnnAudioEngine()
{
	locked = true;
	shutdownOpenAL();

	delete audioFileManager;
}

void AnnAudioEngine::detectPlaybackDevices(const char *list)
{
	//If list not null or fist character end of string
	if (!list || *list == '\0')
		AnnDebug("    !!! none !!!\n");
	else do
	{
		//Get the first string from the list
		std::string deviceName(list);

		AnnDebug() << "detected device : " << deviceName;
		detectedDevices.push_back(deviceName);

		list += strlen(list) + 1; //This advance the start of the string after the end of the current one, because sizeof(char) = 1
	} while (*list != '\0');//End of the list is marked by \0\0 instead of \0
}

bool AnnAudioEngine::initOpenAL()
{
	//Open audio playback device
	//Check if OpenAL support device enumeration extension here
	if (alcIsExtensionPresent(nullptr, "ALC_ENUMERATE_ALL_EXT")
		&& AnnGetVRRenderer()->usesCustomAudioDevice())
	{
		AnnDebug() << "VR device want's to use : " << AnnGetVRRenderer()->getAudioDeviceIdentifierSubString() << " for audio playback...";
		//Get the list of all devices
		detectPlaybackDevices(alcGetString(nullptr, ALC_ALL_DEVICES_SPECIFIER));

		//Iterate through the name of each device and check if we can find the substring the renderer ask for
		for (auto& deviceName : detectedDevices)
			if (deviceName.find(AnnGetVRRenderer()->getAudioDeviceIdentifierSubString())
				!= std::string::npos)
			{
				AnnDebug() << "Found " << deviceName << " alDevice!";
				//Open the selected device
				alDevice = alcOpenDevice(deviceName.c_str());
				break;
			}
	}

	//If no device has been set above :
	if (!alDevice)
	{
		AnnDebug() << "No specific OpenAL device set, opening windows default";
		alDevice = alcOpenDevice(nullptr);
	}
	if (!alDevice)
	{
		lastError = "Failed to open an OpenAL device";
		return false;
	}

	//Create context and make it current
	alContext = alcCreateContext(alDevice, nullptr);
	if (!alContext)
	{
		lastError = "Failed to create an OpenAL Context";
		return false;
	}
	if (!alcMakeContextCurrent(alContext))
	{
		lastError = "failed to make " + std::to_string(reinterpret_cast<uint64_t>(alContext)) + " as current context";
		return false;
	}

	//Display information
	AnnDebug() << "OpenAL version : " << alGetString(AL_VERSION);
	AnnDebug() << "OpenAL vendor  : " << alGetString(AL_VENDOR);
	return true;
}

void AnnAudioEngine::shutdownOpenAL()
{
	//Stop and delete the bgmSource buffer
	alSourceStop(bgmSource);
	alDeleteSources(1, &bgmSource);

	//Stop and delete other audio sources
	audioSources.clear();

	//Delete the BGM buffer if it has been initialized
	if (alIsBuffer(bgmBuffer) == AL_TRUE)
		alDeleteBuffers(1, &bgmBuffer);

	//Delete all buffers created here
	for (auto buffer : buffers)
		alDeleteBuffers(1, &buffer.second);

	//Close the AL environment
	alcMakeContextCurrent(nullptr);
	alcDestroyContext(alContext);
	alcCloseDevice(alDevice);
	alGetError();//Purge pending error.

	AnnAudioFile::clearSndFileVioStruct();
}

void AnnAudioEngine::preLoadBuffer(const std::string& filename)
{
	loadBuffer(filename);
}

ALuint AnnAudioEngine::isBufferLoader(const std::string& filename)
{
	auto query = buffers.find(filename);
	if (query != buffers.end())
		return query->second;
	return false;
}

ALuint AnnAudioEngine::loadBuffer(const std::string& filename)
{
	if (auto buffer = isBufferLoader(filename)) return buffer;

	AnnDebug() << filename << " Not loaded on an OpenAL buffer, loading from file...";

	//Attempt to retrieve the resource...
	auto audioFileResource = audioFileManager->getResourceByName(filename).staticCast<AnnAudioFile>();
	if (!audioFileResource) //Cannot get it? Load that resource by hand to see
	{
		audioFileResource = audioFileManager->load(filename, AnnGetResourceManager()->defaultResourceGroupName);
		if (!audioFileResource) //Okay, that file doesn't exist or something.
		{
			AnnDebug() << "Error, cannot load file " << filename << " as a recognized audio file";
			return 0;
		}
	}

	// Open Audio file with libsndfile
	SF_INFO fileInfos;
	auto File = sf_open_virtual(audioFileResource->getSndFileVioStruct(), SFM_READ, &fileInfos, audioFileResource.getPointer());

	//get the number of sample and the sample-rate (in samples by seconds)
	auto nbSamples = static_cast<ALsizei>(fileInfos.channels * fileInfos.frames);
	auto sampleRate = static_cast<ALsizei>(fileInfos.samplerate);

	AnnDebug() << "Loading " << nbSamples << " samples. Playback sample-rate : " << sampleRate << "Hz";

	//Read samples in 16bits signed
	std::vector<float> samplesBuffer(nbSamples);
	auto readSamples = sf_read_float(File, samplesBuffer.data(), nbSamples);

	//This sometimes happen with OGG files, but it seems to run fine anyway. This is probably due to meta-data/tags present at the end of files
	if (readSamples < nbSamples)
	{
		lastError = "Warning: It looks like the " + (nbSamples - readSamples);
		lastError += " last samples of the file have been omitted. Ignore if file has meta-data appended at the end. ";
		logError();
	}

	if (sf_error(File) != SF_ERR_NO_ERROR)
	{
		lastError = "Error while reading the file " + filename + " through sndfile library: ";
		lastError += sf_error_number(sf_error(File));
		logError();
		return 0;
	}

	std::vector<ALshort> alSamplesBuffer(nbSamples);
	for (size_t i(0); i < alSamplesBuffer.size(); i++)
		//This will step down a bit the amplitude (88% of max) of the signal to prevent saturation while using some formats (OGG)
		alSamplesBuffer[i] = 0x7FFF * samplesBuffer[i] * 0.88f;

	//close file
	sf_close(File);

	//Read the number of channels. sound effects should be mono and background music should be stereo
	ALenum Format;
	switch (fileInfos.channels)
	{
	case 1: AnnEngine::log("Mono 16bits sound loaded");	Format = AL_FORMAT_MONO16; break;
	case 2: AnnEngine::log("Stereo 16bits sound loaded");  Format = AL_FORMAT_STEREO16; break;

	default: lastError = "Error : File has to have either one or two audio channel to be loaded"; logError(); return 0;
	}

	//create OpenAL buffer
	ALuint buffer;
	alGenBuffers(1, &buffer);
	AnnDebug() << "Created OpenAL buffer at index " << buffer;

	//load data into buffer
	alBufferData(buffer, Format, &alSamplesBuffer[0], nbSamples * sizeof(ALshort), sampleRate);

	//check errors
	if (alGetError() != AL_NO_ERROR)
	{
		lastError = "Error : cannot create an audio buffer for : " + filename;
		logError();
		return 0;
	}

	AnnDebug() << filename << " successfully loaded into audio engine";
	buffers[filename] = buffer;
	return buffer;
}

void AnnAudioEngine::unloadBuffer(const std::string& filename)
{
	if (locked) return;

	//Search for the buffer
	AnnDebug() << "Unloading sound file " << filename;
	auto query = buffers.find(filename);
	if (query == buffers.end())
	{
		lastError = "Error: cannot unload buffer " + filename + " is unknown";
		logError();
		return; //if query is equal to iterator::end(), buffer isn't known
	}

	//Get the buffer from the iterator
	AnnDebug() << "Sound file found by the Audio resource system. OpenAL buffer " << query->second;
	auto buffer = query->second;

	//Free it from memory and remove the object from the buffer list
	alDeleteBuffers(1, &buffer);
	AnnDebug() << "Buffer deleted";
	buffers.erase(query);
}

void AnnAudioEngine::playBGM(const std::string& filename, const float volume)
{
	AnnDebug() << "Using " << filename << " as BGM";

	//Load buffer from disk or cache
	bgmBuffer = loadBuffer(filename);

	//Set parameters to the source
	alSourcei(bgmSource, AL_BUFFER, bgmBuffer);
	alSourcei(bgmSource, AL_LOOPING, AL_TRUE);
	alSourcef(bgmSource, AL_GAIN, volume);

	//Put the source in play mode
	alSourcePlay(bgmSource);
}

void AnnAudioEngine::stopBGM() const
{
	AnnDebug() << "Stop any BGM playing";
	alSourceStop(bgmSource);
}

void AnnAudioEngine::updateListenerPos(AnnVect3 pos)
{
	alListener3f(AL_POSITION, pos.x, pos.y, pos.z);
	//make sure that all positions are synced
	for (auto source : audioSources)
		if (source->posRelToPlayer)
		{
			AnnVect3 absolute = pos + source->pos;
			alSource3f(source->source, AL_POSITION, absolute.x, absolute.y, absolute.z);
		}
}

void AnnAudioEngine::updateListenerOrient(AnnQuaternion orient)
{
	auto At = orient.getAtVector(); // Direction object facing
	auto Up = orient.getUpVector(); // Up Vector

	ALfloat Orientation[] = { At.x, At.y, At.z,
							Up.x, Up.y, Up.z };

	alListenerfv(AL_ORIENTATION, Orientation);
}

void AnnAudioEngine::update()
{
	auto pose = AnnGetEngine()->getHmdPose();
	updateListenerPos(pose.position);
	updateListenerOrient(pose.orientation);
}

std::string AnnAudioEngine::getLastError() const
{
	return lastError;
}

std::shared_ptr<AnnAudioSource> AnnAudioEngine::createSource(std::string filename)
{
	auto source = createSource();
	source->changeSound(filename);
	return source;
}

void AnnAudioEngine::removeSource(std::shared_ptr<AnnAudioSource> source)
{
	audioSources.remove(source);
}

std::shared_ptr<AnnAudioSource> AnnAudioEngine::createSource()
{
	auto audioSource = std::make_shared<AnnAudioSource>();
	audioSource->bufferName = "Nothing";
	alGenSources(1, &audioSource->source);

	audioSources.push_back(audioSource);
	AnnDebug() << "OpenAL:source:" << audioSource->source << " successfully created";

	//Return it to the caller
	return audioSource;
}

AnnAudioSource::AnnAudioSource() :
	source(0),
	pos(AnnVect3::ZERO),
	posRelToPlayer(false)
{
}

AnnAudioSource::~AnnAudioSource()
{
	AnnDebug() << "Destroying source";
	stop();
	alDeleteSources(1, &source);
	source = AL_NONE;
}

void AnnAudioSource::setPositon(AnnVect3 position)
{
	alSource3f(source, AL_POSITION, position.x, position.y, position.z);
	pos = position;
}

void AnnAudioSource::setVolume(float gain) const
{
	alSourcef(source, AL_GAIN, gain);
}

void AnnAudioSource::rewind() const
{
	alSourceRewind(source);
}

void AnnAudioSource::play() const
{
	alSourcePlay(source);
}

void AnnAudioSource::pause() const
{
	alSourcePause(source);
}

void AnnAudioSource::stop() const
{
	alSourceStop(source);
}

void AnnAudioSource::changeSound(std::string filename)
{
	if (filename.empty()) return;
	bufferName = filename;

	auto buffer = AnnGetAudioEngine()->loadBuffer(bufferName);
	if (buffer) alSourcei(source, AL_BUFFER, buffer);
}

void AnnAudioSource::setLooping(bool looping) const
{
	if (looping) alSourcei(source, AL_LOOPING, AL_TRUE);
	else alSourcei(source, AL_LOOPING, AL_FALSE);
}

void AnnAudioSource::setPositionRelToPlayer(bool rel)
{
	posRelToPlayer = rel;
}