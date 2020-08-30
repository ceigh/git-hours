CFLAGS=-Wall -Werror
NAME=hours

$(NAME): $(NAME).o -lgit2

clean:
	$(RM) $(NAME) *.o
