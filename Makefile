# Create your own targets that compile the application
libtest: lib
	gcc libtest.c -L./lib -Wl,-rpath=./lib -ldplist -I./lib
	./a.out

lib:
	git show origin/lab4:ex4/dplist.c > ./lib/dplist.c
	gcc -fPIC -c ./lib/dplist.c -o ./lib/dplist.o
	gcc -shared -o ./lib/libdplist.so ./lib/dplist.o -Wall -std=c11 -Werror

datagen:
	gcc file_creator.c -DDEBUG -o file_creator
	./file_creator

main:
	gcc -g -c datamgr.c
	gcc -g -c -lm main.c
	gcc -lm -g -o test_main datamgr.o main.o -L./lib -Wl,-rpath=./lib -ldplist -I./lib
	./test_main

mainWithDefines:
	gcc -lm -g -c datamgr.c -DRUN_AVG_LENGTH=5 -DSET_MIN_TEMP=14.01 -DSET_MAX_TEMP=27.96
	gcc -c -Wall sensor_db.c -lsqlite3
	gcc -lm -g -c main.c
	gcc -lm -g -o test_main datamgr.o sensor_db.o main.o -lsqlite3 -L./lib -Wl,-rpath=./lib -ldplist -I./lib
	./test_main

# the files for ex2 will be ziped and are then ready to
# be submitted to labtools.groept.be
zip:
	zip lab6_ex2.zip sensor_db.c config.h
