#include "stdafx.h"
#include "AnnScriptManager.hpp"
#include "AnnLogger.hpp"
#include "AnnGameObject.hpp"
#include "AnnGameObjectManager.hpp"
#include "AnnEngine.hpp"

using namespace Annwvyn;

AnnScriptManager::AnnScriptManager() : AnnSubSystem("ScriptManager"),
//Initialize with compiled-in Std_Lib
chai(chaiscript::Std_Lib::library())
{
	AnnDebug() << "Initialized ChaiScript Std_Lib";
	registerApi();

	AnnDebug() << "Using ChaiScript version : " << chai.version();
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

std::shared_ptr<AnnBehaviorScript> Annwvyn::AnnScriptManager::getBehaviorScript(const std::string & scriptName, AnnGameObject* owner)
{
	std::string file{ scriptName + scriptExtension };

	try
	{
		//Evaluate the file containing the script class if unknown to ChaiScript yet
		chai.use(file);

		//Increment ID
		ID++;

		std::string ownerTag{ "" };

		//Not giving an owner to a script that wants an owner, or calling an owner to a script that
		if (owner)
		{
			ownerTag = owner->getName();
		}

		std::string ChaiCode{ scriptTemplate };
		ChaiCode.replace(ChaiCode.find(std::string(scriptNameMarker)), nameMarkerLen, scriptName);
		ChaiCode.replace(ChaiCode.find(std::string(scriptNameMarker)), nameMarkerLen, scriptName);
		ChaiCode.replace(ChaiCode.find(std::string(scriptObjectID)), scriptIDMarkerLen, std::to_string(ID));
		ChaiCode.replace(ChaiCode.find(std::string(scriptObjectID)), scriptIDMarkerLen, std::to_string(ID));
		ChaiCode.replace(ChaiCode.find(std::string(scriptObjectID)), scriptIDMarkerLen, std::to_string(ID));

		//This will create a global instance of the class in the ChaiScript global space
		chai.eval(ChaiCode);
		auto creatorFunction = chai.eval<std::function<chaiscript::Boxed_Value(std::string)>>("create" + scriptName + std::to_string(ID));

		//Now we need to get some hook to call the update on the file
		return std::make_shared<AnnBehaviorScript>(
			scriptName,
			chai.eval<std::function<void(chaiscript::Boxed_Value&)>>("update"),
			//chai.eval(std::string(scriptInstanceMarker) + std::to_string(ID))
			creatorFunction(ownerTag)
			);
	}

	// FIXME I genuinely don't know if we should crash the game or just display an error about a missing or missformed script
	catch (const chaiscript::exception::file_not_found_error& fnfe)
	{
		AnnDebug() << fileErrorPrefix << fnfe.what();
	}
	catch (const chaiscript::exception::eval_error& ee)
	{
		AnnDebug() << ee.pretty_print();
	}

	//The user should test if this script is "valid" or not. And should not do it in a looop, obviously
	return std::make_shared<AnnBehaviorScript>();
}

void Annwvyn::AnnScriptManager::registerApi()
{
	using namespace Ogre;
	using namespace chaiscript;
	using namespace std;

	//TODO Add to chai all the useful types (angles, vectors, quaternions...)

	// 3D vector
	chai.add(user_type<Vector3>(), "AnnVect3");
	chai.add(constructor<Vector3()>(), "AnnVect3");
	chai.add(constructor<Vector3(const float, const float, const float)>(), "AnnVect3");
	chai.add(constructor<Vector3(const float[3])>(), "AnnVect3");
	chai.add(constructor<Vector3(const Vector3&)>(), "AnnVect3");
	// (x,y,z)
	chai.add(fun(&Vector3::x), "x");
	chai.add(fun(&Vector3::y), "y");
	chai.add(fun(&Vector3::z), "z");
	// arithmetic operators
	chai.add(fun([](Vector3& u, const Vector3& v) {u = v; }), "=");
	chai.add(fun([](Vector3& u, const Real s) {u = s; }), "=");
	chai.add(fun<Vector3>(&Vector3::operator+), "+");
	chai.add(fun([](const Vector3& v) {return -v; }), "-");
	chai.add(fun([](const Vector3& v, Vector3 w) {return v - w; }), "-");
	chai.add(fun([](const Vector3& v, const Real w) {return v - w; }), "-");
	chai.add(fun([](const Real& v, const Vector3& w) {return v - w; }), "-");
	chai.add(fun([](const Vector3& vector, Real scalar) {return scalar*vector; }), "*");
	chai.add(fun([](const Vector3& v1, const Vector3& v2) {return v1*v2; }), "*");
	chai.add(fun([](const Vector3& vector, Real scalar) {return scalar / vector; }), "/");
	chai.add(fun([](const Vector3& v1, const Vector3& v2) {return v1 / v2; }), "/");
	chai.add(fun([](Vector3& u, Vector3 v) {u *= v; }), "*=");
	chai.add(fun([](Vector3& u, Vector3 v) {u /= v; }), "/=");
	chai.add(fun([](Vector3& u, Vector3 v) {u += v; }), "+=");
	chai.add(fun([](Vector3& u, Vector3 v) {u -= v; }), "-=");
	chai.add(fun([](Vector3& vector, Real scalar) {vector *= scalar; }), "*=");
	chai.add(fun([](Vector3& vector, Real scalar) {vector /= scalar; }), "/=");
	chai.add(fun([](Vector3& vector, Real scalar) {vector += scalar; }), "+=");
	chai.add(fun([](Vector3& vector, Real scalar) {vector -= scalar; }), "-=");
	chai.add(fun([](const Vector3& v1, const Vector3& v2) { return v1 == v2; }), "==");
	chai.add(fun([](const Vector3& v1, const Vector3& v2) { return v1 != v2; }), "!=");
	chai.add(fun([](const Vector3& v1, const Vector3& v2) { return v1 < v2; }), "<");
	chai.add(fun([](const Vector3& v1, const Vector3& v2) { return v1 > v2; }), ">");
	chai.add(fun([](const Vector3& v, const size_t i) {return v[i]; }), "[]");
	chai.add(fun([](Vector3& u, Vector3 v) {u.swap(v); }), "swap");
	chai.add(fun([](const Vector3& v) {return v.length(); }), "length");
	chai.add(fun([](const Vector3& v) {return v.normalisedCopy(); }), "normalisedCopy");
	chai.add(fun([](const Vector3& v) {return v.squaredLength(); }), "squaredLength");
	chai.add(fun([](const Vector3& v) {return v.perpendicular(); }), "perpendicular");
	chai.add(fun([](const Vector3& v) {return v.primaryAxis(); }), "primaryAxis");
	chai.add(fun([](const Vector3& v) {return v.isZeroLength(); }), "isZeroLength");
	chai.add(fun([](const Vector3& v) {return v.isNaN(); }), "isNaN");
	chai.add(fun([](Vector3& v) {return v.normalise(); }), "normalise");
	chai.add(var(&Vector3::UNIT_X), "AnnVect3_UNIT_X");
	chai.add(var(&Vector3::UNIT_Y), "AnnVect3_UNIT_Y");
	chai.add(var(&Vector3::UNIT_Z), "AnnVect3_UNIT_Z");
	chai.add(var(&Vector3::NEGATIVE_UNIT_X), "AnnVect3_NEGATIVE_UNIT_X");
	chai.add(var(&Vector3::NEGATIVE_UNIT_Y), "AnnVect3_NEGATIVE_UNIT_Y");
	chai.add(var(&Vector3::NEGATIVE_UNIT_Z), "AnnVect3_NEGATIVE_UNIT_Z");
	chai.add(var(&Vector3::UNIT_SCALE), "AnnVect3_UNIT_SCALE");

	//Angles
	chai.add(user_type<Radian>(), "AnnRadian");
	chai.add(user_type<Degree>(), "AnnDegree");
	chai.add(constructor<Radian(Real)>(), "AnnRadian");
	chai.add(constructor<Degree(Real)>(), "AnnDegree");
	chai.add(constructor<Radian(const Degree&)>(), "AnnRadian");
	chai.add(constructor<Degree(const Radian&)>(), "AnnDegree");

	chai.add(fun([](const Degree& d) {return +d; }), "+");
	chai.add(fun([](const Degree& d1, const Degree& d2) {return d1 + d2; }), "+");
	chai.add(fun([](const Degree& d1, const Radian& d2) {return d1 + d2; }), "+");
	chai.add(fun([](Degree& d1, const Degree& d2) {d1 += d2; }), "+=");
	chai.add(fun([](const Degree& d) {return -d; }), "-");
	chai.add(fun([](const Degree& d1, const Degree& d2) { return d1 - d2; }), "-");
	chai.add(fun([](const Degree& d1, const Radian& d2) { return d1 - d2; }), "-");
	chai.add(fun([](Degree& d1, const Degree& d2) {d1 -= d2; }), "-=");
	chai.add(fun([](Degree& d1, const Radian& d2) {d1 -= d2; }), "-=");
	chai.add(fun([](const Degree& d, Real f) {return d * f; }), "*");
	chai.add(fun([](Degree& d, Real f) {d *= f; }), "*=");
	chai.add(fun([](const Degree& d, Real f) {return d / f; }), "/");
	chai.add(fun([](Degree& d, Real f) {d /= f; }), "/=");

	chai.add(fun([](const Degree& d1, const Degree& d2) { return d1 < d2; }), "<");
	chai.add(fun([](const Degree& d1, const Degree& d2) { return d1 > d2; }), ">");
	chai.add(fun([](const Degree& d1, const Degree& d2) { return d1 <= d2; }), "<=");
	chai.add(fun([](const Degree& d1, const Degree& d2) { return d1 >= d2; }), ">=");
	chai.add(fun([](const Degree& d1, const Degree& d2) { return d1 == d2; }), "==");
	chai.add(fun([](const Degree& d1, const Degree& d2) { return d1 != d2; }), "!=");

	chai.add(fun([](const Radian& d) {return +d; }), "+");
	chai.add(fun([](const Radian& d1, const Radian& d2) {return d1 + d2; }), "+");
	chai.add(fun([](const Radian& d1, const Degree& d2) {return d1 + d2; }), "+");
	chai.add(fun([](Radian& d1, const Radian& d2) {d1 += d2; }), "+=");
	chai.add(fun([](const Radian& d) {return -d; }), "-");
	chai.add(fun([](const Radian& d1, const Radian& d2) { return d1 - d2; }), "-");
	chai.add(fun([](const Radian& d1, const Degree& d2) { return d1 - d2; }), "-");
	chai.add(fun([](Radian& d1, const Radian& d2) {d1 -= d2; }), "-=");
	chai.add(fun([](Radian& d1, const Degree& d2) {d1 -= d2; }), "-=");
	chai.add(fun([](const Radian& d, Real f) {return d * f; }), "*");
	chai.add(fun([](Radian& d, Real f) {d *= f; }), "*=");
	chai.add(fun([](const Radian& d, Real f) {return d / f; }), "/");
	chai.add(fun([](Radian& d, Real f) {d /= f; }), "/=");

	chai.add(fun([](const Radian& d1, const Radian& d2) {return d1 < d2; }), "<");
	chai.add(fun([](const Radian& d1, const Radian& d2) {return d1 > d2; }), ">");
	chai.add(fun([](const Radian& d1, const Radian& d2) {return d1 <= d2; }), "<=");
	chai.add(fun([](const Radian& d1, const Radian& d2) {return d1 >= d2; }), ">=");
	chai.add(fun([](const Radian& d1, const Radian& d2) {return d1 == d2; }), "==");
	chai.add(fun([](const Radian& d1, const Radian& d2) {return d1 != d2; }), "!=");

	//Quaternions
	chai.add(user_type<Quaternion>(), "AnnQuaternion");
	chai.add(constructor<Quaternion()>(), "AnnQuaternion");
	chai.add(constructor<Quaternion(const float, const float, const float, const float)>(), "AnnQuaternion");
	chai.add(constructor<Quaternion(Radian, Vector3)>(), "AnnQuaternion");
	chai.add(constructor<Quaternion(Vector3, Vector3, Vector3)>(), "AnnQuaternion");
	chai.add(fun(&Quaternion::x), "x");
	chai.add(fun(&Quaternion::y), "y");
	chai.add(fun(&Quaternion::z), "z");
	chai.add(fun(&Quaternion::w), "w");

	chai.add(fun([](const Quaternion& q, const Vector3 v) {return q * v; }), "*");
	chai.add(fun([](const Quaternion& q1, const Quaternion& q2) {return q1 * q2; }), "*");

	//TODO Add to chai a way to access useful Annwvyn components
	chai.add(user_type<AnnGameObject>(), "AnnGameObject");
	chai.add(constructor<AnnGameObject(const AnnGameObject&)>(), "AnnGameObject");
	chai.add(fun([](shared_ptr<AnnGameObject> o, Vector3 v) {o->setPosition(v); }), "setPosition");
	chai.add(fun([](shared_ptr<AnnGameObject> o, Quaternion q) {o->setOrientation(q); }), "setOrientation");
	chai.add(fun([](shared_ptr<AnnGameObject> o) -> Vector3 {return o->getPosition(); }), "getPosition");
	chai.add(fun([](shared_ptr<AnnGameObject> o) -> Quaternion {return o->getOrientation(); }), "getOrientation");
	chai.add(fun([](shared_ptr<AnnGameObject> o, Vector3 v) {o->setScale(v); }), "setScale");
	chai.add(fun([](shared_ptr<AnnGameObject> o) -> Vector3 {return o->getScale(); }), "getScale");
	chai.add(fun([](shared_ptr<AnnGameObject> o, const string& s) {o->playSound(s); }), "playSound");
	chai.add(fun([](shared_ptr<AnnGameObject> o, const string& s) {o->playSound(s, true); }), "playSoundLoop");

	//Engine API

	chai.add(fun([](string id) { return AnnGetGameObjectManager()->getObjectFromID(id); }), "AnnGetGameObject");

	chai.add(fun([](Annwvyn::level_id id) { AnnGetLevelManager()->jump(id); }), "AnnJumpLevel");

	//Register an accessors to the engine's log
	chai.add(fun([](const string& s) {AnnDebug() << logFromScript << s; }), "AnnDebugLog");
	chai.add(fun([](const Vector3& s) {AnnDebug() << logFromScript << s; }), "AnnDebugLog");
	chai.add(fun([](const Vector2& s) {AnnDebug() << logFromScript << s; }), "AnnDebugLog");
	chai.add(fun([](const Quaternion& s) {AnnDebug() << logFromScript << s; }), "AnnDebugLog");
	chai.add(fun([](const Radian& s) {AnnDebug() << logFromScript << s; }), "AnnDebugLog");
	chai.add(fun([](const Degree& s) {AnnDebug() << logFromScript << s; }), "AnnDebugLog");
}

Annwvyn::AnnBehaviorScript::AnnBehaviorScript() :
	valid(false)

{
}

Annwvyn::AnnBehaviorScript::AnnBehaviorScript(std::string scriptName, std::function<void(chaiscript::Boxed_Value&)> updateHook, chaiscript::Boxed_Value chaisriptInstance) :
	valid(true),
	name(scriptName),
	callUpdateOnScriptInstance(updateHook),
	ScriptObjectInstance(chaisriptInstance)
{
}

Annwvyn::AnnBehaviorScript::AnnBehaviorScript(const AnnBehaviorScript & script) :
	valid(script.valid),
	name(script.name),
	ScriptObjectInstance(script.ScriptObjectInstance),
	callUpdateOnScriptInstance(script.callUpdateOnScriptInstance)
{
}

AnnBehaviorScript Annwvyn::AnnBehaviorScript::operator=(const AnnBehaviorScript & script)
{
	return script;
}

void Annwvyn::AnnBehaviorScript::update()
{
	callUpdateOnScriptInstance(ScriptObjectInstance);
}

bool Annwvyn::AnnBehaviorScript::isValid()
{
	return valid;
}