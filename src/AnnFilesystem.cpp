#include "stdafx.h"
#include "AnnFilesystem.hpp"
#include "AnnLogger.hpp"

using namespace Annwvyn;
using namespace std;

std::vector<char> AnnFilesystemManager::charToEscape;
std::vector<char> AnnFilesystemManager::charToStrip;

AnnFileWriter::AnnFileWriter()
{
	AnnDebug() << "FileWriter instantiated";
}

void AnnFileWriter::write(AnnSaveFileData* data)
{
	//Create the resources needed for the write operation
	auto fsmanager(AnnGetFileSystemManager());
	string path(fsmanager->getPathForFileName(data->fileName));
	ofstream saveFile;

	//Make sure the "user save" directory as been created on the user's personal folder
	fsmanager->createSaveDirectory();

	//Open the file, abort if the file isn't openable
	saveFile.open(path);if(!saveFile.is_open())return;
		//push as plain text all key and data in a "key=data\n" format
		for(auto storedData : data->storedTextData)
		{
			saveFile << storedData.first 
				<< "=" 
				<< storedData.second 
				<< endl;
		}
	saveFile.close();
	data->changed = false;
}

AnnFileReader::AnnFileReader()
{
		AnnDebug() << "FileReader instantiated";
}

AnnSaveFileData* AnnFileReader::read(string fileName)
{
	AnnDebug() << "Reading file " << fileName << " to memory";

	//Create the resource needed to the read operation
	auto fsmanager (AnnGetFileSystemManager());
	auto fileData (fsmanager->crateSaveFileDataObject(fileName));
	if(!fileData) return nullptr;
	ifstream ifile;
	string buffer, key, value;

	//make sure the dataObject don't contain old content
	fileData->storedTextData.clear();

	//Open the file 
	ifile.open(fsmanager->getPathForFileName(fileName));

	//While we've not reach the end of the file
	while(!ifile.eof())
	{
		//Read a line
		getline(ifile, buffer);
		if(buffer.empty()) continue;

		//Create a string stream on the buffer
		std::stringstream readStream(buffer);
		//Getline permit you to specify the "endline" character. We use this to split the sting at the '=' in the file
		getline(readStream, key, '=');
		getline(readStream, value);

		//Store on the file data object the given key
		fileData->storedTextData[key]=value;
	}
	ifile.close();

	fileData->changed = false;

	return fileData;
}


AnnFilesystemManager::AnnFilesystemManager(std::string title) : AnnSubSystem("FilesystemManager")
{
	//get from the OS the user's personal directory
#ifdef WIN32
#pragma warning (disable : 4996)
	pathToUserDir = getenv("USERPROFILE");
#pragma warning (default : 4996)
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

	setSaveDirectoryName(title);
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
	AnnDebug() << "Save directory : " << saveDirectoryName;
	AnnDebug() << "Save directory location : " << getSaveDirectoryFullPath();
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
	fileName(name),
	changed(false)
{
	AnnDebug() << "SaveFileData object for file " << name << " created";
}

bool AnnSaveFileData::hasUnsavedChanges()
{
	return changed;
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
	changed = true;
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


void AnnSaveFileData::clearValue(std::string key)
{
	storedTextData.erase(key);
	changed = true;
}

void AnnSaveFileData::clearVectorValue(std::string key)
{
	clearValue(key + ".x");
	clearValue(key + ".y");
	clearValue(key + ".z");
}

void AnnSaveFileData::clearQuaternionValue(std::string key)
{
	//like vectors, quaternion have x, y, and z, compnant. they just add a 'w' one
	clearValue(key + ".w"); clearVectorValue(key);
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

float AnnSaveDataInterpretor::keyStringToFloat(std::string key)
{
	return stringToFloat(dataObject->getValue(key));
}

int AnnSaveDataInterpretor::keyStringToInt(std::string key)
{
	return stringToInt(dataObject->getValue(key));
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
