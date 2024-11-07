#include <err.h>
#include <limits.h>
#include <math.h>
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

void
usage(void)
{
	fputs("number [-h] [-s sec]\n", stderr);
}

int
main(int argc, char *argv[])
{
	char ch;
	size_t counter;
	unsigned int seconds = 5;
	const char *errstr = NULL;

	while ((ch = getopt(argc, argv, "s:h")) != -1) {
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


	if (setvbuf(stdout, NULL, _IONBF, 0) != 0)
		err(EXIT_FAILURE, "setvbuf");

	signal(SIGALRM, signal_handler);

	/*
	 * prepare test data
	 */
	char ullstr[BUFSIZ];
	char ulstr[BUFSIZ];
	char ustr[BUFSIZ];

	char illstr[BUFSIZ];
	char ilstr[BUFSIZ];
	char istr[BUFSIZ];

	char ldstr[BUFSIZ];
	char dstr[BUFSIZ];
	char fstr[BUFSIZ];

	char str0[BUFSIZ] = "0";

	long long unsigned int llu = ULLONG_MAX;
	long unsigned int lu = ULONG_MAX;
	unsigned int u = UINT_MAX;

	long long int lli = LLONG_MAX;
	long int li = LONG_MAX;
	int i = INT_MAX;

	long double ld = MAXFLOAT;
	double d = MAXFLOAT;
	float f = MAXFLOAT;

	snprintf(ullstr, sizeof ullstr, "%llu", llu);
	snprintf(ulstr, sizeof ulstr, "%lu", lu);
	snprintf(ustr, sizeof ustr, "%u", u);
	snprintf(istr, sizeof istr, "%i", i);

	snprintf(ldstr, sizeof ldstr, "%Lf", ld);
	snprintf(dstr, sizeof dstr, "%lf", d);
	snprintf(fstr, sizeof fstr, "%f", f);

	/*
	 * scanf(3)
	 */
	LOOP(seconds, counter)
		sscanf(ullstr, "%llu", &llu);
	printf("%zu scanf(str, \"%%llu\", &var)\n", counter / seconds);

	LOOP(seconds, counter)
		sscanf(ulstr, "%lu", &lu);
	printf("%zu scanf(str, \"%%lu\", &var)\n", counter / seconds);

	LOOP(seconds, counter)
		sscanf(ustr, "%u", &u);
	printf("%zu scanf(str, \"%%u\", &var)\n", counter / seconds);

	/*
	 * atoi(3)
	 */
	LOOP(seconds, counter)
		lli = atoll(illstr);
	printf("%zu atoll(str)\n", counter / seconds);

	LOOP(seconds, counter)
		li = atol(ilstr);
	printf("%zu atol(str)\n", counter / seconds);

	LOOP(seconds, counter)
		i = atoi(istr);
	printf("%zu atoi(str)\n", counter / seconds);

	/*
	 * strtou{l,ll}(3)
	 */
	LOOP(seconds, counter)
		llu = strtoull(ullstr, NULL, 0);
	printf("%zu strtoull(str, NULL, 0)\n", counter / seconds);

	LOOP(seconds, counter)
		lu = strtoul(ulstr, NULL, 0);
	printf("%zu strtoul(str, NULL, 0)\n", counter / seconds);

	LOOP(seconds, counter)
		lu = strtoul(ulstr, NULL, 10);
	printf("%zu strtoul(str, NULL, 10)\n", counter / seconds);

	/*
	 * strto{l,ll}(3)
	 */
	LOOP(seconds, counter)
		lli = strtoll(illstr, NULL, 0);
	printf("%zu strtol(str, NULL, 0)\n", counter / seconds);

	LOOP(seconds, counter)
		li = strtol(ilstr, NULL, 0);
	printf("%zu strtol(str, NULL, 0)\n", counter / seconds);

	/*
	 * strto{ld,d,f}
	 */
	LOOP(seconds, counter)
		ld = strtold(dstr, NULL);
	printf("%zu strtold(str, NULL)\n", counter / seconds);

	LOOP(seconds, counter)
		d = strtod(dstr, NULL);
	printf("%zu strtod(str, NULL)\n", counter / seconds);

	LOOP(seconds, counter)
		f = strtof(fstr, NULL);
	printf("%zu strtof(str, NULL)\n", counter / seconds);

	return EXIT_SUCCESS;
}
