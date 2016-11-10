#ifndef MY_LEVEL
#define MY_LEVEL

#include <Annwvyn.h>
using namespace Annwvyn;

//Each level you can create inherits
//from AnnAbstractLevel
class MyLevel : LEVEL
{
public:
	MyLevel();
	void load() override;
	void runLogic() override;
};

#endif //MY_LEVEL