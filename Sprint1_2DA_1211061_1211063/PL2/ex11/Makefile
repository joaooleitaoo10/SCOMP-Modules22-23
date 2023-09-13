ex11: main.o
	gcc -g -Wall main.o -o ex11
	
main.o: main.c
	gcc -g -Wall -c main.c -o main.o	
	
vg: ex11
	valgrind ./ex11
	
c:
	rm -f *.o ex11
	
r: ex11
	./ex11