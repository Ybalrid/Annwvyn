/**
\file Euler.h
\brief %Euler class for %Ogre
\details License: Do whatever you want with it.
\version 2.1
\author Kojack
\author Transporter
*/

#pragma once

#include "systemMacro.h"

#include <Ogre.h>

namespace Ogre
{
	/**
	\class Euler
	\brief Class for %Euler rotations

	<table><tr><td>Yaw is a rotation around the Y axis.</td><td>Pitch is a rotation around the X axis.</td><td>Roll is a rotation around the Z axis.</td></tr>
	<tr><td><img src="http://www.ogre3d.org/tikiwiki/tiki-download_file.php?fileId=2112" /></td><td><img src="http://www.ogre3d.org/tikiwiki/tiki-download_file.php?fileId=2113" /></td><td><img src="http://www.ogre3d.org/tikiwiki/tiki-download_file.php?fileId=2114" /></td></tr></table>
	*/
	class AnnDllExport Euler
	{
	public:
		/// Default constructor.
		Euler() :
		 mYaw(Radian(0.0f)), mPitch(Radian(0.0f)), mRoll(Radian(0.0f)), mChanged(true)
		{
		}

		/**
		\brief Constructor which takes yaw, pitch and roll values.
		\param y Starting value for yaw
		\param p Starting value for pitch
		\param r Starting value for roll
		*/
		Euler(Radian y, Radian p = Radian(0.0f), Radian r = Radian(0.0f)) :
		 mYaw(y), mPitch(p), mRoll(r), mChanged(true)
		{
		}

		/**
		\brief Constructor which takes yaw, pitch and roll values as ints (degrees).
		\param y Starting value for yaw [degree]
		\param p Starting value for pitch [degree]
		\param r Starting value for roll [degree]
		*/
		Euler(int y, int p = 0, int r = 0) :
		 mYaw(Degree(Real(y))), mPitch(Degree(Real(p))), mRoll(Degree(Real(r))), mChanged(true)
		{
		}

		/**
		\brief Constructor which takes yaw, pitch and roll values as reals (radians).
		\param y Starting value for yaw [radian]
		\param p Starting value for pitch [radian]
		\param r Starting value for roll [radian]
		*/
		Euler(Real y, Real p = 0.0f, Real r = 0.0f) :
		 mYaw(Radian(y)), mPitch(Radian(p)), mRoll(Radian(r)), mChanged(true)
		{
		}

		/**
		\brief Default constructor with presets.
		\param axvec Starting values as a combined vector
		*/
		Euler(Vector3 axvec) :
		 mYaw(Radian(axvec.y)), mPitch(Radian(axvec.x)), mRoll(Radian(axvec.z)), mChanged(true)
		{
		}

		/**
		\brief Copy constructor
		\param copyFrom Euler vector the new one is copied from
		*/
		Euler(const Euler& copyFrom) :
		 mYaw(copyFrom.mYaw), mPitch(copyFrom.mPitch), mRoll(copyFrom.mRoll), mCachedQuaternion(copyFrom.mCachedQuaternion), mChanged(copyFrom.mChanged)
		{
		}

		/**
		\brief Default constructor with presets.
		\param quaternion Calculate starting values from this quaternion
		*/
		explicit Euler(Quaternion quaternion)
		{
			fromQuaternion(quaternion);
		}

		/// Get the Yaw angle.
		inline Radian getYaw() const { return mYaw; }

		/// Get the Pitch angle.
		inline Radian getPitch() const { return mPitch; }

		/// Get the Roll angle.
		inline Radian getRoll() const { return mRoll; }

		/**
		\brief Set the yaw.
		\param y New value for yaw
		*/
		inline Euler& setYaw(Radian y)
		{
			mYaw	 = y;
			mChanged = true;
			return *this;
		}

		/**
		\brief Set the pitch.
		\param p New value for pitch
		*/
		inline Euler& setPitch(Radian p)
		{
			mPitch   = p;
			mChanged = true;
			return *this;
		}

		/**
		\brief Set the roll.
		\param r New value for roll
		*/
		inline Euler& setRoll(Radian r)
		{
			mRoll	= r;
			mChanged = true;
			return *this;
		}

		/**
		\brief Set all rotations at once.
		\param y New value for yaw
		\param p New value for pitch
		\param r New value for roll
		*/
		inline Euler& setRotation(Radian y, Radian p, Radian r)
		{
			mYaw	 = y;
			mPitch   = p;
			mRoll	= r;
			mChanged = true;
			return *this;
		}

		/**
		\brief Apply a relative yaw.
		\param y Angle to add on current yaw
		*/
		inline Euler& yaw(Radian y)
		{
			mYaw += y;
			mChanged = true;
			return *this;
		}

		/**
		\brief Apply a relative pitch.
		\param p Angle to add on current pitch
		*/
		inline Euler& pitch(Radian p)
		{
			mPitch += p;
			mChanged = true;
			return *this;
		}

		/**
		\brief Apply a relative roll.
		\param r Angle to add on current roll
		*/
		inline Euler& roll(Radian r)
		{
			mRoll += r;
			mChanged = true;
			return *this;
		}

		/**
		\brief Apply all relative rotations at once.
		\param y Angle to add on current yaw
		\param p Angle to add on current pitch
		\param r Angle to add on current roll
		*/
		inline Euler& rotate(Radian y, Radian p, Radian r)
		{
			mYaw += y;
			mPitch += p;
			mRoll += r;
			mChanged = true;
			return *this;
		}

		/// Get a vector pointing forwards.
		inline Vector3 getForward() { return toQuaternion() * Vector3::NEGATIVE_UNIT_Z; }

		/// Get a vector pointing to the right.
		inline Vector3 getRight() { return toQuaternion() * Vector3::UNIT_X; }

		/// Get a vector pointing up.
		inline Vector3 getUp() { return toQuaternion() * Vector3::UNIT_Y; }

		/**
		\brief Calculate the quaternion of the Euler object.
		\details The result is cached, it is only recalculated when the component Euler angles are changed.
		*/
		inline Quaternion toQuaternion()
		{
			if(mChanged)
			{
				mCachedQuaternion = Quaternion(mYaw, Vector3::UNIT_Y) * Quaternion(mPitch, Vector3::UNIT_X) * Quaternion(mRoll, Vector3::UNIT_Z);
				mChanged		  = false;
			}
			return mCachedQuaternion;
		}

		/// Casting operator. This allows any ogre function that wants a Quaternion to accept a Euler instead.
		inline operator Quaternion()
		{
			return toQuaternion();
		}

		/**
		\brief Calculate the current Euler of a given quaternion object.
		\param quaternion Quaternion which is used to calculate current euler angles
		*/
		inline void fromQuaternion(Quaternion quaternion)
		{
			mPitch = Math::ATan2(2 * quaternion.y * quaternion.w - 2 * quaternion.x * quaternion.z, 1 - 2 * Math::Pow(quaternion.y, 2) - 2 * Math::Pow(quaternion.z, 2));
			mRoll  = Math::ASin(2 * quaternion.x * quaternion.y + 2 * quaternion.z * quaternion.w);
			mYaw   = Math::ATan2(2 * quaternion.x * quaternion.w - 2 * quaternion.y * quaternion.z, 1 - 2 * Math::Pow(quaternion.x, 2) - 2 * Math::Pow(quaternion.z, 2));

			if(quaternion.x * quaternion.y + quaternion.z * quaternion.w == 0.5)
			{
				mPitch = 2.0f * Math::ATan2(quaternion.x, quaternion.w);
				mYaw   = 0.0f;
			}
			else if(quaternion.x * quaternion.y + quaternion.z * quaternion.w == -0.5)
			{
				mPitch = -2.0f * Math::ATan2(quaternion.x, quaternion.w);
				mYaw   = 0.0f;
			}

			mCachedQuaternion = quaternion;
			mChanged		  = false;
		}

		/**
		\brief Set the yaw and pitch to face in the given direction.
		\details The direction doesn't need to be normalised. Roll is always unaffected.
		\param setYaw If false, the yaw isn't changed.
		\param setPitch If false, the pitch isn't changed.
		*/
		inline Euler& setDirection(const Vector3& v, bool setYaw = true, bool setPitch = true)
		{
			auto d(v.normalisedCopy());
			if(setPitch)
				mPitch = Math::ASin(d.y);
			if(setYaw)
				mYaw = Math::ATan2(-d.x, -d.z); //+Ogre::Math::PI/2.0;
			mChanged = setYaw || setPitch;

			return *this;
		}

		/**
		\brief Normalise the selected rotations to be within the +/-180 degree range.
		\details The normalise uses a wrap around, so for example a yaw of 360 degrees becomes 0 degrees, and -190 degrees becomes 170.
		\param normYaw If false, the yaw isn't normalized.
		\param normPitch If false, the pitch isn't normalized.
		\param normRoll If false, the roll isn't normalized.
		*/
		inline void normalise(bool normYaw = true, bool normPitch = true, bool normRoll = true)
		{
			if(normYaw)
			{
				auto yaw = mYaw.valueRadians();
				if(yaw < -Math::PI)
				{
					yaw = fmod(yaw, Math::PI * 2.0f);
					if(yaw < -Math::PI)
					{
						yaw += Math::PI * 2.0f;
					}
					mYaw	 = yaw;
					mChanged = true;
				}
				else if(yaw > Math::PI)
				{
					yaw = fmod(yaw, Math::PI * 2.0f);
					if(yaw > Math::PI)
					{
						yaw -= Math::PI * 2.0f;
					}
					mYaw	 = yaw;
					mChanged = true;
				}
			}
			if(normPitch)
			{
				auto pitch = mPitch.valueRadians();
				if(pitch < -Math::PI)
				{
					pitch = fmod(pitch, Math::PI * 2.0f);
					if(pitch < -Math::PI)
					{
						pitch += Math::PI * 2.0f;
					}
					mPitch   = pitch;
					mChanged = true;
				}
				else if(pitch > Math::PI)
				{
					pitch = fmod(pitch, Math::PI * 2.0f);
					if(pitch > Math::PI)
					{
						pitch -= Math::PI * 2.0f;
					}
					mPitch   = pitch;
					mChanged = true;
				}
			}
			if(normRoll)
			{
				auto roll = mRoll.valueRadians();
				if(roll < -Math::PI)
				{
					roll = fmod(roll, Math::PI * 2.0f);
					if(roll < -Math::PI)
					{
						roll += Math::PI * 2.0f;
					}
					mRoll	= roll;
					mChanged = true;
				}
				else if(roll > Math::PI)
				{
					roll = fmod(roll, Math::PI * 2.0f);
					if(roll > Math::PI)
					{
						roll -= Math::PI * 2.0f;
					}
					mRoll	= roll;
					mChanged = true;
				}
			}
		}

		/**
		\brief Return the relative euler angles required to rotate from the current forward direction to the specified dir vector.
		\details The result euler can then be added to the current euler to immediately face dir.
		The rotation won't flip upside down then roll instead of a 180 degree yaw.
		\param setYaw If false, the angle is set to 0. If true, the angle is calculated.
		\param setPitch If false, the angle is set to 0. If true, the angle is calculated.
		\param shortest If false, the full value of each angle is used. If true, the angles are normalised and the shortest
		rotation is found to face the correct direction. For example, when false a yaw of 1000 degrees and a dir of
		(0,0,-1) will return a -1000 degree yaw. When true, the same yaw and dir would give 80 degrees (1080 degrees faces
		the same way as (0,0,-1).
		*/
		inline Euler getRotationTo(const Vector3& dir, bool setYaw = true, bool setPitch = true, bool shortest = true) const
		{
			Euler t1;
			t1.setDirection(dir, setYaw, setPitch);
			auto t2 = t1 - *this;
			if(shortest && setYaw)
			{
				t2.normalise();
			}
			return t2;
		}

		/// Clamp the yaw angle to a range of +/-limit.
		inline Euler& limitYaw(const Radian& limit)
		{
			if(mYaw > limit)
			{
				mYaw	 = limit;
				mChanged = true;
			}
			else if(mYaw < -limit)
			{
				mYaw	 = -limit;
				mChanged = true;
			}
			return *this;
		}

		/// Clamp the pitch angle to a range of +/-limit.
		inline Euler& limitPitch(const Radian& limit)
		{
			if(mPitch > limit)
			{
				mPitch   = limit;
				mChanged = true;
			}
			else if(mPitch < -limit)
			{
				mPitch   = -limit;
				mChanged = true;
			}
			return *this;
		}

		/// Clamp the roll angle to a range of +/-limit.
		inline Euler& limitRoll(const Radian& limit)
		{
			if(mRoll > limit)
			{
				mRoll	= limit;
				mChanged = true;
			}
			else if(mRoll < -limit)
			{
				mRoll	= -limit;
				mChanged = true;
			}
			return *this;
		}

		/// Stream operator, for printing the euler component angles to a stream
		inline friend std::ostream& operator<<(std::ostream& o, const Euler& e)
		{
			o << "<Y:" << e.mYaw << ", P:" << e.mPitch << ", R:" << e.mRoll << ">";
			return o;
		}

		/// Add two euler objects.
		inline Euler operator+(const Euler& rhs) const { return Euler(mYaw + rhs.mYaw, mPitch + rhs.mPitch, mRoll + rhs.mRoll); }

		/// Add a vector with radiant euler angles.
		inline Euler operator+(const Vector3& angles) const { return Euler(mYaw + Radian(angles.y), mPitch + Radian(angles.x), mRoll + Radian(angles.z)); }

		/**
		\brief Subtract two euler objects.
		\details This finds the difference as relative angles.
		*/
		inline Euler operator-(const Euler& rhs) const { return Euler(mYaw - rhs.mYaw, mPitch - rhs.mPitch, mRoll - rhs.mRoll); }

		/// Subtract a vector with radiant euler angles.
		inline Euler operator-(const Vector3& angles) const { return Euler(mYaw - Radian(angles.y), mPitch - Radian(angles.x), mRoll - Radian(angles.z)); }

		/// Interpolate the euler angles by rhs.
		inline Euler operator*(Real rhs) const { return Euler(mYaw * rhs, mPitch * rhs, mRoll * rhs); }

		/// Interpolate the euler angle by lhs.
		inline friend Euler operator*(Real lhs, const Euler& rhs) { return Euler(lhs * rhs.mYaw, lhs * rhs.mPitch, lhs * rhs.mRoll); }

		/**
		\brief Multiply two eulers.
		\details This has the same effect as multiplying quaternions.
		\returns The result is a quaternion.
		*/
		inline Quaternion operator*(const Euler& rhs) const
		{
			auto e1(*this), e2(rhs);
			return e1.toQuaternion() * e2.toQuaternion();
		}

		/// Apply the euler rotation to the vector rhs.
		inline Vector3 operator*(const Vector3& rhs) { return toQuaternion() * rhs; }

		/// Copy assignment operator (Euler)
		inline Euler operator=(Euler src)
		{
			setRotation(src.getYaw(), src.getPitch(), src.getRoll());
			return *this;
		}

		/// Copy assignment operator (Quaternion)
		//inline Euler& operator=(Ogre::Quaternion quaternion)
		//{
		//	fromQuaternion(quaternion);
		//	return *this;
		//}

		/// Copy assignment operator (Vector3)
		inline Euler operator=(Vector3 pyr)
		{
			setRotation(Radian(pyr.y), Radian(pyr.x), Radian(pyr.z));
			return *this;
		}

	protected:
		Radian mYaw;				  //!< Rotation around the Y axis.
		Radian mPitch;				  //!< Rotation around the X axis.
		Radian mRoll;				  //!< Rotation around the Z axis.
		Quaternion mCachedQuaternion; //!< Cached quaternion equivalent of this Euler object.
		bool mChanged;				  //!< Is the cached quaternion out of date?
	};
}
