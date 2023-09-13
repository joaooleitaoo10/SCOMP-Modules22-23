ex06: main.o
	gcc -g -Wall main.o -o ex06
	
main.o: main.c
	gcc -g -Wall -c main.c -o main.o		
	
vg: ex06
	valgrind ./ex06
	
c:
	rm -f *.o ex06
	
r: ex06
	./ex06