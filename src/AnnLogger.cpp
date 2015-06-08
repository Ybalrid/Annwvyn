#include "stdafx.h"
#include "AnnLogger.hpp"

using namespace Annwvyn;

AnnDebug::AnnDebug() : std::ostream(new AnnDebugBuff()){}
AnnDebug::AnnDebug(const std::string& message) : std::ostream(new AnnDebugBuff()) {(*this) << message;}
AnnDebug::~AnnDebug() {delete rdbuf();}