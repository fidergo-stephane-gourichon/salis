CC := gcc
AR := ar

## WINDOWS ##
LIB := lib\libsalis.a

## UNIX ##
#LIB := lib/libsalis.a

SOURCES := $(wildcard src/*.c)
OBJECTS := $(patsubst src/%.c,build/%.o,$(SOURCES))
DEPS    := $(patsubst %.o,%.d,$(OBJECTS))
LFLAGS  := rcs

## RELEASE flags ##
CFLAGS  := -Iinclude -c -O3 -DNDEBUG -MMD -Wall -Wextra -std=c89 \
	   -pedantic-errors -Wmissing-prototypes -Wstrict-prototypes \
	   -Wold-style-definition

## DEBUG flags ##
#CFLAGS  := -Iinclude -c -ggdb -MMD -Wall -Wextra -std=c89 -pedantic-errors \
#	    -Wmissing-prototypes -Wstrict-prototypes -Wold-style-definition

all: $(OBJECTS)
	$(AR) $(LFLAGS) $(LIB) $(OBJECTS)

-include $(DEPS)

$(OBJECTS): $(patsubst build/%.o,src/%.c,$@)
	$(CC) $(CFLAGS) $(patsubst build/%.o,src/%.c,$@) -o $@

## WINDOWS ##
clean:
	del /q build\*
	del /q lib\*

## UNIX ##
#clean:
#	rm build/*
#	rm lib/*