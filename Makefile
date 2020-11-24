# Create your own targets that compile the application
dplib:
	git show origin/lab4:ex4/dplist.c > ./lib/dplist.c
	gcc -fPIC -c ./lib/dplist.c -o ./lib/dplist.o
	gcc -shared -o ./lib/libdplist.so ./lib/dplist.o -Wall -std=c11 -Werror

lib:
	gcc -fPIC -c ./lib/tcpsock.c -o ./lib/tcpsock.o
	gcc -shared -o ./lib/libtcpsock.so ./lib/tcpsock.o -Wall -std=c11 -Werror

# the files for ex2 will be ziped and are then ready to
# be submitted to labtools.groept.be
zip:
	zip lab7_ex2.zip connmgr.c connmgr.h config.h lib/dplist.c lib/dplist.h lib/tcpsock.c lib/tcpsock.h
