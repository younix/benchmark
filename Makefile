CC ?= cc
CFLAGS += -std=c99 -pedantic -Wall -Wextra

.PHONY: all clean test caller
all: fork static dynamic func print memcpy syscall
clean:
	rm -f fork static dynamic func print number memcpy \
	    go/true

fork: fork.c
	$(CC) $(CFLAGS) -static -o $@ fork.c -lpthread $(LDLIBS)

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
	$(CC) $(CFLAGS) -O0 -Wno-format-security -o $@ print.c $(LDLIBS)

number: number.c
	$(CC) $(CFLAGS) -O0 -o $@ number.c $(LDLIBS)

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
	#
	# singel threaded
	#
	@echo -n "threads: "
	@./fork -j 1 thread | tee fork.dat
	@echo -n "forks:   "
	@./fork -j 1 fork | tee -a fork.dat
	@echo -n "self:    "
	@./fork -j 1 fork ./fork | tee -a fork.dat
	@echo -n "statics: "
	@./fork -j 1 fork ./static | tee -a fork.dat
	@echo -n "dynamics:"
	@./fork -j 1 fork ./dynamic | tee -a fork.dat
	@echo -n "fork cc: "
	@./fork -j 1 fork /usr/bin/cc -o dummy dummy.c
	#
	# multi threaded
	#
	@echo -n "threads: "
	@./fork thread | tee fork.dat
	@echo -n "forks:   "
	@./fork fork | tee -a fork.dat
	@echo -n "self:    "
	@./fork fork ./fork | tee -a fork.dat
	@echo -n "statics: "
	@./fork fork ./static | tee -a fork.dat
	@echo -n "dynamics:"
	@./fork fork ./dynamic | tee -a fork.dat
	@echo -n "fork cc: "
	@./fork fork /usr/bin/cc -o dummy dummy.c

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
