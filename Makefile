ex1:
	mkdir -p build
	gcc -Wall -Werror -o ./build/ex1 main.c
	./build/ex1

zip:
	zip lab1_ex1.zip main.c

ex2:
	mkdir -p build2
	gcc -Wall -Werror -o ./build/ex2 main.c
	./build/ex2
