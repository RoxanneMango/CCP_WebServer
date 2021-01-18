#include "posix_thread.h"

PosixThread::PosixThread()
{}
PosixThread::~PosixThread()
{}

void 
PosixThread::createThread(void * function(void *), void * argv)
{
	pthread_create(&thread, NULL, function, argv);
}

void 
PosixThread::exitThread(void * return_value)
{
	pthread_exit(return_value);
}