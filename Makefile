CC ?= cc
CFLAGS = -std=c99 -pedantic -Wall -Wextra

.PHONY: all clean test caller
all: fork static dynamic
clean:
	rm -f fork static dynamic

fork: fork.c
	$(CC) $(CFLAGS) -lpthread -o $@ fork.c

static: dummy.c
	$(CC) $(CFLAGS) -static -o $@ dummy.c

dynamic: dummy.c
	$(CC) $(CFLAGS) -o $@ dummy.c

static_caller: caller.c libdummy.a
	$(CC) $(CFLAGS) -o $@ caller.c -static -L. -ldummy

dynamic_caller: caller.c libdummy.so
	$(CC) $(CFLAGS) -o $@ caller.c -L. -ldummy

test: all
	@echo -n "threads: "
	@./fork -t
	@echo -n "forks:   "
	@./fork -E
	@echo -n "statics: "
	@./fork -e ./static
	@echo -n "dynamics:"
	@./fork -e ./dynamic

caller: static_caller dynamic_caller
	@echo -n "statics: "
	@./static_caller
	@echo -n "dynamics:"
	@./dynamic_caller

#.include <bsd.lib.mk>
