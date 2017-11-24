all: picboy
picboy:
	@cc -pedantic Core.c Memory.c posix_main.c -o picboy

.PHONY: clean
clean:
	@rm picboy