// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "AnnFilesystem.hpp"
#include "AnnLogger.hpp"
#include "AnnGetter.hpp"

using namespace Annwvyn;
using namespace std;

vector<char> AnnFilesystemManager::charToEscape;
vector<char> AnnFilesystemManager::charToStrip;

AnnFileWriter::AnnFileWriter()
{
	AnnDebug() << "FileWriter instantiated";
}

void AnnFileWriter::write(shared_ptr<AnnSaveFileData> data)
{
	//Create the resources needed for the write operation
	auto fsmanager(AnnGetFileSystemManager());
	auto path(fsmanager->getPathForFileName(data->fileName));
	ofstream saveFile;

	//Make sure the "user save" directory as been created on the user's personal folder
	fsmanager->createSaveDirectory();

	//Open the file, abort if the file isn't openable
	saveFile.open(path);
	if(!saveFile.is_open()) return;
	//push as plain text all key and data in a "key=data\n" format
	for(auto storedData : data->storedTextData)
		saveFile << storedData.first
				 << "="
				 << storedData.second
				 << endl;
	data->changed = false;
}

AnnFileReader::AnnFileReader()
{
	AnnDebug() << "FileReader instantiated";
}

AnnSaveFileDataPtr AnnFileReader::read(string fileName) const
{
	AnnDebug() << "Reading file " << fileName << " to memory";

	//Create the resource needed to the read operation
	auto fsmanager(AnnGetFileSystemManager());

	//Open the file
	ifstream ifile;
	auto fullPath = fsmanager->getPathForFileName(fileName);
	ifile.open(fullPath);

	if(!ifile)
	{
		AnnDebug() << "file " << fullPath << " doesn't exist or is not openable.";
		return nullptr;
	}

	auto fileData(fsmanager->crateSaveFileDataObject(fileName));
	if(!fileData) return nullptr;
	string buffer, key, value;

	//make sure the dataObject don't contain old content
	fileData->storedTextData.clear();

	//While we've not reach the end of the file
	while(!ifile.eof())
	{
		//Read a line
		getline(ifile, buffer);
		//Don't try to extract data from empty lines on the file
		if(buffer.empty()) continue;

		//Create a string stream on the buffer
		stringstream readStream(buffer);

		//Getline permit you to specify the "end-line" character. We use this to split the sting at the '=' in the file
		getline(readStream, key, '=');
		getline(readStream, value);

		//Store on the file data object the given key
		fileData->storedTextData[key] = value;
	}

	fileData->changed = false;
	return fileData;
}

AnnFilesystemManager::AnnFilesystemManager(string title) :
 AnnSubSystem("FilesystemManager"),
 fileWriter(nullptr),
 fileReader(nullptr)
{
//get from the OS the user's personal directory
#ifdef WIN32
#pragma warning(disable : 4996) //Remove warning at usage of function "getenv"
	// ReSharper disable CppDeprecatedEntity
	pathToUserDir = getenv("USERPROFILE");
	// ReSharper restore CppDeprecatedEntity
#pragma warning(default : 4996) //TODO: V665 https://www.viva64.com/en/w/V665 Possibly, the usage of '#pragma warning(default: X)' is incorrect in this context. The '#pragma warning(push/pop)' should be used instead. Check lines: 99, 103. //TODO: V665https://www.viva64.com/en/w/v665/Possibly, the usage of '#pragma warning(default: X)' is incorrect in this context. The '#pragma warning(push/pop)' should be used instead. Check lines: 101, 105.
#endif

#ifdef __linux__
	pathToUserDir = getenv("HOME");
#endif

	AnnDebug() << "Path got from operating system : " << pathToUserDir;

	//Forbidden characters in filename
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

	//Forbidden characters in text "key=value" file
	charToStrip.push_back('=');
	charToStrip.push_back('\n');

	fileWriter = make_shared<AnnFileWriter>();
	fileReader = make_shared<AnnFileReader>();

	setSaveDirectoryName(title);
}

void AnnFilesystemManager::setSaveDirectoryName(string dirname)
{
	for(auto achar : charToEscape)
		replace(begin(dirname), end(dirname), achar, '_');
	saveDirectoryName = dirname;
#ifdef __linux__
	//Put the save directory as "hidden" in user's home directory on Linux.
	saveDirectoryName = "." + saveDirectoryName;
#endif

	AnnDebug() << "Save directory : " << saveDirectoryName;
	AnnDebug() << "Save directory location : " << getSaveDirectoryFullPath();
}

string AnnFilesystemManager::getPathForFileName(string filename) const
{
	if(!pathToUserDir.empty())
		return pathToUserDir + "/" + saveDirectoryName + "/" + filename;
	return "";
}

string AnnFilesystemManager::getSaveDirectoryFullPath() const
{
	return pathToUserDir + "/" + saveDirectoryName;
}

void AnnFilesystemManager::createDirectory(string path)
{
//TODO ISSUE clean that when upgrading to C++17
#ifdef WIN32
	//Win32 call to create a directory
	CreateDirectory(wstring{ begin(path), end(path) }.c_str(), nullptr);
#endif
#ifdef __linux__
	//POSIX call to create a directory
	auto status = mkdir(path.c_str(), S_IRWXU);
	if(status == 0 || status == EEXIST) return;
	AnnDebug(Log::Important) << "Warning: mkdir() did not return 0 or EEXIST.";
#endif
}

void AnnFilesystemManager::createSaveDirectory() const
{
	createDirectory(getSaveDirectoryFullPath());
}

void AnnFilesystemManager::releaseSaveFileDataObject(shared_ptr<AnnSaveFileData> data)
{
	cachedData.remove(data);
}

AnnSaveFileDataPtr AnnFilesystemManager::crateSaveFileDataObject(string filename)
{
	auto data = make_shared<AnnSaveFileData>(filename);
	cachedData.push_back(data);
	return data;
}

AnnSaveFileDataPtr AnnFilesystemManager::getCachedSaveFileDataObject(string filename)
{
	for(auto dataObject : cachedData)
		if(dataObject->getFilename() == filename)
			return dataObject;
	return nullptr;
}

AnnFileReaderPtr AnnFilesystemManager::getFileReader() const
{
	return fileReader;
}

AnnFileWriterPtr AnnFilesystemManager::getFileWriter() const
{
	return fileWriter;
}

AnnSaveFileData::AnnSaveFileData(string name) :
 fileName(name),
 changed(false)
{
	AnnDebug() << "AnnSaveFileData object for file " << name << " created (not on disk!)";
}

bool AnnSaveFileData::hasUnsavedChanges() const
{
	return changed;
}

string AnnSaveFileData::getFilename() const
{
	return fileName;
}

string AnnSaveFileData::getValue(string key)
{
	//if key exist:
	if(storedTextData.find(key) != end(storedTextData))
		return storedTextData[key];
	//else:
	return "";
}

void AnnSaveFileData::setValue(string key, string value)
{
	for(auto achar : AnnFilesystemManager::charToStrip)
	{
		replace(begin(key), end(key), achar, '_');
		replace(begin(value), end(value), achar, '_');
	}
	storedTextData[key] = value;
	changed				= true;
}

void AnnSaveFileData::setValue(string key, int value)
{
	stringstream sstream;
	sstream << value;
	setValue(key, sstream.str());
}

void AnnSaveFileData::setValue(string key, float value)
{
	stringstream sstream;
	sstream.precision(2 + numeric_limits<float>::max_digits10);
	sstream << fixed << value;
	setValue(key, sstream.str());
}

void AnnSaveFileData::setValue(string key, AnnVect3 vector)
{
	setValue(key + ".x", vector.x);
	setValue(key + ".y", vector.y);
	setValue(key + ".z", vector.z);
}

void AnnSaveFileData::setValue(string key, AnnQuaternion quaternion)
{
	setValue(key + ".x", quaternion.x);
	setValue(key + ".y", quaternion.y);
	setValue(key + ".z", quaternion.z);
	setValue(key + ".w", quaternion.w);
}

void AnnSaveFileData::setValue(string key, const char* value)
{
	setValue(key, string(value));
}

void AnnSaveFileData::clearValue(string key)
{
	storedTextData.erase(key);
	changed = true;
}

void AnnSaveFileData::clearVectorValue(string key)
{
	clearValue(key + ".x");
	clearValue(key + ".y");
	clearValue(key + ".z");
}

void AnnSaveFileData::clearQuaternionValue(string key)
{
	//like vectors, quaternion have x, y, and z, component. they just add a 'w' one
	clearValue(key + ".w");
	clearVectorValue(key);
}

AnnSaveDataInterpretor::AnnSaveDataInterpretor(shared_ptr<AnnSaveFileData> data) :
 dataObject(data)
{
}

float AnnSaveDataInterpretor::stringToFloat(string text) const
{
	return stof(text);
}

int AnnSaveDataInterpretor::stringToInt(string text) const
{
	return stoi(text);
}

float AnnSaveDataInterpretor::keyStringToFloat(string key) const
{
	return stringToFloat(dataObject->getValue(key));
}

int AnnSaveDataInterpretor::keyStringToInt(string key) const
{
	return stringToInt(dataObject->getValue(key));
}

AnnVect3 AnnSaveDataInterpretor::keyStringToVect3(string key) const
{
	//Get text data from the dataObject return an invalid vector if the keyvalue wanted is not found
	string x, y, z;
	if((x = dataObject->getValue(key + ".x")).empty()) return AnnVect3(false);
	if((y = dataObject->getValue(key + ".y")).empty()) return AnnVect3(false);
	if((z = dataObject->getValue(key + ".z")).empty()) return AnnVect3(false);

	//Convert the text data to floats and send them to the AnnVect3 constructor and return the object
	return AnnVect3(
		stringToFloat(x),
		stringToFloat(y),
		stringToFloat(z));
}

AnnQuaternion AnnSaveDataInterpretor::keyStringToQuaternion(string key) const
{
	//Get text data from the dataObject return an invalid quaternion if the keyvalue wanted is not found
	string x, y, z, w;
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
