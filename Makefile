lib:
	gcc -fPIC -c sbuffer.c -o sbuffer.o
	gcc -shared -o libsbuffer.so sbuffer.o -Wall -std=c11 -Werror

libtest: lib
	gcc test.c -L. -Wl,-rpath=. -lsbuffer -I.
	./a.out

datagen:
	gcc file_creator.c -DDEBUG -o file_creator
	./file_creator
