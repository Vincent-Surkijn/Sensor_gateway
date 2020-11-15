# Create your own targets that build the dynamic library for the list and then compile the application

# to test if lib is working
libtest: lib
	gcc test.c -L./lib -Wl,-rpath=./lib -ldplist -I./lib
	./a.out

lib:
	git show origin/lab4:ex4/dplist.c > ./lib/dplist.c
	gcc -fPIC -c ./lib/dplist.c -o ./lib/dplist.o
	gcc -shared -o ./lib/libdplist.so ./lib/dplist.o

datagen:
	gcc file_creator.c -DDEBUG -o file_creator
	./file_creator

main:
	gcc -c datamgr.c
	gcc -c main.c
	gcc -o test_main datamgr.o main.o
	./test_main

# the files for ex3 will be ziped and are then ready to
# be submitted to labtools.groept.be
zip:
	zip lab5_ex3.zip datamgr.c datamgr.h config.h lib/dplist.c lib/dplist.h

