CC = g++

SYSARCH       = i386
ifeq ($(shell uname -m),x86_64)
SYSARCH       = x86_64
endif


CFLAGS = --std=c++0x
LDFLAGS =  -L../OculusSDK/LibOVR/Lib/Linux/Release/$(SYSARCH) -L/usr/local/lib -L/usr/lib/OGRE -L/usr/local/lib/OGRE/  -lovr -lOgreMain -lOIS -lopenal -lBulletDynamics -lBulletCollision -lLinearMath -lsndfile -lX11 -lXinerama -ludev -lboost_system  -lXrandr -lXxf86vm -lGL -lrt
IFLAGS = -I/usr/local/include/OGRE  -I/usr/include/OGRE -I../OculusSDK/LibOVR/Include -I../OculusSDK/LibOVR/Src  -I/usr/include/AL -I/usr/include/bullet -I/usr/local/include/bullet  -I/usr/include/OIS -I/usr/include/boost -I./include/ -I/usr/include/GL

INSTALL_PREFIX = /usr/local

ifeq ($(shell uname), Linux)
all: lib/libAnnwvyn.so

.PHONY: doc
doc:
	(cd doxygen;doxygen Doxyfile)
	(cd doxygen/Gen/latex;make)


.PHONY: clean
clean:
	rm -r obj/*.o lib/*.so 
	rm -r doxygen/Gen 


.PHONY: instal
install: all
	@echo -------------------------------------------------------------------------
	@echo Create install directories on $(INSTALL_PREFIX)
	mkdir --parent $(INSTALL_PREFIX)/lib/Annwvyn $(INSTALL_PREFIX)/include/Annwvyn
	@echo install compiled shared object on $(INSTALL_PREFIX)/lib/Annwvyn/libAnnwvyn.so
	install lib/libAnnwvyn.so $(INSTALL_PREFIX)/lib/Annwvyn/libAnnwvyn.so
	@echo install library headers on  $(INSTALL_PREFIX)/include/Annwvyn/
	install include/* $(INSTALL_PREFIX)/include/Annwvyn/
	@echo install ldconfig Annvyn.conf
	install configFiles/Annwvyn.conf /etc/ld.so.conf.d/Annwvyn.conf
	@echo run ldconfig and display Annwvyn related content
	ldconfig -v | grep Ann | tee ld.log
	@echo DONE ! you can see library install log on ld.log 

test: lib/libAnnwvyn.so
	$(CC) example/main.cpp -o example/test  -L./lib -lAnnwvyn $(CFLAGS) $(LDFLAGS) $(IFLAGS) -lpthread
	@echo 'Copying Shared object to program floder'
	cp lib/libAnnwvyn.so example/
	@echo 'Done. You can try to lanch the executable test from the example directory'

lib/libAnnwvyn.so: obj/AnnAudioEngine.o obj/AnnDefaultEventListener.o obj/AnnEngine.o obj/AnnGameObject.o obj/AnnCharacter.o obj/AnnTools.o obj/AnnTriggerObject.o obj/BtOgre.o  obj/AnnMap.o obj/AnnJoystickController.o obj/OculusInterface.o obj/OgreOculusRender.o obj/Gorilla.o obj/AnnEventManager.o obj/AnnEvents.o obj/AnnPlayer.o
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAGS) -shared -o lib/libAnnwvyn.so obj/*.o

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



endif
