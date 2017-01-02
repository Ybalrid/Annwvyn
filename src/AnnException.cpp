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

AnnGameObject* AnnPhysicsSetupParentError::recurToBody(AnnGameObject* start) const
{
	if (start->getParent()->getBody()) return start->getParent().get();
	return recurToBody(start->getParent().get());
}