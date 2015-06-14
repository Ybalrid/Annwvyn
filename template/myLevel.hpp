#ifndef MY_LEVEL
#define MY_LEVEL

#include <Annwvyn.h>
using namespace Annwvyn;

//Each level you can create hinerits
//from AnnAbstractLevel
class MyLevel : public AnnAbstractLevel
{
public:
	MyLevel();
	void load();
	void runLogic();
};

#endif //MY_LEVEL