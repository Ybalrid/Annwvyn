#include "stdafx.h"
#include "AnnScriptManager.hpp"
#include "AnnLogger.hpp"

using namespace Annwvyn;

AnnScriptManager::AnnScriptManager() : AnnSubSystem("ScriptManager"),
chai(chaiscript::Std_Lib::library())
{
	AnnDebug() << "Initialized ChaiScript Std_Lib";
	chai.add(chaiscript::fun([](const std::string& s) {AnnDebug() << logFromScript << s; }), "AnnDebugLog");

	AnnDebug() << "Using ChaiScript version : " << chai.version();

	chai.eval(R"(
	AnnDebugLog("Log function added to ChaiScript");
	)");

	registerApi();
}

bool Annwvyn::AnnScriptManager::evalFile(const std::string & file)
{
	try
	{
		chai.eval_file(file);
	}
	catch (const chaiscript::exception::file_not_found_error& fnfe)
	{
		AnnDebug() << fileErrorPrefix << fnfe.what();
		return false;
	}
	catch (const chaiscript::exception::eval_error& ee)
	{
		AnnDebug() << fileErrorPrefix << ee.pretty_print();
		return false;
	}
	return true;
}

AnnScriptManager::AnnScriptID AnnScriptManager::ID{ 0 };

AnnBehaviourScript Annwvyn::AnnScriptManager::getBehaviourScript(const std::string & scriptName)
{
	std::string file{ scriptName + scriptExtension };

	try
	{
		//Evaluate the file containing the script class if unknown to ChaiScript yet
		chai.use(file);

		ID++;
		std::string ChaiCode{ scriptTemplate };
		ChaiCode.replace(ChaiCode.find(std::string(scriptNameMarker)), nameMarkerLen, scriptName);
		ChaiCode.replace(ChaiCode.find(std::string(scriptObjectID)), scriptIDMarkerLen, std::to_string(ID));

		//This will create a global instance of the class in the ChaiScript global space
		chai.eval(ChaiCode);

		//Now we need to get some hook to call the update on the file
		return AnnBehaviourScript(
			scriptName,
			chai.eval<std::function<void(chaiscript::Boxed_Value&)>>("update"),
			chai.eval(std::string(scriptInstanceMarker) + std::to_string(ID))
		);
	}
	catch (const chaiscript::exception::file_not_found_error& fnfe)
	{
		AnnDebug() << fileErrorPrefix << fnfe.what();
	}
	catch (const chaiscript::exception::eval_error& ee)
	{
		AnnDebug() << ee.pretty_print();
	}

	return AnnBehaviourScript();
}

void Annwvyn::AnnScriptManager::registerApi()
{
	using namespace Ogre;
	using namespace chaiscript;
	//TODO Add to chai all the useful types (angles, vectors, quaternions...)

	//Base Class of AnnVect3
	chai.add(chaiscript::user_type<Ogre::Vector3>(), "AnnVect3");
	chai.add(chaiscript::constructor<Ogre::Vector3()>(), "AnnVect3");
	chai.add(chaiscript::constructor<Ogre::Vector3(const float, const float, const float)>(), "AnnVect3");
	chai.add(chaiscript::constructor<Ogre::Vector3(const float[3])>(), "AnnVect3");
	chai.add(chaiscript::constructor<Ogre::Vector3(const Ogre::Vector3&)>(), "AnnVect");
	//Public attributes. Thank the simulation authors, this is easy
	chai.add(chaiscript::fun(&Ogre::Vector3::x), "x");
	chai.add(chaiscript::fun(&Ogre::Vector3::y), "y");
	chai.add(chaiscript::fun(&Ogre::Vector3::z), "z");

	//the lambda here is a hack because I can't find a way to give chai operator= cleanly
	chai.add(chaiscript::fun([](Ogre::Vector3& u, const Ogre::Vector3& v) {u = v; }), "=");

	//This works here, that's weird than operator- and operator= don't want to
	chai.add(chaiscript::fun<Ogre::Vector3>(&Ogre::Vector3::operator+), "+");
	//There's multiple overloads of Ogre::Vector3::operator-. Easier to use a lambda here too
	chai.add(chaiscript::fun([](const Ogre::Vector3& v) {return -v; }), "-");
	chai.add(chaiscript::fun([](const Ogre::Vector3& v, Ogre::Vector3 w) {return v - w; }), "-");

	//note to self, at this point, using lambdas seems easier than trying to bind the operator overloads to chaiscript operators
	chai.add(chaiscript::fun([](const Ogre::Vector3& vector, Ogre::Real scalar) {return scalar*vector; }), "*");

	//OMG, it just works now. I <3 Lambdas
	chai.add(chaiscript::fun([](Ogre::Vector3& vector, Ogre::Real scalar) {vector *= scalar; }), "*=");

	chai.add(user_type<Quaternion>(), "AnnQuat");
	chai.add(constructor<Quaternion()>(), "AnnQuat");
	chai.add(constructor<Quaternion(const float, const float, const float, const float)>(), "AnnQuat");

	//TODO Add to chai a way to access useful Annwvyn components
}

Annwvyn::AnnBehaviourScript::AnnBehaviourScript() :
	valid(false)

{
}

Annwvyn::AnnBehaviourScript::AnnBehaviourScript(std::string scriptName, std::function<void(chaiscript::Boxed_Value&)> updateHook, chaiscript::Boxed_Value chaisriptInstance) :
	valid(true),
	name(scriptName),
	callUpdateOnScriptInstance(updateHook),
	ScriptObjectInstance(chaisriptInstance)
{
}

//Annwvyn::AnnBehaviourScript::AnnBehaviourScript(bool invalid) : valid(invalid),
//ScriptObjectInstance(chaiscript::Boxed_Value(false))
//{
//}

void Annwvyn::AnnBehaviourScript::update()
{
	callUpdateOnScriptInstance(ScriptObjectInstance);
}

bool Annwvyn::AnnBehaviourScript::isValid()
{
	return valid;
}