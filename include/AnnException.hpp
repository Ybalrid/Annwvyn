#pragma once

#include "systemMacro.h"
#include <sstream>
#include <exception>
#include <stdexcept>
#include <AnnGameObject.hpp>
#include <AnnErrorCode.hpp>

namespace Annwvyn
{
	///Exception to throw when a physics enabled object will cause object coordinates reset
	class AnnDllExport AnnPhysicsSetupParentError : public std::runtime_error
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
	class AnnDllExport AnnPhysicsSetupChildError : public std::runtime_error
	{
	public:
		AnnPhysicsSetupChildError(AnnGameObject* origin);
		const char* what() const throw() override;

	private:
		AnnGameObject* objectWithProblem;
	};

	///Exception in hand controller "side" detection
	class AnnDllExport AnnInvalidControllerSide : public std::runtime_error
	{
	public:
		AnnInvalidControllerSide();
		const char* what() const throw() override;
	};

	///Exception regarding an AnnGameObject that was null when it wasnt
	class AnnDllExport AnnNullGameObjectError : public std::runtime_error
	{
	public:
		AnnNullGameObjectError();
		const char* what() const throw() override;
	};

	///Exception regarding engine utilisation. See message
	class AnnDllExport AnnInitializationError : public std::runtime_error
	{
	public:
		AnnInitializationError(int errorCode, const std::string& message);
		const char* what() const throw() override;
	};

	///Exception regarding collision shape creation
	class AnnDllExport AnnInvalidPhysicalShapeError : public std::runtime_error
	{
	public:
		AnnInvalidPhysicalShapeError(const std::string& objectName);
		const char* what() const throw() override;

	private:
		const std::string objectName;
	};

	class AnnDllExport AnnLevelLoadingError : public std::runtime_error
	{
	public:
		AnnLevelLoadingError(const std::string& levelName, const std::string& objectName);
		const char* what() const throw() override;

	private:
		const std::string levelName;
		const std::string objectName;
	};
}
