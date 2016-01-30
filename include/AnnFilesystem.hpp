#ifndef ANN_FILESYSTEM
#define ANN_FILESYSTEM
#include "systemMacro.h"
#include <string>
#include <map>
#include <algorithm>
#include <vector>
#include <fstream>
#include <sstream>
#include <regex>
#ifdef WIN32
#include <Windows.h>
#endif

#include "AnnTypes.h"
#include "AnnVect3.hpp"
#include "AnnQuaternion.hpp"

using namespace std;

namespace Annwvyn
{
	class AnnSaveFileData;

	///Handle opening, writing and closing files
	class DLL AnnFileWriter
	{
	public:
		AnnFileWriter();
		///Write the fileData to disc in the appropriate directory
		void write(AnnSaveFileData* dataToWrite);
	};

	///Handle opening, reading and closing files
	class DLL AnnFileReader
	{
	public:
		AnnFileReader();
		///read the asked file and return a new AnnSaveFileData*
		AnnSaveFileData* read(string filename);
	};

	class AnnSaveFileData;

	///Manage where and how files will be wrote and read from the OS filesystem
	class DLL AnnFilesystemManager
	{
	public:
		AnnFilesystemManager();
		~AnnFilesystemManager();

		///Set the name of the app directory
		void setSaveDirectoryName(string name);
		///Get the path to the file name
		string getPathForFileName(string fileName);
		///Get the path to the directory where save are read/written
		string getSaveDirectoryFullPath();
		///Create the given directory (OS call)
		static void createDirectory(string path);
		///Create the save directory (should be done at least once
		void createSaveDirectory();

		///Create en empty SaveFileData Object for a specific file
		AnnSaveFileData* crateSaveFileDataObject(string filename);
		///Get an allready existing SaveFileData object for this filename
		AnnSaveFileData* getCachedSaveFileDataObject(string filename);
		///Destroy this SaveFileData Object. Will loose cached data if this file didn't go through the FileWriter
		void destroySaveFileDataObject(AnnSaveFileData* data);
		
		///Get the FileReader object
		AnnFileReader* getFileReader();
		///Get the FileWriter object
		AnnFileWriter* getFileWriter();

	private:
		string saveDirectoryName;
		string pathToUserDir;
		AnnFileWriter* fileWriter;
		AnnFileReader* fileReader;
		std::list<AnnSaveFileData*> cachedData;
	public:
		static std::vector<char> charToEscape;
		static std::vector<char> charToStrip;

	};
	
	///Class that holds data to read or write
	class DLL AnnSaveFileData
	{
	public:

		///Get the name of this file
		std::string getFilename();

		///Get the value of this key. Return empty if key doesn't exist
		std::string getValue(std::string key);

		///Set the value for this key 
		void setValue(std::string key, std::string value);
		void setValue(std::string, const char* value);
		void setValue(std::string key, int value);
		void setValue(std::string key, float value);
		void setValue(std::string key, AnnVect3 vector);
		void setValue(std::string key, AnnQuaternion quaternion);
	private:
		friend class AnnFileWriter;
		friend class AnnFileReader;
		friend class AnnFilesystemManager;

		///Private constructor of SaveFileData class. 
		AnnSaveFileData(std::string name);
		std::string fileName;
		std::map<std::string, std::string> storedTextData; 
	};

	///Interface class to switch from text to usefull data. 
	///Inherit from this to use your saved data
	class DLL AnnSaveDataInterpretor
	{
	public:
		///FileInterpetor
		AnnSaveDataInterpretor(AnnSaveFileData* data);
		float stringToFloat(std::string text);
		int stringToInt(std::string text);
		AnnVect3 keyStringToVect3(std::string key);
		AnnQuaternion keyStringToQuaternion(std::string key);

		///Overload this method with
		virtual void extract() =0;
	protected:
		AnnSaveFileData* dataObject;
	};


}

#endif