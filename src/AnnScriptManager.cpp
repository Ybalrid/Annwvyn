// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "AnnScriptManager.hpp"
#include "AnnLogger.hpp"
#include "AnnGameObject.hpp"
#include "AnnGameObjectManager.hpp"
#include "AnnEngine.hpp"
#include "AnnGetter.hpp"
#include "Annwvyn.h"

using namespace Annwvyn;

constexpr const char* const AnnScriptManager::fileErrorPrefix;
constexpr const char* const AnnScriptManager::logFromScript;

AnnScriptManager::AnnScriptManager() :
 AnnSubSystem("ScriptManager"),
 scriptFileManager(nullptr)
{
	registerApi();
	AnnDebug(Log::Important) << "Using ChaiScript version 6.0";
	registerResourceManager();
}

void AnnScriptManager::registerApi()
{
	using namespace Ogre;
	using namespace chaiscript;
	using namespace std;

	// TODO ISSUE Add to chai all the useful types (angles, vectors, quaternions...)
	try
	{
		//Random maths
		chai.add(var(&Math::PI), "PI");
		chai.add(var(&Math::HALF_PI), "HALF_PI");
		chai.add(var(&Math::LOG2), "LOG2");
		chai.add(var(&Math::TWO_PI), "TWO_PI");
		chai.add(fun([](const Vector3& a, const Vector3& b, float w) { return Math::lerp(a, b, w); }), "lerp");
		chai.add(fun([](float a, float b, float w) { return Math::lerp(a, b, w); }), "lerp");
		chai.add(fun([](float fT, const Quaternion& rkP, const Quaternion& rkQ) { return Quaternion::Slerp(fT, rkP, rkQ); }), "slerp");
		chai.add(fun([](float f) { return sin(f); }), "sin");
		chai.add(fun([](float f) { return cos(f); }), "cos");
		chai.add(fun([](float f) { return tan(f); }), "tan");
		chai.add(fun([](float f) { return asin(f); }), "asin");
		chai.add(fun([](float f) { return acos(f); }), "acos");
		chai.add(fun([](float f) { return atan(f); }), "atan");
		chai.add(fun([](float y, float x) { return atan2(y, x); }), "atan2");

		chai.add(fun([] { return AnnGetEngine()->getTimeFromStartUp(); }), "getTimeFromStartUp");

		// 3D vector
		chai.add(user_type<Vector3>(), "AnnVect3");
		chai.add(constructor<Vector3()>(), "AnnVect3");
		chai.add(constructor<Vector3(float, float, float)>(), "AnnVect3");
		chai.add(constructor<Vector3(const float[3])>(), "AnnVect3");
		chai.add(constructor<Vector3(const Vector3&)>(), "AnnVect3");
		chai.add(fun(&Vector3::x), "x");
		chai.add(fun(&Vector3::y), "y");
		chai.add(fun(&Vector3::z), "z");
		chai.add(fun([](Vector3& u, const Vector3& v) { u = v; }), "=");
		chai.add(fun([](Vector3& u, const Real s) { u = s; }), "=");
		chai.add(fun<Vector3>(&Vector3::operator+), "+");
		chai.add(fun([](const Vector3& v) { return -v; }), "-");
		chai.add(fun([](const Vector3& v, Vector3 w) { return v - w; }), "-");
		chai.add(fun([](const Vector3& v, const Real w) { return v - w; }), "-");
		chai.add(fun([](const Real& v, const Vector3& w) { return v - w; }), "-");
		chai.add(fun([](const Vector3& vector, Real scalar) { return scalar * vector; }), "*");
		chai.add(fun([](Real scalar, const Vector3& vector) { return scalar * vector; }), "*");
		chai.add(fun([](const Vector3& v1, const Vector3& v2) { return v1 * v2; }), "*");
		chai.add(fun([](const Vector3& vector, Real scalar) { return scalar / vector; }), "/");
		chai.add(fun([](const Vector3& v1, const Vector3& v2) { return v1 / v2; }), "/");
		chai.add(fun([](Vector3& u, Vector3 v) { u *= v; }), "*=");
		chai.add(fun([](Vector3& u, Vector3 v) { u /= v; }), "/=");
		chai.add(fun([](Vector3& u, Vector3 v) { u += v; }), "+=");
		chai.add(fun([](Vector3& u, Vector3 v) { u -= v; }), "-=");
		chai.add(fun([](Vector3& vector, Real scalar) { vector *= scalar; }), "*=");
		chai.add(fun([](Vector3& vector, Real scalar) { vector /= scalar; }), "/=");
		chai.add(fun([](Vector3& vector, Real scalar) { vector += scalar; }), "+=");
		chai.add(fun([](Vector3& vector, Real scalar) { vector -= scalar; }), "-=");
		chai.add(fun([](const Vector3& v1, const Vector3& v2) { return v1 == v2; }), "==");
		chai.add(fun([](const Vector3& v1, const Vector3& v2) { return v1 != v2; }), "!=");
		chai.add(fun([](const Vector3& v1, const Vector3& v2) { return v1 < v2; }), "<");
		chai.add(fun([](const Vector3& v1, const Vector3& v2) { return v1 > v2; }), ">");
		chai.add(fun([](const Vector3& v, const size_t i) { return v[i]; }), "[]");
		chai.add(fun([](Vector3& u, Vector3& v) { u.swap(v); }), "swap");
		chai.add(fun([](const Vector3& v) { return v.length(); }), "length");
		chai.add(fun([](const Vector3& v) { return v.normalisedCopy(); }), "normalisedCopy");
		chai.add(fun([](const Vector3& v) { return v.squaredLength(); }), "squaredLength");
		chai.add(fun([](const Vector3& v) { return v.perpendicular(); }), "perpendicular");
		chai.add(fun([](const Vector3& v) { return v.primaryAxis(); }), "primaryAxis");
		chai.add(fun([](const Vector3& v) { return v.isZeroLength(); }), "isZeroLength");
		chai.add(fun([](const Vector3& v) { return v.isNaN(); }), "isNaN");
		chai.add(fun([](Vector3& v) { return v.normalise(); }), "normalise");
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
		chai.add(fun([](const Degree& d) { return +d; }), "+");
		chai.add(fun([](const Degree& d1, const Degree& d2) { return d1 + d2; }), "+");
		chai.add(fun([](const Degree& d1, const Radian& d2) { return d1 + d2; }), "+");
		chai.add(fun([](Degree& d1, const Degree& d2) { d1 += d2; }), "+=");
		chai.add(fun([](const Degree& d) { return -d; }), "-");
		chai.add(fun([](const Degree& d1, const Degree& d2) { return d1 - d2; }), "-");
		chai.add(fun([](const Degree& d1, const Radian& d2) { return d1 - d2; }), "-");
		chai.add(fun([](Degree& d1, const Degree& d2) { d1 -= d2; }), "-=");
		chai.add(fun([](Degree& d1, const Radian& d2) { d1 -= d2; }), "-=");
		chai.add(fun([](const Degree& d, Real f) { return d * f; }), "*");
		chai.add(fun([](Degree& d, Real f) { d *= f; }), "*=");
		chai.add(fun([](const Degree& d, Real f) { return d / f; }), "/");
		chai.add(fun([](Degree& d, Real f) { d /= f; }), "/=");
		chai.add(fun([](const Degree& d1, const Degree& d2) { return d1 < d2; }), "<");
		chai.add(fun([](const Degree& d1, const Degree& d2) { return d1 > d2; }), ">");
		chai.add(fun([](const Degree& d1, const Degree& d2) { return d1 <= d2; }), "<=");
		chai.add(fun([](const Degree& d1, const Degree& d2) { return d1 >= d2; }), ">=");
		chai.add(fun([](const Degree& d1, const Degree& d2) { return d1 == d2; }), "==");
		chai.add(fun([](const Degree& d1, const Degree& d2) { return d1 != d2; }), "!=");
		chai.add(fun([](const Radian& d) { return +d; }), "+");
		chai.add(fun([](const Radian& r1, const Radian& r2) { return r1 + r2; }), "+");
		chai.add(fun([](const Radian& r1, const Degree& r2) { return r1 + r2; }), "+");
		chai.add(fun([](Radian& r1, const Radian& r2) { r1 += r2; }), "+=");
		chai.add(fun([](const Radian& d) { return -d; }), "-");
		chai.add(fun([](const Radian& r1, const Radian& r2) { return r1 - r2; }), "-");
		chai.add(fun([](const Radian& r1, const Degree& r2) { return r1 - r2; }), "-");
		chai.add(fun([](Radian& r1, const Radian& r2) { r1 -= r2; }), "-=");
		chai.add(fun([](Radian& r1, const Degree& r2) { r1 -= r2; }), "-=");
		chai.add(fun([](const Radian& d, Real f) { return d * f; }), "*");
		chai.add(fun([](Radian& d, Real f) { d *= f; }), "*=");
		chai.add(fun([](const Radian& d, Real f) { return d / f; }), "/");
		chai.add(fun([](Radian& d, Real f) { d /= f; }), "/=");
		chai.add(fun([](const Radian& r1, const Radian& r2) { return r1 < r2; }), "<");
		chai.add(fun([](const Radian& r1, const Radian& r2) { return r1 > r2; }), ">");
		chai.add(fun([](const Radian& r1, const Radian& r2) { return r1 <= r2; }), "<=");
		chai.add(fun([](const Radian& r1, const Radian& r2) { return r1 >= r2; }), ">=");
		chai.add(fun([](const Radian& r1, const Radian& r2) { return r1 == r2; }), "==");
		chai.add(fun([](const Radian& r1, const Radian& r2) { return r1 != r2; }), "!=");

		//Quaternions
		chai.add(user_type<Quaternion>(), "AnnQuaternion");
		chai.add(constructor<Quaternion()>(), "AnnQuaternion");
		chai.add(constructor<Quaternion(float, float, float, float)>(), "AnnQuaternion");
		chai.add(constructor<Quaternion(Radian, Vector3)>(), "AnnQuaternion");
		chai.add(constructor<Quaternion(Vector3, Vector3, Vector3)>(), "AnnQuaternion");
		chai.add(fun(&Quaternion::x), "x");
		chai.add(fun(&Quaternion::y), "y");
		chai.add(fun(&Quaternion::z), "z");
		chai.add(fun(&Quaternion::w), "w");
		chai.add(fun([](const Quaternion& q, const Vector3 v) { return q * v; }), "*");
		chai.add(fun([](const Quaternion& q1, const Quaternion& q2) { return q1 * q2; }), "*");
		chai.add(fun([](const Quaternion& q, const Real& scalar) { return q * scalar; }), "*");
		chai.add(fun([](const Quaternion& q, size_t i) { return q[i]; }), "[]");
		chai.add(fun([](Quaternion& q1, const Quaternion& q2) { q1 = q2; }), "=");
		chai.add(fun([](const Quaternion& q1, const Quaternion& q2) { return q1 + q2; }), "+");
		chai.add(fun([](const Quaternion& q1, const Quaternion& q2) { return q1 - q2; }), "-");
		chai.add(fun([](const Quaternion& q) { return -q; }), "-");
		chai.add(fun([](const Quaternion& q1, const Quaternion& q2) { return q1 == q2; }), "==");
		chai.add(fun([](const Quaternion& q1, const Quaternion& q2) { return q1 != q2; }), "!=");
		chai.add(fun([](const Quaternion& q) { return q.xAxis(); }), "xAxis");
		chai.add(fun([](const Quaternion& q) { return q.yAxis(); }), "yAxis");
		chai.add(fun([](const Quaternion& q) { return q.zAxis(); }), "zAxis");
		chai.add(fun([](const Quaternion& q) { return q.getRoll(); }), "getRoll");
		chai.add(fun([](const Quaternion& q) { return q.getPitch(); }), "getPitch");
		chai.add(fun([](const Quaternion& q) { return q.getYaw(); }), "getYaw");
		chai.add(fun([](const Quaternion& q) { return q.isNaN(); }), "isNaN");
		chai.add(var(&Quaternion::ZERO), "AnnQuaternion_ZERO");
		chai.add(var(&Quaternion::IDENTITY), "AnnQuaternion_IDENTITY");

		chai.add(user_type<AnnGameObject>(), "AnnGameObject");
		chai.add(fun([](AnnGameObject* o, Vector3 v) { o->setPosition(v); }), "setPosition");
		chai.add(fun([](AnnGameObject* o, Quaternion q) { o->setOrientation(q); }), "setOrientation");
		chai.add(fun([](AnnGameObject* o, Vector3 v) { o->setScale(v); }), "setScale");
		chai.add(fun([](AnnGameObject* o) -> Vector3 { return o->getPosition(); }), "getPosition");
		chai.add(fun([](AnnGameObject* o) -> Quaternion { return o->getOrientation(); }), "getOrientation");
		chai.add(fun([](AnnGameObject* o) -> Vector3 { return o->getScale(); }), "getScale");
		chai.add(fun([](AnnGameObject* o, const string& s) { o->playSound(s); }), "playSound");
		chai.add(fun([](AnnGameObject* o, const string& s) { o->playSound(s, true); }), "playSoundLoop");
		chai.add(fun([](AnnGameObject* o) { return o->getName(); }), "getName");
		chai.add(fun([](AnnGameObject* o, const string& animName) { o->setAnimation(animName); }), "setAnimation");
		chai.add(fun([](AnnGameObject* o) { o->playAnimation(); }), "playAnimation");
		chai.add(fun([](AnnGameObject* o, bool play) { o->playAnimation(play); }), "playAnimation");
		chai.add(fun([](AnnGameObject* o) { o->loopAnimation(); }), "loopAnimation");
		chai.add(fun([](AnnGameObject* o, bool play) { o->loopAnimation(play); }), "loopAnimation");
		chai.add(fun([](AnnGameObject* o) { return o->getName(); }), "getName");

		chai.add(user_type<AnnLightObject>(), "AnnLightObject");
		chai.add(fun([](AnnLightObject* o, Vector3 v) { o->setPosition(v); }), "setPosition");
		chai.add(fun([](AnnLightObject* o, Vector3 v) { o->setDirection(v); }), "setDirection");
		chai.add(fun([](AnnLightObject* o, AnnColor c) { o->setDiffuseColor(c); }), "setDiffuseColor");
		chai.add(fun([](AnnLightObject* o, AnnColor c) { o->setSpecularColor(c); }), "setSpecularColor");
		chai.add(fun([](AnnLightObject* o, float lumens) { o->setPower(lumens); }), "setPower");
		chai.add(fun([](AnnLightObject* o) -> Vector3 { return o->getPosition(); }), "getPosition");
		chai.add(fun([](AnnLightObject* o) -> Vector3 { return o->getDirection(); }), "getDirection");
		chai.add(fun([](AnnLightObject* o) -> AnnColor { return o->getSpecularColor(); }), "getSpecularColor");
		chai.add(fun([](AnnLightObject* o) -> AnnColor { return o->getDiffuseColor(); }), "getDiffuseColor");
		chai.add(fun([](AnnLightObject* o) { return o->getName(); }), "getName");

		//Color
		chai.add(user_type<AnnColor>(), "AnnColor");
		chai.add(constructor<AnnColor(float, float, float, float)>(), "AnnColor");
		chai.add(constructor<AnnColor(const ColourValue&)>(), "AnnColor");
		chai.add(constructor<AnnColor(const AnnColor&)>(), "AnnColor");
		chai.add(fun([](AnnColor& c1, AnnColor& c2) { c1 = c2; }), "=");
		chai.add(fun([](AnnColor& color) { return color.getRed(); }), "getRed");
		chai.add(fun([](AnnColor& color) { return color.getGreen(); }), "getGreen");
		chai.add(fun([](AnnColor& color) { return color.getBlue(); }), "getBlue");
		chai.add(fun([](AnnColor& color) { return color.getAlpha(); }), "getAlpha");
		chai.add(fun([](AnnColor& color, float value) { return color.setRed(value); }), "setRed");
		chai.add(fun([](AnnColor& color, float value) { return color.setGreen(value); }), "setGreen");
		chai.add(fun([](AnnColor& color, float value) { return color.setBlue(value); }), "setBlue");
		chai.add(fun([](AnnColor& color, float value) { return color.setAlpha(value); }), "setAlpha");

		//Object getter
		chai.add(fun([](string id) { return AnnGetGameObjectManager()->getGameObject(id).get(); }), "AnnGetGameObject");
		chai.add(fun([](string id) { return AnnGetGameObjectManager()->getLightObject(id).get(); }), "AnnGetLightObject");

		//Level jumper
		chai.add(fun([](AnnLevelID id) { AnnGetLevelManager()->switchToLevel(id); }), "AnnJumpLevel");

		//Create a GameObject form ChaiScript
		chai.add(fun([](const string& mesh, const string& objectName) {
					 AnnGetLevelManager()->addToCurrentLevel(
						 AnnGetGameObjectManager()->createGameObject(mesh.c_str(), objectName));
				 }),
				 "AnnCreateGameObject");
		//Remove object
		chai.add(fun([](const string& objectName) {
					 auto obj = AnnGetGameObjectManager()->getGameObject(objectName);
					 if(!obj) return;
					 AnnGetGameObjectManager()->removeGameObject(obj);
					 AnnGetLevelManager()->removeFromCurrentLevel(obj);
				 }),
				 "AnnRemoveGameObject");

		//Change the gravity
		chai.add(fun([](const Vector3& gravity) { AnnGetPhysicsEngine()->changeGravity(gravity); }), "AnnChangeGravity");
		//Restore the default gravity vector
		chai.add(fun([]() { AnnGetPhysicsEngine()->resetGravity(); }), "AnnRestoreGravity");

		//Add the types of the event representation object
		chai.add(user_type<AnnKeyEvent>(), "AnnKeyEvent");
		chai.add(user_type<AnnMouseEvent>(), "AnnMouseEvent");
		chai.add(user_type<AnnControllerEvent>(), "AnnControllerEvent");
		chai.add(user_type<AnnTimeEvent>(), "AnnTimeEvent");
		chai.add(user_type<AnnTriggerEvent>(), "AnnTriggerEvent");
		chai.add(user_type<AnnHandControllerEvent>(), "AnnHandControllerEvent");
		chai.add(user_type<AnnMouseAxis>(), "AnnMouseAxis");
		chai.add(user_type<MouseAxisID>(), "MouseAxisID");
		chai.add(user_type<MouseButtonId>(), "MouseButtonId");
		chai.add(user_type<AnnControllerAxis>(), "AnnControllerAxis");
		chai.add(user_type<AnnControllerPov>(), "AnnControllerPov");
		chai.add(user_type<AnnTimerID>(), "AnnTimerID");
		chai.add(user_type<AnnCollisionEvent>(), "AnnCollisionEvent");
		chai.add(user_type<AnnPlayerCollisionEvent>(), "AnnPlayerCollisionEvent");

		chai.add(fun([](AnnKeyEvent e) { return e.isPressed(); }), "isPressed");
		chai.add(fun([](AnnKeyEvent e) { return e.isReleased(); }), "isReleased");
		chai.add(fun([](AnnKeyEvent e) { return e.getKey(); }), "getKey");

		chai.add(fun([](AnnMouseEvent e, /*MouseAxisID*/ const int a) { return e.getAxis(MouseAxisID(a)); }), "getAxis");
		chai.add(fun([](AnnMouseEvent e, /*MouseButtonId*/ const int b) { return e.getButtonState(MouseButtonId(b)); }), "getButtonState");
		chai.add(fun([](AnnMouseAxis a) { return a.getRelValue(); }), "getRelValue");
		chai.add(fun([](AnnMouseAxis a) { return a.getAbsValue(); }), "getAbsValue");

		chai.add(fun([](AnnControllerEvent e) { return e.getNbButtons(); }), "getNbButtons");
		chai.add(fun([](AnnControllerEvent e) { return e.getAxisCount(); }), "getAxisCount");
		chai.add(fun([](AnnControllerEvent e) { return e.getPovCount(); }), "getPovCount");
		chai.add(fun([](AnnControllerEvent e) { return e.getVendor(); }), "getVendor");
		chai.add(fun([](AnnControllerEvent e) { return e.getControllerID(); }), "getControllerID");
		chai.add(fun([](AnnControllerEvent e) { return e.isXboxController(); }), "isXboxController");
		chai.add(fun([](AnnControllerEvent e, const int i) { return e.isPressed(i); }), "isPressed");
		chai.add(fun([](AnnControllerEvent e, const int i) { return e.isReleased(i); }), "isReleased");
		chai.add(fun([](AnnControllerEvent e, const int i) { return e.isDown(i); }), "isDown");
		chai.add(fun([](AnnControllerEvent e, const int i) { return e.getAxis(i); }), "getAxis");
		chai.add(fun([](AnnControllerEvent e, const int i) { return e.getPov(i); }), "getPov");

		chai.add(fun([](AnnControllerPov pov) { return pov.getNorth(); }), "getNorth");
		chai.add(fun([](AnnControllerPov pov) { return pov.getSouth(); }), "getSouth");
		chai.add(fun([](AnnControllerPov pov) { return pov.getEast(); }), "getEast");
		chai.add(fun([](AnnControllerPov pov) { return pov.getWest(); }), "getWest");
		chai.add(fun([](AnnControllerPov pov) { return pov.getNorthEast(); }), "getNorthEast");
		chai.add(fun([](AnnControllerPov pov) { return pov.getNorthWest(); }), "getNorthWest");
		chai.add(fun([](AnnControllerPov pov) { return pov.getSouthEast(); }), "getSouthEast");
		chai.add(fun([](AnnControllerPov pov) { return pov.getSouthWest(); }), "getSouthWest");

		chai.add(fun([](AnnControllerAxis a) { return a.getAxisId(); }), "getAxisId");
		chai.add(fun([](AnnControllerAxis a) { return a.getRelValue(); }), "getRelValue");
		chai.add(fun([](AnnControllerAxis a) { return a.getAbsValue(); }), "getAbsValue");

		chai.add(fun([](AnnTimeEvent t) { return t.getID(); }), "getID");

		chai.add(fun([](AnnTriggerEvent e) { return e.getContactStatus(); }), "getContactStatus");
		chai.add(fun([](AnnTriggerEvent e) { return e.getSender(); }), "getSender");

		// TODO ISSUE the hand controller event interface is not finished
		chai.add(user_type<AnnHandController>(), "AnnHandController");
		chai.add(user_type<AnnHandControllerAxis>(), "AnnHandControllerAxis");
		chai.add(user_type<AnnHandController::AnnHandControllerSide>(), "AnnHandControllerSide");
		chai.add(user_type<AnnHandController::AnnHandControllerTypeHash>(), "AnnHandControllerTypeHash");
		chai.add(user_type<AnnHandController::AnnHandControllerGestureHash>(), "AnnHandControllerGestureHash");
		chai.add(var(AnnHandController::AnnHandControllerSide::leftHandController), "leftHandController");
		chai.add(var(AnnHandController::AnnHandControllerSide::rightHandController), "rightHandController");
		chai.add(var(AnnHandController::AnnHandControllerSide::invalidHandController), "invalidHandController");

		chai.add(fun([](AnnHandControllerEvent e) -> Vector3 { return e.getPosition(); }), "getPosition");
		chai.add(fun([](AnnHandControllerEvent e) -> Quaternion { return e.getOrientation(); }), "getOrientation");
		chai.add(fun([](AnnHandControllerEvent e) -> Vector3 { return e.getPointingDirection(); }), "getOrientation");
		chai.add(fun([](AnnHandControllerEvent e) -> Vector3 { return e.getLinearSpeed(); }), "getLinearSpeed");
		chai.add(fun([](AnnHandControllerEvent e) -> Vector3 { return e.getAngularSpeed(); }), "getAngularSpeed");
		chai.add(fun([](AnnHandControllerEvent e, const uint8_t id) { return e.getAxis(id); }), "getAxis");
		chai.add(fun([](AnnHandControllerEvent e) { return e.getAxisCount(); }), "getAxisCount");
		chai.add(fun([](AnnHandControllerEvent e) { return e.getButtonCount(); }), "getButtonCount");
		chai.add(fun([](AnnHandControllerEvent e, const uint8_t id) { return e.buttonPressed(id); }), "buttonPressed");
		chai.add(fun([](AnnHandControllerEvent e, const uint8_t id) { return e.buttonReleased(id); }), "buttonReleased");
		chai.add(fun([](AnnHandControllerEvent e, const uint8_t id) { return e.buttonState(id); }), "buttonState");
		chai.add(fun([](AnnHandControllerEvent e) { return e.getSide(); }), "getSide");
		chai.add(fun([](AnnHandControllerEvent e) { return e.getType(); }), "getType");

		chai.add(fun([](AnnPlayerCollisionEvent e) { return e.getObject(); }), "getObject");
		chai.add(fun([](AnnPlayerCollisionEvent e) { return e.getObject()->getName(); }), "getObjectName");

		chai.add(fun([](AnnCollisionEvent e) { return e.getA(); }), "getAObject");
		chai.add(fun([](AnnCollisionEvent e) { return e.getB(); }), "getBObject");
		chai.add(fun([](AnnCollisionEvent e) { return e.getA()->getName(); }), "getAObjectName");
		chai.add(fun([](AnnCollisionEvent e) { return e.getB()->getName(); }), "getBObjectName");
		chai.add(fun([](AnnCollisionEvent e) -> Vector3 { return e.getPosition(); }), "getPosition");
		chai.add(fun([](AnnCollisionEvent e) -> Vector3 { return e.getNormal(); }), "getNormal");
		chai.add(fun([](AnnCollisionEvent e) { return e.isCeilingCollision(); }), "isCeilingCollision");
		chai.add(fun([](AnnCollisionEvent e) { return e.isGroundCollision(); }), "isGroundCollision");
		chai.add(fun([](AnnCollisionEvent e) { return e.isWallCollision(); }), "isWallCollision");

		//There's capacitive touch surfaces and haptic feedback that aren't available right now on the AnnHandController class

		//Register an accessors to the engine's log
		chai.add(fun([](const string& s) { AnnDebug(Log::Important) << logFromScript << s; }), "AnnDebugLog");
		chai.add(fun([](const Vector3& s) { AnnDebug(Log::Important) << logFromScript << s; }), "AnnDebugLog");
		chai.add(fun([](const Vector2& s) { AnnDebug(Log::Important) << logFromScript << s; }), "AnnDebugLog");
		chai.add(fun([](const Quaternion& s) { AnnDebug(Log::Important) << logFromScript << s; }), "AnnDebugLog");
		chai.add(fun([](const Radian& s) { AnnDebug(Log::Important) << logFromScript << s; }), "AnnDebugLog");
		chai.add(fun([](const Degree& s) { AnnDebug(Log::Important) << logFromScript << s; }), "AnnDebugLog");
		chai.add(fun([](const AnnColor& s) { AnnDebug(Log::Important) << logFromScript << s; }), "AnnDebugLog");
		chai.add(fun([](KeyCode::code c) { AnnDebug(Log::Important) << logFromScript << "keycode:" << c; }), "AnnDebugLog");
		chai.add(fun([](MouseAxisID c) { AnnDebug(Log::Important) << logFromScript << "mouseAxis:" << c; }), "AnnDebugLog");
		chai.add(fun([](bool b) {string s("true"); if (!b) { s = "false"; } AnnDebug(Log::Important) << logFromScript << "bool:" << s; }), "AnnDebugLog");
		chai.add(fun([](int i) { AnnDebug(Log::Important) << logFromScript << "int:" << i; }), "AnnDebugLog");
		chai.add(fun([](float f) { AnnDebug(Log::Important) << logFromScript << "float:" << f; }), "AnnDebugLog");

		///Clear the console
		chai.add(fun([]() { AnnGetOnScreenConsole()->bufferClear(); }), "AnnClearConsole");
		chai.add(fun([]() { AnnEngine::setProcessPriorityHigh(); }), "AnnSetProcessPriorityHigh");
		chai.add(fun([]() { AnnEngine::setProcessPriorityNormal(); }), "AnnSetProcessPriorityNormal");

		chai.add(fun([]() { AnnGetEngine()->requestQuit(); }), "AnnQuit");
		chai.add(fun([](const float& multiplier) { AnnGetPhysicsEngine()->setDebugDrawerColorMultiplier(multiplier); }), "AnnSetDebugDrawerColorMultiplier");
		chai.add(fun([](const float& ev, const float& minEv, const float& maxEv) { AnnGetSceneryManager()->setExposure(ev, minEv, maxEv); }), "AnnSetExposure");
		chai.add(fun([](const float& threshold) { AnnGetSceneryManager()->setBloomThreshold(threshold); }), "AnnSetBloomThreshold");
		chai.add(fun([](AnnColor& color, float& multiplier) { AnnGetSceneryManager()->setSkyColor(color, multiplier); }), "AnnSetSkyColor");
		chai.add(fun([](const AnnColor& ucolor, const float umul, const AnnColor& lcolor, const float lmul, const Vector3& dir, const float envMapScaling) { AnnGetSceneryManager()->setAmbientLight(ucolor, umul, lcolor, lmul, dir, envMapScaling); }), "AnnSetAmbientLight");

		chai.add(fun([](int AA) { AnnOgreVRRenderer::setAntiAliasingLevel(uint8_t(AA)); }), "AnnSetAA");
	}
	catch(const chaiscript::exception::name_conflict_error& e)
	{
		throw AnnInitializationError(ANN_ERR_NOTINIT, "Cannot initialize script manager. Trhowed exception when binding APIs:\n" + string(e.what()));
	}
}

void AnnScriptManager::tryAndGetEventHooks()
{
	//Forgive me.
	try
	{
		callKeyEventOnScriptInstance = chai.eval<std::function<void(chaiscript::Boxed_Value&, AnnKeyEvent)>>("KeyEvent");
	}
	catch(const chaiscript::exception::eval_error&)
	{
		callKeyEventOnScriptInstance = nullptr;
	}

	//Yes. I'm doing this.
	try
	{
		callMouseEventOnScriptInstance = chai.eval<std::function<void(chaiscript::Boxed_Value&, AnnMouseEvent)>>("MouseEvent");
	}
	catch(const chaiscript::exception::eval_error&)
	{
		callMouseEventOnScriptInstance = nullptr;
	}

	//Yes, there's 6 of them
	try
	{
		callStickEventOnScriptInstance = chai.eval<std::function<void(chaiscript::Boxed_Value&, AnnControllerEvent)>>("ControllerEvent");
	}
	catch(const chaiscript::exception::eval_error&)
	{
		callStickEventOnScriptInstance = nullptr;
	}

	//And yes, it's probable that something will be thrown, unless a script already have this function
	try
	{
		callTimeEventOnScriptInstance = chai.eval<std::function<void(chaiscript::Boxed_Value&, AnnTimeEvent)>>("TimeEvent");
	}
	catch(const chaiscript::exception::eval_error&)
	{
		callTimeEventOnScriptInstance = nullptr;
	}

	//And it's also possible than the result is not usable with this script and will throw later at eval time
	try
	{
		callTriggerEventOnScriptInstance = chai.eval<std::function<void(chaiscript::Boxed_Value&, AnnTriggerEvent)>>("TriggerEvent");
	}
	catch(const chaiscript::exception::eval_error&)
	{
		callTriggerEventOnScriptInstance = nullptr;
	}

	//Like I said. Please forgive me.
	try
	{
		callHandControllertOnScriptInstance = chai.eval<std::function<void(chaiscript::Boxed_Value&, AnnHandControllerEvent)>>("HandControllerEvent");
	}
	catch(const chaiscript::exception::eval_error&)
	{
		callHandControllertOnScriptInstance = nullptr;
	}
	try
	{
		callCollisionEventOnScriptInstance = chai.eval<std::function<void(chaiscript::Boxed_Value&, AnnCollisionEvent)>>("CollisionEvent");
	}
	catch(const chaiscript::exception::eval_error&)
	{
		callCollisionEventOnScriptInstance = nullptr;
	}
	try
	{
		callPlayerCollisionEventOnScriptInstance = chai.eval<std::function<void(chaiscript::Boxed_Value&, AnnPlayerCollisionEvent)>>("PlayerCollisionEvent");
	}
	catch(const chaiscript::exception::eval_error&)
	{
		callPlayerCollisionEventOnScriptInstance = nullptr;
	}
}

bool AnnScriptManager::evalFile(const std::string& file)
{
	try
	{
		chai.eval_file(file);
	}
	catch(const chaiscript::exception::file_not_found_error& fnfe)
	{
		AnnDebug() << fileErrorPrefix << fnfe.what();
		return false;
	}
	catch(const chaiscript::exception::eval_error& ee)
	{
		AnnDebug() << fileErrorPrefix << ee.pretty_print();
		return false;
	}
	return true;
}

AnnScriptManager::AnnScriptID AnnScriptManager::ID{ 0 };

std::shared_ptr<AnnBehaviorScript> AnnScriptManager::getBehaviorScript(const std::string& scriptName, AnnGameObject* owner)
{
	auto file{ scriptName + scriptExtension };

	try
	{
		//Evaluate the file containing the script class if unknown to ChaiScript yet

		auto rawScript = scriptFileManager->getResourceByName(file).staticCast<AnnScriptFile>();
		if(!rawScript)
		{
			rawScript = scriptFileManager->load(file, AnnResourceManager::getDefaultResourceGroupName());
			if(!rawScript)
				throw chaiscript::exception::file_not_found_error(file);
		}

		if(!rawScript->loadedInChaiscriptInterpretor())
		{
			AnnDebug() << "now loading " << file << " into the script manager";
			rawScript->signalLoadedInChaiscript();
			chai.eval(rawScript->getSourceCode());
		}

		//Increment ID
		ID++;

		//Get the name of the owner of this script, if relevant;
		std::string ownerTag = owner ? owner->getName() : "";

		//This may looks odd but it's good enough for what we're doing:
		//Copy the template of the init code to a string
		std::string ChaiCode{ scriptTemplate };

		//To "boot" the script, there's a little sniped of ChaiScript that is run from the C++ side. This code is generated from a string,
		//And contains a few fixed tags to be replaced with the script name and an unique ID
		ChaiCode.replace(ChaiCode.find(std::string(scriptNameMarker)), nameMarkerLen, scriptName);
		ChaiCode.replace(ChaiCode.find(std::string(scriptNameMarker)), nameMarkerLen, scriptName);
		ChaiCode.replace(ChaiCode.find(std::string(scriptObjectID)), scriptIDMarkerLen, std::to_string(ID));
		ChaiCode.replace(ChaiCode.find(std::string(scriptObjectID)), scriptIDMarkerLen, std::to_string(ID));
		ChaiCode.replace(ChaiCode.find(std::string(scriptObjectID)), scriptIDMarkerLen, std::to_string(ID));

		//This is the ugly bit, this will try to see if the methods functions have been declared somewhere. Note that this doesn't tell if a script has a specific method implemented.
		//It just permit to know if "a function" with that name exist. Script themselve will deal with knowing if they own theses functions, by attempting to call them, and setting flags
		//if exception occurs.
		tryAndGetEventHooks();

		//This will add a global function in ChaiScript, that will create and return the script instance
		chai.eval(ChaiCode);
		//Get a way to call this function
		auto creatorFunction = chai.eval<std::function<chaiscript::Boxed_Value(std::string)>>("create" + scriptName + std::to_string(ID));

		//Now we need to get some hook to call the update on the file
		return std::make_shared<AnnBehaviorScript>(
			scriptName,
			//Function to call to update the script. Update is mandatory
			chai.eval<std::function<void(chaiscript::Boxed_Value&)>>("update"),

			//Eventual event hooks
			tie(
				callKeyEventOnScriptInstance,
				callMouseEventOnScriptInstance,
				callStickEventOnScriptInstance,
				callTimeEventOnScriptInstance,
				callTriggerEventOnScriptInstance,
				callHandControllertOnScriptInstance,
				callCollisionEventOnScriptInstance,
				callPlayerCollisionEventOnScriptInstance),
			//This return the ScriptInstance, as a Boxed_Value. We're only interested at calling something on
			//this object, so don't need to try to unbox it. It's literally a black box for us
			creatorFunction(ownerTag));
	}

	catch(const chaiscript::exception::file_not_found_error& fnfe)
	{
		AnnDebug(Log::Important) << "Cannot find behavior script " << file;
		AnnDebug(Log::Important) << fileErrorPrefix << fnfe.what();
	}
	catch(const chaiscript::exception::eval_error& ee)
	{
		AnnDebug(Log::Important) << "Error during evaluation of behavior script " << file;
		AnnDebug(Log::Important) << ee.pretty_print();
	}

	//The user should test if this script is "valid" or not. And should not do it in a loop, obviously
	return std::make_shared<AnnBehaviorScript>();
}

AnnBehaviorScript::AnnBehaviorScript() :
 valid(false),
 cannotKey(false),
 cannotMouse(false),
 cannotStick(false),
 cannotTime(false),
 cannotTrigger(false),
 cannotHand(false),
 cannotCollision{ false },
 cannotPlayerCollision{ false }
{
	AnnDebug() << "Invalid script object created";
}

AnnBehaviorScript::AnnBehaviorScript(const std::string& scriptName,
									 std::function<void(chaiscript::Boxed_Value&)> updateHook,
									 AnnBehaviorScriptHooks hooks,
									 chaiscript::Boxed_Value scriptObjectInstance) :
 constructListener(),
 valid{ true },
 name{ scriptName },
 ScriptObjectInstance{ scriptObjectInstance },
 callUpdateOnScriptInstance{ updateHook },
 callKeyEventOnScriptInstance{ std::get<KeyHook>(hooks) },
 callMouseEventOnScriptInstance{ std::get<MouseHook>(hooks) },
 callStickEventOnScriptInstance{ std::get<ControllerHook>(hooks) },
 callTimeEventOnScriptInstance{ std::get<TimeHook>(hooks) },
 callTriggerEventOnScriptInstance{ std::get<TriggerHook>(hooks) },
 callHandControllertOnScriptInstance{ std::get<HandHook>(hooks) },
 callCollisionEventOnScriptInstance{ std::get<CollisionHook>(hooks) },
 callPlayerCollisionEventOnScriptInstance{ std::get<PlayerCollisionHook>(hooks) },
 cannotKey{ false },
 cannotMouse{ false },
 cannotStick{ false },
 cannotTime{ false },
 cannotTrigger{ false },
 cannotHand{ false },
 cannotCollision{ false },
 cannotPlayerCollision{ false }
{
}

AnnBehaviorScript::~AnnBehaviorScript()
{
	AnnDebug() << "Destructing " << name << "Script";
}

void AnnBehaviorScript::update()
{
	try
	{
		callUpdateOnScript();
	}
	catch(const chaiscript::exception::eval_error& ee)
	{
		AnnDebug() << "Evaluation error while "
				   << name
				   << " script update - "
				   << ee.pretty_print();
		//will not crash here.
	}
}

bool AnnBehaviorScript::isValid() const
{
	return valid;
}

void AnnBehaviorScript::registerAsListener()
{
	AnnGetEventManager()->addListener(getSharedListener());
}

void AnnBehaviorScript::unregisterAsListener()
{
	AnnDebug() << "Unregistering ourself has event listener";
	AnnGetEventManager()->removeListener(getSharedListener());
}

void AnnBehaviorScript::KeyEvent(AnnKeyEvent e)
{
	try
	{
		if(callKeyEventOnScriptInstance && !cannotKey)
			callKeyEventOnScriptInstance(ScriptObjectInstance, e);
	}
	catch(const chaiscript::exception::dispatch_error&)
	{
		cannotKey = true;
	}
	catch(const chaiscript::exception::eval_error& ee)
	{
		AnnDebug(Log::Important) << "Event script error " << ee.pretty_print();
	}
}

void AnnBehaviorScript::MouseEvent(AnnMouseEvent e)
{
	try
	{
		if(callMouseEventOnScriptInstance && !cannotMouse)
			callMouseEventOnScriptInstance(ScriptObjectInstance, e);
	}
	catch(const chaiscript::exception::dispatch_error&)
	{
		cannotMouse = true;
	}
	catch(const chaiscript::exception::eval_error& ee)
	{
		AnnDebug(Log::Important) << "Event script error " << ee.pretty_print();
	}
}

void AnnBehaviorScript::ControllerEvent(AnnControllerEvent e)
{
	try
	{
		if(callStickEventOnScriptInstance && !cannotStick)
			callStickEventOnScriptInstance(ScriptObjectInstance, e);
	}
	catch(const chaiscript::exception::dispatch_error&)
	{
		cannotStick = true;
	}
	catch(const chaiscript::exception::eval_error& ee)
	{
		AnnDebug(Log::Important) << "Event script error " << ee.pretty_print();
	}
}

void AnnBehaviorScript::TimeEvent(AnnTimeEvent e)
{
	try
	{
		if(callTimeEventOnScriptInstance && !cannotTime)
			callTimeEventOnScriptInstance(ScriptObjectInstance, e);
	}
	catch(const chaiscript::exception::dispatch_error&)
	{
		cannotTime = true;
	}
	catch(const chaiscript::exception::eval_error& ee)
	{
		AnnDebug(Log::Important) << "Event script error " << ee.pretty_print();
	}
}

void AnnBehaviorScript::TriggerEvent(AnnTriggerEvent e)
{
	try
	{
		if(callTriggerEventOnScriptInstance && !cannotTrigger)
			callTriggerEventOnScriptInstance(ScriptObjectInstance, e);
	}
	catch(const chaiscript::exception::dispatch_error&)
	{
		cannotTrigger = true;
	}
	catch(const chaiscript::exception::eval_error& ee)
	{
		AnnDebug(Log::Important) << "Event script error " << ee.pretty_print();
	}
}

void AnnBehaviorScript::HandControllerEvent(AnnHandControllerEvent e)
{
	try
	{
		if(callHandControllertOnScriptInstance && !cannotHand)
			callHandControllertOnScriptInstance(ScriptObjectInstance, e);
	}
	catch(const chaiscript::exception::dispatch_error&)
	{
		cannotHand = true;
	}
	catch(const chaiscript::exception::eval_error& ee)
	{
		AnnDebug(Log::Important) << "Event script error " << ee.pretty_print();
	}
}

void AnnBehaviorScript::CollisionEvent(AnnCollisionEvent e)
{
	try
	{
		if(callCollisionEventOnScriptInstance && !cannotCollision)
			callCollisionEventOnScriptInstance(ScriptObjectInstance, e);
	}
	catch(const chaiscript::exception::dispatch_error&)
	{
		cannotCollision = true;
	}
	catch(const chaiscript::exception::eval_error& ee)
	{
		AnnDebug(Log::Important) << "Event script error " << ee.pretty_print();
	}
}
void AnnBehaviorScript::PlayerCollisionEvent(AnnPlayerCollisionEvent e)
{
	//AnnDebug() << "player collision on script...";
	try
	{
		if(callPlayerCollisionEventOnScriptInstance && !cannotPlayerCollision)
			callPlayerCollisionEventOnScriptInstance(ScriptObjectInstance, e);
	}
	catch(const chaiscript::exception::dispatch_error&)
	{
		cannotPlayerCollision = true;
	}
	catch(const chaiscript::exception::eval_error& ee)
	{
		AnnDebug(Log::Important) << "Event script error " << ee.pretty_print();
	}
}

void AnnScriptManager::evalString(const std::string& chaiCode)
{
	chai.eval(chaiCode);
}

void AnnScriptManager::registerResourceManager()
{
	scriptFileManager = OGRE_NEW AnnScriptFileResourceManager;
}

void AnnScriptManager::unregisterResourceManager()
{
	OGRE_DELETE scriptFileManager;
	scriptFileManager = nullptr;
}

AnnScriptManager::~AnnScriptManager()
{
	unregisterResourceManager();
}

chaiscript::ChaiScript* AnnScriptManager::_getEngine()
{
	return &chai;
}
