CC ?= cc
CFLAGS = -std=c99 -pedantic -Wall -Wextra

.PHONY: all clean test caller
all: fork static dynamic func lowfat print memcpy
clean:
	rm -f fork static dynamic func lowfat print memcpy

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

func: func.c
	$(CC) $(CFLAGS) -static -o $@ func.c

lowfat: lowfat.c
	$(CC) -static $(CFLAGS) -I/usr/local/include/lowfat -L/usr/local/lib \
	    -o $@ lowfat.c -lowfat 

print: print.c
	$(CC) $(CFLAGS) -o $@ print.c

memcpy: memcpy.c
	$(CC) $(CFLAGS) -o $@ memcpy.c

fork+exec.dat: fork static dynamic
	./fork -E > $@
	./fork -e ./static | sed -e 's/fork+exec/static/' >> $@
	./fork -e ./dynamic | sed -e 's/fork+exec/dynamic/' >> $@

fork.dat: fork
	./fork -E	> $@
	./fork		>> $@
	./fork -Et	>> $@

printf.dat: print
	./print > /dev/null 2>$@

test: all
	@echo -n "threads: "
	@./fork -t
	@echo -n "forks:   "
	@./fork
	@echo -n "statics: "
	@./fork -e ./static
	@echo -n "dynamics:"
	@./fork -e ./dynamic
	@echo -n "min:     "
	@./fork -e ./min

rust/true: rust/true.rs
	rustc -o $@ rust/true.rs

test2: all
	@./fork
	@./fork -e ./static
	@./fork -e ./dynamic
	@./fork -e ./go/true
	@./fork -e ./rust/true
	@./fork -e ./true/true.ksh
	@./fork -e ./true/true.bash
	@./fork -e ./true/true.pl
	@./fork -e ./true/true.rb
	@./fork -e ./true/true.py2
	@./fork -e ./true/true.py3

caller: static_caller dynamic_caller
	@echo -n "statics: "
	@./static_caller
	@echo -n "dynamics:"
	@./dynamic_caller

#.include <bsd.lib.mk>
