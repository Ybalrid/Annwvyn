#include "stdafx.h"
#include "AnnException.hpp"
#include "AnnLogger.hpp"

using namespace std;
using namespace Annwvyn;

ostringstream AnnPhysicsSetupParentError::outputFormater;

AnnPhysicsSetupParentError::AnnPhysicsSetupParentError(AnnGameObject* origin) :
	runtime_error{ "Cannot setup physics for object " },
	objectWithProblem{ origin }
{
	AnnDebug() << AnnPhysicsSetupParentError::what();
}

const char* AnnPhysicsSetupParentError::what() const throw()
{
	outputFormater.str("");

	outputFormater << runtime_error::what() << " : " << objectWithProblem->getName();

	if (objectWithProblem->hasParent())
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
	if (start->getParent()->getBody()) return start->getParent().get();
	return recurToBody(start->getParent().get());
}

AnnPhysicsSetupChildError::AnnPhysicsSetupChildError(AnnGameObject* origin) :
	runtime_error{ "Cannot setup physics for Object " },
	objectWithProblem{ origin }
{
	AnnDebug() << AnnPhysicsSetupChildError::what();
}

const char* AnnPhysicsSetupChildError::what() const throw()
{
	ostringstream out;
	out << runtime_error::what();
	out << objectWithProblem->getName() << '\n';
	out << "a child has a rigid body. Creating a body will mess up the system. consider not using parenting for theses objects";

	return out.str().c_str();
}

AnnInvalidControllerSide::AnnInvalidControllerSide() : std::runtime_error("Invalid hand controller side")
{
	AnnDebug() << AnnInvalidControllerSide::what();
}

const char* AnnInvalidControllerSide::what() const throw()
{
	ostringstream out;
	out << runtime_error::what();
	out << "AnnHandController object initialized without knowing the side. This should not happen.";
	return out.str().c_str();
}