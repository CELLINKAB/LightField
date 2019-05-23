#include "pch.h"

#include "maintenancetab.h"

#include "app.h"
#include "strings.h"
#include "utils.h"

MaintenanceTab::MaintenanceTab( QWidget* parent ): InitialShowEventMixin<MaintenanceTab, TabBase>( parent ) {
    auto origFont = font( );
    auto font16pt = ModifyFont( origFont, 16.0 );
    auto font22pt = ModifyFont( origFont, 22.0 );

    //
    // Main content
    //

    _logoLabel->setAlignment( Qt::AlignCenter );
    _logoLabel->setContentsMargins( { } );
    _logoLabel->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::MinimumExpanding );
//    _logoLabel->setPixmap( QPixmap { QString { ":images/transparent-dark-logo.png" } } );
    _logoLabel->setPixmap( QPixmap { QString { ":images/LUMENX@400.png" } } );

    _versionLabel->setAlignment( Qt::AlignCenter );
    _versionLabel->setContentsMargins( { } );
    _versionLabel->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::MinimumExpanding );
    _versionLabel->setTextFormat( Qt::RichText );
    _versionLabel->setText(
        QString {
            "<span style='font-size: 22pt;'>%1</span><br>"
            "<span style='font-size: 16pt;'>Version %2</span><br>"
            "<span>© 2019 Volumetric, Inc.</span>"
        }
        .arg( QCoreApplication::applicationName( ) )
        .arg( QCoreApplication::applicationVersion( ) )
    );

    auto versionInfoLayout = WrapWidgetsInHBox( { _logoLabel, _versionLabel } );
    versionInfoLayout->setContentsMargins( { } );


    _copyrightsLabel->setAlignment( Qt::AlignCenter );
    _copyrightsLabel->setTextFormat( Qt::RichText );
    _copyrightsLabel->setText( ReadWholeFile( ":text/copyright-message.txt" ) );


    _updateSoftwareButton->setFont( font16pt );
    _updateSoftwareButton->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
    _updateSoftwareButton->setText( "Update software" );
    QObject::connect( _updateSoftwareButton, &QPushButton::clicked, this, &MaintenanceTab::updateSoftwareButton_clicked );

    _updateFirmwareButton->setFont( font16pt );
    _updateFirmwareButton->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
    _updateFirmwareButton->setText( "Update firmware" );
    QObject::connect( _updateFirmwareButton, &QPushButton::clicked, this, &MaintenanceTab::updateFirmwareButton_clicked );


    _restartButton->setFont( font16pt );
    _restartButton->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
    _restartButton->setText( "Restart" );
    QObject::connect( _restartButton, &QPushButton::clicked, this, &MaintenanceTab::restartButton_clicked );

    _shutDownButton->setFont( font16pt );
    _shutDownButton->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
    _shutDownButton->setText( "Shut down" );
    QObject::connect( _shutDownButton, &QPushButton::clicked, this, &MaintenanceTab::shutDownButton_clicked );

    auto mainButtonsLayout = WrapWidgetsInHBox( { nullptr, _restartButton, nullptr, _shutDownButton, nullptr } );
    mainButtonsLayout->setContentsMargins( { } );


    _mainLayout->addLayout( versionInfoLayout );
    _mainLayout->addWidget( _copyrightsLabel );
    _mainLayout->addStretch( );
    _mainLayout->addLayout( mainButtonsLayout );
    _mainLayout->setAlignment( Qt::AlignCenter );
    _mainLayout->setContentsMargins( { } );


    _mainContent->setContentsMargins( { } );
    _mainContent->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    _mainContent->setVisible( true );
    _mainContent->setLayout( _mainLayout );

    //
    // Confirm restart content
    //

    _confirmRestartLabel->setAlignment( Qt::AlignCenter );
    _confirmRestartLabel->setFont( font22pt );
    _confirmRestartLabel->setText( "Are you sure you want to restart?" );


    _confirmRestartYesButton->setFont( font16pt );
    _confirmRestartYesButton->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
    _confirmRestartYesButton->setText( "Yes" );
    QObject::connect( _confirmRestartYesButton, &QPushButton::clicked, this, &MaintenanceTab::confirmRestartYesButton_clicked );

    _confirmRestartNoButton->setFont( font16pt );
    _confirmRestartNoButton->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
    _confirmRestartNoButton->setText( "No" );
    QObject::connect( _confirmRestartNoButton, &QPushButton::clicked, this, &MaintenanceTab::confirmRestartNoButton_clicked );

    auto confirmRestartButtonsLayout = WrapWidgetsInHBox( { nullptr, _confirmRestartYesButton, nullptr, _confirmRestartNoButton, nullptr } );
    confirmRestartButtonsLayout->setContentsMargins( { } );


    _confirmRestartLayout = WrapWidgetsInVBox( { nullptr, _confirmRestartLabel, nullptr } );
    _confirmRestartLayout->addLayout( confirmRestartButtonsLayout );
    _confirmRestartLayout->addStretch( );
    _confirmRestartLayout->setAlignment( Qt::AlignCenter );
    _confirmRestartLayout->setContentsMargins( { } );


    _confirmRestartContent->setContentsMargins( { } );
    _confirmRestartContent->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    _confirmRestartContent->setVisible( false );
    _confirmRestartContent->setLayout( _confirmRestartLayout );

    //
    // Confirm shutdown content
    //

    _confirmShutdownLabel->setAlignment( Qt::AlignCenter );
    _confirmShutdownLabel->setFont( font22pt );
    _confirmShutdownLabel->setText( "Are you sure you want to shut down?" );


    _confirmShutdownYesButton->setFont( font16pt );
    _confirmShutdownYesButton->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
    _confirmShutdownYesButton->setText( "Yes" );
    QObject::connect( _confirmShutdownYesButton, &QPushButton::clicked, this, &MaintenanceTab::confirmShutdownYesButton_clicked );

    _confirmShutdownNoButton->setFont( font16pt );
    _confirmShutdownNoButton->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
    _confirmShutdownNoButton->setText( "No" );
    QObject::connect( _confirmShutdownNoButton, &QPushButton::clicked, this, &MaintenanceTab::confirmShutdownNoButton_clicked );

    auto confirmShutdownButtonsLayout = WrapWidgetsInHBox( { nullptr, _confirmShutdownYesButton, nullptr, _confirmShutdownNoButton, nullptr } );
    confirmShutdownButtonsLayout->setContentsMargins( { } );


    _confirmShutdownLayout = WrapWidgetsInVBox( { nullptr, _confirmShutdownLabel, nullptr } );
    _confirmShutdownLayout->addLayout( confirmShutdownButtonsLayout );
    _confirmShutdownLayout->addStretch( );
    _confirmShutdownLayout->setAlignment( Qt::AlignCenter );
    _confirmShutdownLayout->setContentsMargins( { } );


    _confirmShutdownContent->setContentsMargins( { } );
    _confirmShutdownContent->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    _confirmShutdownContent->setVisible( false );
    _confirmShutdownContent->setLayout( _confirmShutdownLayout );

    //
    // Top level
    //

    _layout = WrapWidgetsInVBox( { _mainContent, _confirmRestartContent, _confirmShutdownContent } );
    setLayout( _layout );
}

MaintenanceTab::~MaintenanceTab( ) {
    /*empty*/
}

void MaintenanceTab::initialShowEvent( QShowEvent* event ) {
    QSize newSize = _shutDownButton->size( );
    newSize.setWidth( newSize.width( ) + 20 );

    _restartButton           ->setFixedSize( newSize );
    _shutDownButton          ->setFixedSize( newSize );
    _confirmRestartYesButton ->setFixedSize( newSize );
    _confirmRestartNoButton  ->setFixedSize( newSize );
    _confirmShutdownYesButton->setFixedSize( newSize );
    _confirmShutdownNoButton ->setFixedSize( newSize );

    event->accept( );

    update( );
}

void MaintenanceTab::updateSoftwareButton_clicked( bool ) {
}

void MaintenanceTab::updateFirmwareButton_clicked( bool ) {
}

void MaintenanceTab::restartButton_clicked( bool ) {
    _mainContent->setVisible( false );
    _confirmRestartContent->setVisible( true );

    update( );
}

void MaintenanceTab::tab_uiStateChanged( TabIndex const sender, UiState const state ) {
    debug( "+ MaintenanceTab::tab_uiStateChanged: from %sTab: %s => %s\n", ToString( sender ), ToString( _uiState ), ToString( state ) );
    _uiState = state;

    switch ( _uiState ) {
        case UiState::SelectStarted:
        case UiState::SelectCompleted:
        case UiState::SliceStarted:
        case UiState::SliceCompleted:
        case UiState::PrintStarted:
        case UiState::PrintCompleted:
            break;
    }
}

void MaintenanceTab::shutDownButton_clicked( bool ) {
    _mainContent->setVisible( false );
    _confirmShutdownContent->setVisible( true );

    update( );
}

void MaintenanceTab::confirmRestartYesButton_clicked( bool ) {
    _confirmRestartContent->setVisible( false );
    _mainContent->setVisible( true );

    update( );

    system( "sudo shutdown -r now" );
}

void MaintenanceTab::confirmRestartNoButton_clicked( bool ) {
    _confirmRestartContent->setVisible( false );
    _mainContent->setVisible( true );

    update( );
}

void MaintenanceTab::confirmShutdownYesButton_clicked( bool ) {
    _confirmShutdownContent->setVisible( false );
    _mainContent->setVisible( true );

    update( );

    system( "sudo shutdown -h now" );
}

void MaintenanceTab::confirmShutdownNoButton_clicked( bool ) {
    _confirmShutdownContent->setVisible( false );
    _mainContent->setVisible( true );

    update( );
}
