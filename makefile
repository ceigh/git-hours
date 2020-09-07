CFLAGS=-Wall -Werror -O
PREFIX=/usr/local
NAME=git-hours

$(NAME): $(NAME).o -lgit2

clean:
	$(RM) $(NAME) *.o

install:
	cp $(NAME) $(PREFIX)/bin/$(NAME)
	cp $(NAME).1 /usr/local/man/man1/$(NAME).1
	gzip /usr/local/man/man1/$(NAME).1

uninstall:
	$(RM) $(PREFIX)/bin/$(NAME)
	$(RM) /usr/local/man/man1/$(NAME).1.gz
