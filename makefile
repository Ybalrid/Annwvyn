CC = g++

SYSARCH       = i386
ifeq ($(shell uname -m),x86_64)
SYSARCH       = x86_64
endif


CFLAGS = -g -Wall
LDFLAGS =  -L../OculusSDK/LibOVR/Lib/Linux/Release/$(SYSARCH) -L/usr/local/lib -L/usr/lib/OGRE -L/usr/local/lib/OGRE/  -lovr -lOgreMain -lOIS -lopenal -lBulletDynamics -lBulletCollision -lLinearMath -lsndfile -lX11 -lXinerama -ludev -lboost_system 
IFLAGS = -I/usr/local/include/OGRE  -I/usr/include/OGRE -I../OculusSDK/LibOVR/Include  -I/usr/include/AL -I/usr/include/bullet -I/usr/local/include/bullet  -I/usr/include/OIS -I/usr/include/boost -I./include/

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

lib/libAnnwvyn.so: obj/AnnAudioEngine.o obj/AnnEngine.o obj/AnnGameObject.o obj/AnnTools.o obj/AnnTriggerObject.o obj/BtOgre.o obj/OgreOculus.o obj/AnnMap.o obj/AnnJoystickController.o obj/Gorilla.o
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

obj/OgreOculus.o: src/OgreOculus.cpp include/OgreOculus.h include/euler.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAGS) -fpic -c src/OgreOculus.cpp -o obj/OgreOculus.o

obj/AnnMap.o: src/AnnMap.cpp include/AnnMap.hpp
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAGS) -fpic -c src/AnnMap.cpp -o obj/AnnMap.o

obj/AnnJoystickController.o: src/AnnJoystickController.cpp include/AnnJoystickController.hpp
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAGS) -fpic -c src/AnnJoystickController.cpp -o obj/AnnJoystickController.o

obj/Gorilla.o: src/Gorilla.cpp include/Gorilla.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAGS) -fpic -c src/Gorilla.cpp -o obj/Gorilla.o
endif
