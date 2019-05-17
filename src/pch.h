#ifndef __PCH_H__
#   define __PCH_H__

#   if defined WIN32

// Yes, we have no intentions of running this software under Windows.
// However, one of the developers uses Visual Studio as their IDE, and it
// doesn't recognize POSIX stuff. They don't even try to *build* under
// Windows; they just want the IntelliSense® source browsing &c to work.

#       define _CRT_NONSTDC_NO_WARNINGS

#   endif // defined WIN32

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pwd.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>

#include <cerrno>
#include <climits>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>

#include <algorithm>
#include <exception>
#include <functional>
#include <future>
#include <numeric>
#include <stdexcept>
#include <vector>

#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include <QtOpenGL>
#include <QtXml/QtXml>

#include "constants.h"
#include "debug.h"

#   if defined WIN32

#       include <io.h>

using error_t = int;

int const R_OK = 4;
int const W_OK = 2;
int const X_OK = 1;
int const F_OK = 0;

int const CLOCK_BOOTTIME = 7;

#define SIGHUP   1
#define SIGQUIT  3
#define SIGUSR1 10

struct passwd {
    char* pw_name;
};

extern int access( char const* pathName, int mode );
extern int clock_gettime( int clk_id, timespec* tp );
extern int getpid( );
extern int getpwuid_r( int uid, passwd* pwd, char* buf, size_t buflen, passwd** result );
extern int getuid( );
extern struct tm* gmtime_r( time_t const* timep, tm* result );
extern int mkdir( char const* pathName, int mode );
extern unsigned int sleep( unsigned int seconds );
extern char* strsignal( int sig );

#   endif // defined WIN32

#endif // __PCH_H__
