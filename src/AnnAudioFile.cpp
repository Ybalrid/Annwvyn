#include "stdafx.h"
#include "AnnAudioFile.hpp"
#include "AnnLogger.hpp"

void Annwvyn::AnnAudioFile::readFromStream(Ogre::DataStreamPtr& stream)
{
	AnnDebug() << "Reading audio data...";
	const auto size = stream->size();
	AnnDebug() << "Will read " << size << " bytes";

	data.resize(size);
	AnnDebug() << "New size of audio data is " << data.size() << " elements of size " << sizeof data[0] << " bytes";
	const auto read = stream->read(reinterpret_cast<void*>(data.data()), size);
	AnnDebug() << "Read " << read << " bytes into audio data vector";
}

//Ogre's loading from disk
void Annwvyn::AnnAudioFile::loadImpl()
{
	AnnDebug() << "AnnAudioFile::loadImpl for resource (" << mName << ", " << mGroup << ")";
	auto stream = Ogre::ResourceGroupManager::getSingleton().openResource(mName, mGroup, true, this);
	readFromStream(stream);
}

void Annwvyn::AnnAudioFile::unloadImpl()
{
	data.clear();
}

size_t Annwvyn::AnnAudioFile::calculateSize() const
{
	return getSize();
}

Annwvyn::AnnAudioFile::AnnAudioFile(Ogre::ResourceManager* creator, const Ogre::String& name, Ogre::ResourceHandle handle, const Ogre::String& group, bool isManual, Ogre::ManualResourceLoader* loader) :
	Ogre::Resource(creator, name, handle, group, isManual, loader),
	sf_offset(0)
{
	createParamDictionary("AnnAudioFile");
}

Annwvyn::AnnAudioFile::~AnnAudioFile()
{
	AnnAudioFile::unload();
}

const Annwvyn::byte* Annwvyn::AnnAudioFile::getData() const
{
	return data.data();
}

//Singleton
template<> Annwvyn::AnnAudioFileManager* Ogre::Singleton <Annwvyn::AnnAudioFileManager>::msSingleton = nullptr;

Ogre::Resource* Annwvyn::AnnAudioFileManager::createImpl(const Ogre::String& name, Ogre::ResourceHandle handle, const Ogre::String& group, bool isManual, Ogre::ManualResourceLoader* loader, const Ogre::NameValuePairList* createParams)
{
	return new AnnAudioFile(this, name, handle, group, isManual, loader);
}

Annwvyn::AnnAudioFileManager::AnnAudioFileManager()
{
	mResourceType = "AnnAudioFile";
	mLoadOrder = 2;
	Ogre::ResourceGroupManager::getSingleton()._registerResourceManager(mResourceType, this);
}

Annwvyn::AnnAudioFileManager::~AnnAudioFileManager()
{
	Ogre::ResourceGroupManager::getSingleton()._unregisterResourceManager(mResourceType);
}

Annwvyn::AnnAudioFilePtr Annwvyn::AnnAudioFileManager::load(const Ogre::String& name, const Ogre::String& group)
{
	auto file = createOrRetrieve(name, group).first.staticCast<AnnAudioFile>();
	file->load();
	return file;
}

Annwvyn::AnnAudioFileManager& Annwvyn::AnnAudioFileManager::getSingleton()
{
	return *msSingleton;
}

Annwvyn::AnnAudioFileManager* Annwvyn::AnnAudioFileManager::getSingletonPtr()
{
	return msSingleton;
}

sf_count_t Annwvyn::AnnAudioFile::sfVioGetFileLen(void* audioFileRawPtr)
{
	auto file = cast(audioFileRawPtr);
	return file->getSize();
}

sf_count_t Annwvyn::AnnAudioFile::sfVioSeek(sf_count_t offset, int whence, void* audioFileRawPtr)
{
	auto file = cast(audioFileRawPtr);

	switch (whence)
	{
	case SEEK_CUR:
		AnnDebug() << "sndfile seek cursor position " << offset << " from current";
		file->sf_offset += offset;
		break;
	case SEEK_END:
		AnnDebug() << "sndfile seek cursor position " << offset << " from end";
		file->sf_offset = file->getSize() - offset;
		break;
	case SEEK_SET:
		AnnDebug() << "sndfile seek cursor position " << offset;
		file->sf_offset = offset;
		break;
	default:
		AnnDebug() << "invalid whence when sdVioSeek";
		break;
	}

	return file->sf_offset;
}

sf_count_t Annwvyn::AnnAudioFile::sfVioRead(void* ptr, sf_count_t count, void* audioFileRawPtr)
{
	auto file = cast(audioFileRawPtr);

	//Get the absolute start position in the array
	const auto start = file->sf_offset;
	//For bound checking
	const auto stop = std::min(size_t(start + count), file->getSize());
	const auto bytesCopied = stop - start;

	//Load bytes into output buffer
	memcpy(ptr, reinterpret_cast<void*>(file->data.data() + start), bytesCopied);

	//advance cursor
	file->sf_offset += bytesCopied;
	return bytesCopied;
}

sf_count_t Annwvyn::AnnAudioFile::sfVioWriteDummy(const void*, sf_count_t, void*)
{
	return 0; //we do not write.
}

sf_count_t Annwvyn::AnnAudioFile::sfVioTell(void* audioFileRawPtr)
{
	const auto file = cast(audioFileRawPtr);
	AnnDebug() << "telling sndfile that current cursor position is " << file->sf_offset;
	return file->sf_offset;
}

Annwvyn::AnnAudioFile* Annwvyn::AnnAudioFile::cast(void* audioFileRawPtr)
{
	return static_cast<AnnAudioFile*>(audioFileRawPtr);
}

SF_VIRTUAL_IO* Annwvyn::AnnAudioFile::sfVioStruct{ nullptr };

SF_VIRTUAL_IO* Annwvyn::AnnAudioFile::getSndFileVioStruct()
{
	//Lazy initialize the virtual IO structure
	if (!sfVioStruct)
	{
		AnnDebug() << "Initializing the Virtual I/O callbacks for libsndfile";
		sfVioStruct = new SF_VIRTUAL_IO;

		//Fill it with function pointers to static methods...
		sfVioStruct->get_filelen = &AnnAudioFile::sfVioGetFileLen;
		sfVioStruct->read = &AnnAudioFile::sfVioRead;
		sfVioStruct->write = &AnnAudioFile::sfVioWriteDummy;
		sfVioStruct->seek = &AnnAudioFile::sfVioSeek;
		sfVioStruct->tell = &AnnAudioFile::sfVioTell;
	}

	return sfVioStruct;
}

void Annwvyn::AnnAudioFile::clearSndFileVioStruct()
{
	if (sfVioStruct) delete sfVioStruct;
	sfVioStruct = nullptr;
}

size_t Annwvyn::AnnAudioFile::getSize() const
{
	return data.size();
}