#include <sys/wait.h>

#include <err.h>
#include <errno.h>
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
forking(bool exec_enoent, const char *exec_str)
{
	size_t counter;
	int status;

	for (counter = 0; loop; counter++) {
		switch (fork()) {
		case -1:
			err(EXIT_FAILURE, "fork");
		case 0:
			if (exec_str != NULL) {
				execlp(exec_str, exec_str, NULL);
				if (!exec_enoent || errno != ENOENT)
					err(EXIT_FAILURE, "execl");
			}
			exit(EXIT_SUCCESS);
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
	fputs("fork [-Et] [-e path] [-s sec]\n", stderr);
	exit(EXIT_FAILURE);
}

int
main(int argc, char *argv[])
{
	size_t counter;
	const char *errstr;
	char *exec_str = NULL;
	unsigned int seconds = 5;
	int ch;
	bool exec_enoent = false;
	bool thread_flag = false;
	bool fork_flag = false;

	if (argc == 1)
		_exit(0);

	/* parameter handling */
	while ((ch = getopt(argc, argv, "Ee:fs:t")) != -1) {
		switch (ch) {
		case 'E':
			exec_enoent = true;
			break;
		case 'e':
			exec_str = optarg;
			break;
		case 'f':
			fork_flag = true;
			break;
		case 's':
			seconds = strtonum(optarg, 1, UINT_MAX, &errstr);
			if (errstr != NULL)
				errx(EXIT_FAILURE, "strtonum: %s", errstr);
			break;
		case 't':
			thread_flag = true;
			break;
		case 'h':
		default:
			usage();
		}
	}
	argc -= optind;
	argv += optind;

	/* signal handling */
	signal(SIGALRM, signal_handler);

	/* set timer */
	if (alarm(seconds) == (unsigned int)-1)
		err(EXIT_FAILURE, "alarm");

	/* doing */
	if (thread_flag)
		counter = threading();
	if (fork_flag)
		counter = forking(exec_enoent, exec_str);

	printf("%6zu ", counter / seconds);
	if (exec_str != NULL)
		printf("fork+exec %s\n", exec_str);
	else if (thread_flag)
		puts("thread");
	else
		puts("fork");

	return EXIT_SUCCESS;
}
