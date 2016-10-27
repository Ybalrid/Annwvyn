Fix the physics
---------------
- [ ] Rework the collision feedback system. 
- [ ] Re-implement trigger system with Bullet
____
TODO list
---------
- [x] Use anti aliasing
- [x] Handle hand-controller event via the Event Manager **very basic for now**
- [ ] Integrate a scripting language for gameplay. Probably ChaiScript
- [ ] Add an "engine configuration file" system. As a unix-like .conf file (or a windows like .ini file|unix file prefered for the sake of simplicity)
______
Upgrade the engine to Ogre 2.1
-----
  - [ ] Use the compositor to do stereo rendering with RTT
  - [ ] Integrate the HLMS shader library to the core resources of the engine 
  - [ ] Convert every existing material to the HLMS
  - [ ] Find a acceptable work-flow to export HLMS PBS materials from Blender
  - [ ] Make a acceptable work-flow to export Ogre V2 binary mesh format from Blender
______
After upgrading to Ogre2.1 
-----
- [ ] Object parenting
- [ ] Rework the animation system. Animation state should run in a *state machine automata* with proper transition programming 
____
Maybe after Ogre 2.1
-----
- [ ] Pipeline the rendering pass with a logic thread and a render thread -> this require to store the gameplay and render positions/orientation/scale of GameObects independently
- [ ] Make the engine render a "loading screen" when loading resources/constructing levels
