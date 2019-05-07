#ifndef _UNISTD_H

#define _UNISTD_H
#ifdef WIN32
#include <Windows.h>
#include <getopt.h>
#include <io.h>
#include <process.h>
#define usleep(n) _sleep(n / 1000000)
#define sleep(n) _sleep(n)
#endif
#endif /* _UNISTD_H */