#Makefile for TWMailerClient

CC=g++
CFLAGS= -g -Wall -std=c++0x -pthread
VERSION=1.0

all: TWMailerClient

TWMailerClient: TWMailerClient.o Helper.o CommandHandler.o
	${CC} ${CFLAGS} -o TWMailerClient TWMailerClient.o Helper.o CommandHandler.o

TWMailerClient.o: TWMailerClient.cpp Helper.h
	${CC} ${CFLAGS} -c TWMailerClient.cpp

Helper.o: Helper.cpp Helper.h
	${CC} ${CFLAGS} -c Helper.cpp
	
CommandHandler.o: CommandHandler.cpp CommandHandler.h
	${CC} ${CFLAGS} -c CommandHandler.cpp

clean: 
	rm -f *.o
	rm -f TWMailerClient

dist:
	mkdir TWMailerClient-${VERSION}
	cp *.h TWMailerClient-${VERSION}/
	cp *.cpp TWMailerClient-${VERSION}/
	cp Makefile TWMailerClient-${VERSION}/Makefile
	tar -cvzf TWMailerClient-${VERSION}.tgz TWMailerClient-${VERSION}/
	rm -R TWMailerClient-${VERSION}

distclean:
	rm -f TWMailerClient-${VERSION}.tgz
