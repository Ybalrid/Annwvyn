#include "stdafx.h"
#include "AnnAudioEngine.hpp"
#include "AnnLogger.hpp" //for accessing the logger

using namespace Annwvyn;

AnnAudioEngine::AnnAudioEngine()
{
	lastError = "Initialize OpenAL based sound system";
	if(!initOpenAL())
		lastError = "Cannot Init OpenAL";

	Ogre::LogManager::getSingleton().logMessage(lastError);

	alListener3f(AL_POSITION, 0.0f, 0.0f, 10.0f);

	ALfloat Orientation[] = {0.0f, 0.0f, 1.0f,
							0.0f, 1.0f, 0.0f};

	alListenerfv(AL_ORIENTATION, Orientation);
	alGenSources(1, &bgm);
	locked = false;
}

AnnAudioEngine::~AnnAudioEngine()
{
	locked = true;
	shutdownOpenAL();
}

bool AnnAudioEngine::initOpenAL()
{
    Device = alcOpenDevice(NULL);
    if (!Device)
        return false;
   	Context = alcCreateContext(Device, NULL);
    if (!Context)
        return false;
     if (!alcMakeContextCurrent(Context))
        return false;
    return true;
}

void AnnAudioEngine::shutdownOpenAL()
{
	//Stop and delete the bgm buffer
	alSourceStop(bgm);
    alDeleteSources(1, &bgm);
	if(alIsBuffer(bgmBuffer) == AL_TRUE)
		alDeleteBuffers(1,&bgmBuffer);

	//Delete all buffers created here
	auto iterator = buffers.begin();
	while(iterator != buffers.end())
		alDeleteBuffers(1, &(*iterator++).second);
		
	//Close the AL environement 
    alcMakeContextCurrent(NULL);
	alcDestroyContext(Context);
	alcCloseDevice(Device);
    alGetError();
}

ALuint AnnAudioEngine::loadSndFile(const std::string& Filename)
{
	AnnDebug() << "Loading audio file : " << Filename;
	AnnDebug() << "checking if file allready loaded on the soundEngine";
	
	auto query = buffers.find(Filename);
	if(query != buffers.end())
	{
		AnnDebug() << Filename << " allready loaded. Will use the coresponding buffer";
		return query->second;
	}
	AnnEngine::log("This sound resource is unkown to the engine. Loading from file...");

	// Open Audio file with libsndfile
    SF_INFO FileInfos;
    SNDFILE* File = sf_open(Filename.c_str(), SFM_READ, &FileInfos);
    if (!File)
	{
		AnnDebug() << "Error, cannot load file " << Filename << " as a recognized audio file";
        return 0;
	}

	//get the number of sample and the samplerate (in samples by seconds)
    ALsizei NbSamples  = static_cast<ALsizei>(FileInfos.channels * FileInfos.frames);
    ALsizei SampleRate = static_cast<ALsizei>(FileInfos.samplerate);

	AnnDebug() << "Loading " << NbSamples << " samples. Playback samplerate : " << SampleRate;
	
	//Read samples in 16bits signed
	std::vector<float> SamplesFloat(NbSamples);
    if (sf_read_float(File, &SamplesFloat[0], NbSamples) < NbSamples)
	{
		lastError = "Error while reading the file" + Filename + " through sndfile library";
		AnnDebug() << lastError;
		AnnEngine::log(sf_error_number(sf_error(File)));
        //return 0;
	}

	std::vector<ALshort> Samples(NbSamples);
	ALshort max = 0x7FFF; //biggest 16bits siged float (positive)
	for(size_t i(0); i < Samples.size(); i++)
		//This will step down a bit the amplitude of the signal to prevent saturation while using some formats (OGG)
		Samples[i] = max*SamplesFloat[i]*0.88f;

	//close file
    sf_close(File);

	//Read the number of chanels. sound effects should be mono and background music should be stereo
    ALenum Format;
    switch (FileInfos.channels)
    {
		case 1 : AnnEngine::log("Mono 16bits sound loaded");	Format = AL_FORMAT_MONO16;   break;
		case 2 : AnnEngine::log("Stereo 16bits sound loaded");  Format = AL_FORMAT_STEREO16; break;
		
        default : return 0;
    }
	
    //create OpenAL buffer
	ALuint buffer;
    alGenBuffers(1, &buffer);
	AnnDebug() << "Created OpenAL buffer at index "<< buffer;

	// load buffer
    alBufferData(buffer, Format, &Samples[0], NbSamples * sizeof(ALshort), SampleRate);
 
    //check errors
    if (alGetError() != AL_NO_ERROR)
	{
		lastError = "Error : cannot create an audio buffer for : " + Filename;
		AnnEngine::log(lastError);
        return 0;
	}

	AnnDebug() << Filename << " sucessfully loaded into audio engine";
	AnnEngine::log("buffer added to the Audio engine");
	buffers[Filename] = buffer;
    return buffer;
}

void AnnAudioEngine::unloadBuffer(const std::string& path)
{
	if(locked) return;

	//Search for the buffer
	AnnDebug() << "Unloading soudfile " << path;
	auto query = buffers.find(path);
	if(query == buffers.end()) return; //if querry is equal to iterator::end(), buffer isn't known

	//Get the buffer from the iterator
	AnnDebug() << "Sound file found by the Audio resource system. OpenAL buffer " << query->second;
	ALuint buffer = query->second;

	//Free it from memory and remove the object from the buffer list
	alDeleteBuffers(1, &buffer);
	AnnDebug() << "Buffer deleted";
	buffers.erase(query);
}

void AnnAudioEngine::playBGM(const std::string path, const float volume)
{
    bgmBuffer = loadSndFile(path);

	AnnDebug() << "Using " << path << " as BGM";

	alSourcei(bgm, AL_BUFFER, bgmBuffer);	
	alSourcei(bgm, AL_LOOPING, AL_TRUE);
	alSourcef(bgm, AL_GAIN, volume);

	alSourcePlay(bgm);
}

void AnnAudioEngine::stopBGM()
{
	AnnDebug() << "Stop any BGM playing";
	alSourceStop(bgm);
}

void AnnAudioEngine::updateListenerPos(AnnVect3 pos)
{
	alListener3f(AL_POSITION, pos.x, pos.y, pos.z);
}

void AnnAudioEngine::updateListenerOrient(AnnQuaternion orient)
{
	Ogre::Vector3 At = orient.getAtVector(); // Direction object facing 
	Ogre::Vector3 Up = orient.getUpVector(); // Up Vector 

	ALfloat Orientation[] = {At.x, At.y, At.z,
							Up.x, Up.y, Up.z};

	alListenerfv(AL_ORIENTATION, Orientation);
}

const std::string AnnAudioEngine::getLastError()
{
	return lastError;
}

AnnAudioSource* AnnAudioEngine::createSource(const std::string& path)
{
	ALuint buffer = loadSndFile(path);
	if(buffer == 0)
	{
		AnnDebug() << "Cannot create audio source " << path;
		return nullptr;
	}

	AnnAudioSource* audioSource = new AnnAudioSource;
	audioSource->bufferName = path;
	alGenSources(1, &audioSource->source);
	alSourcei(audioSource->source, AL_BUFFER, buffer);

	return audioSource;
}

AnnAudioSource::AnnAudioSource()
{
	source = 0;
}

void AnnAudioSource::setPositon(AnnVect3 position)
{
	alSource3f(source, AL_POSITION, position.x, position.y, position.z);
}

void AnnAudioSource::setVolume(float gain)
{
	alSourcef(source, AL_GAIN, gain);
	AnnDebug() << bufferName << ":s:" << source << " set volume to : " << gain;
}

void AnnAudioSource::rewind()
{
	alSourceRewind(source);
}

void AnnAudioSource::play()
{
	alSourcePlay(source);
}

void AnnAudioSource::pause()
{
	alSourcePause(source);
}

void AnnAudioSource::stop()
{
	alSourceStop(source);
}

void AnnAudioSource::setLooping(bool looping)
{
	if(looping) alSourcei(source, AL_LOOPING, AL_TRUE);
	else 	alSourcei(source, AL_LOOPING, AL_FALSE);
}