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

	///Exception relating physics setup and parent/child objects
	class DLL AnnPhysicsSetupChildError : public std::runtime_error
	{
	public:
		AnnPhysicsSetupChildError(AnnGameObject* origin);
		const char* what() const throw() override;
	private:
		AnnGameObject* objectWithProblem;
	};

	///Exception in hand controller "side" detection
	class DLL AnnInvalidControllerSide : public std::runtime_error
	{
	public:
		AnnInvalidControllerSide();
		const char* what() const throw() override;
	};

	///Exception regarding an AnnGameObject that was null when it wasnt
	class DLL AnnNullGameObjectError : public std::runtime_error
	{
	public:
		AnnNullGameObjectError();
		const char* what() const throw() override;
	};

	///Exception regarding engine utilisation. See message
	class DLL AnnInitializationError : public std::runtime_error
	{
	public:
		AnnInitializationError(int errorCode, const std::string& message);
		const char* what() const throw() override;
	};
}