#include <err.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <iostream>

static bool loop = true;

static void
signal_handler(int sig)
{
	if (sig == SIGALRM)
		loop = false;
}

#define LOOP(seconds, counter) 					\
		if (alarm(seconds) == (unsigned int)-1)		\
			err(EXIT_FAILURE, "alarm");		\
		for (loop = true, counter = 0; loop; counter++)

int
main(void)
{
	size_t counter;
	unsigned int seconds = 5;

	signal(SIGALRM, signal_handler);

	FILE *fh_printf = stderr;
	FILE *fh_printfs = stderr;
	FILE *fh_puts = stderr;

	size_t size = BUFSIZ;
	char str[size];
	memset(str, 'a', sizeof str);
	str[size - 1] = '\0';

	LOOP(seconds, counter)
		std::cout << str;
	fprintf(fh_puts, "%zu %zu std::cout << str\n", size, counter / seconds);

	return EXIT_SUCCESS;
}
