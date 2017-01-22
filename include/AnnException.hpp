#pragma once

#include "systemMacro.h"
#include <sstream>
#include <exception>
#include <stdexcept>
#include <AnnGameObject.hpp>
namespace Annwvyn
{
	///Exception to throw when a physics enabled object will cause object coordinates reset
	class DLL AnnPhysicsSetupParentError : public std::runtime_error
	{
	public:
		AnnPhysicsSetupParentError(AnnGameObject* origin);

		///Pretty text to explain what's wrong
		const char* what() const throw() override;

		AnnGameObject* getObject() const;

		AnnGameObject* getParentWithBody() const;

	private:
		static AnnGameObject* recurToBody(AnnGameObject* start);
		AnnGameObject* objectWithProblem;
		static std::ostringstream outputFormater;
	};

	class DLL AnnPhysicsSetupChildError : public std::runtime_error
	{
	public:
		AnnPhysicsSetupChildError(AnnGameObject* origin);
		const char* what() const throw() override;
	private:
		AnnGameObject* objectWithProblem;
	};
}