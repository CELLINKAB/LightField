#include "pch.h"

#include <ctime>

#include "utils.h"

QVBoxLayout* WrapWidgetInVBox( QWidget* widget, Qt::AlignmentFlag const alignment ) {
    auto layout = new QVBoxLayout;

    layout->setAlignment( alignment );
    layout->setContentsMargins( { } );
    layout->addWidget( widget );
    return layout;
}

QHBoxLayout* WrapWidgetsInHBox( std::initializer_list<QWidget*> widgets ) {
    auto layout = new QHBoxLayout;

    for ( auto widget : widgets ) {
        if ( widget ) {
            layout->addWidget( widget );
        } else {
            layout->addStretch( );
        }
    }
    return layout;
}

QVBoxLayout* WrapWidgetsInVBox( std::initializer_list<QWidget*> widgets ) {
    auto layout = new QVBoxLayout;

    for ( auto widget : widgets ) {
        if ( widget ) {
            layout->addWidget( widget );
        } else {
            layout->addStretch( );
        }
    }
    return layout;
}

QFont ModifyFont( QFont const& font_, double const pointSizeF ) {
    auto font { font_ };
    font.setPointSizeF( pointSizeF );
    return font;
}

QFont ModifyFont( QFont const& font_, double const pointSizeF, QFont::Weight const weight ) {
    auto font { font_ };
    font.setPointSizeF( pointSizeF );
    font.setWeight( weight );
    return font;
}

QFont ModifyFont( QFont const& font_, QFont::Weight const weight ) {
    auto font { font_ };
    font.setWeight( weight );
    return font;
}

QFont ModifyFont( QFont const& font_, QString const& familyName ) {
    auto font { font_ };
    font.setFamily( familyName );
    return font;
}

QPalette ModifyPalette( QPalette const& palette_, QPalette::ColorGroup const group, QPalette::ColorRole const role, QColor const& color ) {
    auto palette { palette_ };
    palette.setColor( group, role, color );
    return palette;
}

QPalette ModifyPalette( QPalette const& palette_, QPalette::ColorRole const role, QColor const& color ) {
    auto palette { palette_ };
    palette.setColor( role, color );
    return palette;
}

QString GetUserName( ) {
    char* buf = new char[16384];
    struct passwd pwd;
    struct passwd* result;
    QString userName;

    if ( 0 == ::getpwuid_r( ::getuid( ), &pwd, buf, 16384, &result ) ) {
        userName = pwd.pw_name;
    } else {
        debug( "+ GetUserName: getpwuid_r failed?!\n" );
    }

    delete[] buf;
    return userName;
}

QString GetFirstDirectoryIn( QString const& directory ) {
    auto dir = new QDir( directory );
    dir->setFilter( QDir::Dirs | QDir::NoDotAndDotDot );

    QString dirname;
    for ( auto name : dir->entryList( ) ) {
        dirname = name;
        break;
    }

    return dirname;
}

qreal Distance( QPointF const& a, QPointF const& b ) {
    return sqrt( pow( a.x( ) - b.x( ), 2.0 ) + pow( a.y( ) - b.y( ), 2.0 ) );
}

int Distance( QPoint const& a, QPoint const& b ) {
    return sqrt( pow( a.x( ) - b.x( ), 2.0 ) + pow( a.y( ) - b.y( ), 2.0 ) );
}

double GetBootTimeClock( ) {
    timespec now;
    clock_gettime( CLOCK_BOOTTIME, &now );
    return now.tv_sec + now.tv_nsec / 1'000'000'000.0;
}

QString ReadWholeFile( QString const& fileName ) {
    QFile file { fileName };
    QString result;

    if ( file.open( QFile::ReadOnly | QFile::ExistingOnly ) ) {
        auto fileContents = file.readAll( );
        file.close( );
        result = fileContents;
    }
    return result;
}
