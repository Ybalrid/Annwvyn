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

using namespace std;

namespace Annwvyn
{
	class AnnSaveFileData;
	class DLL AnnFileWriter
	{
	public:
		AnnFileWriter();
		void write(AnnSaveFileData* dataToWrite);
	};

	class DLL AnnFileReader
	{
	public:
		AnnFileReader();
		AnnSaveFileData* read(string path);
	};

	class AnnSaveFileData;
	class DLL AnnFilesystemManager
	{
	public:
		AnnFilesystemManager();
		~AnnFilesystemManager();
		void setSaveDirectoryName(string name);
		string getPathForFileName(string fileName);
		string getSaveDirectoryFullPath();
		static void createDirectory(string path);
		void createSaveDirectory();

		AnnSaveFileData* crateSaveFileDataObject(string filename);
		void destroySaveFileDataObject(AnnSaveFileData* data);
		
		AnnFileReader* getFileReader();
		AnnFileWriter* getFileWriter();

	private:
		string saveDirectoryName;
		string pathToUserDir;
		std::vector<char> charToEscape, charToStrip;
		AnnFileWriter* fileWriter;
		AnnFileReader* fileReader;
		std::list<AnnSaveFileData*> cachedData;
	};
	
	class DLL AnnSaveFileData
	{
	public:
		AnnSaveFileData(string name);
	private:
		friend class AnnFileWriter;
		string fileName;
	public:
		std::map<string, string> storedTextData; 
	};


}

#endif