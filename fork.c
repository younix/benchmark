#include <sys/wait.h>
#include <sys/types.h>
#include <sys/sysctl.h>

#include <err.h>
#include <errno.h>
#include <limits.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static bool loop = true;

static void *
dummy(void *arg)
{
	return arg;
}

static size_t
threading(void)
{
	size_t counter;
	pthread_t thread;
	int ret;

	for (counter = 0; loop; counter++) {
		if ((ret = pthread_create(&thread, NULL, dummy, NULL)) != 0)
			err(EXIT_FAILURE, "pthread_create: %d", ret);
		if ((ret = pthread_join(thread, NULL)) != 0)
			err(EXIT_FAILURE, "pthread_join: %d", ret);
	}

	return counter;
}

static size_t
forking(bool exec_enoent, char *argv[])
{
	size_t counter;
	int status;

	for (counter = 0; loop; counter++) {
		switch (fork()) {
		case -1:
			err(EXIT_FAILURE, "fork");
		case 0:
			if (argv[0] != NULL) {
				execvp(argv[0], argv);
				if (!exec_enoent || errno != ENOENT)
					err(EXIT_FAILURE, "execl");
			}
			_exit(EXIT_SUCCESS);
		default:
 again:
			if (wait(&status) == -1) {
				if (errno == EINTR)
					goto again;
				err(EXIT_FAILURE, "wait");
			}
			break;
		}
	}

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
	fputs("fork [-E] [-j jobs] [-s sec] thread\n", stderr);
	fputs("fork [-E] [-j jobs] [-s sec] fork [cmd [args [...]]]\n", stderr);

	exit(EXIT_FAILURE);
}

int
main(int argc, char *argv[])
{
	size_t counter = 0;
	const char *errstr;
	unsigned int seconds = 5;
	int ch;
	bool exec_enoent = false;
	int ncpu = 1;
	size_t size = sizeof ncpu;
	int name[2] = { CTL_HW, HW_NCPUONLINE };

#ifdef __OpenBSD__
	if (sysctl(name, 2, &ncpu, &size, NULL, 0))
		err(EXIT_FAILURE, "sysctl");
#endif

	if (argc == 1)
		_exit(0);

	/* parameter handling */
	while ((ch = getopt(argc, argv, "Ee:fj:s:t")) != -1) {
		switch (ch) {
		case 'E':
			exec_enoent = true;
			break;
		case 'j':
			ncpu = strtonum(optarg, 1, INT_MAX, &errstr);
			if (errstr != NULL)
				errx(EXIT_FAILURE, "strtonum: %s", errstr);
			break;
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

	if (argc < 1)
		usage();

	char *test = argv[0];

	argc -= optind;
	argv += optind;

	/* signal handling */
	signal(SIGALRM, signal_handler);

	int fd[ncpu][2];

	for (int i = 0; i < ncpu; i++) {

		if (pipe(fd[i]) == -1)
			err(EXIT_FAILURE, "pipe");

		switch (fork()) {
		case -1:
			err(EXIT_FAILURE, "fork");
		case 0:	/* child */
			/* close the reading side */
			if (close(fd[i][0]) == -1)
				err(EXIT_FAILURE, "close");

			/* set timer */
			if (alarm(seconds) == (unsigned int)-1)
				err(EXIT_FAILURE, "alarm");

			/* doing */
			if (strcmp(test, "thread") == 0)
				counter = threading();
			else if (strcmp(test, "fork") == 0)
				counter = forking(exec_enoent, argv);
			else
				usage();

			/* write results to master */
			if (write(fd[i][1], &counter, sizeof counter) !=
			    sizeof counter)
				err(EXIT_FAILURE, "write");

			exit(EXIT_SUCCESS);
			break;
		default: /* parent */
			/* close the writing side */
			if (close(fd[i][1]) == -1)
				err(EXIT_FAILURE, "close");
		}
	}

	/* collect the results */
	for (int i = 0; i < ncpu; i++) {
		size_t c;

		if (read(fd[i][0], &c, sizeof c) != sizeof c)
			err(EXIT_FAILURE, "read");
		counter += c;
	}

	printf("%6zu %s\n", counter / seconds, test);

	return EXIT_SUCCESS;
}
