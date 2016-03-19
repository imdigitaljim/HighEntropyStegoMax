CC=g++
CFLAGS=-std=c++11 -pedantic -Wall


exe: main.o wavmod.o
	$(CC) $(CFLAGS) -o exe main.o wavmod.o
	
main.o: main.cpp wavmod.h
	$(CC) $(CFLAGS) -c main.cpp
	
wavmod.o: wavmod.cpp wavmod.h
	$(CC) $(CFLAGS) -c wavmod.cpp
	
clean:
	rm *.o exe
cleanf:
	rm clip.wav*.wav