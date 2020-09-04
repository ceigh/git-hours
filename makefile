CFLAGS=-Wall -Werror
SPACE=/usr/local
NAME=git-hours

$(NAME): $(NAME).o -lgit2

clean:
	$(RM) $(NAME) *.o

install:
	cp $(NAME) $(SPACE)/bin/$(NAME)
	cp $(NAME).1 $(SPACE)/man/man1/$(NAME).1
	gzip $(SPACE)/man/man1/$(NAME).1

uninstall:
	$(RM) $(SPACE)/bin/$(NAME)
	$(RM) $(SPACE)/man/man1/$(NAME).1.gz
