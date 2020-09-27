ex1:
    mkdir -p build
    gcc -Wall -Werror -o ./build/ex1 main.c
    ./build/ex1

zip:
    zip lab1_ex1.zip main.c
