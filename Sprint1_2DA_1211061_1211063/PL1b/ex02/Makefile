ex02: main.o
	gcc -g -Wall main.o -o ex02
	
main.o: main.c
	gcc -g -Wall -c main.c -o main.o	
	
vg: ex02	
	valgrind ./ex02
	
c:
	rm -f *.o ex02
	
r: ex02
	./ex02