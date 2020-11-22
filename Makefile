# Create your own targets that compile the application
datagen:
	gcc file_creator.c -DDEBUG -o file_creator
	./file_creator

main:
	gcc -lm -c -Wall sensor_db.c -lsqlite3
	gcc -lm -g -c -Wall main.c
	gcc -lm -g -o test_main sensor_db.o main.o -lsqlite3
	./test_main

# the files for ex2 will be ziped and are then ready to
# be submitted to labtools.groept.be
zip:
	zip lab6_ex2.zip sensor_db.c config.h
