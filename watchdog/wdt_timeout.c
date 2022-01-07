#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <limits.h>
#include <pthread.h>

#define error_en(en, msg) \
	do { \
		errno = en; \
		perror(msg); \
		exit(EXIT_FAILURE); \
	} while (0)

static pthread_spinlock_t spinlock;

static void help(char *program)
{
	printf("%s <cores>\n", program);
	printf("\tcores: Number of CPU cores.\n");
}

static void *test_thread(void *arg)
{
	int ret = 0;
	cpu_set_t cpuset;

	printf("Thread %zu created\n", (size_t)arg);

	CPU_ZERO(&cpuset);
	CPU_SET((size_t)arg, &cpuset);
	ret = pthread_setaffinity_np(pthread_self(), sizeof(cpuset), &cpuset);
	if (ret)
		error_en(ret, "Set affinity failed");

	ret = pthread_spin_lock(&spinlock);
	if (ret)
		error_en(ret, "Spinlock lock failed");

	while (1) ;

	return NULL;
}

int main(int argc, char *argv[])
{
	unsigned long int cores = 0;
	int ret = 0;
	void *result = NULL;
	pthread_t *t = NULL;

	if (argc < 2) {
		help(argv[0]);
		return -1;
	}

	cores = strtoul(argv[1], NULL, 10);
	if (cores == ULONG_MAX) {
		help(argv[0]);
		return -errno;
	}

	ret = pthread_spin_init(&spinlock, PTHREAD_PROCESS_PRIVATE);
	if (ret) {
		printf("Cannot initialize spinlock\n");
		return ret;
	}

	t = calloc(sizeof(pthread_t), cores);
	if (!t) {
		printf("Cannot allocate memory\n");
		return -1;
	}

	for (size_t i = 0; i != cores; i++) {
		ret = pthread_create(&t[i], NULL, test_thread, (void *)i);
		if (ret)
			error_en(ret, "Thread create failed");
	}

	for (size_t i = 0; i != cores; i++) {
		ret = pthread_join(t[i], &result);
		if (ret)
			error_en(ret, "Thread join failed");
	}

	free(t);
	t = NULL;
	pthread_spin_destroy(&spinlock);

	exit(EXIT_SUCCESS);
}
