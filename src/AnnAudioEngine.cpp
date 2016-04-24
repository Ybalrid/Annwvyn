#include "stdafx.h"
#include "AnnAudioEngine.hpp"
#include "AnnLogger.hpp"

using namespace Annwvyn;

AnnAudioEngine::AnnAudioEngine()
{
	AnnDebug() << "Starting Audio subsystem";
	lastError = "Initialize OpenAL based sound system";

	//Try to init OpenAL
	if(!initOpenAL())
		lastError = "Cannot Init OpenAL";
	logError();

	//Set the listener to base position
	alListener3f(AL_POSITION, 0.0f, 0.0f, 10.0f);

	//Define the default orientation 
	ALfloat Orientation[] = {0.0f, 0.0f, 1.0f, //LookAt vector
							0.0f, 1.0f, 0.0f}; //Up vector

	//Apply the orientation
	alListenerfv(AL_ORIENTATION, Orientation);
	
	//Create a soruce for the BGM
	alGenSources(1, &bgm);
	locked = false;
}

void AnnAudioEngine::logError()
{
	AnnDebug() << lastError;
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

	AnnDebug() << "OpenAL version : " << alGetString(AL_VERSION);
	AnnDebug() << "OpenAL vendor  : " << alGetString(AL_VENDOR);

    return true;
}

void AnnAudioEngine::shutdownOpenAL()
{
	//Stop and delete the bgm buffer
	alSourceStop(bgm);
    alDeleteSources(1, &bgm);
	
	//Stop and delete other audio sources
	for(auto source : AudioSources)
		delete source;
	//Delete the BGM buffer if it has been initialized
	if(alIsBuffer(bgmBuffer) == AL_TRUE)
		alDeleteBuffers(1,&bgmBuffer);
	//Delete all buffers created here
	for(auto buffer : buffers)
		alDeleteBuffers(1, &buffer.second);

	//Close the AL environement 
    alcMakeContextCurrent(NULL);
	alcDestroyContext(Context);
	alcCloseDevice(Device);
    alGetError();//Purge pending error.
}

void AnnAudioEngine::preLoadBuffer(const std::string& filepath)
{
	loadBuffer(filepath);
}

ALuint AnnAudioEngine::isBufferLoader(const std::string& filepath)
{
	auto query = buffers.find(filepath);
	if(query != buffers.end())
		return query->second;
	return false;
}

ALuint AnnAudioEngine::loadBuffer(const std::string& filepath)
{
	if(ALuint buffer = isBufferLoader(filepath))
		return buffer;
	
	AnnDebug() << filepath << " is unkown to the engine. Loading from file...";

	// Open Audio file with libsndfile
    SF_INFO FileInfos;
    SNDFILE* File = sf_open(filepath.c_str(), SFM_READ, &FileInfos);
    if (!File)
	{
		AnnDebug() << "Error, cannot load file " << filepath << " as a recognized audio file";
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
		lastError = "Error while reading the file" + filepath + " through sndfile library\nsndfile error: ";
		lastError +=  sf_error_number(sf_error(File));
		logError();
        //return 0;
	}

	std::vector<ALshort> Samples(NbSamples);
	for(size_t i(0); i < Samples.size(); i++)
		//This will step down a bit the amplitude (88% of max) of the signal to prevent saturation while using some formats (OGG)
		Samples[i] = 0x7FFF * SamplesFloat[i] * 0.88f;

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
		lastError = "Error : cannot create an audio buffer for : " + filepath;
		logError();
        return 0;
	}

	AnnDebug() << filepath << " sucessfully loaded into audio engine";
	AnnEngine::log("buffer added to the Audio engine");
	buffers[filepath] = buffer;
    return buffer;
}

void AnnAudioEngine::unloadBuffer(const std::string& path)
{
	if(locked) return;

	//Search for the buffer
	AnnDebug() << "Unloading soudfile " << path;
	auto query = buffers.find(path);
	if(query == buffers.end()) 
	{
		lastError = "Error: cannot unload buffer " + path + " is unknown";
		logError();
		return; //if querry is equal to iterator::end(), buffer isn't known
	}

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
	AnnDebug() << "Using " << path << " as BGM";
	
	//Load buffer from disk or cache
	bgmBuffer = loadBuffer(path);

	//Set parameters to the source
	alSourcei(bgm, AL_BUFFER, bgmBuffer);	
	alSourcei(bgm, AL_LOOPING, AL_TRUE);
	alSourcef(bgm, AL_GAIN, volume);

	//Put the source in play mode
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
	//make sure that all positions are synced
	for(auto source : AudioSources)
		if(source->posRelToPlayer)
		{
			AnnVect3 absolute = pos+source->pos; 
			alSource3f(source->source, AL_POSITION, absolute.x, absolute.y, absolute.z);
		}
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
AnnAudioSource* AnnAudioEngine::createSource(std::string path)
{
	auto source = createSource();
	source->changeSound(path);
	return source;
}

void Annwvyn::AnnAudioEngine::destroySource(AnnAudioSource * source)
{
	if (!source) return;
	AudioSources.remove(source);
	delete source;
}

AnnAudioSource* AnnAudioEngine::createSource()
{
	
	AnnAudioSource* audioSource = new AnnAudioSource;
	audioSource->bufferName = "Nothing";
	alGenSources(1, &audioSource->source);
	
	AudioSources.push_back(audioSource);
	AnnDebug() << "OpenAL:source:" << audioSource->source << " sucessfully created";

	//Return it to the caller
	return audioSource;
}

AnnAudioSource::AnnAudioSource()
{
	source = 0;
	pos = AnnVect3::ZERO;
}

AnnAudioSource::~AnnAudioSource()
{
	stop();
	alDeleteSources(1, &source);
	source = AL_NONE;
}

void AnnAudioSource::setPositon(AnnVect3 position)
{
	alSource3f(source, AL_POSITION, position.x, position.y, position.z);
	pos = position;
}

void AnnAudioSource::setVolume(float gain)
{
	alSourcef(source, AL_GAIN, gain);
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

void Annwvyn::AnnAudioSource::changeSound(std::string path)
{
	if (path.empty()) return;
	bufferName = path;
	
	ALuint buffer = AnnEngine::Instance()->getAudioEngine()->loadBuffer(bufferName);
	if (buffer) alSourcei(source, AL_BUFFER, buffer);
}

void AnnAudioSource::setLooping(bool looping)
{
	if(looping) alSourcei(source, AL_LOOPING, AL_TRUE);
	else alSourcei(source, AL_LOOPING, AL_FALSE);
}

void AnnAudioSource::setPositionRelToPlayer(bool rel)
{
	posRelToPlayer = rel;
}


