#ifndef SLEEP_H
#define SLEEP_H

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
	#include <windows.h>
	#define sleep Sleep
	#define SLEEP(x) Sleep(x*1000)
#elif defined(__linux__) || (__unix__) || defined(_POSIX_VERSION)
	#include <unistd.h>
	#define SLEEP(x) sleep(x)
#else
	#error "Unknown compiler"
#endif

#endif