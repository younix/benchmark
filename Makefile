CC ?= cc
CFLAGS = -std=c99 -pedantic -Wall -Wextra

.PHONY: all clean test caller
all: fork static dynamic func print number memcpy
clean:
	rm -f fork static dynamic func print number memcpy \
	    go/true

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

#lowfat: lowfat.c
#	$(CC) -static $(CFLAGS) -I/usr/local/include/lowfat -L/usr/local/lib \
#	    -o $@ lowfat.c -lowfat

print: print.c
	$(CC) $(CFLAGS) -O0 -Wno-format-security -o $@ print.c

number: number.c
	$(CC) $(CFLAGS) -O0 -o $@ number.c

number.dat: number
	./number -s 1 > $@

number.svg: number.dat
	cat number.dat | ./plotter > $@

memcpy: memcpy.c
	$(CC) $(CFLAGS) -o $@ memcpy.c

fork+exec.dat: fork static dynamic
	./fork > $@
	./fork -e ./static | sed -e 's/fork+exec/static/' >> $@
	./fork -e ./dynamic | sed -e 's/fork+exec/dynamic/' >> $@

fork.dat: fork static
	./fork -t		> $@
	./fork			>> $@
	./fork	-Ee notexist	>> $@
	./fork	-e ./static	>> $@

fork.svg: fork.dat plotter
	./plotter < fork.dat > $@

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

langcmp.dat:
	@./fork -s1			| sed -e 's/fork+exec.*$$/fork/'	> $@
	@./fork -s1 -Ee notexist	| sed -e 's/fork+exec.*$$/notexist/'	>> $@
	@./fork -s1 -e ./static		| sed -e 's/fork+exec.*$$/static/'	>> $@
	@./fork -s1 -e ./dynamic	| sed -e 's/fork+exec.*$$/dynamic/'	>> $@
	@./fork -s1 -e ./go/true	| sed -e 's/fork+exec.*$$/Go/'		>> $@
	@./fork -s1 -e ./rust/true	| sed -e 's/fork+exec.*$$/Rust/'	>> $@
	@./fork -s1 -e ./true/true.ksh	| sed -e 's/fork+exec.*$$/"ksh (static)"/'	>> $@
	@./fork -s1 -e ./true/true.bash	| sed -e 's/fork+exec.*$$/Bash/'	>> $@
	@./fork -s1 -e ./true/true.pl	| sed -e 's/fork+exec.*$$/Perl/'	>> $@
	@./fork -s1 -e ./true/true.rb	| sed -e 's/fork+exec.*$$/Ruby/'	>> $@
	@./fork -s1 -e ./true/true.py2	| sed -e 's/fork+exec.*$$/Python2/'	>> $@
	@./fork -s1 -e ./true/true.py3	| sed -e 's/fork+exec.*$$/Python3/'	>> $@

langcmp_zoom.svg: langcmp.dat
	sort -rn langcmp.dat \
		| grep -Ev '(fork|notexist|static)' \
		| ./plotter > $@

.PHONY: langcmp.svg langcmp_zoom.svg
langcmp.svg: langcmp.dat
	sort -rn langcmp.dat | ./plotter > $@

caller: static_caller dynamic_caller
	@echo -n "statics: "
	@./static_caller
	@echo -n "dynamics:"
	@./dynamic_caller
