ex16: main.o
	gcc -g -Wall main.o -o ex16
	
main.o: main.c
	gcc -g -Wall -c main.c -o main.o		
	
vg: ex16
	valgrind ./ex16
	
c:
	rm -f *.o ex16
	
r: ex16
	./ex16