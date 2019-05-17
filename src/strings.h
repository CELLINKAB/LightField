#ifndef __STRINGS_H__
#define __STRINGS_H__

//
// Functions for converting various types to constant strings.
//

inline char const*        ToString( bool const value ) { return value ? "true"      : "false";  }
inline char const*     YesNoString( bool const value ) { return value ? "yes"       : "no";     }
inline char const* SucceededString( bool const value ) { return value ? "succeeded" : "failed"; }

char const* ToString( QDialog::DialogCode           const value );

char const* ToString( QProcess::ProcessError        const value );
char const* ToString( QProcess::ProcessState        const value );
char const* ToString( QProcess::ExitStatus          const value );

char const* ToString( QSwipeGesture::SwipeDirection const value );

char const* ToString( Qt::GestureState              const value );

//
// Functions for converting various types to variable strings.
//

QString     ToString( QPoint                        const value );
QString     ToString( QRect                         const value );
QString     ToString( QSize                         const value );

QString     ToString( QPointF                       const value );
QString     ToString( QRectF                        const value );
QString     ToString( QSizeF                        const value );

//
// Other string-related functions.
//

QString FormatDouble( double const value, int const fieldWidth = 0, int const precision = -1 );

QString GroupDigits( QString const& input, char const groupSeparator, char const decimalSeparator );

inline QString GroupDigits( QString const& input, char const groupSeparator ) {
    return GroupDigits( input, groupSeparator, '.' );
}

QString TimeDeltaToString( double delta );

#endif // __STRINGS_H__
