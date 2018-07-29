
#include "engineBootstrap.hpp"

namespace Annwvyn
{

    TEST_CASE("Vector BT convert")
    {
        AnnVect3 vect(1,2,3);

	    const btVector3 physicsVector(vect);

        REQUIRE(vect.x == physicsVector.x());
        REQUIRE(vect.y == physicsVector.y());
        REQUIRE(vect.z == physicsVector.z());


        btVector3 sumPhyVect(0,0,0);
        sumPhyVect += vect;

        REQUIRE(vect.x == sumPhyVect.x());
        REQUIRE(vect.y == sumPhyVect.y());
        REQUIRE(vect.z == sumPhyVect.z());

    }


    TEST_CASE("Quaternion BT convert")
    {
        AnnQuaternion q(0,0,0,1);

	    const btQuaternion btq(q);

        REQUIRE(q.x == btq.x());
        REQUIRE(q.y == btq.y());
        REQUIRE(q.z == btq.z());
        REQUIRE(q.w == btq.w());
    }

}
