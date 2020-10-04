CFLAGS=-Wall -Werror -O
PREFIX=/usr/local
MAN_PREFIX=/usr/local/man
NAME=git-hours
HELPERS=helpers

$(NAME): $(NAME).o $(HELPERS).o -lgit2

clean:
	$(RM) $(NAME) *.o

install:
	cp $(NAME) $(PREFIX)/bin/$(NAME) || exit 1
	mkdir -p $(MAN_PREFIX)/man1 && \
		cp $(NAME).1 $(MAN_PREFIX)/man1/$(NAME).1 && \
			gzip $(MAN_PREFIX)/man1/$(NAME).1 || exit 1

uninstall:
	$(RM) $(PREFIX)/bin/$(NAME)
	$(RM) $(MAN_PREFIX)/man1/$(NAME).1.gz
