/**
* \file AnnFilesystem.hpp
* \brief Implement file I/O for saving game data
* \author A. Brainville (Ybalrid)
*/

#ifndef ANN_FILESYSTEM
#define ANN_FILESYSTEM
#include "systemMacro.h"
#include <string>
#include <map>
#include <algorithm>
#include <vector>
#include <fstream>
#include <sstream>
#include <list>
#include <limits>
#include <memory>

#ifdef WIN32
#include <Windows.h>
#endif

#include "AnnTypes.h"
#include "AnnVect3.hpp"
#include "AnnQuaternion.hpp"
#include "AnnSubsystem.hpp"

namespace Annwvyn
{
	class AnnSaveFileData;

	///Handle opening, writing and closing files
	class DLL AnnFileWriter
	{
	public:
		///Construct file writer object
		AnnFileWriter();

		///Write the fileData to disc in the appropriate directory
		static void write(std::shared_ptr<AnnSaveFileData> dataToWrite);
	};

	///Handle opening, reading and closing files
	class DLL AnnFileReader
	{
	public:
		///Construct file reader object
		AnnFileReader();

		///read the asked file and return a new AnnSaveFileData*
		std::shared_ptr<AnnSaveFileData> read(std::string filename) const;
	};

	class AnnSaveFileData;

	///Manage where and how files will be wrote and read from the OS file-system
	class DLL AnnFilesystemManager : public AnnSubSystem
	{
	public:

		///Construct FileSystem manager
		AnnFilesystemManager(std::string title);

		///Set the name of the app directory
		void setSaveDirectoryName(std::string name);
		///Get the path to the file name
		std::string getPathForFileName(std::string fileName) const;
		///Get the path to the directory where save are read/written
		std::string getSaveDirectoryFullPath() const;

		///Create the given directory (OS call)
		static void createDirectory(std::string path);
		///Create the save directory (should be done at least once)
		void createSaveDirectory() const;

		///Create en empty SaveFileData Object for a specific file
		std::shared_ptr<AnnSaveFileData> crateSaveFileDataObject(std::string filename);

		///Get an already existing SaveFileData object for this filename
		std::shared_ptr<AnnSaveFileData> getCachedSaveFileDataObject(std::string filename);

		///Destroy this SaveFileData Object. Will loose cached data if this file didn't go through the FileWriter
		void releaseSaveFileDataObject(std::shared_ptr<AnnSaveFileData> data);

		///Get the FileReader object
		std::shared_ptr<AnnFileReader> getFileReader() const;

		///Get the FileWriter object
		std::shared_ptr<AnnFileWriter> getFileWriter() const;

	private:
		///Name of the save directory
		std::string saveDirectoryName;

		///Path to the save directory
		std::string pathToUserDir;

		///File writter
		std::shared_ptr<AnnFileWriter> fileWriter;

		///File reader
		std::shared_ptr<AnnFileReader> fileReader;

		///Data cache for saves
		std::list<std::shared_ptr<AnnSaveFileData>> cachedData;
	public:

		///Escape map
		static std::vector<char> charToEscape;

		///Strip map
		static std::vector<char> charToStrip;
	};

	///Class that holds data to read or write
	class DLL AnnSaveFileData
	{
	public:
		///Private constructor of SaveFileData class.
		AnnSaveFileData(std::string name);

		///Get the name of this file
		std::string getFilename() const;

		///Get the value of this key. Return empty if key doesn't exist
		std::string getValue(std::string key);

		///Set the value for this key (string)
		void setValue(std::string key, std::string value);
		///Set the value for this key (C style const string)
		void setValue(std::string, const char* value);
		///Set the value for this key (integer)
		void setValue(std::string key, int value);
		///Set the value for this key (floating point)
		void setValue(std::string key, float value);
		///Set the value for this key (vector as 3 floating point at key.x, key.y, key.z)
		void setValue(std::string key, AnnVect3 vector);
		///Set the value for this key (quaternion as 4 floating point at key.w, key.x, key.y, key.z)
		void setValue(std::string key, AnnQuaternion quaternion);

		///Remove the key and it's value from the stored data
		void clearValue(std::string key);
		///Vectors are stored under 3 keys because they are 3 floats
		void clearVectorValue(std::string key);
		///Quaternions are stored under 4 keys because they are 4 floats
		void clearQuaternionValue(std::string key);

		///Return true if keys were manipulated but changes weren't wrote to disk yet
		bool hasUnsavedChanges() const;

	private:
		friend class AnnFileWriter;
		friend class AnnFileReader;
		friend class AnnFilesystemManager;

		///Name of the file
		std::string fileName;

		///Stored data
		std::map<std::string, std::string> storedTextData;

		///If true, the content of this object should be wrote to disk when possible
		bool changed;
	};

	///Interface class to switch from text to useful data.
	class DLL AnnSaveDataInterpretor
	{
		//Inherit from this to use your saved data

	public:

		///Default destructor
		virtual ~AnnSaveDataInterpretor() = default;

		///FileInterpetor
		AnnSaveDataInterpretor(std::shared_ptr<AnnSaveFileData> data);

		///Get a float from this string
		float stringToFloat(std::string text) const;
		///Get a int from this string
		int stringToInt(std::string text) const;
		///Extract a float from the data-object stored at the given key
		float keyStringToFloat(std::string key) const;
		///Extract a int from the data-object stored at the given key
		int keyStringToInt(std::string key) const;
		///Extract a Vector3 from the data-object stored at the given key
		AnnVect3 keyStringToVect3(std::string key) const;
		///Extract a quaternion from the data-object stored at the given key
		AnnQuaternion keyStringToQuaternion(std::string key) const;

		///Overload this method with
		virtual void extract() = 0;

	protected:
		///Object
		std::shared_ptr<AnnSaveFileData> dataObject;
	};
}

#endif