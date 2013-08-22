CC = g++
CFLAGS =
LDFLAGS =  -L../OculusSDK/LibOVR/Lib/Linux/Release/x86_64 -L/usr/local/lib -L/usr/local/lib/OGRE/ -lovr -lOgreMain -lOIS -lopenal -lBulletDynamics -lBulletCollision -lLinearMath -lsndfile -lX11 -lXinerama -ludev -lboost_system 
IFLAGS = -I/usr/local/include/OGRE  -I../OculusSDK/LibOVR/Include  -I/usr/include/AL -I/usr/include/bullet  -I/usr/include/OIS -I/usr/include/boost -I./include/ 
UNAME = $(shell uname)


#This only work on linux
ifeq ($(UNAME), Linux)


all: examples

lib:
	echo "not configured yet"



examples:
	$(CC) src/* example/*.cpp -o example/test $(CFLAGS) $(IFLAGS) $(LDFLAGS) 
endif
