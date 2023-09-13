ex13: main.o
	gcc -g -Wall main.o -o ex13
	
main.o: main.c
	gcc -g -Wall -c main.c -o main.o	
	
vg: ex13	
	valgrind ./ex13
	
c:
	rm -f *.o ex13
	
r: ex13
	./ex13