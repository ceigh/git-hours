CFLAGS=-Wall -Werror -O
PREFIX=/usr/local
MAN_PREFIX=/usr/local/man
NAME=git-hours

$(NAME): $(NAME).o -lgit2

clean:
	$(RM) $(NAME) *.o

install:
	cp $(NAME) $(PREFIX)/bin/$(NAME)
	cp $(NAME).1 $(MAN_PREFIX)/man1/$(NAME).1
	gzip $(MAN_PREFIX)/man1/$(NAME).1

uninstall:
	$(RM) $(PREFIX)/bin/$(NAME)
	$(RM) $(MAN_PREFIX)/man1/$(NAME).1.gz
