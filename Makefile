all: picboy
picboy:
	@cc -pedantic Memory.c posix_main.c -o picboy

.PHONY: clean
clean:
	@rm picboy