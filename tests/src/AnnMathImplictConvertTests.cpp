#include "stdafx.h"
#include "engineBootstrap.hpp"

namespace Annwvyn
{

    TEST_CASE("Vector convert")
    {
        AnnVect3 vect(1,2,3);

        btVector3 physicsVector(vect);

        REQUIRE(vect.x == physicsVector.x());
        REQUIRE(vect.y == physicsVector.y());
        REQUIRE(vect.z == physicsVector.z());


        btVector3 sumPhyVect(0,0,0);
        sumPhyVect += vect;

        REQUIRE(vect.x == sumPhyVect.x());
        REQUIRE(vect.y == sumPhyVect.y());
        REQUIRE(vect.z == sumPhyVect.z());

    }

}
