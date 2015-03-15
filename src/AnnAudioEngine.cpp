#include "AnnAudioEngine.hpp"

using namespace Annwvyn;

AnnAudioEngine::AnnAudioEngine()
{
	lastError = "No error";
	if(!initOpenAL())
		lastError = "Cannot Init OpenAL";

	std::cerr << lastError << std::endl;
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
	 // Open Audio file with libsndfile
    SF_INFO FileInfos;
    SNDFILE* File = sf_open(Filename.c_str(), SFM_READ, &FileInfos);
    if (!File)
        return 0;

	// Lecture du nombre d'échantillons et du taux d'échantillonnage (nombre d'échantillons ŕ lire par seconde)
    ALsizei NbSamples  = static_cast<ALsizei>(FileInfos.channels * FileInfos.frames);
    ALsizei SampleRate = static_cast<ALsizei>(FileInfos.samplerate);

	 // Lecture des échantillons audio au format entier 16 bits signé (le plus commun)
    std::vector<ALshort> Samples(NbSamples);
    if (sf_read_short(File, &Samples[0], NbSamples) < NbSamples)
        return 0;

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
//    ALuint Buffer;
    alGenBuffers(1, &buffer);
	
	// load buffer
    alBufferData(buffer, Format, &Samples[0], NbSamples * sizeof(ALushort), SampleRate);
 
    // check errors
    if (alGetError() != AL_NO_ERROR)
        return 0;
 
    return buffer;
}

void AnnAudioEngine::playBGM(const std::string path, const float volume)
{
    loadSndFile(path);
//	ALuint buffer = loadSndFile(path);

	alGenSources(1, &bgm);
	alSourcei(bgm, AL_BUFFER, buffer);
	alSourcei(bgm, AL_LOOPING, AL_TRUE);
	alSourcef(bgm,AL_GAIN,volume);

	alSourcePlay(bgm);
}

void AnnAudioEngine::updateListenerPos(Ogre::Vector3 pos)
{
	Ogre::LogManager::getSingleton().logMessage("OPENAL POSITION UPDATE");
	alListener3f(AL_POSITION, pos.x, pos.y, pos.z);
}

void AnnAudioEngine::updateListenerOrient(Ogre::Quaternion orient)
{
	Ogre::LogManager::getSingleton().logMessage("OPENAL ORIENTATION UPDATE");
	Ogre::Vector3 At = (orient * Ogre::Vector3::NEGATIVE_UNIT_Z); // Direction object facing 
	Ogre::Vector3 Up = (orient * Ogre::Vector3::UNIT_Y); // Up Vector 

	ALfloat Orientation[] = {At.x, At.y, At.z,
							Up.x, Up.y, Up.z};

	alListenerfv(AL_ORIENTATION, Orientation);
}
