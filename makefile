CFLAGS=-Wall -Werror
NAME=git-hours

$(NAME): $(NAME).o -lgit2

clean:
	$(RM) $(NAME) *.o

install:
	cp $(NAME) /usr/local/bin/$(NAME)

uninstall:
	$(RM) /usr/local/bin/$(NAME)
