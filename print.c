#define _XOPEN_SOURCE 700
#include <err.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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
main(int argc, char *argv[])
{
	size_t counter;
	unsigned int seconds = 5;
	int ch;
	const char *errstr;

	while ((ch = getopt(argc, argv, "s:h")) != -1) {
		switch (ch) {
		case 's':
			seconds = strtonum(optarg, 1, 100000000, &errstr);
			if (errstr != NULL)
				errx(EXIT_FAILURE, "strtonum: %s", errstr);
			break;
		case 'h':
		default:
			errx(EXIT_FAILURE, "print [-s sec]");
		}
	}
	argc -= optind;
	argv += optind;

	if (setvbuf(stdout, NULL, _IONBF, 0) != 0)
		err(EXIT_FAILURE, "setvbuf");

	signal(SIGALRM, signal_handler);

//	FILE *fh_printf = fopen("printf.dat", "w");
//	FILE *fh_printfs = fopen("printfs.dat", "w");
//	FILE *fh_puts = fopen("puts.dat", "w");

	FILE *fh_printf = stderr;
	FILE *fh_printfs = stderr;
	FILE *fh_puts = stderr;

//	for (size_t size = 1;  size <= BUFSIZ; size *= 2) {
		size_t size = BUFSIZ;
		char str[size];
		memset(str, 'a', sizeof str);
		str[size - 1] = '\0';

		LOOP(seconds, counter)
			printf(str);
		fprintf(fh_printf, "%zu %zu printf(str)\n", size, counter / seconds);

		LOOP(seconds, counter)
			printf("%s", str);
		fprintf(fh_printfs, "%zu %zu printf(\"%%s\", str)\n", size, counter / seconds);

		LOOP(seconds, counter)
			puts(str);
		fprintf(fh_puts, "%zu %zu puts(str)\n", size, counter / seconds);

		LOOP(seconds, counter)
			write(STDOUT_FILENO, str, sizeof str);
		fprintf(fh_puts, "%zu %zu write(1, str, sizeof str)\n", size, counter / seconds);

//		LOOP(seconds, counter)
//			std::cout << str;
//		fprintf(fh_puts, "%zu %zu std::cout << str\n", size, counter / seconds);
//	}

//	fclose(fh_printf);
//	fclose(fh_printfs);
//	fclose(fh_puts);

	return EXIT_SUCCESS;
}
