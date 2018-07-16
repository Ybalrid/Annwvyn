// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "AnnEventListener.hpp"
#include "AnnGetter.hpp"

using namespace Annwvyn;

AnnEventListener::~AnnEventListener()
{
}

AnnEventListener::AnnEventListener() :
 player(AnnGetPlayer().get())
{
}

float AnnEventListener::trim(float v, float dz)
{
	//The test is done on the abs value. Return the actual value, or 0 if under the dead-zone
	if(abs(v) >= dz) return v;
	return 0.0f;
}

std::shared_ptr<AnnEventListener> AnnEventListener::getSharedListener()
{
	return shared_from_this();
}
