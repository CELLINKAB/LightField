#include "pch.h"

#include "strings.h"

#include "filetab.h"
#include "printmanager.h"
#include "printtab.h"
#include "shepherd.h"
#include "window.h"

namespace {

    char const* ProcessErrorStrings[] {
        "FailedToStart",
        "Crashed",
        "Timedout",
        "WriteError",
        "ReadError",
        "UnknownError",
    };

    char const* ProcessStateStrings[] {
        "NotRunning",
        "Starting",
        "Running",
    };

    char const* ExitStatusStrings[] {
        "NormalExit",
        "CrashExit",
    };

    char const* DialogCodeStrings[] {
        "Rejected",
        "Accepted",
    };

    char const* PendingCommandStrings[] {
        "none",
        "moveRelative",
        "moveAbsolute",
        "home",
        "send",
    };

    char const* TabIndexStrings[] {
        "File",
        "Prepare",
        "Print",
        "Status",
        "Advanced",
        "Maintenance",
    };

    char const* SwipeDirectionStrings[] {
        "NoDirection",
        "Left",
        "Right",
        "Up",
        "Down",
    };

    char const* GestureStateStrings[] {
        "NoGesture",
        "GestureStarted",
        "GestureUpdated",
        "GestureFinished",
        "GestureCanceled",
    };

    char const* UiStateStrings[] {
        "SelectStarted",
        "SelectCompleted",
        "SliceStarted",
        "SliceCompleted",
        "PrintStarted",
        "PrintCompleted",
    };

    char const* PrintResultStrings[] {
        "None",
        "Failure",
        "Success",
        "Abort",
    };

    char const* PrintStepStrings[] {
        "none",
        "A1", "A2", "A3", "A4", "A5",
        "B1", "B2", "B3", "B4", "B5", "B6", "B7",
        "C1", "C2"
    };

    char const* BuildPlatformStateStrings[] {
        "Lowered",
        "Raising",
        "Raised",
        "Lowering",
    };

    char const* ModelsLocationStrings[] {
        "Library",
        "Usb",
    };

    void _BreakDownTime( uint64_t totalSeconds, int& days, int& hours, int& minutes, int& seconds ) {
        seconds = totalSeconds % 60; totalSeconds /= 60;
        minutes = totalSeconds % 60; totalSeconds /= 60;
        hours   = totalSeconds % 24;
        days    = totalSeconds / 24;
    }

}

char const* ToString( QProcess::ProcessError const value ) {
#if defined _DEBUG
    if ( ( value >= QProcess::FailedToStart ) && ( value <= QProcess::UnknownError ) ) {
#endif
        return ProcessErrorStrings[value];
#if defined _DEBUG
    } else {
        return nullptr;
    }
#endif
}

char const* ToString( QProcess::ProcessState const value ) {
#if defined _DEBUG
    if ( ( value >= QProcess::NotRunning ) && ( value <= QProcess::Running ) ) {
#endif
        return ProcessStateStrings[value];
#if defined _DEBUG
    } else {
        return nullptr;
    }
#endif
}

char const* ToString( QProcess::ExitStatus const value ) {
#if defined _DEBUG
    if ( ( value >= QProcess::NormalExit ) && ( value <= QProcess::CrashExit ) ) {
#endif
        return ExitStatusStrings[value];
#if defined _DEBUG
    } else {
        return nullptr;
    }
#endif
}

char const* ToString( QDialog::DialogCode const value ) {
#if defined _DEBUG
    if ( ( value >= QDialog::Rejected ) && ( value <= QDialog::Accepted ) ) {
#endif
        return DialogCodeStrings[static_cast<int>( value )];
#if defined _DEBUG
    } else {
        return nullptr;
    }
#endif
}

QString ToString( QPoint const value ) {
    return QString( "(%1,%2)" ).arg( value.x( ) ).arg( value.y( ) );
}

QString ToString( QSize const value ) {
    return QString( "%1×%2" ).arg( value.width( ) ).arg( value.height( ) );
}

QString ToString( QRect const value ) {
    return QString( "%1-%2 [%3]" ).arg( ToString( value.topLeft( ) ) ).arg( ToString( value.bottomRight( ) ) ).arg( ToString( value.size( ) ) );
}

QString ToString( QPointF const value ) {
    return QString( "(%1,%2)" ).arg( value.x( ), 0, 'f', 2 ).arg( value.y( ), 0, 'f', 2 );
}

QString ToString( QSizeF const value ) {
    return QString( "%1×%2" ).arg( value.width( ), 0, 'f', 2 ).arg( value.height( ), 0, 'f', 2 );
}

QString ToString( QRectF const value ) {
    return QString( "%1-%2 [%3]" ).arg( ToString( value.topLeft( ) ) ).arg( ToString( value.bottomRight( ) ) ).arg( ToString( value.size( ) ) );
}

char const* ToString( PendingCommand const value ) {
#if defined _DEBUG
    if ( ( value >= PendingCommand::none ) && ( value <= PendingCommand::send ) ) {
#endif
        return PendingCommandStrings[static_cast<int>( value )];
#if defined _DEBUG
    } else {
        return nullptr;
    }
#endif
}

char const* ToString( TabIndex const value ) {
#if defined _DEBUG
    if ( ( value >= TabIndex::File ) && ( value <= TabIndex::Maintenance ) ) {
#endif
        return TabIndexStrings[static_cast<int>( value )];
#if defined _DEBUG
    } else {
        return nullptr;
    }
#endif
}

char const* ToString( QSwipeGesture::SwipeDirection const value ) {
#if defined _DEBUG
    if ( ( value >= QSwipeGesture::NoDirection ) && ( value <= QSwipeGesture::Down ) ) {
#endif
        return SwipeDirectionStrings[static_cast<int>( value )];
#if defined _DEBUG
    } else {
        return nullptr;
    }
#endif
}

char const* ToString( Qt::GestureState const value ) {
#if defined _DEBUG
    if ( ( value >= Qt::NoGesture ) && ( value <= Qt::GestureCanceled ) ) {
#endif
        return GestureStateStrings[static_cast<int>( value )];
#if defined _DEBUG
    } else {
        return nullptr;
    }
#endif
}

char const* ToString( UiState const value ) {
#if defined _DEBUG
    if ( ( value >= UiState::SelectStarted ) && ( value <= UiState::PrintCompleted ) ) {
#endif
        return UiStateStrings[static_cast<int>( value )];
#if defined _DEBUG
    } else {
        return nullptr;
    }
#endif
}

char const* ToString( PrintResult const value ) {
#if defined _DEBUG
    if ( ( value >= PrintResult::None ) && ( value <= PrintResult::Abort ) ) {
#endif
        return PrintResultStrings[static_cast<int>( value )];
#if defined _DEBUG
    } else {
        return nullptr;
    }
#endif
}

char const* ToString( PrintStep const value ) {
#if defined _DEBUG
    if ( ( value >= PrintStep::none ) && ( value <= PrintStep::C2 ) ) {
#endif
        return PrintStepStrings[static_cast<int>( value )];
#if defined _DEBUG
    } else {
        return nullptr;
    }
#endif
}

char const* ToString( BuildPlatformState const value ) {
#if defined _DEBUG
    if ( ( value >= BuildPlatformState::Lowered ) && ( value <= BuildPlatformState::Lowering ) ) {
#endif
        return BuildPlatformStateStrings[static_cast<int>( value )];
#if defined _DEBUG
    } else {
        return nullptr;
    }
#endif
}

char const* ToString( ModelsLocation const value ) {
#if defined _DEBUG
    if ( ( value >= ModelsLocation::Library ) && ( value <= ModelsLocation::Usb ) ) {
#endif
        return ModelsLocationStrings[static_cast<int>( value )];
#if defined _DEBUG
    } else {
        return nullptr;
    }
#endif
}

QString FormatDouble( double const value, int const fieldWidth, int const precision ) {
    QString str = QString( "%1" ).arg( value, fieldWidth, 'f', precision );
    int index = str.length( ) - 1;
    while ( ( index > -1 ) && ( str[index].unicode( ) == L'0' ) ) {
        --index;
    }
    if ( str[index].unicode( ) == L'.' ) {
        --index;
    }
    str.resize( index + 1 );
    return str;
}

QString GroupDigits( QString const& input, char const groupSeparator_, char const decimalSeparator ) {
    QChar   groupSeparator { groupSeparator_ };
    QString str            { input           };

    int index = str.indexOf( QChar { decimalSeparator } );
    if ( -1 == index ) {
        index = str.length( );
    }

    while ( index - 3 > 0 ) {
        index -= 3;
        str.insert( index, groupSeparator );
    }
    return str;
}

QString TimeDeltaToString( double delta ) {
    int days, hours, minutes, seconds;
    _BreakDownTime( delta + 0.5, days, hours, minutes, seconds );

    QString timeString { };
    if ( days > 0 ) {
        timeString += QString( "%1 d " ).arg( days );
    }
    if ( hours > 0 ) {
        timeString += QString( "%1 h " ).arg( hours );
    }
    if ( minutes > 0 ) {
        timeString += QString( "%1 min " ).arg( minutes );
    }
    timeString += QString( "%1 s" ).arg( seconds );

    return timeString;
}
