#include "pch.h"

#include "printtab.h"

#include "printjob.h"
#include "printmanager.h"
#include "shepherd.h"
#include "strings.h"
#include "utils.h"

using namespace std::placeholders;

namespace {

    char const* BuildPlatformStateStrings[] { "Lowered", "Raising", "Raised", "Lowering" };

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

}

PrintTab::PrintTab( QWidget* parent ): TabBase( parent ) {
    _initialShowEventFunc = std::bind( &PrintTab::_initialShowEvent, this, _1 );

    auto origFont = font( );
    auto boldFont = ModifyFont( origFont, origFont.pointSizeF( ), QFont::Bold );


    _exposureTimeLabel->setAlignment( Qt::AlignTop | Qt::AlignLeft );
    _exposureTimeLabel->setText( "Exposure time (seconds):" );
    _exposureTimeLabel->setBuddy( _exposureTimeSlider );

    _exposureTimeValue->setAlignment( Qt::AlignTop | Qt::AlignRight );
    _exposureTimeValue->setFont( boldFont );

    _exposureTimeValueLayout = WrapWidgetsInHBox( { _exposureTimeLabel, nullptr, _exposureTimeValue } );
    _exposureTimeValueLayout->setContentsMargins( { } );

    _exposureTimeSlider->setMinimum( 1 );
    _exposureTimeSlider->setMaximum( 40 );
    _exposureTimeSlider->setOrientation( Qt::Horizontal );
    _exposureTimeSlider->setTickInterval( 2 );
    _exposureTimeSlider->setTickPosition( QSlider::TicksBothSides );
    QObject::connect( _exposureTimeSlider, &QDial::valueChanged, this, &PrintTab::exposureTimeSlider_valueChanged );


    _exposureTimeLayout->setContentsMargins( { } );
    _exposureTimeLayout->addLayout( _exposureTimeValueLayout );
    _exposureTimeLayout->addWidget( _exposureTimeSlider );


    _exposureTimeScaleFactorLabel->setAlignment( Qt::AlignTop | Qt::AlignLeft );
    _exposureTimeScaleFactorLabel->setText( "First layers time scale factor:" );
    _exposureTimeScaleFactorLabel->setBuddy( _exposureTimeScaleFactorSlider );

    _exposureTimeScaleFactorValue->setAlignment( Qt::AlignTop | Qt::AlignRight );
    _exposureTimeScaleFactorValue->setFont( boldFont );

    _exposureTimeScaleFactorValueLayout = WrapWidgetsInHBox( { _exposureTimeScaleFactorLabel, nullptr, _exposureTimeScaleFactorValue } );
    _exposureTimeScaleFactorValueLayout->setContentsMargins( { } );

    _exposureTimeScaleFactorSlider->setMinimum( 1 );
    _exposureTimeScaleFactorSlider->setMaximum( 5 );
    _exposureTimeScaleFactorSlider->setOrientation( Qt::Horizontal );
    _exposureTimeScaleFactorSlider->setTickInterval( 1 );
    _exposureTimeScaleFactorSlider->setTickPosition( QSlider::TicksBothSides );
    QObject::connect( _exposureTimeScaleFactorSlider, &QSlider::valueChanged, this, &PrintTab::exposureTimeScaleFactorSlider_valueChanged );


    _exposureTimeScaleFactorLayout->setContentsMargins( { } );
    _exposureTimeScaleFactorLayout->addLayout( _exposureTimeScaleFactorValueLayout );
    _exposureTimeScaleFactorLayout->addWidget( _exposureTimeScaleFactorSlider );


    _exposureLayout->addLayout( _exposureTimeLayout,            0, 0 );
    _exposureLayout->addLayout( _exposureTimeScaleFactorLayout, 0, 2 );
    _exposureLayout->setRowStretch( 0, 1 );
    _exposureLayout->setRowStretch( 1, 1 );
    _exposureLayout->setColumnStretch( 0, 8 );
    _exposureLayout->setColumnStretch( 1, 1 );
    _exposureLayout->setColumnStretch( 2, 4 );


    _powerLevelLabel->setText( "Projector power level:" );
    _powerLevelLabel->setBuddy( _powerLevelSlider );

    _powerLevelValue->setAlignment( Qt::AlignRight );
    _powerLevelValue->setFont( boldFont );

    _powerLevelValueLayout = WrapWidgetsInHBox( { _powerLevelLabel, nullptr, _powerLevelValue } );
    _powerLevelValueLayout->setContentsMargins( { } );

    _powerLevelSlider->setMinimum( 20 );
    _powerLevelSlider->setMaximum( 100 );
    _powerLevelSlider->setOrientation( Qt::Horizontal );
    _powerLevelSlider->setTickInterval( 1 );
    _powerLevelSlider->setTickPosition( QSlider::TicksBothSides );
    QObject::connect( _powerLevelSlider, &QDial::valueChanged, this, &PrintTab::powerLevelSlider_valueChanged );


    _optionsLayout->setContentsMargins( { } );
    _optionsLayout->addLayout( _exposureLayout );
    _optionsLayout->addLayout( _powerLevelValueLayout );
    _optionsLayout->addWidget( _powerLevelSlider );
    _optionsLayout->addStretch( );

    _optionsContainer->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    _optionsContainer->setLayout( _optionsLayout );

    _printButton->setEnabled( false );
    _printButton->setFixedSize( MainButtonSize );
    _printButton->setFont( ModifyFont( _printButton->font( ), 22.0 ) );
    _printButton->setText( "Print" );
    QObject::connect( _printButton, &QPushButton::clicked, this, &PrintTab::printButton_clicked );

    _raiseOrLowerButton->setText( "Raise Build Platform" );
    _raiseOrLowerButton->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );
    QObject::connect( _raiseOrLowerButton, &QPushButton::clicked, this, &PrintTab::raiseOrLowerButton_clicked );

    _homeButton->setText( "Home" );
    _homeButton->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );
    QObject::connect( _homeButton, &QPushButton::clicked, this, &PrintTab::homeButton_clicked );

    _adjustmentsGroup->setTitle( QString( "Adjustments" ) );
    _adjustmentsGroup->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    _adjustmentsGroup->setFixedHeight( MainButtonSize.height( ) );
    _adjustmentsGroup->setLayout( WrapWidgetsInHBox( { nullptr, _homeButton, nullptr, _raiseOrLowerButton, nullptr } ) );

    _layout->setContentsMargins( { } );
    _layout->addWidget( _optionsContainer,  0, 0, 1, 2 );
    _layout->addWidget( _printButton,       1, 0, 1, 1 );
    _layout->addWidget( _adjustmentsGroup, 1, 1, 1, 1 );
    _layout->setRowStretch( 0, 4 );
    _layout->setRowStretch( 1, 1 );

    setLayout( _layout );
}

PrintTab::~PrintTab( ) {
    /*empty*/
}

void PrintTab::_initialShowEvent( QShowEvent* event ) {
    auto size = QSize {
        std::max( { _raiseOrLowerButton->width( ),  _homeButton->width( ),  } ),
        std::max( { _raiseOrLowerButton->height( ), _homeButton->height( ), } )
    };

    _raiseOrLowerButton->setFixedSize( size );
    _homeButton        ->setFixedSize( size );

    event->accept( );
}

void PrintTab::exposureTimeSlider_valueChanged( int value ) {
    _printJob->exposureTime = value / 2.0;
    _exposureTimeValue->setText( QString( "%1 s" ).arg( _printJob->exposureTime, 0, 'f', 1 ) );
}

void PrintTab::exposureTimeScaleFactorSlider_valueChanged( int value ) {
    _printJob->exposureTimeScaleFactor = value;
    _exposureTimeScaleFactorValue->setText( QString( "%1×" ).arg( value ) );
}

void PrintTab::powerLevelSlider_valueChanged( int value ) {
    _printJob->powerLevel = value / 100.0 * 255.0 + 0.5;
    _powerLevelValue->setText( QString( "%1%" ).arg( value ) );
}

void PrintTab::printButton_clicked( bool ) {
    debug( "+ PrintTab::printButton_clicked\n" );
    emit printButtonClicked( );
}

void PrintTab::raiseOrLowerButton_clicked( bool ) {
    debug( "+ PrintTab::raiseOrLowerButton_clicked: build platform state %s [%d]\n", ToString( _buildPlatformState ), _buildPlatformState );
    setAdjustmentButtonsEnabled( false );

    switch ( _buildPlatformState ) {
        case BuildPlatformState::Lowered:
        case BuildPlatformState::Raising:
            _buildPlatformState = BuildPlatformState::Raising;

            QObject::connect( _shepherd, &Shepherd::action_moveToComplete, this, &PrintTab::raiseBuildPlatform_moveToComplete );
            _shepherd->doMoveAbsolute( PrinterMaximumZ );
            break;

        case BuildPlatformState::Raised:
        case BuildPlatformState::Lowering:
            _buildPlatformState = BuildPlatformState::Lowering;

            QObject::connect( _shepherd, &Shepherd::action_moveToComplete, this, &PrintTab::lowerBuildPlatform_moveToComplete );
            _shepherd->doMoveAbsolute( std::max( 100, _printJob->layerThickness ) / 1000.0 );
            break;

        default:
            return;
    }
}

void PrintTab::raiseBuildPlatform_moveToComplete( bool const success ) {
    debug( "+ PrintTab::raiseBuildPlatform_moveToComplete: %s\n", success ? "succeeded" : "failed" );
    QObject::disconnect( _shepherd, &Shepherd::action_moveToComplete, this, &PrintTab::raiseBuildPlatform_moveToComplete );

    if ( success ) {
        _buildPlatformState = BuildPlatformState::Raised;
        _raiseOrLowerButton->setText( "Lower Build Platform" );
        _raiseOrLowerButton->setEnabled( true );
    } else {
        _buildPlatformState = BuildPlatformState::Lowered;
    }

    setAdjustmentButtonsEnabled( true );
}

void PrintTab::lowerBuildPlatform_moveToComplete( bool const success ) {
    debug( "+ PrintTab::lowerBuildPlatform_moveToComplete: %s\n", success ? "succeeded" : "failed" );
    QObject::connect( _shepherd, &Shepherd::action_moveToComplete, this, &PrintTab::lowerBuildPlatform_moveToComplete );

    if ( success ) {
        _buildPlatformState = BuildPlatformState::Lowered;
        _raiseOrLowerButton->setText( "Raise Build Platform" );
        _raiseOrLowerButton->setEnabled( true );
    } else {
        _buildPlatformState = BuildPlatformState::Raised;
    }

    setAdjustmentButtonsEnabled( true );
}

void PrintTab::homeButton_clicked( bool ) {
    debug( "+ PrintTab::homeButton_clicked\n" );
    setAdjustmentButtonsEnabled( false );

    QObject::connect( _shepherd, &Shepherd::action_homeComplete, this, &PrintTab::home_homeComplete );
    _shepherd->doHome( );
}

void PrintTab::home_homeComplete( bool const success ) {
    debug( "+ PrintTab::home_homeComplete: %s\n", success ? "succeeded" : "failed" );
    setAdjustmentButtonsEnabled( true );
}

void PrintTab::setAdjustmentButtonsEnabled( bool const value ) {
    debug( "+ PrintTab::setAdjustmentButtonsEnabled: value %s\n", value ? "enabled" : "disabled" );
    _raiseOrLowerButton->setEnabled( value );
    _homeButton        ->setEnabled( value );
}

void PrintTab::setPrintButtonEnabled( bool const value ) {
    debug( "+ PrintTab::setPrintButtonEnabled: value %s\n", value ? "enabled" : "disabled" );
    _printButton->setEnabled( value );
}

void PrintTab::_connectPrintJob( ) {
    int value = _printJob->exposureTime / 0.5;
    _printJob->exposureTime = value / 2.0;
    _exposureTimeSlider->setValue( value );
    _exposureTimeValue->setText( QString( "%1 s" ).arg( _printJob->exposureTime, 0, 'f', 1 ) );

    debug( "+ PrintTab::setPrintJob: _printJob->exposureTimeScaleFactor=%f\n", _printJob->exposureTimeScaleFactor );
    _exposureTimeScaleFactorSlider->setValue( _printJob->exposureTimeScaleFactor );
    _exposureTimeScaleFactorValue->setText( QString( "%1×" ).arg( _printJob->exposureTimeScaleFactor ) );

    _powerLevelSlider->setValue( _printJob->powerLevel / 255.0 * 100.0 + 0.5 );
    _powerLevelValue->setText( QString( "%1%" ).arg( static_cast<int>( _printJob->powerLevel / 255.0 * 100.0 + 0.5 ) ) );
}
