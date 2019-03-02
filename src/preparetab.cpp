#include "pch.h"

#include "preparetab.h"

#include "printjob.h"
#include "shepherd.h"
#include "strings.h"
#include "svgrenderer.h"
#include "utils.h"

PrepareTab::PrepareTab( QWidget* parent ): QWidget( parent ) {
    _initialShowEventFunc = std::bind( &PrepareTab::_initialShowEvent, this );

    auto origFont = font( );
    auto boldFont = ModifyFont( origFont, origFont.pointSizeF( ), QFont::Bold );
    auto font22pt = ModifyFont( origFont, 22.0f );

    layerThicknessLabel->setText( "Layer height:" );

    layerThickness100Button->setChecked( true );
    layerThickness100Button->setText( "Standard res (100 µm)" );
    QObject::connect( layerThickness100Button, &QPushButton::clicked, this, &PrepareTab::layerThickness100Button_clicked );

    layerThickness50Button->setText( "High res (50 µm)" );
    QObject::connect( layerThickness50Button,  &QPushButton::clicked, this, &PrepareTab::layerThickness50Button_clicked  );

    layerThicknessButtonsLayout->setContentsMargins( { } );
    layerThicknessButtonsLayout->addWidget( layerThickness100Button );
    layerThicknessButtonsLayout->addWidget( layerThickness50Button  );

    sliceStatusLabel->setText( "Slicer status:" );
    sliceStatusLabel->setBuddy( sliceStatus );

    sliceStatus->setText( "idle" );
    sliceStatus->setFont( boldFont );

    imageGeneratorStatusLabel->setText( "Image generator:" );
    imageGeneratorStatusLabel->setBuddy( imageGeneratorStatus );

    imageGeneratorStatus->setText( "idle" );
    imageGeneratorStatus->setFont( boldFont );

    _prepareMessage->setAlignment( Qt::AlignCenter );
    _prepareMessage->setTextFormat( Qt::RichText );
    _prepareMessage->setText( QString( "Tap the <b>Prepare</b> button below<br>to prepare the printer." ) );

    _prepareProgress->setRange( 0, 0 );
    _prepareProgress->hide( );

    _prepareInnerLayout->setContentsMargins( { } );
    _prepareInnerLayout->addStretch( ); _prepareInnerLayout->addWidget( _prepareMessage,  0, Qt::AlignCenter );
    _prepareInnerLayout->addStretch( ); _prepareInnerLayout->addWidget( _prepareProgress, 0, Qt::AlignCenter );
    _prepareInnerLayout->addStretch( );

    _prepareGroup->setTitle( "Printer preparation" );
    _prepareGroup->setLayout( _prepareInnerLayout );

    _prepareButton->setEnabled( true );
    _prepareButton->setFixedSize( MainButtonSize );
    _prepareButton->setFont( font22pt );
    _prepareButton->setText( QString( "Prepare" ) );
    QObject::connect( _prepareButton, &QPushButton::clicked, this, &PrepareTab::_prepareButton_clicked );

    _prepareLayout = WrapWidgetsInVBox( { _prepareGroup, _prepareButton } );

    optionsLayout->setContentsMargins( { } );
    optionsLayout->addWidget( layerThicknessLabel );
    optionsLayout->addLayout( layerThicknessButtonsLayout );
    optionsLayout->addLayout( WrapWidgetsInHBox( { sliceStatusLabel,          nullptr, sliceStatus          } ) );
    optionsLayout->addLayout( WrapWidgetsInHBox( { imageGeneratorStatusLabel, nullptr, imageGeneratorStatus } ) );
    optionsLayout->addLayout( _prepareLayout );

    optionsContainer->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    optionsContainer->setLayout( optionsLayout );

    sliceButton->setEnabled( false );
    sliceButton->setFixedSize( MainButtonSize );
    sliceButton->setFont( font22pt );
    sliceButton->setText( "Slice" );
    QObject::connect( sliceButton, &QPushButton::clicked, this, &PrepareTab::sliceButton_clicked );

    currentSliceLabel->setText( "Current layer:" );
    currentSliceLabel->setBuddy( currentSliceImage );

    currentSliceImage->setAlignment( Qt::AlignCenter );
    currentSliceImage->setContentsMargins( { } );
    currentSliceImage->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    currentSliceImage->setStyleSheet( QString( "QWidget { background: black }" ) );

    currentSliceLayout->setAlignment( Qt::AlignCenter );
    currentSliceLayout->setContentsMargins( { } );
    currentSliceLayout->addWidget( currentSliceImage );

    currentSliceGroup->setTitle( "Current layer" );
    currentSliceGroup->setMinimumSize( MaximalRightHandPaneSize );
    currentSliceGroup->setLayout( currentSliceLayout );

    _layout->setContentsMargins( { } );
    _layout->addWidget( optionsContainer,  0, 0, 1, 1 );
    _layout->addWidget( sliceButton,       1, 0, 1, 1 );
    _layout->addWidget( currentSliceGroup, 0, 1, 2, 1 );
    _layout->setRowStretch( 0, 4 );
    _layout->setRowStretch( 1, 1 );

    setContentsMargins( { } );
    setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    setLayout( _layout );
}

PrepareTab::~PrepareTab( ) {
    /*empty*/
}

void PrepareTab::showEvent( QShowEvent* ev ) {
    if ( _initialShowEventFunc ) {
        _initialShowEventFunc( );
        _initialShowEventFunc = nullptr;
    }
    ev->ignore( );
}

void PrepareTab::_initialShowEvent( ) {
    currentSliceImage->setFixedWidth( currentSliceImage->width( ) );
    currentSliceImage->setFixedHeight( currentSliceImage->width( ) / AspectRatio16to10 + 0.5 );
}

void PrepareTab::layerThickness50Button_clicked( bool checked ) {
    debug( "+ PrepareTab::layerThickness50Button_clicked\n" );
    _printJob->layerThickness = 50;
}

void PrepareTab::layerThickness100Button_clicked( bool checked ) {
    debug( "+ PrepareTab::layerThickness100Button_clicked\n" );
    _printJob->layerThickness = 100;
}

void PrepareTab::sliceButton_clicked( bool ) {
    debug( "+ PrepareTab::sliceButton_clicked\n" );

    _printJob->pngFilesPath = StlModelLibraryPath + QString( "/working_%1" ).arg( static_cast<unsigned long long>( getpid( ) ) * 10000000000ull + static_cast<unsigned long long>( rand( ) ) );
    mkdir( _printJob->pngFilesPath.toUtf8( ).data( ), 0700 );

    QString baseName = GetFileBaseName( _printJob->modelFileName );
    _printJob->slicedSvgFileName =
        _printJob->pngFilesPath +
        Slash +
        baseName.left( baseName.length( ) - ( baseName.endsWith( ".stl", Qt::CaseInsensitive ) ? 4 : 0 ) ) +
        QString( ".svg" );

    debug(
        "  + model filename:      '%s'\n"
        "  + sliced SVG filename: '%s'\n"
        "  + PNG files path:      '%s'\n"
        "",
        _printJob->modelFileName.toUtf8( ).data( ),
        _printJob->slicedSvgFileName.toUtf8( ).data( ),
        _printJob->pngFilesPath.toUtf8( ).data( )
    );

    slicerProcess = new QProcess( this );
    QObject::connect( slicerProcess, &QProcess::errorOccurred, this, &PrepareTab::slicerProcessErrorOccurred );
    QObject::connect( slicerProcess, &QProcess::started,       this, &PrepareTab::slicerProcessStarted       );
    QObject::connect( slicerProcess, QOverload<int, QProcess::ExitStatus>::of( &QProcess::finished ), this, &PrepareTab::slicerProcessFinished );
    slicerProcess->start(
        QString     { "slic3r" },
        QStringList {
            _printJob->modelFileName,
            QString( "--export-svg" ),
            QString( "--layer-height" ),
            QString( "%1" ).arg( _printJob->layerThickness / 1000.0 ),
            QString( "--output" ),
            _printJob->slicedSvgFileName
        }
    );
}

void PrepareTab::slicerProcessErrorOccurred( QProcess::ProcessError error ) {
    debug( "+ PrepareTab::slicerProcessErrorOccurred: error %s [%d]\n", ToString( error ), error );

    if ( QProcess::FailedToStart == error ) {
        debug( "  + slicer process failed to start\n" );
        sliceStatus->setText( "failed to start" );
    } else if ( QProcess::Crashed == error ) {
        debug( "  + slicer process crashed? state is %s [%d]\n", ToString( slicerProcess->state( ) ), slicerProcess->state( ) );
        if ( slicerProcess->state( ) != QProcess::NotRunning ) {
            slicerProcess->kill( );
            debug( "  + slicer terminated\n" );
        }
        sliceStatus->setText( "crashed" );
    }
}

void PrepareTab::slicerProcessStarted( ) {
    debug( "+ PrepareTab::slicerProcessStarted\n" );
    sliceStatus->setText( "started" );
    imageGeneratorStatus->setText( "waiting" );
    currentSliceImage->clear( );
    emit sliceStarted( );
}

void PrepareTab::slicerProcessFinished( int exitCode, QProcess::ExitStatus exitStatus ) {
    QObject::disconnect( slicerProcess, nullptr, this, nullptr );

    debug( "+ PrepareTab::slicerProcessFinished: exitCode: %d, exitStatus: %s [%d]\n", exitCode, ToString( exitStatus ), exitStatus );

    slicerProcess->deleteLater( );
    slicerProcess = nullptr;

    if ( exitStatus == QProcess::CrashExit ) {
        debug( "  + slicer process crashed?\n" );
        sliceStatus->setText( "crashed" );
        emit sliceComplete( false );
        return;
    }

    sliceStatus->setText( "finished" );
    emit sliceComplete( true );

    svgRenderer = new SvgRenderer;
    QObject::connect( svgRenderer, &SvgRenderer::nextLayer, this, &PrepareTab::svgRenderer_progress );
    QObject::connect( svgRenderer, &SvgRenderer::done,      this, &PrepareTab::svgRenderer_done     );
    svgRenderer->startRender( _printJob->slicedSvgFileName, _printJob->pngFilesPath );

    emit renderStarted( );
}

void PrepareTab::svgRenderer_progress( int const currentLayer ) {
    imageGeneratorStatus->setText( QString( "layer %1" ).arg( currentLayer ) );
    if ( 0 != ( currentLayer % 5 ) ) {
        return;
    }

    auto pixmap = QPixmap( _printJob->pngFilesPath + QString( "/%2.png" ).arg( currentLayer - 1, 6, 10, DigitZero ) );
    if ( ( pixmap.width( ) > currentSliceImage->width( ) ) || ( pixmap.height( ) > currentSliceImage->height( ) ) ) {
        pixmap = pixmap.scaled( currentSliceImage->size( ), Qt::KeepAspectRatio, Qt::SmoothTransformation );
    }
    currentSliceImage->setPixmap( pixmap );
}

void PrepareTab::svgRenderer_done( int const totalLayers ) {
    if ( totalLayers == -1 ) {
        imageGeneratorStatus->setText( QString( "failed" ) );
    } else {
        imageGeneratorStatus->setText( QString( "finished" ) );
        _printJob->layerCount = totalLayers;
    }

    svgRenderer->deleteLater( );
    svgRenderer = nullptr;

    emit renderComplete( totalLayers != -1 );
}

void PrepareTab::_prepareButton_clicked( bool ) {
    debug( "+ PrepareTab::_prepareButton_clicked\n" );

    QObject::disconnect( _prepareButton, nullptr, this, nullptr );

    _prepareMessage->setText( QString( "Moving the printer to<br>its home location..." ) );
    _prepareProgress->show( );

    _prepareButton->setText( QString( "Continue" ) );
    _prepareButton->setEnabled( false );

    QObject::connect( _shepherd, &Shepherd::action_homeComplete, this, &PrepareTab::_shepherd_homeComplete );
    _shepherd->doHome( );
}

void PrepareTab::_shepherd_homeComplete( bool const success ) {
    debug( "+ PrepareTab::_shepherd_homeComplete: success: %s\n", success ? "true" : "false" );

    QObject::disconnect( _shepherd, nullptr, this, nullptr );
    _prepareProgress->hide( );

    if ( !success ) {
        _prepareMessage->setText( QString( "Preparation failed." ) );

        _prepareButton->setText( QString( "Retry" ) );
        _prepareButton->setEnabled( true );

        emit preparePrinterComplete( false );
        return;
    }

    _prepareMessage->setText( QString( "<div style='text-align: center;'>Adjust the build platform position,<br>then tap <b>Continue</b>.</div>" ) );

    QObject::connect( _prepareButton, &QPushButton::clicked, this, &PrepareTab::_adjustBuildPlatform_complete );
    _prepareButton->setEnabled( true );
}

void PrepareTab::_adjustBuildPlatform_complete( bool ) {
    debug( "+ PrepareTab::_adjustBuildPlatform_complete\n" );

    QObject::disconnect( _prepareButton, nullptr, this, nullptr );
    _prepareButton->setEnabled( false );

    _prepareMessage->setText( QString( "<div style='text-align: center;'>Raising the build platform<br>out of the way...</div>" ) );
    _prepareProgress->show( );

    QObject::connect( _shepherd, &Shepherd::action_moveToComplete, this, &PrepareTab::_shepherd_resinLoadMoveToComplete );
    _shepherd->doMoveTo( PrinterMaximumZ );
}

void PrepareTab::_shepherd_resinLoadMoveToComplete( bool const success ) {
    debug( "+ PrepareTab::_shepherd_resinLoadMoveToComplete: success: %s\n", success ? "true" : "false" );

    QObject::disconnect( _shepherd, nullptr, this, nullptr );
    _prepareProgress->hide( );

    if ( !success ) {
        _prepareMessage->setText( QString( "Preparation failed." ) );

        _prepareButton->setText( QString( "Retry" ) );
        _prepareButton->setEnabled( true );

        emit preparePrinterComplete( false );
        return;
    }

    _prepareMessage->setText( QString( "Preparation completed." ) );

    QObject::connect( _prepareButton, &QPushButton::clicked, this, &PrepareTab::_prepareButton_clicked );
    _prepareButton->setText( "Prepare" );
    _prepareButton->setEnabled( true );

    emit preparePrinterComplete( true );
}

void PrepareTab::setPrepareButtonEnabled( bool const value ) {
    _prepareButton->setEnabled( value );
}

void PrepareTab::setPrintJob( PrintJob* printJob ) {
    debug( "+ PrepareTab::setPrintJob: printJob %p\n", printJob );
    _printJob = printJob;
}

void PrepareTab::setShepherd( Shepherd* newShepherd ) {
    if ( _shepherd ) {
        QObject::disconnect( _shepherd, nullptr, this, nullptr );
    }

    _shepherd = newShepherd;
}

void PrepareTab::setSliceButtonEnabled( bool const value ) {
    sliceButton->setEnabled( value );
}
