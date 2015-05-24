#Change the name of your project here
NAME=Game


#Linux build environement with OculusSDK located under ~/OculusSDK/
ifeq ($(shell uname), Linux)

SYSARCH       = i386
ifeq ($(shell uname -m),x86_64)
SYSARCH       = x86_64
endif

CC = g++ 

CFLAGS = --std=c++0x -Wall -O2

LDFLAGS =  -L/usr/local/lib/Annwvyn -L ~/OculusSDK/LibOVR/Lib/Linux/Release/$(SYSARCH) -L/usr/local/lib -L/usr/lib/OGRE -L/usr/local/lib/OGRE/ -lAnnwvyn -lOgreMain -lOgreOverlay -lOIS -lopenal -lBulletDynamics -lBulletCollision -lLinearMath -lsndfile -lX11 -lXrandr -lXxf86vm -lXinerama -ludev -lboost_system -lpthread -lGL -lrt  -lm -ldl -lOVR

IFLAGS = -I/usr/local/include/OGRE  -I/usr/include/OGRE -I ~/OculusSDK/LibOVR/Include -I ~/OculusSDK/LibOVR/Src -I ~/OculusSDK/LibOVRKernel/Src -I/usr/include/AL -I/usr/include/bullet -I/usr/local/include/bullet  -I/usr/include/OIS -I/usr/include/boost -I/usr/local/include/Annwvyn 

endif


all: $(NAME)

#edit that target to match the configuration of your project.
$(NAME): obj/main.o 
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAGS) obj/main.o -o $(NAME)

obj/main.o: main.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAGS) main.cpp -c -o obj/main.o

clean:
	rm $(NAME) obj/*.o 2> /dev/null

#here you can add the other objects you need to build.

