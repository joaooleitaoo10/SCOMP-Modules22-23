ex12: main.o spawn_childs.o
	gcc -g -Wall main.o spawn_childs.o -o ex12
	
main.o: main.c
	gcc -g -Wall -c main.c -o main.o	

spawn_childs.o: spawn_childs.c
	gcc -g -Wall -c spawn_childs.c -o spawn_childs.o		
	
vg: ex12
	valgrind ./ex12
	
c:
	rm -f *.o ex12
	
r: ex12
	./ex12