FLAGS = -O3
SOURCES = Core.c Memory.c posix_main.c
OBJECTS = $(addprefix Build/, $(patsubst %.c,%.o,$(SOURCES)))
HEADERS = Core.h Memory.h Types.h

all: picboy

debug: picboy
debug: FLAGS = -g3 -D _DEBUG=1

$(OBJECTS): Build/%.o : %.c $(HEADERS)
	@mkdir -p $(@D)
	@clang -pedantic $(FLAGS) $< -c -o $@

picboy: $(OBJECTS)
	@clang -pedantic $^ -o $@

.PHONY: clean
clean:
	@rm -rf picboy Core.ll Core.s picboy.dSYM/*