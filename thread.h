#ifndef THREAD_H
#define THREAD_H

class Thread
{
public:
	virtual void createThread(void * function(void *), void * argv) = 0;
	virtual void exitThread(void * return_value) = 0;
};

#endif // THREAD_H