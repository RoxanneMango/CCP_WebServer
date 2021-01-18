#ifndef THREAD_H
#define THREAD_H

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
	#include "mingw.thread.h"
#elif defined(__linux__) || (__unix__) || defined(_POSIX_VERSION)
	#include <thread>
#else
	#error "Unknown compiler"
#endif

#endif // THREAD_H