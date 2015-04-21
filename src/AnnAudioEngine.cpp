#include "AnnAudioEngine.hpp"
#include "AnnEngine.hpp" //for accessing the logger
using namespace Annwvyn;

AnnAudioEngine::AnnAudioEngine()
{
	lastError = "Initialize OpenAL based sound system";
	if(!initOpenAL())
		lastError = "Cannot Init OpenAL";

	//std::cerr << lastError << std::endl;

	Ogre::LogManager::getSingleton().logMessage(lastError);

	alListener3f(AL_POSITION, 0.0f, 0.0f, 10.0f);

	ALfloat Orientation[] = {0.0f, 0.0f, 1.0f,
							0.0f, 1.0f, 0.0f};

	alListenerfv(AL_ORIENTATION, Orientation);

}

AnnAudioEngine::~AnnAudioEngine()
{
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
	alSourceStop(bgm);
    alDeleteSources(1,&bgm);
    alDeleteBuffers(1,&buffer);
    alcMakeContextCurrent(NULL);
	alcDestroyContext(Context);
	alcCloseDevice(Device);
    alGetError();
}

ALuint AnnAudioEngine::loadSndFile(const std::string& Filename)
{
	std::stringstream ss; 
	ss << "Loading audio file : " << Filename;
	AnnEngine::log(ss.str());
	ss.str("");

	// Open Audio file with libsndfile
    SF_INFO FileInfos;
    SNDFILE* File = sf_open(Filename.c_str(), SFM_READ, &FileInfos);
    if (!File)
	{
		ss << "Error, cannot load file " << Filename << " as a recognized audio file";
		AnnEngine::log(ss.str());
		ss.str("");
        return 0;
	}

	// Lecture du nombre d'échantillons et du taux d'échantillonnage (nombre d'échantillons à lire par seconde)
    ALsizei NbSamples  = static_cast<ALsizei>(FileInfos.channels * FileInfos.frames);
    ALsizei SampleRate = static_cast<ALsizei>(FileInfos.samplerate);

	 // Lecture des échantillons audio au format entier 16 bits signé (le plus commun)
    std::vector<ALshort> Samples(NbSamples);
    if (sf_read_short(File, &Samples[0], NbSamples) < NbSamples)
	{
		lastError = "Error while reading the file" + Filename + " through sndfile library";
		AnnEngine::log(lastError);
        return 0;
	}

	 // close file
    sf_close(File);

	// read canal number
    ALenum Format;
    switch (FileInfos.channels)
    {
        case 1 :  Format = AL_FORMAT_MONO16;   break;
        case 2 :  Format = AL_FORMAT_STEREO16; break;
        default : return 0;
    }

	
    // create OpenAL buffer
    alGenBuffers(1, &buffer);
	
	// load buffer
    alBufferData(buffer, Format, &Samples[0], NbSamples * sizeof(ALushort), SampleRate);
 
    // check errors
    if (alGetError() != AL_NO_ERROR)
	{
		lastError = "Error : cannot create an audio buffer for : " + Filename;
		AnnEngine::log(lastError);
        return 0;
	}
 
	ss.clear();
	ss << Filename << " sucessfully loaded into audio engine";
	AnnEngine::log(ss.str());
	ss.str("");
    return buffer;
}

void AnnAudioEngine::playBGM(const std::string path, const float volume)
{
    loadSndFile(path);
//	ALuint buffer = loadSndFile(path);

	std::stringstream ss; 
	ss << "Using " << path << " as BGM";
	AnnEngine::log(ss.str());

	alGenSources(1, &bgm);
	alSourcei(bgm, AL_BUFFER, buffer);
	alSourcei(bgm, AL_LOOPING, AL_TRUE);
	alSourcef(bgm, AL_GAIN, volume);

	alSourcePlay(bgm);
}

void AnnAudioEngine::updateListenerPos(Ogre::Vector3 pos)
{
	alListener3f(AL_POSITION, pos.x, pos.y, pos.z);
}

void AnnAudioEngine::updateListenerOrient(Ogre::Quaternion orient)
{
	Ogre::Vector3 At = (orient * Ogre::Vector3::NEGATIVE_UNIT_Z); // Direction object facing 
	Ogre::Vector3 Up = (orient * Ogre::Vector3::UNIT_Y); // Up Vector 

	ALfloat Orientation[] = {At.x, At.y, At.z,
							Up.x, Up.y, Up.z};

	alListenerfv(AL_ORIENTATION, Orientation);
}

const std::string AnnAudioEngine::getLastError()
{
	return lastError;
}