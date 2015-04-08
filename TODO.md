
Todo list : 
Critial
- [ ] Start to "ungodify" the AnnEngine class by removing everything that has something to do with bullet and collision testing from it.
- [ ] Need to think of a proper way of giving arguments to the engine befor graphical initialization...
- [ ] Allow playing multiple animations
- [ ] Create Annwvyn classes for representing vectors and quaternion relying on Ogre code + being able to convert itself to Bullet equivalent or other in-engine dependencies

Non critial
- [ ] Find a good name for AnnEngine::m_Camera wich is not tne camera but is used to calculate camera orientation and position
- [ ] Add friction setting for bullet
- [ ] Object parenting
- [ ] Add debug drawing for trigger position when debug drawing is active for bullet
- [ ] Permit to create non spherical trigger
- [ ] Simplify the linux makefile template. Maybe make something to regenerate it from a list of files

Will probably not be added to the engine (map system has to be redone from scratch...)
- [ ] Add name identification for objects to be found in Map Files
- [ ] Add light vector to AnnMap Class