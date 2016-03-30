CC=g++
CFLAGS=-std=c++11 -pedantic -Wall


stegomax: main.o wavmod.o
	$(CC) $(CFLAGS) -o stegomax main.o wavmod.o
	
main.o: main.cpp wavmod.h
	$(CC) $(CFLAGS) -c main.cpp
	
wavmod.o: wavmod.cpp wavmod.h
	$(CC) $(CFLAGS) -c wavmod.cpp
	
clean:
	rm *.o stegomax
cleanf:
	rm clip.wav*.wav