#include "pch.h"

#include "debug.h"

#if defined _DEBUG

namespace {

    FILE* DebugLog       { stderr };
    FILE* OriginalStderr { stderr };

    char const* DebugLogPaths[] {
        "/var/log/lightfield/debug.log",
        "/var/log/lightfield/debug.1.log",
        "/var/log/lightfield/debug.2.log",
        "/var/log/lightfield/debug.3.log",
        "/var/log/lightfield/debug.4.log",
        "/var/log/lightfield/debug.5.log",
    };

}

DebugManager::DebugManager( ) {
    ::unlink( DebugLogPaths[5] );
    for ( int n = 5; n > 0; --n ) {
        ::rename( DebugLogPaths[n - 1], DebugLogPaths[n] );
    }

    DebugLog = ::fopen( DebugLogPaths[0], "wtx" );
    if ( !DebugLog ) {
        error_t err = errno;
        ::fprintf( stderr, "failed to open log file '%s': %s [%d]", DebugLogPaths[0], strerror( err ), err );
        DebugLog = stderr;
    } else {
        // save the original stderr
        int fd = ::dup( 2 );

        // redirect stderr to the debug log
        ::dup2( ::fileno( DebugLog ), 2 );

        // get a FILE* for the original stderr
        OriginalStderr = ::fdopen( fd, "wt" );

        // disable buffering on both FILE*:s
        ::setvbuf( DebugLog,       nullptr, _IONBF, 0 );
        ::setvbuf( OriginalStderr, nullptr, _IONBF, 0 );
    }
}

DebugManager::~DebugManager( ) {
    /*empty*/
}

void debug( char const* str ) {
    ::fputs( str, DebugLog       );
    ::fputs( str, OriginalStderr );
}

#endif // defined _DEBUG
