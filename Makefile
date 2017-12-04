FLAGS = -O3

all: picboy

debug: picboy
debug: FLAGS = -g3

picboy:
	@cc -pedantic $(FLAGS) Core.c Memory.c posix_main.c -o picboy

.PHONY: clean
clean:
	@rm picboy