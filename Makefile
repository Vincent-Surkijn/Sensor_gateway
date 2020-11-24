# Create your own targets that compile the application
dplib:
	git show origin/lab4:ex4/dplist.c > ./lib/dplist.c
	gcc -fPIC -c ./lib/dplist.c -o ./lib/dplist.o
	gcc -shared -o ./lib/libdplist.so ./lib/dplist.o -Wall -std=c11 -Werror

lib:
	gcc -fPIC -c ./lib/tcpsock.c -o ./lib/tcpsock.o
	gcc -shared -o ./lib/libtcpsock.so ./lib/tcpsock.o -Wall -std=c11 -Werror

libtest: lib	dplib
	gcc libtest.c -L./lib -Wl,-rpath=./lib -ldplist -ltcpsock -I./lib
	./a.out

testsetup: lib	dplib
	gcc -o ./sensor sensor_node.c -L./lib -Wl,-rpath=./lib -ltcpsock -I./lib
	gcc -o ./server test_server.c -L./lib -Wl,-rpath=./lib -ltcpsock -I./lib

sensor:
	./sensor 111 2 127.0.0.1 5678

# the files for ex2 will be ziped and are then ready to
# be submitted to labtools.groept.be
zip:
	zip lab7_ex2.zip connmgr.c connmgr.h config.h lib/dplist.c lib/dplist.h lib/tcpsock.c lib/tcpsock.h
