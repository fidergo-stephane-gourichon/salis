CC      := gcc
AR      := ar
LIB     := lib\libsalis.a
TSALIS  := ..\..\apps\tsalis

SOURCES := $(wildcard src/*.c)
OBJECTS := $(patsubst src/%.c,build/%.o,$(SOURCES))
DEPS    := $(patsubst %.o,%.d,$(OBJECTS))

LFLAGS  := rcs
#CFLAGS  := -Iinclude -c -ggdb -MMD -Wall -Wextra -std=c89 -pedantic-errors \
#           -Wmissing-prototypes -Wstrict-prototypes -Wold-style-definition
CFLAGS  := -Iinclude -c -O3 -DNDEBUG -MMD -Wall -Wextra -std=c89 -pedantic-errors \
           -Wmissing-prototypes -Wstrict-prototypes -Wold-style-definition

all: $(OBJECTS)
	$(AR) $(LFLAGS) $(LIB) $(OBJECTS)

exe: $(OBJECTS)
	$(AR) $(LFLAGS) $(LIB) $(OBJECTS)
	make -C $(TSALIS)

-include $(DEPS)

$(OBJECTS): $(patsubst build/%.o,src/%.c,$@)
	$(CC) $(CFLAGS) $(patsubst build/%.o,src/%.c,$@) -o $@

clean:
	del /q build\*
	del /q lib\*
