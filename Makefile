CC=gcc
CFLAGS=-Wall -Wextra -pedantic -std=c99
LIBS=
LDFLAGS=
SRC=main.c color.c
OBJ=$(SRC:.c=.o)
DEPS=$(wildcard *.dep)

all: test

-include $(DEPS)

%.o: %.c
	@$(CC) -MM $(CFLAGS) -c -o $@.dep $<
	$(CC) $(CFLAGS) -c -o $@ $<

test:  $(OBJ)
	$(CC) $^ -o $@ $(LDFLAGS) $(LIBS)

clean:
	$(RM) $(OBJ)

fullclean: clean
	$(RM) test $(DEPS)
