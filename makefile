CC = g++
CFLAGS =
LDFLAGS =  -L../OculusSDK/LibOVR/Lib/Linux/Release/x86_64 -L/usr/local/lib -L/usr/local/lib/OGRE/ -lovr -lOgreMain -lOIS -lopenal -lBulletDynamics -lBulletCollision -lLinearMath -lsndfile -lX11 -lXinerama -ludev -lboost_system 
IFLAGS = -I/usr/local/include/OGRE  -I../OculusSDK/LibOVR/Include  -I/usr/include/AL -I/usr/include/bullet  -I/usr/include/OIS -I/usr/include/boost -I./include/ 

INSTALL_PREFIX = /usr/local

ifeq ($(shell uname), Linux)
all: lib/libAnnwvyn.so



.PHONY: clean
clean:
	rm -r obj/*.o lib/*.so

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
	$(CC) example/main.cpp -o example/testDyna  -L./lib -lAnnwvyn $(CFLAGS) $(LDFLAGS) $(IFLAGS) 
	@echo 'Copying Shared object to program floder'
	cp lib/libAnnwvyn.so example/
	@echo 'Done. You can try to lanch the executable test from the example directory'

lib/libAnnwvyn.so: obj/AnnAudioEngine.o obj/AnnEngine.o obj/AnnGameObject.o obj/AnnTools.o obj/AnnTriggerObject.o obj/BtOgre.o obj/OgreOculus.o
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAGS) -shared -o lib/libAnnwvyn.so obj/*.o

obj/AnnEngine.o:
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAGS) -fpic -c src/AnnEngine.cpp -o obj/AnnEngine.o

obj/AnnAudioEngine.o:
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAGS) -fpic -c src/AnnAudioEngine.cpp -o obj/AnnAudioEngine.o

obj/AnnGameObject.o:
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAGS) -fpic -c src/AnnGameObject.cpp -o obj/AnnGameObject.o

obj/AnnTools.o:
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAGS) -fpic -c src/AnnTools.cpp -o obj/AnnTools.o

obj/AnnTriggerObject.o:
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAGS) -fpic -c src/AnnTriggerObject.cpp -o obj/AnnTriggerObject.o

obj/BtOgre.o:
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAGS) -fpic -c src/BtOgre.cpp -o obj/BtOgre.o

obj/OgreOculus.o:
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAGS) -fpic -c src/OgreOculus.cpp -o obj/OgreOculus.o

endif
