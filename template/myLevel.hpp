#ifndef MY_LEVEL
#define MY_LEVEL

#include <Annwvyn.h>

/*
 * This is the signature of a super simple "Level" class.
 * A level is the virtual environment where you'll set your player into.
 * 
 * You can have as many level as you want, and you can jump between levels at will.
 * 
 * You need to implement a constructor that initialize the level object, and at minimal 2 methods:
 * 
 *  1) void load() override; <- construct the environment (put objects where they should, attach scripts to objects, set physics...
 *  2) void runLogic() override; <- this will be called at each frame by the engine. If you need code that "heartbeat" inside the level, it's here.
 *
 *  If you use the "addXXXObject" of AnnLevel to create your level content, you don't even need to think about how your level will unload.
 *  If for some reason you're custom things at your level loading (setting up additional event listeners, custom object you use in your runLogic())
 *  and you need to clean up after yourself, override the "unload()" method, and don't forget to start it by calling AnnLevel::unload()!
 * 
 *  The construction/destruction of the level object is NOT the same thing as the engine load/unload of the level.
 *  When you create a level object, and you add it to the level manager, you're telling the engine that this level can be loaded/unloaded into the engine.
 *  The "load()" code will only be executed if the engine "jumps" inside a level. Creating GameObjects in the level constructor is undefined behavior as far as
 *  Annwvyn is concerned.
 */
class MyLevel : LEVEL
{
public:
	MyLevel();
	void load() override;
	void runLogic() override;
};

#endif //MY_LEVEL
