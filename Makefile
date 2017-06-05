CC=gcc
CFLAGS=-Wall -Wextra -std=c99 -O2
LIBS=
LDFLAGS=-O2
GENERATED_SOURCES=eux_readelf_cmdline.c eux_readelf_cmdline.h
SRC=eux_readelf.c color.c eux_readelf_cmdline.c symtab.c strtab.c
OBJ=$(SRC:.c=.o)
DEPS=$(wildcard *.dep)
TARGETS=eux_readelf
GENGETOPT=gengetopt

all: $(TARGETS)

-include $(DEPS)

%.o: %.c
	@$(CC) -MM $(CFLAGS) -c -o $@.dep $<
	$(CC) $(CFLAGS) -c -o $@ $<

eux_readelf.o: eux_readelf_cmdline.h

eux_readelf_cmdline.c eux_readelf_cmdline.h: eux_readelf.ggo
	$(GENGETOPT) -u --no-help --no-version -F eux_readelf_cmdline < $<

eux_readelf:  $(OBJ)
	$(CC) $^ -o $@ $(LDFLAGS) $(LIBS)

clean:
	$(RM) $(OBJ)

fullclean: clean
	$(RM) $(TARGETS) $(DEPS) $(GENERATED_SOURCES)
