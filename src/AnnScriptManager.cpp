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

void Annwvyn::AnnScriptManager::registerApi()
{
	using namespace Ogre;
	using namespace chaiscript;
	using namespace std;

	// TODO Add to chai all the useful types (angles, vectors, quaternions...)

	chai.add(var(&Math::PI), "PI");
	chai.add(var(&Math::HALF_PI), "HALF_PI");

	// 3D vector
	chai.add(user_type<Vector3>(), "AnnVect3");
	chai.add(constructor<Vector3()>(), "AnnVect3");
	chai.add(constructor<Vector3(const float, const float, const float)>(), "AnnVect3");
	chai.add(constructor<Vector3(const float[3])>(), "AnnVect3");
	chai.add(constructor<Vector3(const Vector3&)>(), "AnnVect3");
	chai.add(fun(&Vector3::x), "x");
	chai.add(fun(&Vector3::y), "y");
	chai.add(fun(&Vector3::z), "z");
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
	chai.add(fun([](Vector3& u, Vector3& v) {u.swap(v); }), "swap");
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
	chai.add(fun([](const Radian& r1, const Radian& r2) {return r1 + r2; }), "+");
	chai.add(fun([](const Radian& r1, const Degree& r2) {return r1 + r2; }), "+");
	chai.add(fun([](Radian& r1, const Radian& r2) {r1 += r2; }), "+=");
	chai.add(fun([](const Radian& d) {return -d; }), "-");
	chai.add(fun([](const Radian& r1, const Radian& r2) { return r1 - r2; }), "-");
	chai.add(fun([](const Radian& r1, const Degree& r2) { return r1 - r2; }), "-");
	chai.add(fun([](Radian& r1, const Radian& r2) {r1 -= r2; }), "-=");
	chai.add(fun([](Radian& r1, const Degree& r2) {r1 -= r2; }), "-=");
	chai.add(fun([](const Radian& d, Real f) {return d * f; }), "*");
	chai.add(fun([](Radian& d, Real f) {d *= f; }), "*=");
	chai.add(fun([](const Radian& d, Real f) {return d / f; }), "/");
	chai.add(fun([](Radian& d, Real f) {d /= f; }), "/=");
	chai.add(fun([](const Radian& r1, const Radian& r2) {return r1 < r2; }), "<");
	chai.add(fun([](const Radian& r1, const Radian& r2) {return r1 > r2; }), ">");
	chai.add(fun([](const Radian& r1, const Radian& r2) {return r1 <= r2; }), "<=");
	chai.add(fun([](const Radian& r1, const Radian& r2) {return r1 >= r2; }), ">=");
	chai.add(fun([](const Radian& r1, const Radian& r2) {return r1 == r2; }), "==");
	chai.add(fun([](const Radian& r1, const Radian& r2) {return r1 != r2; }), "!=");

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
	chai.add(fun([](const Quaternion& q, const Real& scalar) {return q * scalar; }), "*");
	chai.add(fun([](const Quaternion& q, size_t i) {return q[i]; }), "[]");
	chai.add(fun([](Quaternion& q1, const Quaternion& q2) { q1 = q2; }), "=");
	chai.add(fun([](const Quaternion& q1, const Quaternion& q2) {return q1 + q2; }), "+");
	chai.add(fun([](const Quaternion& q1, const Quaternion& q2) {return q1 - q2; }), "-");
	chai.add(fun([](const Quaternion& q) {return -q; }), "-");
	chai.add(fun([](const Quaternion& q1, const Quaternion& q2) {return q1 == q2; }), "==");
	chai.add(fun([](const Quaternion& q1, const Quaternion& q2) {return q1 != q2; }), "!=");
	chai.add(fun([](const Quaternion& q) {return q.xAxis(); }), "xAxis");
	chai.add(fun([](const Quaternion& q) {return q.yAxis(); }), "yAxis");
	chai.add(fun([](const Quaternion& q) {return q.zAxis(); }), "zAxis");
	chai.add(fun([](const Quaternion& q) { return q.getRoll(); }), "getRoll");
	chai.add(fun([](const Quaternion& q) { return q.getPitch(); }), "getPitch");
	chai.add(fun([](const Quaternion& q) { return q.getYaw(); }), "getYaw");
	chai.add(fun([](const Quaternion& q) {return q.isNaN(); }), "isNaN");
	chai.add(var(&Quaternion::ZERO), "AnnQuaternion_ZERO");
	chai.add(var(&Quaternion::IDENTITY), "AnnQuaternion_IDENTITY");

	//TODO Add to chai a way to access useful Annwvyn components
	chai.add(user_type<AnnGameObject>(), "AnnGameObject");
	chai.add(fun([](AnnGameObject* o, Vector3 v) {o->setPosition(v); }), "setPosition");
	chai.add(fun([](AnnGameObject* o, Quaternion q) {o->setOrientation(q); }), "setOrientation");
	chai.add(fun([](AnnGameObject* o) -> Vector3 {return o->getPosition(); }), "getPosition");
	chai.add(fun([](AnnGameObject* o) -> Quaternion {return o->getOrientation(); }), "getOrientation");
	chai.add(fun([](AnnGameObject* o, Vector3 v) {o->setScale(v); }), "setScale");
	chai.add(fun([](AnnGameObject* o) -> Vector3 {return o->getScale(); }), "getScale");
	chai.add(fun([](AnnGameObject* o, const string& s) {o->playSound(s); }), "playSound");
	chai.add(fun([](AnnGameObject* o, const string& s) {o->playSound(s, true); }), "playSoundLoop");

	//Color
	chai.add(user_type<AnnColor>(), "AnnColor");
	chai.add(constructor<AnnColor(float, float, float, float)>(), "AnnColor");
	chai.add(constructor<AnnColor(const ColourValue&)>(), "AnnColor");
	// HACK copy constructor and operator= of AnnColor is broken
	//chai.add(constructor<AnnColor(const AnnColor&)>, "AnnColor");
	chai.add(fun([](AnnColor& c1, AnnColor& c2)
	{
		c1.setRed(c2.getRed());
		c1.setGreen(c2.getGreen());
		c1.setBlue(c2.getBlue());
		c1.setAlpha(c2.getAlpha());
	}), "=");
	chai.add(fun([](AnnColor& color) {return color.getRed(); }), "getRed");
	chai.add(fun([](AnnColor& color) {return color.getGreen(); }), "getGreen");
	chai.add(fun([](AnnColor& color) {return color.getBlue(); }), "getBlue");
	chai.add(fun([](AnnColor& color) {return color.getAlpha(); }), "getAlpha");
	chai.add(fun([](AnnColor& color, float value) {return color.setRed(value); }), "setRed");
	chai.add(fun([](AnnColor& color, float value) {return color.setGreen(value); }), "setGreen");
	chai.add(fun([](AnnColor& color, float value) {return color.setBlue(value); }), "setBlue");
	chai.add(fun([](AnnColor& color, float value) {return color.setAlpha(value); }), "setAlpha");

	//Object getter
	chai.add(fun([](string id) { return AnnGetGameObjectManager()->getObjectFromID(id).get(); }), "AnnGetGameObject");
	//Level jumper
	chai.add(fun([](Annwvyn::level_id id) { AnnGetLevelManager()->jump(id); }), "AnnJumpLevel");
	//Changing the color of the background
	chai.add(fun([](const AnnColor& color) {AnnGetSceneryManager()->setWorldBackgroundColor(color); }), "AnnSetWorldBackgroundColor");
	//Change the ambient lighting
	chai.add(fun([](const AnnColor& color) {AnnGetSceneryManager()->setAmbientLight(color); }), "AnnSetAmbientLight");
	//Create a GameObject form ChaiScript
	chai.add(fun([](const std::string& mesh, const std::string& objectName)
	{
		AnnGetLevelManager()->addToCurrentLevel
		(
			AnnGetGameObjectManager()->createGameObject(mesh.c_str(), objectName)
		);
	}), "AnnCreateGameObject");
	//Remove object
	chai.add(fun([](const std::string& objectName)
	{
		auto obj = AnnGetGameObjectManager()->getObjectFromID(objectName);
		if (!obj) return;
		AnnGetGameObjectManager()->removeGameObject(obj);
	}
	), "AnnRemoveGameObject");

	//Change the gravity
	chai.add(fun([](const Vector3& gravity) {AnnGetPhysicsEngine()->changeGravity(gravity); }), "AnnChangeGravity");
	//Restore the default gravity vector
	chai.add(fun([]() {AnnGetPhysicsEngine()->resetGravity(); }), "AnnRestoreGravity");

	//Add the types of the event representation object
	chai.add(user_type<AnnKeyEvent>(), "AnnKeyEvent");
	chai.add(user_type<AnnMouseEvent>(), "AnnMouseEvent");
	chai.add(user_type<AnnStickEvent>(), "AnnStickEvent");
	chai.add(user_type<AnnTimeEvent>(), "AnnTimeEvent");
	chai.add(user_type<AnnTriggerEvent>(), "AnnTriggerEvent");
	chai.add(user_type<AnnHandControllerEvent>(), "AnnHandControllerEvent");
	chai.add(user_type<AnnMouseAxis>(), "AnnMouseAxis");
	chai.add(user_type<MouseAxisId>(), "MouseAxisId");
	chai.add(user_type<MouseButtonId>(), "MouseButtonId");
	chai.add(user_type<AnnStickAxis>(), "AnnStickAxis");
	chai.add(user_type<AnnStickPov>(), "AnnStickPov");
	chai.add(user_type<timerID>(), "timerID");

	chai.add(fun([](AnnKeyEvent e) {return e.isPressed(); }), "isPressed");
	chai.add(fun([](AnnKeyEvent e) {return e.isReleased(); }), "isReleased");
	chai.add(fun([](AnnKeyEvent e) {return e.getKey(); }), "getKey");

	chai.add(fun([](AnnMouseEvent e, /*MouseAxisId*/const int a) {return e.getAxis(MouseAxisId(a)); }), "getAxis");
	chai.add(fun([](AnnMouseEvent e, /*MouseButtonId*/const int b) {return e.getButtonState(MouseButtonId(b)); }), "getButtonState");
	chai.add(fun([](AnnMouseAxis a) {return a.getRelValue(); }), "getRelValue");
	chai.add(fun([](AnnMouseAxis a) {return a.getAbsValue(); }), "getAbsValue");

	chai.add(fun([](AnnStickEvent e) {return e.getNbButtons(); }), "getNbButtons");
	chai.add(fun([](AnnStickEvent e) {return e.getNbAxis(); }), "getNbAxis");
	chai.add(fun([](AnnStickEvent e) {return e.getNbPov(); }), "getNbPov");
	chai.add(fun([](AnnStickEvent e) {return e.getVendor(); }), "getVendor");
	chai.add(fun([](AnnStickEvent e) {return e.getStickID(); }), "getStickID");
	chai.add(fun([](AnnStickEvent e) {return e.isXboxController(); }), "isXboxController");
	chai.add(fun([](AnnStickEvent e, const int i) {return e.isPressed(i); }), "isPressed");
	chai.add(fun([](AnnStickEvent e, const int i) {return e.isReleased(i); }), "isReleased");
	chai.add(fun([](AnnStickEvent e, const int i) {return e.isDown(i); }), "isDown");
	chai.add(fun([](AnnStickEvent e, const int i) {return e.getAxis(i); }), "getAxis");
	chai.add(fun([](AnnStickEvent e, const int i) {return e.getPov(i); }), "getPov");

	chai.add(fun([](AnnStickPov pov) {return pov.getNorth(); }), "getNorth");
	chai.add(fun([](AnnStickPov pov) {return pov.getSouth(); }), "getSouth");
	chai.add(fun([](AnnStickPov pov) {return pov.getEast(); }), "getEast");
	chai.add(fun([](AnnStickPov pov) {return pov.getWest(); }), "getWest");
	chai.add(fun([](AnnStickPov pov) {return pov.getNorthEast(); }), "getNorthEast");
	chai.add(fun([](AnnStickPov pov) {return pov.getNorthWest(); }), "getNorthWest");
	chai.add(fun([](AnnStickPov pov) {return pov.getSouthEast(); }), "getSouthEast");
	chai.add(fun([](AnnStickPov pov) {return pov.getSouthWest(); }), "getSouthWest");

	chai.add(fun([](AnnStickAxis a) {return a.getAxisId(); }), "getAxisId");
	chai.add(fun([](AnnStickAxis a) {return a.getRelValue(); }), "getRelValue");
	chai.add(fun([](AnnStickAxis a) {return a.getAbsValue(); }), "getAbsValue");

	chai.add(fun([](AnnTimeEvent t) {return t.getID(); }), "getID");

	chai.add(fun([](AnnTriggerEvent e) {return e.getContactStatus(); }), "getContactStatus");
	chai.add(fun([](AnnTriggerEvent e) {return e.getSender(); }), "getSender");

	// TODO the hand controller event interface is not finished
	chai.add(user_type<AnnHandController>(), "AnnHandController");
	chai.add(user_type<AnnHandControllerAxis>(), "AnnHandControllerAxis");
	chai.add(user_type<AnnHandController::AnnHandControllerSide>(), "AnnHandControllerSide");
	chai.add(var(AnnHandController::AnnHandControllerSide::leftHandController), "leftHandController");
	chai.add(var(AnnHandController::AnnHandControllerSide::rightHandController), "rightHandController");
	chai.add(var(AnnHandController::AnnHandControllerSide::invalidHandController), "invalidHandController");

	// HACK I'm exposing a pointer to the controller for now. This will be encapsulated after
	//we have access to Oculus's hand controllers (December 2016...)
	chai.add(fun([](AnnHandControllerEvent e) {return e.getController(); }), "getController");

	chai.add(fun([](AnnHandController* c) {return c->getWorldPosition(); }), "getWorldPosition");
	chai.add(fun([](AnnHandController* c) {return c->getWorldOrientation(); }), "getWorldOrientation");
	chai.add(fun([](AnnHandController* c) {return c->getLinearSpeed(); }), "getLinearSpeed");
	chai.add(fun([](AnnHandController* c) {return c->getAngularSpeed(); }), "getAngularSpeed");
	chai.add(fun([](AnnHandController* c) {return c->getPointingDirection(); }), "getPointingDirection");
	chai.add(fun([](AnnHandController* c) {return c->getNbAxes(); }), "getNbAxes");
	chai.add(fun([](AnnHandController* c) {return c->getNbButton(); }), "getNbButton");
	chai.add(fun([](AnnHandController* c) {return c->isTracked(); }), "isTracked");
	chai.add(fun([](AnnHandController* c, uint8_t i) {return c->getButtonState(i); }), "getButtonState");
	chai.add(fun([](AnnHandController* c, uint8_t i) {return c->getAxis(i); }), "getAxis");
	chai.add(fun([](AnnHandController* c, uint8_t i) {return c->hasBeenPressed(i); }), "hasBeenPressed");
	chai.add(fun([](AnnHandController* c, uint8_t i) {return c->hasBeenReleased(i); }), "hasBeenReleased");

	//There's capacitive touch surfaces and haptic feedback that aren't available right now on the AnnHandController class

	//Register an accessors to the engine's log
	chai.add(fun([](const string& s) {AnnDebug() << logFromScript << s; }), "AnnDebugLog");
	chai.add(fun([](const Vector3& s) {AnnDebug() << logFromScript << s; }), "AnnDebugLog");
	chai.add(fun([](const Vector2& s) {AnnDebug() << logFromScript << s; }), "AnnDebugLog");
	chai.add(fun([](const Quaternion& s) {AnnDebug() << logFromScript << s; }), "AnnDebugLog");
	chai.add(fun([](const Radian& s) {AnnDebug() << logFromScript << s; }), "AnnDebugLog");
	chai.add(fun([](const Degree& s) {AnnDebug() << logFromScript << s; }), "AnnDebugLog");
	chai.add(fun([](const AnnColor& s) {AnnDebug() << logFromScript << s; }), "AnnDebugLog");
	chai.add(fun([](KeyCode::code c) {AnnDebug() << logFromScript << "keycode:" << c; }), "AnnDebugLog");
	chai.add(fun([](MouseAxisId c) {AnnDebug() << logFromScript << "mouseAxis:" << c; }), "AnnDebugLog");
	chai.add(fun([](bool b) {std::string s("true"); if (!b) s = "false"; AnnDebug() << logFromScript << "bool:" << s; }), "AnnDebugLog");
	chai.add(fun([](int i) {AnnDebug() << logFromScript << "int:" << i; }), "AnnDebugLog");
	chai.add(fun([](float f) {AnnDebug() << logFromScript << "float:" << f; }), "AnnDebugLog");
}

void Annwvyn::AnnScriptManager::tryAndGetEventHooks()
{
	//Forgive me.
	try
	{
		callKeyEventOnScriptInstance = chai.eval<std::function<void(chaiscript::Boxed_Value&, AnnKeyEvent)>>("KeyEvent");
	}
	catch (const chaiscript::exception::eval_error&)
	{
		callKeyEventOnScriptInstance = nullptr;
	}

	//Yes. I'm doing this.
	try
	{
		callMouseEventOnScriptInstance = chai.eval<std::function<void(chaiscript::Boxed_Value&, AnnMouseEvent)>>("MouseEvent");
	}
	catch (const chaiscript::exception::eval_error&)
	{
		callMouseEventOnScriptInstance = nullptr;
	}

	//Yes, there's 6 of them
	try
	{
		callStickEventOnScriptInstance = chai.eval<std::function<void(chaiscript::Boxed_Value&, AnnStickEvent)>>("StickEvent");
	}
	catch (const chaiscript::exception::eval_error&)
	{
		callStickEventOnScriptInstance = nullptr;
	}

	//And yes, it's probable that something will be thrown, unless a script already have this function
	try
	{
		callTimeEventOnScriptInstance = chai.eval<std::function<void(chaiscript::Boxed_Value&, AnnTimeEvent)>>("TimeEvent");
	}
	catch (const chaiscript::exception::eval_error&)
	{
		callTimeEventOnScriptInstance = nullptr;
	}

	//And it's also possible than the result is not usable with this script and will throw later at eval time
	try
	{
		callTriggerEventOnScriptInstance = chai.eval<std::function<void(chaiscript::Boxed_Value&, AnnTriggerEvent)>>("TriggerEvent");
	}
	catch (const chaiscript::exception::eval_error&)
	{
		callTriggerEventOnScriptInstance = nullptr;
	}

	//Like I said. Please forgive me.
	try
	{
		callHandControllertOnScriptInstance = chai.eval<std::function<void(chaiscript::Boxed_Value&, AnnHandControllerEvent)>>("HandControllerEvent");
	}
	catch (const chaiscript::exception::eval_error&)
	{
		callHandControllertOnScriptInstance = nullptr;
	}
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
	auto file{ scriptName + scriptExtension };

	try
	{
		//Evaluate the file containing the script class if unknown to ChaiScript yet
		chai.use(file);

		//Increment ID
		ID++;

		std::string ownerTag{ "" };

		//Not giving an owner to a script that wants an owner, or calling an owner to a script that
		if (owner)
			ownerTag = owner->getName();

		//This may looks odd but it's good enough for what we're doing:
		//Copy the template of the init code to a string
		std::string ChaiCode{ scriptTemplate };
		//Replace the tags with the actual values, one by one
		ChaiCode.replace(ChaiCode.find(std::string(scriptNameMarker)), nameMarkerLen, scriptName);
		ChaiCode.replace(ChaiCode.find(std::string(scriptNameMarker)), nameMarkerLen, scriptName);
		ChaiCode.replace(ChaiCode.find(std::string(scriptObjectID)), scriptIDMarkerLen, std::to_string(ID));
		ChaiCode.replace(ChaiCode.find(std::string(scriptObjectID)), scriptIDMarkerLen, std::to_string(ID));
		ChaiCode.replace(ChaiCode.find(std::string(scriptObjectID)), scriptIDMarkerLen, std::to_string(ID));

		//This is the ugly bit.
		tryAndGetEventHooks();

		//This will add a global function in ChaiScript, that will create and return the script instance
		chai.eval(ChaiCode);
		//Get a way to call this function
		auto creatorFunction = chai.eval<std::function<chaiscript::Boxed_Value(std::string)>>("create" + scriptName + std::to_string(ID));

		//Now we need to get some hook to call the update on the file
		return std::make_shared<AnnBehaviorScript>(
			scriptName,
			//Function to call to update the script
			chai.eval<std::function<void(chaiscript::Boxed_Value&)>>("update"),

			//Eventual event hooks
			callKeyEventOnScriptInstance,
			callMouseEventOnScriptInstance,
			callStickEventOnScriptInstance,
			callTimeEventOnScriptInstance,
			callTriggerEventOnScriptInstance,
			callHandControllertOnScriptInstance,

			//This return the ScriptInstance, as a Boxed_Value. We're only interested at calling something on
			//this object, so don't need to try to unbox it. It's literally a black box for us
			creatorFunction(ownerTag)
			);
	}

	// TODO I genuinely don't know if we should crash the game or just display an error about a missing or miss-formed script
	catch (const chaiscript::exception::file_not_found_error& fnfe)
	{
		AnnDebug() << fileErrorPrefix << fnfe.what();
	}
	catch (const chaiscript::exception::eval_error& ee)
	{
		AnnDebug() << ee.pretty_print();
	}

	//The user should test if this script is "valid" or not. And should not do it in a loop, obviously
	return std::make_shared<AnnBehaviorScript>();
}

Annwvyn::AnnBehaviorScript::AnnBehaviorScript() :
	valid(false),
	cannotKey(false),
	cannotMouse(false),
	cannotStick(false),
	cannotTime(false),
	cannotTrigger(false),
	cannotHand(false)
{
	AnnDebug() << "Invalid script object created";
}

AnnBehaviorScript::AnnBehaviorScript(std::string scriptName,
									 std::function<void(chaiscript::Boxed_Value&)> updateHook,
									 std::function<void(chaiscript::Boxed_Value&, AnnKeyEvent)> KeyEventHook,
									 std::function<void(chaiscript::Boxed_Value&, AnnMouseEvent)> MouseEventHook,
									 std::function<void(chaiscript::Boxed_Value&, AnnStickEvent)> StickEventHook,
									 std::function<void(chaiscript::Boxed_Value&, AnnTimeEvent)> TimeEventHook,
									 std::function<void(chaiscript::Boxed_Value&, AnnTriggerEvent)> TriggerEventHook,
									 std::function<void(chaiscript::Boxed_Value&, AnnHandControllerEvent)> HandControllertHook,
									 chaiscript::Boxed_Value chaisriptInstance) : constructListener(),
	valid(true),
	name(scriptName),
	ScriptObjectInstance(chaisriptInstance),
	callUpdateOnScriptInstance(updateHook),
	callKeyEventOnScriptInstance(KeyEventHook),
	callMouseEventOnScriptInstance(MouseEventHook),
	callStickEventOnScriptInstance(StickEventHook),
	callTriggerEventOnScriptInstance(TriggerEventHook),
	callHandControllertOnScriptInstance(HandControllertHook),
	cannotKey(false),
	cannotMouse(false),
	cannotStick(false),
	cannotTime(false),
	cannotTrigger(false),
	cannotHand(false)
{
}

Annwvyn::AnnBehaviorScript::~AnnBehaviorScript()
{
	AnnDebug() << "Destructing " << name << "Script";
}

void Annwvyn::AnnBehaviorScript::update()
{
	try
	{
		callUpdateOnScript();
	}
	catch (const chaiscript::exception::eval_error& ee)
	{
		std::cerr << "Update script - " << ee.pretty_print();
		//will not crash here.
	}
}

bool Annwvyn::AnnBehaviorScript::isValid() const
{
	return valid;
}

void Annwvyn::AnnBehaviorScript::registerAsListener()
{
	AnnGetEventManager()->addListener(getSharedListener());
}

void Annwvyn::AnnBehaviorScript::unregisterAsListener()
{
	AnnDebug() << "Unregistering ourself has event listener";
	AnnGetEventManager()->removeListener(getSharedListener());
}

void Annwvyn::AnnBehaviorScript::KeyEvent(AnnKeyEvent e)
{
	try
	{
		if (callKeyEventOnScriptInstance && !cannotKey)
			callKeyEventOnScriptInstance(ScriptObjectInstance, e);
	}
	catch (const chaiscript::exception::dispatch_error&) { cannotKey = true; }
	catch (const chaiscript::exception::eval_error& ee) { AnnDebug() << "Event script error " << ee.pretty_print(); }
}

void Annwvyn::AnnBehaviorScript::MouseEvent(AnnMouseEvent e)
{
	try
	{
		if (callMouseEventOnScriptInstance && !cannotMouse)
			callMouseEventOnScriptInstance(ScriptObjectInstance, e);
	}
	catch (const chaiscript::exception::dispatch_error&) { cannotMouse = true; }
	catch (const chaiscript::exception::eval_error& ee) { AnnDebug() << "Event script error " << ee.pretty_print(); }
}

void Annwvyn::AnnBehaviorScript::StickEvent(AnnStickEvent e)
{
	try
	{
		if (callStickEventOnScriptInstance && !cannotStick)
			callStickEventOnScriptInstance(ScriptObjectInstance, e);
	}
	catch (const chaiscript::exception::dispatch_error&) { cannotStick = true; }
	catch (const chaiscript::exception::eval_error& ee) { AnnDebug() << "Event script error " << ee.pretty_print(); }
}

void Annwvyn::AnnBehaviorScript::TimeEvent(AnnTimeEvent e)
{
	try
	{
		if (callTimeEventOnScriptInstance && !cannotTime)
			callTimeEventOnScriptInstance(ScriptObjectInstance, e);
	}
	catch (const chaiscript::exception::dispatch_error&) { cannotTime = true; }
	catch (const chaiscript::exception::eval_error& ee) { AnnDebug() << "Event script error " << ee.pretty_print(); }
}

void Annwvyn::AnnBehaviorScript::TriggerEvent(AnnTriggerEvent e)
{
	try
	{
		if (callTriggerEventOnScriptInstance && cannotTrigger)
			callTriggerEventOnScriptInstance(ScriptObjectInstance, e);
	}
	catch (const chaiscript::exception::dispatch_error&) { cannotTrigger = true; }
	catch (const chaiscript::exception::eval_error& ee) { AnnDebug() << "Event script error " << ee.pretty_print(); }
}

void Annwvyn::AnnBehaviorScript::HandControllerEvent(AnnHandControllerEvent e)
{
	try
	{
		if (callHandControllertOnScriptInstance && cannotHand)
			callHandControllertOnScriptInstance(ScriptObjectInstance, e);
	}
	catch (const chaiscript::exception::dispatch_error&) { cannotHand = true; }
	catch (const chaiscript::exception::eval_error& ee) { AnnDebug() << "Event script error " << ee.pretty_print(); }
}