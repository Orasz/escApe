main.e:build/main.o build/classes.o
	g++ build/main.o build/classes.o -o main.e -lsfml-graphics -lsfml-window -lsfml-system

build/main.o:src/main.cpp
	g++ -g -c -lstdc++ -std=c++11 src/main.cpp -o build/main.o

build/classes.o:src/classes.cpp
	g++ -g -c -std=c++11 src/classes.cpp -o build/classes.o

clean:
	rm -f *.o main
