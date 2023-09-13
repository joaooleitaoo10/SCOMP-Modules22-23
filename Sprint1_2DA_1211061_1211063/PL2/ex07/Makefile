ex07: main.o
	gcc -g -Wall main.o -o ex07
	
main.o: main.c
	gcc -g -Wall -c main.c -o main.o	
	
vg: ex07	
	valgrind ./ex07
	
c:
	rm -f *.o ex07
	
r: ex07
	./ex07