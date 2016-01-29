#include "stdafx.h"
#include "AnnFilesystem.hpp"
#include "AnnLogger.hpp"

using namespace Annwvyn;
using namespace std;

AnnFileWriter::AnnFileWriter()
{

}

void AnnFileWriter::write(AnnSaveFileData* data)
{
	auto fsmanager(AnnEngine::Instance()->getFileSystemManager());
	string path(fsmanager->getPathForFileName(data->fileName));
	ofstream saveFile;
	fsmanager->createSaveDirectory();
	saveFile.open(path);
	if(!saveFile.is_open())return;
	for(auto storedData : data->storedTextData)
	{
		saveFile << storedData.first 
			<< "=" 
			<< storedData.second 
			<< endl;
	}
	saveFile.close();
}

AnnFileReader::AnnFileReader()
{

}

AnnSaveFileData* AnnFileReader::read(string fileName)
{
	auto fsmanager (AnnEngine::Instance()->getFileSystemManager());
	auto fileData (fsmanager->crateSaveFileDataObject(fileName));
	ifstream ifile;
	string buffer, key, value;
	ifile.open(fsmanager->getPathForFileName(fileName));
	while(!ifile.eof())
	{
		getline(ifile, buffer);
		if(buffer.empty()) continue;
		std::stringstream readStream(buffer);
		getline(readStream, key, '=');
		getline(readStream, value);
		fileData->storedTextData[key]=value;
	}
	ifile.close();
	return fileData;
}


AnnFilesystemManager::AnnFilesystemManager() 
{
	AnnDebug() << "Filesystem manager created";
	//get from the OS the user's personal directory
#ifdef WIN32
	pathToUserDir = getenv("USERPROFILE");
#endif

	AnnDebug() << "Path got from operating system : " << pathToUserDir;

	//Forbiden characters in filename
	charToEscape.push_back(' ');
	charToEscape.push_back('&');
	charToEscape.push_back('<');
	charToEscape.push_back('>');
	charToEscape.push_back(':');
	charToEscape.push_back('|');
	charToEscape.push_back('?');
	charToEscape.push_back('*');
	charToEscape.push_back('"');
	charToEscape.push_back(':');
	charToEscape.push_back('%');

	//Forbiden charcters in text "key=value" file
	charToStrip.push_back('=');
	charToStrip.push_back('\n');

	fileWriter = new AnnFileWriter;
	fileReader = new AnnFileReader;
}

AnnFilesystemManager::~AnnFilesystemManager()
{
	delete fileWriter;
	delete fileReader;
	for(auto dataObject : cachedData)
		delete dataObject;
}

void AnnFilesystemManager::setSaveDirectoryName(string dirname)
{
	for(auto achar : charToEscape)
		replace(dirname.begin(), dirname.end(), achar, '_');
	saveDirectoryName = dirname;
	AnnDebug() << "Path example : " << getPathForFileName("example");
}

string AnnFilesystemManager::getPathForFileName(string filename)
{
	if(!pathToUserDir.empty())
	return pathToUserDir + "/" + saveDirectoryName + "/" + filename;
	return "";
}

string AnnFilesystemManager::getSaveDirectoryFullPath()
{
	return pathToUserDir + "/" + saveDirectoryName;
}

void AnnFilesystemManager::createDirectory(string path)
{
#ifdef WIN32
	CreateDirectory(wstring(path.begin(), path.end()).c_str(), NULL);
#endif
}

void AnnFilesystemManager::createSaveDirectory()
{
	createDirectory(getSaveDirectoryFullPath());
}

void AnnFilesystemManager::destroySaveFileDataObject(AnnSaveFileData* data)
{
	cachedData.remove(data);
	delete data;
}


AnnSaveFileData* AnnFilesystemManager::crateSaveFileDataObject(string filename)
{
	AnnSaveFileData* data = new AnnSaveFileData(filename);
	cachedData.push_back(data);
	return data;
}

AnnSaveFileData* AnnFilesystemManager::getCachedSaveFileDataObject(string filename)
{
	for(auto dataObject : cachedData)
		if(dataObject->getFilename() == filename)
			return dataObject;
	return nullptr;
}

AnnFileReader* AnnFilesystemManager::getFileReader()
{
	return fileReader;
}

AnnFileWriter* AnnFilesystemManager::getFileWriter()
{
	return fileWriter;
}

AnnSaveFileData::AnnSaveFileData(string name) :
	fileName(name)
{
}

std::string AnnSaveFileData::getFilename()
{
	return fileName;
}

std::string AnnSaveFileData::getValue(std::string key)
{
	//if key exist:
	if(storedTextData.find(key) != storedTextData.end())
		return storedTextData[key];
	//else:
	return "";
}

void AnnSaveFileData::setValue(std::string key, std::string value)
{
	storedTextData[key] = value;
}

AnnSaveDataInterpretor::AnnSaveDataInterpretor(AnnSaveFileData* data) :
	dataObject(data)
{
}

float AnnSaveDataInterpretor::stringToFloat(std::string text)
{
	return std::stof(text);
}

int AnnSaveDataInterpretor::stringToInt(std::string text)
{
	return std::stoi(text);
}




