// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "AnnException.hpp"
#include "AnnLogger.hpp"

using namespace std;
using namespace Annwvyn;

ostringstream AnnPhysicsSetupParentError::outputFormater;

AnnPhysicsSetupParentError::AnnPhysicsSetupParentError(AnnGameObject* origin) :
 runtime_error{ "Cannot setup physics for object " },
 objectWithProblem{ origin }
{
	AnnDebug(Log::Important) << AnnPhysicsSetupParentError::what();
}

const char* AnnPhysicsSetupParentError::what() const throw()
{
	outputFormater.str("");

	outputFormater << runtime_error::what() << " : " << objectWithProblem->getName();

	if(objectWithProblem->hasParent())
	{
		outputFormater << " child of : " << objectWithProblem->getParent()->getName() << '\n';
		outputFormater << "The parent with a physics body is : " << getParentWithBody()->getName();
	}

	outputFormater << "\nSetting up physics of a child object with parent"
					  " break the parent/transform derivation because the Physics engine"
					  " move object in world position.";

	outputFormater << "\n Consider not parenting theses two object if you want them to be independent physics objects.";

	return outputFormater.str().c_str();
}

AnnGameObject* AnnPhysicsSetupParentError::getObject() const { return objectWithProblem; }

AnnGameObject* AnnPhysicsSetupParentError::getParentWithBody() const { return recurToBody(objectWithProblem); }

AnnGameObject* AnnPhysicsSetupParentError::recurToBody(AnnGameObject* start)
{
	if(start->getParent()->getBody()) return start->getParent().get();
	return recurToBody(start->getParent().get());
}

AnnPhysicsSetupChildError::AnnPhysicsSetupChildError(AnnGameObject* origin) :
 runtime_error{ "Cannot setup physics for Object " },
 objectWithProblem{ origin }
{
	AnnDebug(Log::Important) << AnnPhysicsSetupChildError::what();
}

const char* AnnPhysicsSetupChildError::what() const throw()
{
	ostringstream out;
	out << runtime_error::what();
	out << objectWithProblem->getName() << '\n';
	out << "a child has a rigid body. Creating a body will mess up the system. consider not using parenting for theses objects";
	return out.str().c_str();
}

AnnInvalidControllerSide::AnnInvalidControllerSide() :
 runtime_error("Invalid hand controller side")
{
	AnnDebug(Log::Important) << AnnInvalidControllerSide::what();
}

const char* AnnInvalidControllerSide::what() const throw()
{
	ostringstream out;
	out << runtime_error::what();
	out << "AnnHandController object initialized without knowing the side. This should not happen.";
	return out.str().c_str();
}

AnnNullGameObjectError::AnnNullGameObjectError() :
 runtime_error("Error : Trying to do an operation on a null GameObject")
{
	AnnDebug(Log::Important) << AnnNullGameObjectError::what();
}

const char* AnnNullGameObjectError::what() const throw()
{
	ostringstream out;
	out << runtime_error::what();
	out << "Check what you're doing with your pointers...";
	return out.str().c_str();
}

AnnInitializationError::AnnInitializationError(int errorCode, const string& message) :
 runtime_error("Error : " + to_string(errorCode) + " " + message)
{
	AnnDebug(Log::Important) << AnnInitializationError::what();
}

const char* AnnInitializationError::what() const throw()
{
	ostringstream out;
	out << runtime_error::what();
	out << " AnnInitializationError thrown";
	return out.str().c_str();
}

AnnInvalidPhysicalShapeError::AnnInvalidPhysicalShapeError(const std::string& objName) :
 std::runtime_error("Error : Cannot create a physics shape from arguments"),
 objectName{ objName }
{
	AnnDebug(Log::Important) << AnnInvalidPhysicalShapeError::what();
}

const char* AnnInvalidPhysicalShapeError::what() const throw()
{
	ostringstream out;
	out << runtime_error::what();
	out << " AnnInvalidPhysicalShapeError thrown on object ";
	out << objectName;
	return out.str().c_str();
}

AnnLevelLoadingError::AnnLevelLoadingError(const std::string& level, const std::string& obj) :
 std::runtime_error("Error : Cannot load level object"),
 levelName{ level },
 objectName{ obj }
{
	AnnDebug(Log::Important) << AnnLevelLoadingError::what();
}

const char* AnnLevelLoadingError::what() const throw()
{
	ostringstream out;
	out << runtime_error::what();
	out << " AnnLevelLoadingError ";
	out << levelName;
	out << " Additional object informations : " << objectName;
	return out.str().c_str();
}
