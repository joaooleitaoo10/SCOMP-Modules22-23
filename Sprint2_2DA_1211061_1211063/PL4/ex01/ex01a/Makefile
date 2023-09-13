space := $(subst ,, )
NAME=$(notdir $(patsubst %/,%,$(subst $(space),_,$(CURDIR))))
C_FILENAME=$(basename $(firstword $(wildcard *.c)) )


$(NAME):  $(C_FILENAME).o
	gcc $(C_FILENAME).o -o $(NAME)

$(C_FILENAME).o: $(C_FILENAME).c 
	gcc -g -Wall -c $(C_FILENAME).c -o $(C_FILENAME).o -lpthread

c: 
	rm -f *.o $(NAME) 

r: $(NAME)
	./$(NAME)