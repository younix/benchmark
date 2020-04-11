#include <err.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "dummy.h"

static bool loop = true;

static void
signal_handler(int sig)
{
	if (sig == SIGALRM)
		loop = false;
}

#define LOOP(seconds, counter)                                  \
		if (alarm(seconds) == (unsigned int)-1)         \
			err(EXIT_FAILURE, "alarm");             \
		for (loop = true, counter = 0; loop; counter++)


int
main(void)
{
	size_t counter;
	unsigned int seconds = 5;

	size_t size = 128 * 1024 * 1024;
	char *src = malloc(size);
	char *dst = malloc(size);

	signal(SIGALRM, signal_handler);

	uint8_t *s = (uint8_t *) src;
	uint8_t *d = (uint8_t *) dst;

	LOOP(seconds, counter)
		for (; s < (uint8_t *)src + size; *d++ = *s++);
	printf("%zu\n", counter / seconds);

//	LOOP(seconds, counter)
//		memcpy(dst, src, size);
//	printf("%zu\n", counter / seconds);

	return EXIT_SUCCESS;
}
