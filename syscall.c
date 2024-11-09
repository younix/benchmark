#include <sys/wait.h>

#include <err.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static bool loop = true;

static size_t
sys_getpid(void)
{
	size_t counter;

	for (counter = 0; loop; counter++)
		getpid();

	return counter;
}

static void
signal_handler(int sig)
{
	if (sig == SIGALRM)
		loop = false;
}

static void
usage(void)
{
	fputs("syscall [-s sec] syscall\n", stderr);
	exit(EXIT_FAILURE);
}

int
main(int argc, char *argv[])
{
	size_t counter = 0;
	const char *errstr;
	unsigned int seconds = 5;
	int ch;

	if (argc == 1)
		_exit(0);

	/* parameter handling */
	while ((ch = getopt(argc, argv, "Ee:fs:t")) != -1) {
		switch (ch) {
		case 's':
			seconds = strtonum(optarg, 1, UINT_MAX, &errstr);
			if (errstr != NULL)
				errx(EXIT_FAILURE, "strtonum: %s", errstr);
			break;
		case 'h':
		default:
			usage();
		}
	}
	argc -= optind;
	argv += optind;

	if (argc == 0)
		usage();

	/* signal handling */
	signal(SIGALRM, signal_handler);

	/* set timer */
	if (alarm(seconds) == (unsigned int)-1)
		err(EXIT_FAILURE, "alarm");

	if (strcmp(argv[0], "getpid") == 0)
		counter = sys_getpid();

	printf("%6zu %s\n", counter / seconds, argv[0]);

	return EXIT_SUCCESS;
}
