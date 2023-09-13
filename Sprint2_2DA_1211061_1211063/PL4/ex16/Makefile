ex16: main.o up_down.o
	gcc -g -Wall -lrt main.o up_down.o -o ex16
	
main.o: main.c
	gcc -g -Wall -c main.c -o main.o


up_down.o: up_down.c
	gcc -g -Wall -c up_down.c -o up_down.o	
	
vg: ex16
	valgrind ./ex16
	
c:
	rm -f *.o ex16
	
r: ex16
	./ex16