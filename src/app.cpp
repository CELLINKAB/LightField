#include "pch.h"

#include "app.h"

#include "signalhandler.h"
#include "utils.h"
#include "window.h"

AppSettings g_settings;

namespace {

    QCommandLineParser commandLineParser;

    bool moveMainWindow = false;

    QList<QCommandLineOption> commandLineOptions {
        QCommandLineOption { QStringList { "?", "help"  }, "Displays this help."                                                                    },
        QCommandLineOption { QStringList { "l", "light" }, "Selects the \"light\" theme."                                                           },
        QCommandLineOption {               "s",            "Run at 800×480.",                                                                       },
        QCommandLineOption {               "x",            "Offsets the projected image horizontally.",                              "xOffset", "0" },
        QCommandLineOption {               "y",            "Offsets the projected image vertically.",                                "yOffset", "0" },
#if defined _DEBUG
        QCommandLineOption {               "h",            "Positions main window at (0, 0)."                                                       },
        QCommandLineOption {               "i",            "Sets FramelessWindowHint instead of BypassWindowManagerHint on windows."                },
        QCommandLineOption {               "j",            "Pretend printer preparation is complete."                                               },
        QCommandLineOption {               "k",            "Ignore stdio-shepherd failure reports."                                                 },
        QCommandLineOption {               "m",            "Pretend printer is online."                                                             },
        QCommandLineOption {               "n",            "Ignore USB."                                                                            },
#endif // defined _DEBUG
    };

    QList<std::function<void( )>> commandLineActions {
        [] ( ) { // -? or --help
            ::fputs( commandLineParser.helpText( ).toUtf8( ).data( ), stderr );
            ::exit( 0 );
        },
        [] ( ) { // -l or --light
            g_settings.theme = Theme::Light;
        },
        [ ] ( ) { // -s
            MainWindowSize           = SmallMainWindowSize;
            MainButtonSize           = SmallMainButtonSize;
            MaximalRightHandPaneSize = SmallMaximalRightHandPaneSize;
        },
        [] ( ) { // -x
            auto value = commandLineParser.value( "xOffset" );

            bool ok = false;
            auto xOffset = value.toInt( &ok, 10 );
            if ( ok ) {
                g_settings.projectorOffset.setX( xOffset );
            } else {
                ::fprintf( stderr, "Invalid value given for -x parameter.\n" );
                ::exit( 1 );
            }
        },
        [] ( ) { // -y
            auto value = commandLineParser.value( "yOffset" );

            bool ok = false;
            auto yOffset = value.toInt( &ok, 10 );
            if ( ok ) {
                g_settings.projectorOffset.setY( yOffset );
            } else {
                ::fprintf( stderr, "Invalid value given for -y parameter.\n" );
                ::exit( 1 );
            }
        },
#if defined _DEBUG
        [] ( ) { // -h
            moveMainWindow = true;
        },
        [] ( ) { // -i
            g_settings.frameless = true;
        },
        [] ( ) { // -j
            g_settings.pretendPrinterIsPrepared = true;
        },
        [] ( ) { // -k
            g_settings.ignoreShepherdFailures = true;
        },
        [] ( ) { // -m
            g_settings.pretendPrinterIsOnline = true;
        },
        [] ( ) { // -n
            g_settings.ignoreUsb = true;
        },
#endif // defined _DEBUG
    };

}

void App::_parseCommandLine( ) {
    commandLineParser.setOptionsAfterPositionalArgumentsMode( QCommandLineParser::ParseAsOptions );
    commandLineParser.setSingleDashWordOptionMode( QCommandLineParser::ParseAsCompactedShortOptions );
    commandLineParser.addOptions( commandLineOptions );
    commandLineParser.process( *this );

    for ( auto i = 0; i < commandLineOptions.count( ); ++i ) {
        if ( commandLineParser.isSet( commandLineOptions[i] ) ) {
            commandLineActions[i]( );
        }
    }

    if ( moveMainWindow ) {
        g_settings.mainWindowPosition.setY( 0 );
        g_settings.projectorWindowPosition.setY( MainWindowSize.height( ) );
    }
}

void App::_setTheme( ) {
    setStyle( QStyleFactory::create( "Fusion" ) );

    struct Color { QPalette::ColorGroup group; QPalette::ColorRole role; QColor color; };

    const QColor mainColor = QColor::fromRgb(0xDFE1E6);
    const QColor textColor = QColor::fromRgb(0x454646);
    const QColor disabledTextColor = QColor::fromRgba(0xc0454646);
    const QColor white = QColor::fromRgb(0xffffff);
    const QColor heroBlue = QColor::fromRgb(0x0A39B7);

    const Color Colors[] = {
        { QPalette::All, QPalette::AlternateBase, QColor::fromRgb(0xf4f4f4) },
        { QPalette::All, QPalette::Base, white},
        { QPalette::All, QPalette::BrightText, white },
        { QPalette::All, QPalette::Button, mainColor },
        { QPalette::All, QPalette::ButtonText, textColor },
        { QPalette::Disabled, QPalette::ButtonText, disabledTextColor },
        { QPalette::All, QPalette::Dark, QColor::fromRgb(0x696a6b) },
        { QPalette::All, QPalette::HighlightedText, white },
        { QPalette::Disabled, QPalette::HighlightedText, QColor::fromRgba(0xc0ffffff) },
        { QPalette::All, QPalette::Light, QColor::fromRgb(0xdedfe0) },
        { QPalette::All, QPalette::Mid, QColor::fromRgb(0x9e9e9f) },
        { QPalette::All, QPalette::Midlight, QColor::fromRgb(0xcecfd0) },
        { QPalette::All, QPalette::Text, textColor },
        { QPalette::Disabled, QPalette::Text, disabledTextColor },
        { QPalette::All, QPalette::ToolTipBase, QColor::fromRgba(0xc0000000) },
        { QPalette::All, QPalette::ToolTipText, white },
        { QPalette::Disabled, QPalette::ToolTipText, disabledTextColor },
        { QPalette::All, QPalette::Window, mainColor },
        { QPalette::All, QPalette::WindowText, textColor },
        { QPalette::All, QPalette::Highlight, heroBlue},
        { QPalette::All, QPalette::Link, QColor( 80, 80, 80)},
        { QPalette::All, QPalette::LinkVisited, QColor( 80, 80, 80)}
    };

    QPalette cellinkPalette;

    for (const Color &c : Colors) {
        cellinkPalette.setColor(c.group, c.role, c.color);
    }

    setPalette( cellinkPalette );
}

App::App( int& argc, char* argv[] ): QApplication( argc, argv ) {
    g_signalHandler = new SignalHandler;

    QCoreApplication::setOrganizationName( "Volumetric" );
    QCoreApplication::setOrganizationDomain( "https://www.volumetricbio.com/" );
    QCoreApplication::setApplicationName( "LightField" );
    QCoreApplication::setApplicationVersion( LIGHTFIELD_VERSION );

    QFontDatabase::addApplicationFont(":/Mark Simonson - Proxima Nova.otf");
    QFontDatabase::addApplicationFont(":/Raleway/Raleway/Raleway-Regular.ttf");
    QGuiApplication::setFont(ModifyFont( QGuiApplication::font( ), "Raleway" ));

    QProcess::startDetached( SetpowerCommand, { "0" } );

    _parseCommandLine( );
    _debugManager = new DebugManager;
    _setTheme( );

    _window = new Window;
    _window->show( );
}

App::~App( ) {
    delete _window;
    delete g_signalHandler;

    QProcess::startDetached( SetpowerCommand, { "0" } );
}
