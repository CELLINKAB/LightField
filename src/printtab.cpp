#include "pch.h"

#include "printtab.h"

#include "app.h"
#include "printjob.h"
#include "printmanager.h"
#include "shepherd.h"
#include "strings.h"
#include "utils.h"

PrintTab::PrintTab( QWidget* parent ): InitialShowEventMixin<PrintTab, TabBase>( parent ) {
#if defined _DEBUG
    _isPrinterPrepared = g_settings.pretendPrinterIsPrepared;
#endif // _DEBUG

    auto boldFont = ModifyFont( font( ), QFont::Bold );


    _exposureTimeLabel->setAlignment( Qt::AlignTop | Qt::AlignLeft );
    _exposureTimeLabel->setText( "Exposure time (seconds):" );

    _exposureTimeValue->setAlignment( Qt::AlignTop | Qt::AlignRight );
    _exposureTimeValue->setFont( boldFont );

    _exposureTimeValueLayout = WrapWidgetsInHBox( { _exposureTimeLabel, nullptr, _exposureTimeValue } );


    _exposureTimeSlider->setMinimum( 1 );
    _exposureTimeSlider->setMaximum( 40 );
    _exposureTimeSlider->setOrientation( Qt::Horizontal );
    _exposureTimeSlider->setPageStep( 1 );
    _exposureTimeSlider->setSingleStep( 1 );
    _exposureTimeSlider->setTickInterval( 5 );
    _exposureTimeSlider->setTickPosition( QSlider::TicksBothSides );
    QObject::connect( _exposureTimeSlider, &QSlider::valueChanged, this, &PrintTab::exposureTimeSlider_valueChanged );


    _exposureTimeLayout->addLayout( _exposureTimeValueLayout );
    _exposureTimeLayout->addWidget( _exposureTimeSlider );


    _exposureTimeScaleFactorLabel->setAlignment( Qt::AlignTop | Qt::AlignLeft );
    _exposureTimeScaleFactorLabel->setText( "First layers time scale factor:" );

    _exposureTimeScaleFactorValue->setAlignment( Qt::AlignTop | Qt::AlignRight );
    _exposureTimeScaleFactorValue->setFont( boldFont );

    _exposureTimeScaleFactorValueLayout = WrapWidgetsInHBox( { _exposureTimeScaleFactorLabel, nullptr, _exposureTimeScaleFactorValue } );


    _exposureTimeScaleFactorSlider->setMinimum( 1 );
    _exposureTimeScaleFactorSlider->setMaximum( 5 );
    _exposureTimeScaleFactorSlider->setOrientation( Qt::Horizontal );
    _exposureTimeScaleFactorSlider->setPageStep( 1 );
    _exposureTimeScaleFactorSlider->setSingleStep( 1 );
    _exposureTimeScaleFactorSlider->setTickInterval( 1 );
    _exposureTimeScaleFactorSlider->setTickPosition( QSlider::TicksBothSides );
    QObject::connect( _exposureTimeScaleFactorSlider, &QSlider::valueChanged, this, &PrintTab::exposureTimeScaleFactorSlider_valueChanged );


    _exposureTimeScaleFactorLayout->addLayout( _exposureTimeScaleFactorValueLayout );
    _exposureTimeScaleFactorLayout->addWidget( _exposureTimeScaleFactorSlider );


    _exposureLayout->addLayout( _exposureTimeLayout,            8 );
    _exposureLayout->addStretch( 1 );
    _exposureLayout->addLayout( _exposureTimeScaleFactorLayout, 4 );


    _powerLevelLabel->setText( "Projector power level:" );

    _powerLevelValue->setAlignment( Qt::AlignRight );
    _powerLevelValue->setFont( boldFont );

    _powerLevelSlider->setMinimum( 20 );
    _powerLevelSlider->setMaximum( 100 );
    _powerLevelSlider->setOrientation( Qt::Horizontal );
    _powerLevelSlider->setPageStep( 1 );
    _powerLevelSlider->setSingleStep( 1 );
    _powerLevelSlider->setTickInterval( 1 );
    _powerLevelSlider->setTickPosition( QSlider::TicksBothSides );
    QObject::connect( _powerLevelSlider, &QSlider::valueChanged, this, &PrintTab::powerLevelSlider_valueChanged );


    _printSpeedLabel->setText( "Print speed:" );

    _printSpeedValue->setAlignment( Qt::AlignRight );
    _printSpeedValue->setFont( boldFont );

    _printSpeedSlider->setMinimum( 50 );
    _printSpeedSlider->setMaximum( 200 );
    _printSpeedSlider->setOrientation( Qt::Horizontal );
    _printSpeedSlider->setPageStep( 50 );
    _printSpeedSlider->setSingleStep( 25 );
    _printSpeedSlider->setTickInterval( 10 );
    _printSpeedSlider->setTickPosition( QSlider::TicksBothSides );
    QObject::connect( _printSpeedSlider, &QSlider::valueChanged, this, &PrintTab::printSpeedSlider_valueChanged );


    _optionsLayout->addLayout( _exposureLayout );
    _optionsLayout->addLayout( WrapWidgetsInHBox( { _powerLevelLabel, nullptr, _powerLevelValue } ) );
    _optionsLayout->addWidget( _powerLevelSlider );
    _optionsLayout->addLayout( WrapWidgetsInHBox( { _printSpeedLabel, nullptr, _printSpeedValue } ) );
    _optionsLayout->addWidget( _printSpeedSlider );
    _optionsLayout->addStretch( );

    _optionsGroup->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    _optionsGroup->setLayout( _optionsLayout );
    _optionsGroup->setTitle( "Print settings" );

    _printButton->setEnabled( false );
    _printButton->setFixedSize( MainButtonSize );
    _printButton->setFont( ModifyFont( _printButton->font( ), 22.0 ) );
    _printButton->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
    _printButton->setText( "Print…" );
    QObject::connect( _printButton, &QPushButton::clicked, this, &PrintTab::printButton_clicked );

    _raiseOrLowerButton->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
    _raiseOrLowerButton->setText( "Raise Build Platform" );
    QObject::connect( _raiseOrLowerButton, &QPushButton::clicked, this, &PrintTab::raiseOrLowerButton_clicked );

    _homeButton->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
    _homeButton->setText( "Home" );
    QObject::connect( _homeButton, &QPushButton::clicked, this, &PrintTab::homeButton_clicked );

    _adjustmentsGroup->setFixedHeight( MainButtonSize.height( ) );
    _adjustmentsGroup->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
    _adjustmentsGroup->setLayout( WrapWidgetsInHBox( { nullptr, _homeButton, nullptr, _raiseOrLowerButton, nullptr } ) );
    _adjustmentsGroup->setTitle( "Adjustments" );

    _layout->setContentsMargins( { } );
    _layout->addWidget( _optionsGroup,     0, 0, 1, 2 );
    _layout->addWidget( _printButton,      1, 0, 1, 1 );
    _layout->addWidget( _adjustmentsGroup, 1, 1, 1, 1 );
    _layout->setRowStretch( 0, 4 );
    _layout->setRowStretch( 1, 1 );

    setLayout( _layout );
}

PrintTab::~PrintTab( ) {
    /*empty*/
}

void PrintTab::_connectPrintJob( ) {
    debug( "+ PrintTab::setPrintJob: _printJob %p\n", _printJob );

    {
        int value = _printJob->exposureTime / 0.5;
        _printJob->exposureTime = value / 2.0;
        _exposureTimeSlider->setValue( value );
        _exposureTimeValue->setText( QString( "%1 s" ).arg( _printJob->exposureTime, 0, 'f', 1 ) );
    }

    _exposureTimeScaleFactorSlider->setValue( _printJob->exposureTimeScaleFactor );
    _exposureTimeScaleFactorValue->setText( QString( "%1×" ).arg( _printJob->exposureTimeScaleFactor ) );

    _powerLevelSlider->setValue( _printJob->powerLevel / 255.0 * 100.0 + 0.5 );
    _powerLevelValue->setText( QString( "%1%" ).arg( static_cast<int>( _printJob->powerLevel / 255.0 * 100.0 + 0.5 ) ) );

    _printSpeedSlider->setValue( _printJob->printSpeed );
    _printSpeedValue->setText( QString( "%1 mm/min" ).arg( _printJob->printSpeed ) );

    update( );
}

void PrintTab::_connectShepherd( ) {
    QObject::connect( _shepherd, &Shepherd::printer_online,  this, &PrintTab::printer_online  );
    QObject::connect( _shepherd, &Shepherd::printer_offline, this, &PrintTab::printer_offline );
}

void PrintTab::_updateUiState( ) {
    bool isEnabled = _isPrinterOnline && _isPrinterAvailable;

    _optionsGroup      ->setEnabled( isEnabled );
    _printButton       ->setEnabled( isEnabled && _isPrinterPrepared && _isModelRendered );
    _raiseOrLowerButton->setEnabled( isEnabled );
    _homeButton        ->setEnabled( isEnabled );

    update( );
}

void PrintTab::initialShowEvent( QShowEvent* event ) {
    auto size = QSize {
        std::max( { _raiseOrLowerButton->width( ),  _homeButton->width( ),  } ),
        std::max( { _raiseOrLowerButton->height( ), _homeButton->height( ), } )
    };

    auto fm = _raiseOrLowerButton->fontMetrics( );
    auto raiseSize = fm.size( Qt::TextSingleLine | Qt::TextShowMnemonic, "Raise Build Platform" );
    auto lowerSize = fm.size( Qt::TextSingleLine | Qt::TextShowMnemonic, "Lower Build Platform" );
    if ( lowerSize.width( ) > raiseSize.width( ) ) {
        size.setWidth( size.width( ) + lowerSize.width( ) - raiseSize.width( ) );
    }
    size.setWidth( size.width( ) + 20 );

    _raiseOrLowerButton->setFixedSize( size );
    _homeButton        ->setFixedSize( size );

    event->accept( );

    update( );
}

void PrintTab::exposureTimeSlider_valueChanged( int value ) {
    _printJob->exposureTime = value / 2.0;
    _exposureTimeValue->setText( QString( "%1 s" ).arg( _printJob->exposureTime, 0, 'f', 1 ) );

    update( );
}

void PrintTab::exposureTimeScaleFactorSlider_valueChanged( int value ) {
    _printJob->exposureTimeScaleFactor = value;
    _exposureTimeScaleFactorValue->setText( QString( "%1×" ).arg( value ) );

    update( );
}

void PrintTab::powerLevelSlider_valueChanged( int value ) {
    _printJob->powerLevel = value / 100.0 * 255.0 + 0.5;
    _powerLevelValue->setText( QString( "%1%" ).arg( value ) );

    update( );
}

void PrintTab::printSpeedSlider_valueChanged( int value ) {
    _printJob->printSpeed = value;
    _printSpeedValue->setText( QString( "%1 mm/min" ).arg( value ) );

    update( );
}

void PrintTab::printButton_clicked( bool ) {
    debug( "+ PrintTab::printButton_clicked\n" );
    emit printRequested( );
    emit uiStateChanged( TabIndex::Print, UiState::PrintStarted );

    update( );
}

void PrintTab::raiseOrLowerButton_clicked( bool ) {
    debug( "+ PrintTab::raiseOrLowerButton_clicked: build platform state %s [%d]\n", ToString( _buildPlatformState ), _buildPlatformState );

    switch ( _buildPlatformState ) {
        case BuildPlatformState::Lowered:
        case BuildPlatformState::Raising:
            _buildPlatformState = BuildPlatformState::Raising;

            QObject::connect( _shepherd, &Shepherd::action_moveAbsoluteComplete, this, &PrintTab::raiseBuildPlatform_moveAbsoluteComplete );
            _shepherd->doMoveAbsolute( PrinterRaiseToMaxZHeight );
            break;

        case BuildPlatformState::Raised:
        case BuildPlatformState::Lowering:
            _buildPlatformState = BuildPlatformState::Lowering;

            QObject::connect( _shepherd, &Shepherd::action_moveAbsoluteComplete, this, &PrintTab::lowerBuildPlatform_moveAbsoluteComplete );
            _shepherd->doMoveAbsolute( std::max( 100, _printJob->layerThickness ) / 1000.0 );
            break;
    }

    setPrinterAvailable( false );
    emit printerAvailabilityChanged( false );

    update( );
}

void PrintTab::raiseBuildPlatform_moveAbsoluteComplete( bool const success ) {
    debug( "+ PrintTab::raiseBuildPlatform_moveAbsoluteComplete: %s\n", success ? "succeeded" : "failed" );
    QObject::disconnect( _shepherd, &Shepherd::action_moveAbsoluteComplete, this, &PrintTab::raiseBuildPlatform_moveAbsoluteComplete );

    if ( success ) {
        _buildPlatformState = BuildPlatformState::Raised;
        _raiseOrLowerButton->setText( "Lower Build Platform" );
        _raiseOrLowerButton->setEnabled( true );
    } else {
        _buildPlatformState = BuildPlatformState::Lowered;
    }

    update( );
}

void PrintTab::lowerBuildPlatform_moveAbsoluteComplete( bool const success ) {
    debug( "+ PrintTab::lowerBuildPlatform_moveAbsoluteComplete: %s\n", success ? "succeeded" : "failed" );
    QObject::disconnect( _shepherd, &Shepherd::action_moveAbsoluteComplete, this, &PrintTab::lowerBuildPlatform_moveAbsoluteComplete );

    if ( success ) {
        _buildPlatformState = BuildPlatformState::Lowered;
        _raiseOrLowerButton->setText( "Raise Build Platform" );
        _raiseOrLowerButton->setEnabled( true );
    } else {
        _buildPlatformState = BuildPlatformState::Raised;
    }

    setPrinterAvailable( true );
    emit printerAvailabilityChanged( true );

    update( );
}

void PrintTab::homeButton_clicked( bool ) {
    debug( "+ PrintTab::homeButton_clicked\n" );

    QObject::connect( _shepherd, &Shepherd::action_homeComplete, this, &PrintTab::home_homeComplete );
    _shepherd->doHome( );

    setPrinterAvailable( false );
    emit printerAvailabilityChanged( false );

    update( );
}

void PrintTab::home_homeComplete( bool const success ) {
    debug( "+ PrintTab::home_homeComplete: %s\n", success ? "succeeded" : "failed" );

    setPrinterAvailable( true );
    emit printerAvailabilityChanged( true );

    update( );
}

void PrintTab::tab_uiStateChanged( TabIndex const sender, UiState const state ) {
    debug( "+ PrintTab::tab_uiStateChanged: from %sTab: %s => %s; PO? %s PA? %s PP? %s MR? %s\n", ToString( sender ), ToString( _uiState ), ToString( state ), YesNoString( _isPrinterOnline ), YesNoString( _isPrinterAvailable ), YesNoString( _isPrinterPrepared ), YesNoString( _isModelRendered ) );
    _uiState = state;

    switch ( _uiState ) {
        case UiState::SelectStarted:
        case UiState::SelectCompleted:
        case UiState::SliceStarted:
        case UiState::SliceCompleted:
            break;

        case UiState::PrintStarted:
            setPrinterAvailable( false );
            emit printerAvailabilityChanged( false );
            break;

        case UiState::PrintCompleted:
            setPrinterAvailable( true );
            emit printerAvailabilityChanged( true );
            break;
    }

    update( );
}

void PrintTab::printer_online( ) {
    _isPrinterOnline = true;
    debug( "+ PrintTab::printer_online: PO? %s PA? %s PP? %s MR? %s\n", YesNoString( _isPrinterOnline ), YesNoString( _isPrinterAvailable ), YesNoString( _isPrinterPrepared ), YesNoString( _isModelRendered ) );

    _updateUiState( );
}

void PrintTab::printer_offline( ) {
    _isPrinterOnline = false;
    debug( "+ PrintTab::printer_offline: PO? %s PA? %s PP? %s MR? %s\n", YesNoString( _isPrinterOnline ), YesNoString( _isPrinterAvailable ), YesNoString( _isPrinterPrepared ), YesNoString( _isModelRendered ) );

    _updateUiState( );
}

void PrintTab::setModelRendered( bool const value ) {
    _isModelRendered = value;
    debug( "+ PrintTab::setModelRendered: PO? %s PA? %s PP? %s MR? %s\n", YesNoString( _isPrinterOnline ), YesNoString( _isPrinterAvailable ), YesNoString( _isPrinterPrepared ), YesNoString( _isModelRendered ) );

    _updateUiState( );
}

void PrintTab::setPrinterPrepared( bool const value ) {
    _isPrinterPrepared = value;
    debug( "+ PrintTab::setPrinterPrepared: PO? %s PA? %s PP? %s MR? %s\n", YesNoString( _isPrinterOnline ), YesNoString( _isPrinterAvailable ), YesNoString( _isPrinterPrepared ), YesNoString( _isModelRendered ) );

    _updateUiState( );
}

void PrintTab::clearPrinterPrepared( ) {
    setPrinterPrepared( false );
}

void PrintTab::setPrinterAvailable( bool const value ) {
    _isPrinterAvailable = value;
    debug( "+ PrintTab::setPrinterAvailable: PO? %s PA? %s PP? %s MR? %s\n", YesNoString( _isPrinterOnline ), YesNoString( _isPrinterAvailable ), YesNoString( _isPrinterPrepared ), YesNoString( _isModelRendered ) );

    _updateUiState( );
}
