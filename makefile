#if we are on linux
ifeq ($(shell uname), Linux)
all: lib/libAnnwvyn.so

CC = g++

#Get the operating system type.
SYSARCH       = i386
ifeq ($(shell uname -m),x86_64)
SYSARCH       = x86_64
endif

#define flags
CFLAGS = --std=c++0x -Wall -O2 
LDFLAGS =  -L../OculusSDK/LibOVR/Lib/Linux/Release/$(SYSARCH) -L/usr/local/lib -L/usr/lib/OGRE -L/usr/local/lib/OGRE/ -lOgreMain -lOIS -lopenal -lBulletDynamics -lBulletCollision -lLinearMath -lsndfile -lX11  -ludev -lboost_system  -lXrandr -lXxf86vm -lGL -lrt -lOVR -lm -ldl

IFLAGS = -I/usr/local/include/OGRE  -I/usr/include/OGRE -I../OculusSDK/LibOVR/Include -I../OculusSDK/LibOVR/Src -I../OculusSDK/LibOVRKernel/Src  -I/usr/include/AL -I/usr/include/bullet -I/usr/local/include/bullet  -I/usr/include/OIS -I/usr/include/boost -I./include/ -I/usr/include/GL

#define the installation location for the engine (should be a local system folder)
INSTALL_PREFIX = /usr/local

#install the library to the system, configuration
.PHONY: install
install: all
	@echo cleaning all old installation of Annwvyn
	rm -rf $(INSTALL_PREFIX)/include/Annwvn $(INSTALL_PREFIX)/lib/Annwvyn 2> /dev/null >/dev/null 
	@echo -------------------------------------------------------------------------
	@echo Create install directories on $(INSTALL_PREFIX)
	mkdir --parent $(INSTALL_PREFIX)/lib/Annwvyn $(INSTALL_PREFIX)/include/Annwvyn
	@echo -------------------------------------------------------------------------
	@echo install compiled shared object on $(INSTALL_PREFIX)/lib/Annwvyn/libAnnwvyn.so
	install lib/libAnnwvyn.so $(INSTALL_PREFIX)/lib/Annwvyn/libAnnwvyn.so
	@echo -------------------------------------------------------------------------
	@echo install library headers on  $(INSTALL_PREFIX)/include/Annwvyn/
	install include/* $(INSTALL_PREFIX)/include/Annwvyn/
	@echo -------------------------------------------------------------------------
	@echo install ldconfig Annvyn.conf
	install configFiles/Annwvyn.conf /etc/ld.so.conf.d/Annwvyn.conf
	@echo -------------------------------------------------------------------------
	@echo run ldconfig and display Annwvyn related content
	ldconfig -v | grep Ann | tee ldconfig_update.log
	@echo -------------------------------------------------------------------------
	@echo "Installation finished. You can see library install log on ldconfig_update.log"

endif

#build doxygen documentation
.PHONY: doc
doc:
	(cd doxygen; doxygen Doxyfile)
	(cd doxygen/Gen/latex; make)

#remove everything created with the makefile
.PHONY: clean
clean:
	@echo "Clear .o and .so file"
	rm -rf obj/*.o lib/*.so 2> /dev/null > /dev/null
	@echo "Clear doxygen generated site"
	rm -rf doxygen/Gen  2> /dev/null > /dev/null


#build the test programm
test: lib/libAnnwvyn.so
	$(CC) example/main.cpp -o example/test  -L./lib -lAnnwvyn $(CFLAGS) $(LDFLAGS) $(IFLAGS) -lpthread
	@echo 'Copying Shared object to program floder'
	cp lib/libAnnwvyn.so example/
	@echo 'Done. You can try to lanch the executable test from the example directory'
	@echo "(you may need to call sudo make install tu actually run the test with the lattest Annwvyn version)"


#build the DSO from the objects file
lib/libAnnwvyn.so: obj/AnnAudioEngine.o obj/AnnDefaultEventListener.o obj/AnnEngine.o obj/AnnGameObject.o obj/AnnCharacter.o obj/AnnTools.o obj/AnnTriggerObject.o obj/BtOgre.o  obj/AnnMap.o obj/AnnJoystickController.o obj/OculusInterface.o obj/OgreOculusRender.o obj/Gorilla.o obj/AnnEventManager.o obj/AnnEvents.o obj/AnnPlayer.o obj/AnnPhysicsEngine.o
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAGS) -shared -o lib/libAnnwvyn.so obj/*.o

#build each class separatly
obj/AnnEngine.o: src/AnnEngine.cpp include/AnnEngine.hpp
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAGS) -fpic -c src/AnnEngine.cpp -o obj/AnnEngine.o

obj/AnnAudioEngine.o: src/AnnAudioEngine.cpp include/AnnAudioEngine.hpp
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAGS) -fpic -c src/AnnAudioEngine.cpp -o obj/AnnAudioEngine.o

obj/AnnGameObject.o: src/AnnGameObject.cpp include/AnnAudioEngine.hpp
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAGS) -fpic -c src/AnnGameObject.cpp -o obj/AnnGameObject.o

obj/AnnTools.o: src/AnnTools.cpp include/AnnTools.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAGS) -fpic -c src/AnnTools.cpp -o obj/AnnTools.o

obj/AnnTriggerObject.o: src/AnnTriggerObject.cpp include/AnnTriggerObject.hpp
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAGS) -fpic -c src/AnnTriggerObject.cpp -o obj/AnnTriggerObject.o

obj/BtOgre.o: src/BtOgre.cpp include/BtOgreExtras.h  include/BtOgreGP.h  include/BtOgrePG.h 
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAGS) -fpic -c src/BtOgre.cpp -o obj/BtOgre.o

obj/AnnMap.o: src/AnnMap.cpp include/AnnMap.hpp
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAGS) -fpic -c src/AnnMap.cpp -o obj/AnnMap.o

obj/AnnJoystickController.o: src/AnnJoystickController.cpp include/AnnJoystickController.hpp
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAGS) -fpic -c src/AnnJoystickController.cpp -o obj/AnnJoystickController.o

obj/Gorilla.o: src/Gorilla.cpp include/Gorilla.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAGS) -fpic -c src/Gorilla.cpp -o obj/Gorilla.o

obj/OculusInterface.o: src/OculusInterface.cpp include/OculusInterface.hpp
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAGS) -fpic -c src/OculusInterface.cpp -o obj/OculusInterface.o 

obj/OgreOculusRender.o: src/OgreOculusRender.cpp include/OgreOculusRender.hpp
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAGS) -fpic -c src/OgreOculusRender.cpp -o obj/OgreOculusRender.o

obj/AnnCharacter.o: src/AnnCharacter.cpp include/AnnCharacter.hpp
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAGS) -fpic -c src/AnnCharacter.cpp -o obj/AnnCharacter.o

obj/AnnDefaultEventListener.o: src/AnnDefaultEventListener.cpp include/AnnEventManager.hpp include/AnnKeyCode.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAGS) -fpic -c src/AnnDefaultEventListener.cpp -o obj/AnnDefaultEventListener.o

obj/AnnEventManager.o: src/AnnEventManager.cpp include/AnnEventManager.hpp include/AnnKeyCode.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAGS) -fpic -c src/AnnEventManager.cpp -o obj/AnnEventManager.o

obj/AnnEvents.o: src/AnnEvents.cpp include/AnnEventManager.hpp include/AnnKeyCode.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAGS) -fpic -c src/AnnEvents.cpp -o obj/AnnEvents.o 

obj/AnnPlayer.o: src/AnnPlayer.cpp include/AnnPlayer.hpp
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAGS) -fpic -c src/AnnPlayer.cpp -o obj/AnnPlayer.o


obj/AnnPhysicsEngine.o: src/AnnPhysicsEngine.cpp include/AnnPhysicsEngine.hpp
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAGS) -fpic -c src/AnnPhysicsEngine.cpp -o obj/AnnPhysicsEngine.o

