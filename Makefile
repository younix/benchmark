CC ?= cc
CFLAGS = -std=c99 -pedantic -Wall -Wextra

.PHONY: all clean
all: fork static dynamic
clean:
	rm -f fork static dynamic

fork: fork.c
	$(CC) $(CFLAGS) -lpthread -o $@ fork.c

static: dummy.c
	$(CC) -static $(CFLAGS) -o $@ dummy.c

dynamic: dummy.c
	$(CC) $(CFLAGS) -o $@ dummy.c

test: all
	@echo -n "threads: "
	@./fork -t
	@echo -n "forks:   "
	@./fork -E
	@echo -n "statics: "
	@./fork -e ./static
	@echo -n "dynamics:"
	@./fork -e ./dynamic
