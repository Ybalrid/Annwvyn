// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "stdafx.h"

#include <sndfile.h>
#include "AnnAudioFile.hpp"
#include "AnnLogger.hpp"

using namespace Annwvyn;
using namespace Ogre;

void AnnAudioFile::readFromStream(DataStreamPtr& stream)
{
	AnnDebug() << "Reading audio data...";
	data.resize(stream->size());
	const auto read = stream->read(reinterpret_cast<void*>(data.data()), data.size());
	AnnDebug() << "Read " << read << " raw bytes into audio data vector";
}

//Ogre's loading from disk
void AnnAudioFile::loadImpl()
{
	AnnDebug() << "AnnAudioFile::loadImpl for resource (" << mName << ", " << mGroup << ")";
	auto stream = ResourceGroupManager::getSingleton().openResource(mName, mGroup, true, this);
	readFromStream(stream);
}

void AnnAudioFile::unloadImpl()
{
	data.clear();
}

size_t AnnAudioFile::calculateSize() const
{
	return getSize();
}

AnnAudioFile::AnnAudioFile(ResourceManager* creator, const String& name, ResourceHandle handle, const String& group, bool isManual, ManualResourceLoader* loader) :
	Resource(creator, name, handle, group, isManual, loader),
	sf_offset(0)
{
	createParamDictionary("AnnAudioFile");
}

AnnAudioFile::~AnnAudioFile()
{
	AnnAudioFile::unload();
}

const byte* AnnAudioFile::getData() const
{
	return data.data();
}

Resource* AnnAudioFileManager::createImpl(const String& name, ResourceHandle handle, const String& group, bool isManual, ManualResourceLoader* loader, const NameValuePairList* createParams)
{
	return OGRE_NEW AnnAudioFile(this, name, handle, group, isManual, loader);
}

//Singleton
template<> AnnAudioFileManager *Singleton<AnnAudioFileManager>::msSingleton = nullptr;

AnnAudioFileManager::AnnAudioFileManager()
{
	mResourceType = "AnnAudioFile";
	mLoadOrder = 2;
	ResourceGroupManager::getSingleton()._registerResourceManager(mResourceType, this);
}

AnnAudioFileManager::~AnnAudioFileManager()
{
	ResourceGroupManager::getSingleton()._unregisterResourceManager(mResourceType);
}

AnnAudioFilePtr AnnAudioFileManager::load(const String& name, const String& group)
{
	auto file = createOrRetrieve(name, group).first.staticCast<AnnAudioFile>();
	file->load();
	return file;
}

AnnAudioFileManager& AnnAudioFileManager::getSingleton()
{
	return *msSingleton;
}

AnnAudioFileManager* AnnAudioFileManager::getSingletonPtr()
{
	return msSingleton;
}

sf_count_t AnnAudioFile::sfVioGetFileLen(void* audioFileRawPtr)
{
	const auto file = cast(audioFileRawPtr);
	return file->getSize();
}

sf_count_t AnnAudioFile::sfVioSeek(sf_count_t offset, int whence, void* audioFileRawPtr)
{
	auto file = cast(audioFileRawPtr);

	switch (whence)
	{
	case SEEK_CUR:
		//AnnDebug() << "sndfile seek cursor position " << offset << " from current";
		file->sf_offset += offset;
		break;
	case SEEK_END:
		//AnnDebug() << "sndfile seek cursor position " << offset << " from end";
		file->sf_offset = file->getSize() - offset;
		break;
	case SEEK_SET:
		//AnnDebug() << "sndfile seek cursor position " << offset;
		file->sf_offset = offset;
		break;
	default:
		AnnDebug() << "invalid whence when sfVioSeek";
		break;
	}

	return file->sf_offset;
}

sf_count_t AnnAudioFile::sfVioRead(void* ptr, sf_count_t count, void* audioFileRawPtr)
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

sf_count_t AnnAudioFile::sfVioWriteDummy(const void*, sf_count_t, void*)
{
	return 0; //we do not write.
}

sf_count_t AnnAudioFile::sfVioTell(void* audioFileRawPtr)
{
	const auto file = cast(audioFileRawPtr);
	//AnnDebug() << "telling sndfile that current cursor position is " << file->sf_offset;
	return file->sf_offset;
}

AnnAudioFile* AnnAudioFile::cast(void* audioFileRawPtr)
{
	return static_cast<AnnAudioFile*>(audioFileRawPtr);
}

std::unique_ptr<SF_VIRTUAL_IO> AnnAudioFile::sfVioStruct{ nullptr };

SF_VIRTUAL_IO* AnnAudioFile::getSndFileVioStruct()
{
	//Lazy initialize the virtual IO structure
	if (!sfVioStruct)
	{
		AnnDebug() << "Initializing the Virtual I/O callbacks for libsndfile";
		sfVioStruct = std::make_unique<SF_VIRTUAL_IO>();

		//Fill it with function pointers to static methods...
		sfVioStruct->get_filelen = &AnnAudioFile::sfVioGetFileLen;
		sfVioStruct->read = &AnnAudioFile::sfVioRead;
		sfVioStruct->write = &AnnAudioFile::sfVioWriteDummy;
		sfVioStruct->seek = &AnnAudioFile::sfVioSeek;
		sfVioStruct->tell = &AnnAudioFile::sfVioTell;
	}

	return sfVioStruct.get();
}

void AnnAudioFile::clearSndFileVioStruct()
{
	if (sfVioStruct) sfVioStruct.reset();
}

size_t AnnAudioFile::getSize() const
{
	return data.size();
}
