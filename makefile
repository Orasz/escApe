main.e:main.o classes.o
	g++ main.o classes.o -o main.e -lsfml-graphics -lsfml-window -lsfml-system

main.o:main.cpp
	g++ -g -c -lstdc++ -std=c++11 main.cpp

classes.o:classes.cpp
	g++ -g -c -std=c++11 classes.cpp

clean:
	rm -f *.o main
