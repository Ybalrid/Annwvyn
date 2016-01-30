#include "stdafx.h"
#include "AnnFilesystem.hpp"
#include "AnnLogger.hpp"

using namespace Annwvyn;
using namespace std;

std::vector<char> AnnFilesystemManager::charToEscape;
std::vector<char> AnnFilesystemManager::charToStrip;

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
	for(auto achar : AnnFilesystemManager::charToStrip)
	{
		replace(key.begin(), key.end(), achar, '_');
		replace(value.begin(), value.end(), achar, '_');
	}
	storedTextData[key] = value;
}

void AnnSaveFileData::setValue(std::string key, int value)
{
	std::stringstream sstream;
	sstream << value;
	setValue(key, sstream.str());
}

void AnnSaveFileData::setValue(std::string key, float value)
{
	std::stringstream sstream;
	sstream << value;
	setValue(key, sstream.str());
}

void AnnSaveFileData::setValue(std::string key, AnnVect3 vector)
{
	setValue(key + ".x", vector.x);
	setValue(key + ".y", vector.y);
	setValue(key + ".z", vector.z);
}

void AnnSaveFileData::setValue(std::string key, AnnQuaternion quaternion)
{
	setValue(key + ".x", quaternion.x);
	setValue(key + ".y", quaternion.y);
	setValue(key + ".z", quaternion.z);
	setValue(key + ".w", quaternion.w);
}

void AnnSaveFileData::setValue(std::string key, const char* value)
{
	setValue(key, std::string(value));
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

AnnVect3 AnnSaveDataInterpretor::keyStringToVect3(std::string key)
{
	//Get text data from teh dataObject return an invalid vector if the keyvalue wanted is not found
	std::string x,y,z;
	if((x = dataObject->getValue(key + ".x")).empty()) return AnnVect3(false);
	if((y = dataObject->getValue(key + ".y")).empty()) return AnnVect3(false);
	if((z = dataObject->getValue(key + ".z")).empty()) return AnnVect3(false);
	//Convert the text data to floats and send them to the AnnVect3 constructor and return the object 
	return AnnVect3(
		stringToFloat(x),
		stringToFloat(y),
		stringToFloat(z));
}

AnnQuaternion AnnSaveDataInterpretor::keyStringToQuaternion(std::string key)
{
	//Get text data from teh dataObject return an invalid quaternion if the keyvalue wanted is not found
	std::string x, y, z, w;
	if((x = dataObject->getValue(key + ".x")).empty()) return AnnQuaternion(false);
	if((y = dataObject->getValue(key + ".y")).empty()) return AnnQuaternion(false);
	if((z = dataObject->getValue(key + ".z")).empty()) return AnnQuaternion(false);
	if((w = dataObject->getValue(key + ".w")).empty()) return AnnQuaternion(false);
	//Convert the text data to floats and send them to the AnnQuaternion constructor and return the object 
	return AnnQuaternion(
		stringToFloat(w),
		stringToFloat(x),
		stringToFloat(y),
		stringToFloat(z));
}
