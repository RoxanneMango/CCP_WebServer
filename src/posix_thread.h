#ifndef POSIX_THREAD_H
#define POSIX_THREAD_H

#include <pthread.h>

#include "thread.h"

class PosixThread
{
private:
	pthread_t thread;
public:
	PosixThread();
	~PosixThread();

	void createThread(void * function(void *), void * argv);
	void exitThread(void * return_value);
};

#endif // POSIX_THREAD_H