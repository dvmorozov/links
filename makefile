#DIRCYGLIB = e:/cygwin/usr/include
#DIRMINGW = e:/cygwin/usr/include/mingw

all : main.o 
	gcc -o bookmarks main.o 

main.o : main.c
	gcc -c main.c
#	gcc -c -I $(DIRMINGW) main.c

