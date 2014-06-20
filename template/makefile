SYSARCH       = i386
ifeq ($(shell uname -m),x86_64)
SYSARCH       = x86_64
endif

NAME=Game

ifeq ($(shell uname), Linux)

CC = g++


CFLAGS = -Wall
LDFLAGS =  -L/usr/local/lib/Annwvyn -L ~/OculusSDK/LibOVR/Lib/Linux/Release/$(SYSARCH) -L/usr/local/lib -L/usr/lib/OGRE -L/usr/local/lib/OGRE/ -lAnnwvyn -lovr -lOgreMain -lOIS -lopenal -lBulletDynamics -lBulletCollision -lLinearMath -lsndfile -lX11 -lXinerama -ludev -lboost_system -lpthread
IFLAGS = -I/usr/local/include/OGRE  -I/usr/include/OGRE -I ~/OculusSDK/LibOVR/Include  -I/usr/include/AL -I/usr/include/bullet -I/usr/local/include/bullet  -I/usr/include/OIS -I/usr/include/boost -I/usr/local/include/Annwvyn 


all: $(NAME)

$(NAME): obj/main.o
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAGS) obj/main.o -o $(NAME)

obj/main.o: main.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAGS) main.cpp -c -o obj/main.o


clean:
	rm $(NAME) obj/*.o 2> /dev/null
endif
