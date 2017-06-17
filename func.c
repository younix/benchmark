#include <sys/wait.h>

#include <err.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static bool loop = true;
static size_t counter = 0;

inline static void
increment_global(void)
{
	counter++;
}

inline static void
increment_reference(size_t *i)
{
	(*i)++;
}

static size_t
increment_value(size_t i)
{
	return ++i;
}

static void
signal_handler(int sig)
{
	if (sig == SIGALRM)
		loop = false;
}

int
main(void)
{
	unsigned int seconds = 5;

	/* signal handling */
	signal(SIGALRM, signal_handler);

	/*
	 * just increment 
	 */
	if (alarm(seconds) == (unsigned int)-1)
		err(EXIT_FAILURE, "alarm");
	for (counter = 0, loop = true; loop; counter++);
	printf("plain: %5zu/s\n", counter / seconds);

	/*
	 * increment global
	 */
	if (alarm(seconds) == (unsigned int)-1)
		err(EXIT_FAILURE, "alarm");
	for (counter = 0, loop = true; loop; increment_global());
	printf("globl: %5zu/s\n", counter / seconds);

	/*
	 * increment by reference
	 */
	if (alarm(seconds) == (unsigned int)-1)
		err(EXIT_FAILURE, "alarm");
	for (counter = 0, loop = true; loop; increment_reference(&counter));
	printf("refer: %5zu/s\n", counter / seconds);

	/*
	 * increment by value
	 */
	if (alarm(seconds) == (unsigned int)-1)
		err(EXIT_FAILURE, "alarm");
	for (counter = 0, loop = true; loop; counter = increment_value(counter));
	printf("value: %5zu/s\n", counter / seconds);

	return EXIT_SUCCESS;
}
